#include "stdafx.h"
#include "game_sv_teamdeathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrserver.h"

void	game_sv_TeamDeathmatch::Create					(shared_str& options)
{
	inherited::Create					(options);
//	fraglimit	= get_option_i		(*options,"fraglimit",0);
//	timelimit	= get_option_i		(*options,"timelimit",0)*60000;	// in (ms)
	m_bAutoTeamBalance	= get_option_i(*options, "abalance") != 0;
	
	switch_Phase(GAME_PHASE_PENDING);
///	switch_Phase(GAME_PHASE_INPROGRESS);
	
	game_TeamState td;
	td.score		= 0;
	td.num_targets	= 0;
	teams.push_back(td);
	teams.push_back(td);
}

u8 game_sv_TeamDeathmatch::AutoTeam() 
{
	u32	cnt = get_players_count(), l_teams[2] = {0,0};
	for(u32 it=0; it<cnt; it++)	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC->net_Ready) continue;
		if (ps->Skip) continue;
		if(ps->team>=1) ++(l_teams[ps->team-1]);
	}
	return (l_teams[0]>l_teams[1])?2:1;
}

void	game_sv_TeamDeathmatch::AutoBalanceTeams()
{
	if (!m_bAutoTeamBalance) return;
	//calc team count
	s16 MinTeam, MaxTeam;
	u32 NumToMove;
	u32	cnt = get_players_count(), l_teams[2] = {0,0};
	for(u32 it=0; it<cnt; it++)	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC->net_Ready) continue;
		if (ps->Skip) continue;
		if(ps->team>=1) ++(l_teams[ps->team-1]);
	};

	if (l_teams[0] == l_teams[1]) return;

	if (l_teams[0] > l_teams[1]) 
	{
		MinTeam = 1;
		MaxTeam = 0;
	}
	else
	{
		MinTeam = 0;
		MaxTeam = 1;
	};

	NumToMove = (l_teams[MaxTeam] - l_teams[MinTeam]) / 2;
	if (!NumToMove) return;
	///////////////////////////////////////////////////////////////////////
	while (NumToMove)
	{
		///////// get lowest score player from MaxTeam
		u32 LowestPlayer = 0;
		s16 LowestScore = 32767;
		for(it=0; it<cnt; it++)	{
			xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
			game_PlayerState* ps	= l_pC->ps;
			if (!l_pC->net_Ready) continue;
			if (ps->Skip) continue;
			if (ps->team-1 != MaxTeam) continue;

			if (ps->kills < LowestScore)
			{
				LowestScore = ps->kills;
				LowestPlayer = it;
			};
		};
		///////// move player to opposite team
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(LowestPlayer);
		game_PlayerState* ps	= l_pC->ps;
		ps->team = MinTeam+1;
		NumToMove--;
	}
};

void	game_sv_TeamDeathmatch::OnRoundStart			()
{
	AutoBalanceTeams();

	inherited::OnRoundStart	();
};

void game_sv_TeamDeathmatch::OnPlayerConnect	(ClientID id_who)
{
	inherited::OnPlayerConnect	(id_who);

	game_PlayerState*	ps_who	=	get_id	(id_who);
	LPCSTR	options				=	get_name_id	(id_who);
	ps_who->team				=	u8(get_option_i(options,"team",AutoTeam()));


	if (ps_who->Skip) return;


	xrClientData* xrCData		=	m_server->ID_to_client(id_who);
	// Send Message About Client join Team
	if (xrCData)
	{
		NET_Packet			P;
//		P.w_begin			(M_GAMEMESSAGE);
		GenerateGameMessage (P);
		P.w_u32				(GAME_EVENT_PLAYER_JOIN_TEAM);
		P.w_stringZ			(get_option_s(*xrCData->Name,"name",*xrCData->Name));
		P.w_u16				(ps_who->team);
		u_EventSend(P);
	};

	Money_SetStart(id_who);
	SetPlayersDefItems(ps_who);
}

void game_sv_TeamDeathmatch::OnPlayerChangeTeam(ClientID id_who, s16 team) 
{
	game_PlayerState*	ps_who	=	get_id	(id_who);
	if (!team) team				= AutoTeam();
	if (!ps_who || ps_who->team == team) return;
	
	KillPlayer(id_who, ps_who->GameID);
/////////////////////////////////////////////////////////
	//Send Switch team message
	NET_Packet			P;
//	P.w_begin			(M_GAMEMESSAGE);
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_PLAYER_CHANGE_TEAM);
	P.w_u16				(ps_who->GameID);
	P.w_u16				(ps_who->team);
	P.w_u16				(team);
	u_EventSend(P);
/////////////////////////////////////////////////////////
	ps_who->team = team;
	
	ClearPlayerItems(ps_who);
	SetPlayersDefItems(ps_who);
}


void	game_sv_TeamDeathmatch::OnPlayerKillPlayer		(ClientID id_killer, ClientID id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);

	if(ps_killed){
		ps_killed->setFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
		ps_killed->deaths				+=	1;
		ps_killed->DeathTime			= Device.dwTimeGlobal;
	};

	signal_Syncronize();

	if (!ps_killed || !ps_killer) return;

	TeamStruct* pTeam		= GetTeamData(u8(ps_killer->team));

	if (ps_killer == ps_killed || ps_killed->team == ps_killer->team)	
	{
		// By himself
		ps_killer->kills			-=	1;

		if (pTeam)
		{
			if (ps_killer == ps_killed)
				Player_AddMoney(ps_killer, pTeam->m_iM_KillSelf);
			else
				Player_AddMoney(ps_killer, pTeam->m_iM_KillTeam);
		}
	} else {
		// Opponent killed - frag 
		ps_killer->kills			+=	1;

		if (pTeam)
		{
			s16 ResMoney = pTeam->m_iM_KillRival;
			if (ps_killer->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))
				ResMoney = s16(ResMoney * pTeam->m_fInvinsibleKillModifier);
			Player_AddMoney(ps_killer, ResMoney);
		};
	}
	
