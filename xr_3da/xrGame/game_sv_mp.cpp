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
#include "game_cl_base.h"

u32		g_dwMaxCorpses = 10;

game_sv_mp::game_sv_mp() :inherited()
{
	m_bVotingActive = false;
	//------------------------------------------------------
//	g_pGamePersistent->Environment.SetWeather("mp_weather");
	m_aRanks.clear();	
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
		//-------------------------------------------------------
		OnPlayerKillPlayer(xrCData->ps, xrCData->ps, KT_HIT, SKT_NONE, NULL);
		xrCData->ps->m_bClearRun = false;
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
	//-------------------------------------------------------
	SendPlayerKilledMessage(PlayerID, KT_HIT, PlayerID, 0, SKT_NONE);
	//-------------------------------------------------------
	// Kill Player on all clients
	NET_Packet			P;
	u_EventGen(P, GE_DIE, PlayerID);
	P.w_u16				(PlayerID);
	P.w_clientID		(id_who);
	ClientID clientID;clientID.setBroadcast();
	u_EventSend(P);
	
	if (xrCData) SetPlayersDefItems		(xrCData->ps);
	signal_Syncronize();
	//-------------------------------------------------------	
};


void	game_sv_mp::OnEvent (NET_Packet &P, u16 type, u32 time, ClientID sender )
{
	switch	(type)
	{	
	case GAME_EVENT_PLAYER_KILLED:  //playerKillPlayer
		{
			OnPlayerKilled(P);
		}break;
	case GAME_EVENT_PLAYER_HITTED:
		{
			OnPlayerHitted(P);			
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
	case GAME_EVENT_PLAYER_NAME:
		{
			OnPlayerChangeName(P, sender);
		}break;
	default:
		inherited::OnEvent(P, type, time, sender);
	};//switch

}

bool g_bConsoleCommandsCreated = false;

void game_sv_mp::Create (shared_str &options)
{
	SetVotingActive(false);
	inherited::Create(options);
	//-------------------------------------------------------------------	
	if (!g_bConsoleCommandsCreated)
	{
		g_bConsoleCommandsCreated = true;
	}
	string64	StartTime, TimeFactor;
	strcpy(StartTime,get_option_s		(*options,"estime","12:00:00"));
	strcpy(TimeFactor,get_option_s		(*options,"etimef","1"));

	u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
	sscanf				(StartTime,"%d:%d:%d.%d",&hours,&mins,&secs,&milisecs);
	u64 StartEnvGameTime	= generate_time	(year,month,day,hours,mins,secs,milisecs);
	float EnvTimeFactor = float(atof(TimeFactor))*GetEnvironmentGameTimeFactor();

	SetEnvironmentGameTimeFactor(StartEnvGameTime,EnvTimeFactor);
	//------------------------------------------------------------------
	LoadRanks();
	//------------------------------------------------------------------
	Set_RankUp_Allowed(true);
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
		//---------------------------------------------
		Game().m_WeaponUsageStatistic.OnPlayerSpawned(ps_who);
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

	ps_who->SetGameID(CL->owner->ID);

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
	//��������� �� ����� ��� ���� ��������

	if (!TeamList.empty()	&&
		TeamList.size() > Team	&&
		!TeamList[Team].aSkins.empty())
	{
		//��������� �� ���������� ������ ��� ���� ��������
		if (TeamList[Team].aSkins.size() > ID)
		{
			std::strcat(SkinName, TeamList[Team].aSkins[ID].c_str());
		}
		else
			std::strcat(SkinName, TeamList[Team].aSkins[0].c_str());
	}
	else
	{
		//����� ��� ����� �������� �� ���������
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
	//���� ��� ������ - ������� ��� � ������ ���������
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

void	game_sv_mp::ClearPlayerItems		(game_PlayerState* ps)
{
	ps->pItemList.clear();
	ps->LastBuyAcount = 0;
//	ps->m_bClearRun = false;
};

void	game_sv_mp::SetPlayersDefItems		(game_PlayerState* ps)
{
	ClearPlayerItems(ps);
	//-------------------------------------------

	//fill player with default items
	if (ps->team < s16(TeamList.size()))
	{
		DEF_ITEMS_LIST	aDefItems = TeamList[ps->team].aDefaultItems;

		for (u16 i=0; i<aDefItems.size(); i++)
		{
			ps->pItemList.push_back(aDefItems[i]);
		}
	};
};

void	game_sv_mp::ClearPlayerState		(game_PlayerState* ps)
{
	if (!ps) return;

	ps->kills				= 0;
	ps->m_iKillsInRow		= 0;
	ps->deaths				= 0;
	ps->lasthitter			= 0;
	ps->lasthitweapon		= 0;

	ClearPlayerItems		(ps);
};

void	game_sv_mp::OnPlayerKilled			(NET_Packet P)
{
	u16 KilledID = P.r_u16();
	KILL_TYPE KillType = KILL_TYPE(P.r_u8());
	u16 KillerID = P.r_u16();
	u16	WeaponID = P.r_u16();
	SPECIAL_KILL_TYPE SpecialKill = SPECIAL_KILL_TYPE(P.r_u8());

	game_PlayerState* ps_killer = get_eid(KillerID);
	game_PlayerState* ps_killed = get_eid(KilledID);
	CSE_Abstract* pWeaponA = get_entity_from_eid(WeaponID);

	OnPlayerKillPlayer(ps_killer, ps_killed, KillType, SpecialKill, pWeaponA);
	//---------------------------------------------------
	SendPlayerKilledMessage((ps_killed)?ps_killed->GameID:KilledID, KillType, (ps_killer)?ps_killer->GameID:KillerID, WeaponID, SpecialKill);
};

void	game_sv_mp::OnPlayerHitted			(NET_Packet P)
{
	u16		id_hitted = P.r_u16();
	u16     id_hitter = P.r_u16();
	float	dHealth = P.r_float();
	game_PlayerState* PSHitter		=	get_eid			(id_hitter);
	if (!PSHitter) return;
	game_PlayerState* PSHitted		=	get_eid			(id_hitted);

	if (!PSHitted || !CheckTeams() || PSHitted->team != PSHitter->team)
		Player_AddExperience(PSHitter, dHealth);
};
	
void	game_sv_mp::SendPlayerKilledMessage	(u16 KilledID, KILL_TYPE KillType, u16 KillerID, u16 WeaponID, SPECIAL_KILL_TYPE SpecialKill)
{
	NET_Packet			P;
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_PLAYER_KILLED);

	P.w_u8	(u8(KillType));
	P.w_u16	(KilledID);
	P.w_u16	(KillerID);
	P.w_u16	(WeaponID);
	P.w_u8	(u8(SpecialKill));

	u32	cnt = get_players_count();	
	for(u32 it=0; it<cnt; it++)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC || !l_pC->net_Ready || !ps) continue;
		m_server->SendTo(l_pC->ID, P);
	};
};

