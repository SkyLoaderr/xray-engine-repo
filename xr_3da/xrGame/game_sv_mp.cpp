#include "stdafx.h"
#include "game_sv_mp.h"
#include "xrServer.h"
#include "xrMessages.h"
#include "xrServer_Object_Base.h"
#include "xrServer_Objects.h"
#include "level.h"
#include "xrserver_objects_alife_monsters.h"
#include "actor.h"
#include "clsid_game.h"
#include "../XR_IOConsole.h"
#include "../igame_persistent.h"
#include "date_time.h"

u32		g_dwMaxCorpses = 10;

game_sv_mp::game_sv_mp() :inherited()
{
	m_bVotingActive = false;
	//------------------------------------------------------
	g_pGamePersistent->Environment.SetWeather("mp_weather");
	
}

void	game_sv_mp::Update	()
{
	inherited::Update();

	// remove corpses if their number exceed limit
	while (m_CorpseList.size()>g_dwMaxCorpses)
	{
		u16 CorpseID = m_CorpseList.front();
		m_CorpseList.pop_front();
		//---------------------------------------------
		NET_Packet			P;
		u_EventGen			(P,GE_DESTROY,CorpseID);
		Level().Send(P,net_flags(TRUE,TRUE));
	};

	if (IsVoteEnabled() && IsVotingActive()) UpdateVote();
}

void game_sv_mp::OnRoundStart			()
{
	inherited::OnRoundStart();

	m_CorpseList.clear();

	switch_Phase	(GAME_PHASE_INPROGRESS);
	++round;

	// clear "ready" flag
	u32		cnt		= get_players_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->resetFlag(GAME_PLAYER_FLAG_READY+GAME_PLAYER_FLAG_VERY_VERY_DEAD);
	};

	// 1. We have to destroy all player-entities and entities
	m_server->SLS_Clear	();

	// 2. We have to create them at respawn points and/or specified positions
	m_server->SLS_Default	();

	//send "RoundStarted" Message To Allclients
	NET_Packet			P;
//	P.w_begin			(M_GAMEMESSAGE);
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_ROUND_STARTED);
	u_EventSend(P);

	signal_Syncronize();
}

void game_sv_mp::OnRoundEnd				(LPCSTR reason)
{
	inherited::OnRoundEnd( reason );

	switch_Phase		(GAME_PHASE_PENDING);
	//send "RoundOver" Message To All clients
	NET_Packet			P;
//	P.w_begin			(M_GAMEMESSAGE);
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_ROUND_END);
	P.w_stringZ			(reason);
	u_EventSend(P);
	//-------------------------------------------------------
}


void	game_sv_mp::KillPlayer				(ClientID id_who, u16 GameID)
{
	CObject* pObject =  Level().Objects.net_Find(GameID);
	if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) return;
	// Remove everything	
	xrClientData* xrCData	=	m_server->ID_to_client(id_who);
	
	if (xrCData && xrCData->ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) return;
	if (xrCData) 
	{
		xrCData->ps->setFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
		xrCData->ps->deaths				+=	1;
		xrCData->ps->DeathTime			= Device.dwTimeGlobal;

	};
	//-------------------------------------------------------
	CActor* pActor = smart_cast <CActor*>(pObject);
	if (pActor)
	{
		if (!pActor->g_Alive())
		{
			return;
		}
		pActor->set_death_time		();
	}
	//-------------------------------------------------------
	u16 PlayerID = (xrCData != 0) ? xrCData->ps->GameID : GameID;
	// Kill Player on all clients
	NET_Packet			P;
	u_EventGen(P, GE_DIE, PlayerID);
	P.w_u16				(PlayerID);
	P.w_clientID		(id_who);
	ClientID clientID;clientID.setBroadcast();
	u_EventSend(P);
	
	signal_Syncronize();
};


void	game_sv_mp::OnEvent (NET_Packet &P, u16 type, u32 time, ClientID sender )
{

	switch	(type)
	{	
	case GAME_EVENT_PLAYER_KILLED:  //playerKillPlayer
		{
			ClientID cl1, cl2;
			P.r_clientID(cl1);
			P.r_clientID(cl2);
			OnPlayerKillPlayer( cl1, cl2 );

		}break;

	case GAME_EVENT_PLAYER_READY:// cs & dm 
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			OnPlayerReady		(l_pC->ID);
			
		}break;
	case GAME_EVENT_VOTE_START:
		{
			if (!IsVoteEnabled()) break;
			string1024 VoteCommand;
			P.r_stringZ(VoteCommand);
			OnVoteStart(VoteCommand, sender);
		}break;
	case GAME_EVENT_VOTE_YES:
		{
			if (!IsVoteEnabled()) break;
			OnVoteYes(sender);
		}break;
	case GAME_EVENT_VOTE_NO:
		{
			if (!IsVoteEnabled()) break;
			OnVoteNo(sender);
		}break;
	default:
		inherited::OnEvent(P, type, time, sender);
	};//switch

}

