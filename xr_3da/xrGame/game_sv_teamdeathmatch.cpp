#include "stdafx.h"
#include "game_sv_teamdeathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"

void	game_sv_TeamDeathmatch::Create					(LPSTR &options)
{
	__super::Create					(options);
	fraglimit	= get_option_i		(options,"fraglimit",0);
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)
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
		ps->kills				=	0;
		ps->deaths				=	0;

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
	CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	if (!pA) return 0;

	return u32(pA->s_team);
};