void	game_sv_mp::OnPlayerChangeName		(NET_Packet& P, ClientID sender)
{
	string1024 NewName = "";
	P.r_stringZ(NewName);
	xrClientData*	pClient	= (xrClientData*)m_server->ID_to_client	(sender);
	
	if (!pClient || !pClient->net_Ready) return;
	game_PlayerState* ps = pClient->ps;
	if (!ps) return;

	if (pClient->owner)
	{
		NET_Packet			P;
		GenerateGameMessage(P);
		P.w_u32(GAME_EVENT_PLAYER_NAME);
		P.w_u16(pClient->owner->ID);
		P.w_s16(ps->team);
		P.w_stringZ(ps->getName());
		P.w_stringZ(NewName);
		//---------------------------------------------------		
		u32	cnt = get_players_count();	
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
			game_PlayerState* ps	= l_pC->ps;
			if (!l_pC || !l_pC->net_Ready || !ps) continue;
			m_server->SendTo(l_pC->ID, P);
		};
		//---------------------------------------------------
		pClient->owner->set_name_replace(NewName);
		pClient->owner->set_name(NewName);
		if (strstr(pClient->Name.c_str(), "name="))
		{
			string1024 tmpName = "";
			strcpy(tmpName, pClient->Name.c_str());
			*(strstr(tmpName, "name=")+5) = 0;
			sprintf(tmpName, "%s%s", tmpName, NewName);
			char* ptmp = strstr(strstr(pClient->Name.c_str(), "name="), "/");
			if (ptmp)
				sprintf(tmpName, "%s%s", tmpName, ptmp);
			pClient->Name._set(tmpName);
		}
	};

	ps->setName(NewName);
	signal_Syncronize();
};

void	game_sv_mp::LoadRanks	()
{
	m_aRanks.clear();
	for (int i=0; ; i++)
	{
		string256 RankSect;
		sprintf(RankSect, "rank_%d",i);
		if (!pSettings->section_exist(RankSect)) break;
		Rank_Struct NewRank; 
		
		NewRank.m_sTitle = pSettings->r_string(RankSect, "rank_name");
		shared_str sTerms = pSettings->r_string(RankSect, "rank_exp");
		int TermsCount = _GetItemCount(sTerms.c_str());
		R_ASSERT2((TermsCount != 0 && TermsCount <= MAX_TERMS), "Error Number of Terms for Rank");

		for (int t =0; t<TermsCount; t++)
		{
			string16						temp;			
			NewRank.m_iTerms[t] = atoi(_GetItem(sTerms.c_str(), t, temp));
		}
		m_aRanks.push_back(NewRank);
	};
};

void	game_sv_mp::Player_AddExperience	(game_PlayerState* ps, float Exp)
{
	if (!ps) return;

	ps->experience_New += Exp;
	
	if (Player_Check_Rank(ps) && Player_RankUp_Allowed()) Player_Rank_Up(ps);	

	if (ps->rank==m_aRanks.size()-1) ps->experience_D = 1.0f;
	else
	{
		int CurExp = m_aRanks[ps->rank].m_iTerms[0];
		int NextExp = m_aRanks[ps->rank+1].m_iTerms[0];
		if ((ps->experience_Real+ps->experience_New) > NextExp) ps->experience_D = 1.0f;
		else ps->experience_D = 1.0f - (NextExp - ps->experience_Real- ps->experience_New)/(NextExp - CurExp);
		clamp(ps->experience_D, 0.0f, 1.0f);
	};
};

bool	game_sv_mp::Player_Check_Rank		(game_PlayerState* ps)
{
	if (!ps) return false;
	if (ps->rank==m_aRanks.size()-1) return false;
	int NextExp = m_aRanks[ps->rank+1].m_iTerms[0];
	if ((ps->experience_Real+ps->experience_New) < NextExp) return false;
	return true;
}

void	game_sv_mp::Player_Rank_Up		(game_PlayerState* ps)
{
	if (!ps) return;

	if (ps->rank==m_aRanks.size()-1) return;
	
	ps->rank++;
	Player_ExperienceFin(ps);
};

void	game_sv_mp::Player_ExperienceFin	(game_PlayerState* ps)
{
	if (!ps) return;
	ps->experience_Real += ps->experience_New;
	ps->experience_New = 0;
}