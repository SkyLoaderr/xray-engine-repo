#include "stdafx.h"
#include "game_sv_ArtefactHunt.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"

void	game_sv_ArtefactHunt::Create					(LPSTR &options)
{
	inherited::Create					(options);
}

void	game_sv_ArtefactHunt::OnRoundStart			()
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

void	game_sv_ArtefactHunt::OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)
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

u32		game_sv_ArtefactHunt::RP_2_Use				(CSE_Abstract* E)
{
	CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	if (!pA) return 0;

	return u32(pA->g_team());
};


void	game_sv_ArtefactHunt::OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P)
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

void	game_sv_ArtefactHunt::LoadWeapons				()
{
	/////////////////////////////////////////////////////////////////////////
	//Loading Weapons List
	WPN_LISTS		DefaultTeam, Team1, Team2;

	LoadWeaponsForTeam				(&Team1, "artefacthunt_team1");
	LoadWeaponsForTeam				(&Team2, "artefacthunt_team2");

	wpnTeamsSectStorage.push_back(DefaultTeam);
	wpnTeamsSectStorage.push_back(Team1);
	wpnTeamsSectStorage.push_back(Team2);
	/////////////////////////////////////////////////////////////////////////
};

void	game_sv_ArtefactHunt::LoadSkins				()
{
	/////////////////////////////////////////////////////////////////////////
	//Loading Skins List
	SkinsStruct		DefaultTeam, Team1, Team2;

	LoadSkinsForTeam				(&Team1, "artefacthunt_team1");
	LoadSkinsForTeam				(&Team2, "artefacthunt_team2");

	SkinsTeamSectStorage.push_back(DefaultTeam);
	SkinsTeamSectStorage.push_back(Team1);
	SkinsTeamSectStorage.push_back(Team2);
	/////////////////////////////////////////////////////////////////////////
};