void	game_sv_mp::SendPlayerKilledMessage	(ClientID id_killer, ClientID id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	if (!ps_killed || !ps_killer) return;

	NET_Packet			P;
//	P.w_begin			(M_GAMEMESSAGE);
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_PLAYER_KILLED);
	P.w_u16				(ps_killed->GameID);
	if (ps_killer->GameID == ps_killed->lasthitter)
	{
		P.w_u16				(ps_killer->GameID);
		P.w_u16				(ps_killed->lasthitweapon);
	}
	else
	{
		P.w_u16				(ps_killer->GameID);
		P.w_u16				(0);
	};

	u_EventSend(P);
};

void game_sv_mp::Create (shared_str &options)
{
	SetVotingActive(false);
	inherited::Create(options);
	//-------------------------------------------------------------------
	string64	StartTime, TimeFactor;
	strcpy(StartTime,get_option_s		(*options,"estime","12:00:00"));
	strcpy(TimeFactor,get_option_s		(*options,"etimef","1"));
	
	u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
	sscanf				(StartTime,"%d:%d:%d.%d",&hours,&mins,&secs,&milisecs);
	u64 StartEnvGameTime	= generate_time	(year,month,day,hours,mins,secs,milisecs);
	float EnvTimeFactor = float(atof(TimeFactor))*GetEnvironmentGameTimeFactor();

	SetEnvironmentGameTimeFactor(StartEnvGameTime,EnvTimeFactor);
//	SetGameTimeFactor(StartEnvGameTime,EnvTimeFactor);
};

void game_sv_mp::net_Export_State		(NET_Packet& P, ClientID id_to)
{
	inherited::net_Export_State(P, id_to);
};

void	game_sv_mp::RespawnPlayer			(ClientID id_who, bool NoSpectator)
{
	//------------------------------------------------------------
	xrClientData* xrCData	=	m_server->ID_to_client(id_who);
	if (!xrCData || !xrCData->owner) return;
//	game_PlayerState*	ps	=	&(xrCData->ps);
	CSE_Abstract* pOwner = xrCData->owner;
	CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(pOwner);
	CSE_Spectator			*pS =	smart_cast<CSE_Spectator*>(pOwner);

	if (pA)
	{
		//------------------------------------------------------------			
		AllowDeadBodyRemove(id_who, xrCData->ps->GameID);
		//------------------------------------------------------------
		m_CorpseList.push_back(pOwner->ID);
		//------------------------------------------------------------
	};

	if (pA && !NoSpectator)
	{
		//------------------------------------------------------------
		SpawnPlayer(id_who, "spectator");
		//------------------------------------------------------------
	}
	else
	{
		//------------------------------------------------------------
		if (pOwner->owner != m_server->GetServerClient())
		{
			pOwner->owner = (xrClientData*)m_server->GetServerClient();
		};
		//------------------------------------------------------------
		//remove spectator entity
		if (pS)
		{
			NET_Packet			P;
			u_EventGen			(P,GE_DESTROY,pS->ID);
			//		pObject->u_EventSend		(P);
			Level().Send(P,net_flags(TRUE,TRUE));
		};
		//------------------------------------------------------------
		SpawnPlayer(id_who, "mp_actor");
		//------------------------------------------------------------
//		SpawnWeaponsForActor(xrCData->owner, ps);
		//------------------------------------------------------------
	};
};


