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
		// Opponent killed - frag 
		ps_killer->kills			+=	1;
	}
	Unlock	();
}
void	game_sv_Deathmatch::OnTimeElapsed			()
{
}
void	game_sv_Deathmatch::Update					()
{
}
