#include "stdafx.h"
#include "game_sv_teamdeathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"

void	game_sv_TeamDeathmatch::Create					(LPSTR &options)
{
	__super::Create					(options);
	fraglimit	= get_option_i		(options,"fraglimit",0);
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)
	int iFF = get_option_i(options,"friendlyfire",0);
	if (iFF != 0) m_fFriendlyFireModifier	= float(iFF) / 100.0f;
	else m_fFriendlyFireModifier = 0.000001f;

	switch_Phase(GAME_PHASE_PENDING);
///	switch_Phase(GAME_PHASE_INPROGRESS);
	
	teams.push_back(game_TeamState());
	teams.push_back(game_TeamState());

	teams[0].score			= 0;
	teams[0].num_targets	= 0;

	teams[1].score			= 0;
	teams[1].num_targets	= 0;
}

u8 game_sv_TeamDeathmatch::AutoTeam() 
{
	u32	cnt = get_count(), l_teams[2] = {0,0};
	for(u32 it=0; it<cnt; it++)	{
		game_PlayerState* ps = get_it(it);
		if(ps->team>=1) ++(l_teams[ps->team-1]);
	}
	return (l_teams[0]>l_teams[1])?2:1;
}

void game_sv_TeamDeathmatch::OnPlayerConnect	(u32 id_who)
{
	inherited::OnPlayerConnect	(id_who);

	game_PlayerState*	ps_who	=	get_id	(id_who);
	LPCSTR	options			=	get_name_id	(id_who);

	ps_who->team				=	u8(get_option_i(options,"team",AutoTeam()));
}

void game_sv_TeamDeathmatch::OnPlayerChangeTeam(u32 id_who, s16 team) 
{
	game_PlayerState*	ps_who	=	get_id	(id_who);
	if (!team) team = AutoTeam();
	if (!ps_who || ps_who->team == team) return;
	
	ps_who->team = team;
	Memory.mem_fill(ps_who->Slots, 0xff, sizeof(ps_who->Slots));
//	ps_who->kills--;
//	ps_who->deaths++;

	if (OnServer())
	{
		KillPlayer(id_who);
	};	

	xrClientData* xrCData	=	Level().Server->ID_to_client(id_who);
	char	pTeamName[2][1024] = {"Red Team", "Blue Team"};
	DWORD	pTeamColor[2] = {0xffff0000, 0xff0000ff};
	
	HUD().outMessage		(pTeamColor[team-1],"","%s has switched to %s",get_option_s(xrCData->Name,"name",xrCData->Name), pTeamName[team-1]);
		
	signal_Syncronize();
}

void	game_sv_TeamDeathmatch::OnRoundStart			()
{
	__super::OnRoundStart	();

	// Respawn all players and some info
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		ClearPlayerState(ps);

		SpawnActor(get_it_2_id(it), "spectator");
	}
}

void	game_sv_TeamDeathmatch::OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	if (!ps_killed || !ps_killer) return;

	ps_killed->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
	ps_killed->deaths				+=	1;
	if (ps_killer == ps_killed || ps_killed->team == ps_killer->team)	
	{
		// By himself
		ps_killer->kills			-=	1;
	} else {
		// Opponent killed - frag 
		ps_killer->kills			+=	1;
	}
	
	teams[ps_killer->team-1].score = 0;
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->team != ps_killer->team) continue;

		teams[ps_killer->team-1].score += ps->kills;
	};	

	if (fraglimit && (teams[ps_killer->team-1].score >= fraglimit) )OnFraglimitExceed();

	signal_Syncronize();
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

	CSE_Abstract*		a_hitter		= dynamic_cast <CSE_ALifeCreatureActor*> (e_hitter);
	CSE_Abstract*		a_hitted		= dynamic_cast <CSE_ALifeCreatureActor*> (e_hitted);

	if (!a_hitter || !a_hitted) return;

	game_PlayerState*	ps_hitter = &a_hitter->owner->ps;
	game_PlayerState*	ps_hitted = &a_hitted->owner->ps;
	
	if (ps_hitter == ps_hitted) return;

	if (ps_hitted->team != ps_hitter->team)
	{
	}
	else
	{
		//friendly fire case
		//---------------------------------------
		// read hit event
		u32 PowRPos, ImpRPos;

		Fvector			dir;
		float			power, impulse;
		s16				element;
		Fvector			position_in_bone_space;
		u16				hit_type;

		u32	RPos = P.r_pos;
		P.r_dir			(dir);						PowRPos = P.r_pos;
		P.r_float		(power);
		P.r_s16			(element);
		P.r_vec3		(position_in_bone_space);	ImpRPos = P.r_pos;
		P.r_float		(impulse);
		P.r_u16			(hit_type);	//hit type
		P.r_pos = RPos;

		power *= m_fFriendlyFireModifier;
		impulse *= m_fFriendlyFireModifier;

		u32 BCount = P.B.count;
		P.B.count	= PowRPos;	P.w_float(power);
		P.B.count	= ImpRPos;	P.w_float(impulse);
		P.B.count	= BCount;
		//---------------------------------------
	};
};

/*
void	game_sv_TeamDeathmatch::SetSkin					(CSE_Abstract* E, u16 Team, u16 ID)
{
	//-------------------------------------------
	CSE_Visual* pV = dynamic_cast<CSE_Visual*>(E);
	if (!E) return;
	
	switch (Team)
	{
	case 0:
		break;
	case 1:
		pV->set_visual("actors\\Different_stalkers\\soldat_beret.ogf");
		break;
	case 2:
		pV->set_visual("actors\\Different_stalkers\\stalker_black_mask.ogf");
		break;
	}	;
	//-------------------------------------------
};
*/
void	game_sv_TeamDeathmatch::LoadWeapons				()
{
	/////////////////////////////////////////////////////////////////////////
	//Loading Weapons List
	WPN_LISTS		DefaultTeam, Team1, Team2;

	LoadWeaponsForTeam				(&Team1, "teamdeathmatch_team1");
	LoadWeaponsForTeam				(&Team2, "teamdeathmatch_team2");

	wpnTeamsSectStorage.push_back(DefaultTeam);
	wpnTeamsSectStorage.push_back(Team1);
	wpnTeamsSectStorage.push_back(Team2);
	/////////////////////////////////////////////////////////////////////////
};

void	game_sv_TeamDeathmatch::LoadSkins				()
{
	/////////////////////////////////////////////////////////////////////////
	//Loading Skins List
	SKINS_NAMES		DefaultTeam, Team1, Team2;

	LoadSkinsForTeam				(&Team1, "teamdeathmatch_team1");
	LoadSkinsForTeam				(&Team2, "teamdeathmatch_team2");

	SkinsTeamSectStorage.push_back(DefaultTeam);
	SkinsTeamSectStorage.push_back(Team1);
	SkinsTeamSectStorage.push_back(Team2);
	/////////////////////////////////////////////////////////////////////////
};