void	game_sv_mp::SpawnPlayer				(ClientID id, LPCSTR N)
{
	xrClientData* CL	= m_server->ID_to_client(id);
	game_PlayerState*	ps_who	=	CL->ps;
	ps_who->setFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
	
	// Spawn "actor"
	LPCSTR			options			=	get_name_id	(id);
	CSE_Abstract*	E				=	spawn_begin	(N);													// create SE
	
	E->set_name_replace		(get_option_s(options,"name","Player"));					// name

	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER);	// flags

	CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(E);
	CSE_Spectator			*pS	=	smart_cast<CSE_Spectator*>(E);

	R_ASSERT2	(pA || pS,"Respawned Client is not Actor nor Spectator");
	
	if (pA) 
	{
		pA->s_team				=	u8(ps_who->team);
		assign_RP				(pA, ps_who);
		SetSkin(E, pA->s_team, ps_who->skin);
		ps_who->resetFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
		if (!ps_who->RespawnTime)
		{
			OnPlayerEnteredGame(id);
		};
		ps_who->RespawnTime = Device.dwTimeGlobal;
	}
	else
		if (pS)
		{
			Fvector Pos, Angle;
			ps_who->setFlag(GAME_PLAYER_FLAG_CS_SPECTATOR);
			if (!GetPosAngleFromActor(id, Pos, Angle)) assign_RP				(E, ps_who);
			else
			{
				E->o_Angle.set(Angle);
				E->o_Position.set(Pos);
			}
		};
	
	Msg		("* %s respawned as %s",get_option_s(options,"name","Player"), (0 == pA) ? "spectator" : "actor");
	spawn_end				(E,id);

	ps_who->GameID = CL->owner->ID;

	signal_Syncronize();
}

void	game_sv_mp::AllowDeadBodyRemove		(ClientID id, u16 GameID)
{
	CSE_Abstract* pSObject = get_entity_from_eid(GameID);
	pSObject->owner = (xrClientData*)m_server->GetServerClient();

	CObject* pObject =  Level().Objects.net_Find(GameID);
	

	if (pObject && pObject->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		CActor* pActor = smart_cast <CActor*>(pObject);
		if (pActor)
		{
			pActor->set_death_time		();
			pActor->m_bAllowDeathRemove = true;
		};
	};	
};

void game_sv_mp::OnPlayerConnect			(ClientID id_who)
{
	inherited::OnPlayerConnect (id_who);
}

void game_sv_mp::OnPlayerDisconnect		(ClientID id_who, LPSTR Name, u16 GameID)
{
	//---------------------------------------------------
	NET_Packet			P;
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_PLAYER_DISCONNECTED);
	P.w_stringZ			(Name);
	u_EventSend(P);
	//---------------------------------------------------
	KillPlayer	(id_who, GameID);
	
	AllowDeadBodyRemove(id_who, GameID);
	inherited::OnPlayerDisconnect (id_who, Name, GameID);
}

void	game_sv_mp::SetSkin					(CSE_Abstract* E, u16 Team, u16 ID)
{
	if (!E) return;
	//-------------------------------------------
	CSE_Visual* pV = smart_cast<CSE_Visual*>(E);
	if (!pV) return;
	//-------------------------------------------
	string256 SkinName;
	std::strcpy(SkinName, pSettings->r_string("mp_skins_path", "skin_path"));
	//загружены ли скины для этой комманды

	if (!TeamList.empty()	&&
		TeamList.size() > Team	&&
		!TeamList[Team].aSkins.empty())
	{
		//загружено ли достаточно скинов для этой комманды
		if (TeamList[Team].aSkins.size() > ID)
		{
			std::strcat(SkinName, TeamList[Team].aSkins[ID].c_str());
		}
		else
			std::strcat(SkinName, TeamList[Team].aSkins[0].c_str());
	}
	else
	{
		//скины для такой комманды не загружены
		switch (Team)
		{
		case 0:
			std::strcat(SkinName, "stalker_hood_multiplayer");
			break;
		case 1:
			std::strcat(SkinName, "soldat_beret");
			break;
		case 2:
			std::strcat(SkinName, "stalker_black_mask");
			break;
		default:
			R_ASSERT2(0,"Unknown Team");
			break;
		};
	};
	std::strcat(SkinName, ".ogf");
	Msg("* Skin - %s", SkinName);
	int len = xr_strlen(SkinName);
	R_ASSERT2(len < 64, "Skin Name is too LONG!!!");
	pV->set_visual(SkinName);
	//-------------------------------------------
};

#include "../CameraBase.h"

