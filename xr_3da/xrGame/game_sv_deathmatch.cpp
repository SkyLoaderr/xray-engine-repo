#include "stdafx.h"
#include "game_sv_deathmatch.h"

void	game_sv_Deathmatch::OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)
{
	Lock	();
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	ps_killed->deaths				+=	1;
	if (ps_killer == ps_killed)	
	{
		// By himself
		ps_killer->kills			-=	1;
	} else {
		// Opponent killed - frag + money
		ps_killer->money_for_round	+=	500;
		ps_killer->kills			+=	1;

		// Check if there is no opponents left
		u32 alive					=	get_alive_count	(ps_killed->team);
		if (0==alive)				OnTeamScore(ps_killer->team);
	}
	Unlock	();
}
void	game_sv_Deathmatch::OnTimeElapsed			()
{
}
void	game_sv_Deathmatch::Update					()
{
}