//	teams[ps_killer->team-1].score = 0;
	SetTeamScore(ps_killer->team-1, 0);
	u32		cnt = get_players_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->team != ps_killer->team) continue;

//		teams[ps_killer->team-1].score += ps->kills;
		SetTeamScore(ps_killer->team-1, GetTeamScore(ps_killer->team-1)+ps->kills );
	};	

	// Send Message About Player Killed
	SendPlayerKilledMessage(id_killer, id_killed);

	ps_killed->lasthitter			= 0;
	ps_killed->lasthitweapon		= 0;
	ClearPlayerItems		(ps_killed);
	SetPlayersDefItems		(ps_killed);
}

bool game_sv_TeamDeathmatch::checkForFragLimit()
{
	if (fraglimit && ((teams[0].score >= fraglimit )||(teams[1].score >= fraglimit ) ) ){
		OnFraglimitExceed();
		return true;
	};
	return false;
}

u32		game_sv_TeamDeathmatch::RP_2_Use				(CSE_Abstract* E)
{
	return 0;
};


void	game_sv_TeamDeathmatch::OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P)
{
	CSE_Abstract*		e_hitter		= get_entity_from_eid	(id_hitter	);
	CSE_Abstract*		e_hitted		= get_entity_from_eid	(id_hitted	);

	if (!e_hitter || !e_hitted) return;

	CSE_ALifeCreatureActor*		a_hitter		= smart_cast <CSE_ALifeCreatureActor*> (e_hitter);
	CSE_ALifeCreatureActor*		a_hitted		= smart_cast <CSE_ALifeCreatureActor*> (e_hitted);

	if (!a_hitter || !a_hitted) return;

	game_PlayerState*	ps_hitter = a_hitter->owner->ps;
	game_PlayerState*	ps_hitted = a_hitted->owner->ps;

	u32 BCount = P.B.count;
	//---------------------------------------
	// read hit event
	u32 PowRPos, ImpRPos;

	u16				WeaponID;
	Fvector			dir;
	float			power, impulse;
	s16				element;
	Fvector			position_in_bone_space;
	u16				hit_type;

	u32	RPos = P.r_pos;
	P.r_u16			(WeaponID);
	P.r_dir			(dir);						PowRPos = P.r_pos;
	P.r_float		(power);
	P.r_s16			(element);
	P.r_vec3		(position_in_bone_space);	ImpRPos = P.r_pos;
	P.r_float		(impulse);
	P.r_u16			(hit_type);	//hit type
	P.r_pos = RPos;
	//---------------------------------------
	if (ps_hitted->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))//Device.dwTimeGlobal<ps_hitted->RespawnTime + damageblocklimit)
	{
		power = 0;
		impulse = 0;
	}
	//---------------------------------------
	if (ps_hitted != ps_hitter)
	{	
		if (ps_hitted->team != ps_hitter->team)
		{
		}
		else
		{
			//---------------------------------------
			//friendly fire case

			power *= m_fFriendlyFireModifier;
			impulse *= m_fFriendlyFireModifier;
		};
	};
	//---------------------------------------
	P.B.count	= PowRPos;	P.w_float(power);
	P.B.count	= ImpRPos;	P.w_float(impulse);
	//---------------------------------------
	if (power > 0)
	{
		ps_hitted->lasthitter = a_hitter->ID;
		ps_hitted->lasthitweapon = WeaponID;
	};
	//---------------------------------------
	P.B.count	= BCount;
};

void	game_sv_TeamDeathmatch::LoadTeams			()
{
	m_sBaseWeaponCostSection._set("teamdeathmatch_base_cost");
	if (!pSettings->section_exist(m_sBaseWeaponCostSection))
	{
		R_ASSERT2(0, "No section for base weapon cost for this type of the Game!");
		return;
	};

	LoadTeamData("teamdeathmatch_team0");
	LoadTeamData("teamdeathmatch_team1");
	LoadTeamData("teamdeathmatch_team2");
};

void	game_sv_TeamDeathmatch::Update					()
{
	inherited::Update	();
	switch(Phase()) 	
	{
	case GAME_PHASE_TEAM1_SCORES :
	case GAME_PHASE_TEAM2_SCORES :
	case GAME_PHASE_TEAMS_IN_A_DRAW :
		{
			if(m_delayedRoundEnd && m_roundEndDelay < Device.TimerAsync())
				OnRoundEnd("Finish");
		} break;
	};
}
bool	game_sv_TeamDeathmatch::HasChampion				()
{
	return GetTeamScore(0)!=GetTeamScore(1);
}

void	game_sv_TeamDeathmatch::OnTimelimitExceed		()
{
	u8 winning_team = (GetTeamScore(0) < GetTeamScore(1))? 1 : 0;
	OnTeamScore(winning_team, false);
	phase = u16((winning_team)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
	switch_Phase		(phase);
	OnDelayedRoundEnd	("TIME_limit");
}
void	game_sv_TeamDeathmatch::OnFraglimitExceed		()
{
	u8 winning_team = (GetTeamScore(0) < GetTeamScore(1))? 1 : 0;
	OnTeamScore(winning_team, false);
	phase = u16((winning_team)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
	switch_Phase		(phase);
	OnDelayedRoundEnd	("FRAG_limit");
}