bool	game_sv_mp::GetPosAngleFromActor				(ClientID id, Fvector& Pos, Fvector &Angle)
{
	xrClientData* xrCData	=	m_server->ID_to_client(id);
	if (!xrCData || !xrCData->owner) return false;
	
	CObject* pObject =  Level().Objects.net_Find(xrCData->owner->ID);
///	R_ASSERT2	((pObject && pObject->CLS_ID == CLSID_OBJECT_ACTOR),"Dead Player is not Actor");

	if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) return false;

	CActor* pActor = smart_cast <CActor*>(pObject);
	if (!pActor) return false;

	Angle.set(-pActor->cam_Active()->pitch, -pActor->cam_Active()->yaw, 0);
	Pos.set(pActor->cam_Active()->vPosition);
	return true;
};

TeamStruct* game_sv_mp::GetTeamData				(u8 Team)
{
	VERIFY(TeamList.size());
	if (TeamList.empty()) return NULL;
	
	VERIFY(TeamList.size()>Team);
	if (TeamList.size()<=Team) return NULL;

	return &(TeamList[Team]);
};

void	game_sv_mp::SpawnWeaponForActor		(u16 actorId,  LPCSTR N, bool isScope, bool isGrenadeLauncher, bool isSilencer)
{
		u8 addon_flags = 0;
		if(isScope)
			addon_flags |= CSE_ALifeItemWeapon::eWeaponAddonScope;

		if(isGrenadeLauncher)
			addon_flags |= CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;

		if(isSilencer)
			addon_flags |= CSE_ALifeItemWeapon::eWeaponAddonSilencer;

		SpawnWeapon4Actor(actorId, N, addon_flags);
}

void	game_sv_mp::SpawnWeapon4Actor		(u16 actorId,  LPCSTR N, u8 Addons)
{
	if (!N) return;
	
	CSE_Abstract			*E	=	spawn_begin	(N);
	E->ID_Parent = actorId;

	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL);	// flags

	/////////////////////////////////////////////////////////////////////////////////
	//если это оружие - спавним его с полным магазином
	CSE_ALifeItemWeapon		*pWeapon	=	smart_cast<CSE_ALifeItemWeapon*>(E);
	if (pWeapon)
	{
		pWeapon->a_elapsed = pWeapon->get_ammo_magsize();

		pWeapon->m_addon_flags.assign(Addons);
	};
	/////////////////////////////////////////////////////////////////////////////////

	spawn_end				(E,m_server->GetServerClient()->ID);
};

void game_sv_mp::OnDestroyObject			(u16 eid_who)
{
	for (u32 i=0; i<m_CorpseList.size();)
	{
		if (m_CorpseList[i] == eid_who)
		{
			m_CorpseList.erase(m_CorpseList.begin()+i);
		}
		else i++;
	};
};

bool game_sv_mp::OnNextMap				()
{
	if (!m_bMapRotation) return false;
	if (!m_pMapRotation_List.size()) return false;

	xr_string MapName = m_pMapRotation_List.front();
	m_pMapRotation_List.pop_front();
	m_pMapRotation_List.push_back(MapName);

	MapName = m_pMapRotation_List.front();

	Msg("Going to level %s", MapName.c_str());

	if (!stricmp(MapName.c_str(), Level().name().c_str())) return false;
	string1024 Command;
	sprintf(Command, "sv_changelevel %s", MapName.c_str());
	Console->Execute(Command);
	return true;
};

void game_sv_mp::OnPrevMap				()
{
	if (!m_bMapRotation) return;
	if (!m_pMapRotation_List.size()) return;
	
	xr_string MapName = m_pMapRotation_List.back();
	m_pMapRotation_List.pop_back();
	m_pMapRotation_List.push_front(MapName);

	Msg("Goint to level %s", MapName.c_str());
	if (!stricmp(MapName.c_str(), Level().name().c_str())) return;

	string1024	Command;
	sprintf(Command, "sv_changelevel %s", MapName.c_str());
	Console->Execute(Command);
};

struct _votecommands		{
	char *	name;
	char *	command;
};

_votecommands	votecommands[] = {
	{ "kick",			"sv_kick"	},
	{ "ban",			"sv_banplayer"	},
	{ "restart",		"g_restart"	},
	{ "nextmap",		"sv_nextmap"},
	{ "prevmap",		"sv_prevmap"},
	{ "changemap",		"sv_changelevel"},
	{ "changegame",		"sv_changegametype"},
	{ "changemapgame",	"sv_changelevelgametype"},

	{ NULL, 			NULL }
};

#define		VOTE_LENGTH_TIME		120000

