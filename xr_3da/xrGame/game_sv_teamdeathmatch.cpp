#include "stdafx.h"
#include "game_sv_teamdeathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrserver.h"
#include "level.h"

void	game_sv_TeamDeathmatch::Create					(ref_str& options)
{
	inherited::Create					(options);
//	fraglimit	= get_option_i		(*options,"fraglimit",0);
//	timelimit	= get_option_i		(*options,"timelimit",0)*60000;	// in (ms)
	int iFF = get_option_i(*options,"ffire",100);
	if (iFF != 0) m_fFriendlyFireModifier	= float(iFF) / 100.0f;
	else m_fFriendlyFireModifier = 0.000001f;
	
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
		game_PlayerState* ps = get_it(it);
		if (ps->Skip) continue;
		if(ps->team>=1) ++(l_teams[ps->team-1]);
	}
	return (l_teams[0]>l_teams[1])?2:1;
}

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
	
	KillPlayer(id_who);
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
	};


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
			Player_AddMoney(ps_killer, pTeam->m_iM_KillRival);
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

//	if (fraglimit && (teams[ps_killer->team-1].score >= fraglimit ) )OnFraglimitExceed();

	signal_Syncronize();
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
	/*
	CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	if (!pA) return 0;

	return u32(pA->s_team);
	*/
};


void	game_sv_TeamDeathmatch::OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P)
{
	CSE_Abstract*		e_hitter		= get_entity_from_eid	(id_hitter	);
	CSE_Abstract*		e_hitted		= get_entity_from_eid	(id_hitted	);

	if (!e_hitter || !e_hitted) return;

	CSE_ALifeCreatureActor*		a_hitter		= dynamic_cast <CSE_ALifeCreatureActor*> (e_hitter);
	CSE_ALifeCreatureActor*		a_hitted		= dynamic_cast <CSE_ALifeCreatureActor*> (e_hitted);

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
	if (Device.dwTimeGlobal<ps_hitted->RespawnTime + damageblocklimit)
	{
		power = 0;
		impulse = 0;
	}
	//---------------------------------------
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