void game_sv_mp::OnVoteStart				(LPCSTR VoteCommand, ClientID sender)
{
	if (!IsVoteEnabled()) return;
	char	CommandName[256];	CommandName[0]=0;
	char	CommandParams[256];	CommandParams[0]=0;
	sscanf	(VoteCommand,"%s ", CommandName);
	if (xr_strlen(CommandName)+1 < xr_strlen(VoteCommand))
	{
		strcpy(CommandParams, VoteCommand + xr_strlen(CommandName)+1);
	}

	int i=0;
	bool Found = false;
	while (votecommands[i].command)
	{
		if (!stricmp(votecommands[i].name, CommandName))
		{
			Found = true;
			break;
		};
		i++;
	};
	if (!Found) 
	{
		Msg("Unknown Vote Command - %s", CommandName);
		return;
	};

	//-----------------------------------------------------------------------------
	SetVotingActive(true);
	u32 CurTime = Level().timeServer();
	m_uVoteEndTime = CurTime+VOTE_LENGTH_TIME;
	m_pVoteCommand.sprintf("%s %s", votecommands[i].command, CommandParams);

	xrClientData *pStartedPlayer = NULL;
	u32	cnt = get_players_count();	
	for(u32 it=0; it<cnt; it++)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		if (!l_pC) continue;
		if (l_pC->ID == sender)
		{
			l_pC->ps->m_bCurrentVoteAgreed = 1;
			pStartedPlayer = l_pC;
		}
		else
			l_pC->ps->m_bCurrentVoteAgreed = 0;
	};

	signal_Syncronize();
	//-----------------------------------------------------------------------------
	NET_Packet P;
	GenerateGameMessage (P);
	P.w_u32(GAME_EVENT_VOTE_START);
	P.w_stringZ(VoteCommand);
	P.w_stringZ(pStartedPlayer ? pStartedPlayer->ps->getName() : "");
	P.w_u32(VOTE_LENGTH_TIME);
	u_EventSend(P);
	//-----------------------------------------------------------------------------	
};

void		game_sv_mp::UpdateVote				()
{
	if (!IsVoteEnabled() || !IsVotingActive()) return;

	u32 NumAgreed = 0;
	u32 NumToCount = 0;
	u32	cnt = get_players_count();	
	for(u32 it=0; it<cnt; it++)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC || !l_pC->net_Ready || !ps || ps->Skip) continue;
		if (ps->m_bCurrentVoteAgreed == 1) NumAgreed++;
		NumToCount++;
	};

	bool VoteSucceed = false;
	u32 CurTime = Level().timeServer();
	if (m_uVoteEndTime > CurTime)
	{
		if (NumToCount == NumAgreed) VoteSucceed = true;
		if (!VoteSucceed) return;
	}
	else
	{
		VoteSucceed = (float(NumAgreed)/float(NumToCount)) > 50.0f;		
	};

	SetVotingActive(false);
	if (!VoteSucceed) 
	{
		NET_Packet P;
		GenerateGameMessage (P);
		P.w_u32(GAME_EVENT_VOTE_END);
		P.w_stringZ("Voting failed!");
		u_EventSend(P);
		return;
	};

	Console->Execute(m_pVoteCommand.c_str());

	NET_Packet P;
	GenerateGameMessage (P);
	P.w_u32(GAME_EVENT_VOTE_END);
	P.w_stringZ("Voting Succeed!");
	u_EventSend(P);
};


void		game_sv_mp::OnVoteYes				(ClientID sender)
{
	game_PlayerState* ps = get_id(sender);
	if (!ps) return;
	ps->m_bCurrentVoteAgreed = 1;
	signal_Syncronize();
};

void		game_sv_mp::OnVoteNo				(ClientID sender)
{
	game_PlayerState* ps = get_id(sender);
	if (!ps) return;
	ps->m_bCurrentVoteAgreed = 0;
};

void		game_sv_mp::OnVoteStop				()
{
	SetVotingActive(false);
	//-----------------------------------------------------------------
	NET_Packet P;
	GenerateGameMessage (P);
	P.w_u32(GAME_EVENT_VOTE_STOP);
	u_EventSend(P);
	//-----------------------------------------------------------------
	signal_Syncronize();
};

void		game_sv_mp::OnPlayerEnteredGame		(ClientID id_who)
{
	xrClientData* xrCData	=	m_server->ID_to_client(id_who);
	if (!xrCData) return;

	NET_Packet			P;
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_PLAYER_ENTERED_GAME);
	P.w_stringZ			(get_option_s(*xrCData->Name,"name",*xrCData->Name));
	u_EventSend(P);
};