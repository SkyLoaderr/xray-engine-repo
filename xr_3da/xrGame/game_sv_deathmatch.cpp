#include "stdafx.h"
#include "game_sv_deathmatch.h"

void	game_sv_Deathmatch::Create					(LPCSTR options)
{
	fraglimit	= get_option_i		(options,"fraglimit",0);
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)
	phase		= GAME_PHASE_PENDING;
}

void	game_sv_Deathmatch::OnRoundStart			()
{
	__super::OnRoundStart	();
}

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
		if (fraglimit && (ps_killer->kills > fraglimit) )OnFraglimitExceed();
	}
	Unlock	();
}

void	game_sv_Deathmatch::OnTimelimitExceed		()
{
	OnRoundEnd	("TIME_limit");
}
void	game_sv_Deathmatch::OnFraglimitExceed		()
{
	OnRoundEnd	("FRAG_limit");
}
void	game_sv_Deathmatch::Update					()
{
	if (timelimit)
	{
		if ((Device.TimerAsync()-start_time)>u32(timelimit))
			OnTimelimitExceed	();
	}
}
BOOL	game_sv_Deathmatch::OnTargetTouched			(u32 id_who, u32 eid_target)
{
	return TRUE;
}
void	game_sv_Deathmatch::OnPlayerReady			(u32 id)
{
	if	(GAME_PHASE_INPROGRESS == phase) return;

	Lock	();
	Msg		("--- Player '%s' ready-switch",get_name_id(id));
	game_PlayerState*	ps	=	get_id	(id);
	if (ps)
	{
		if (ps->flags & GAME_PLAYER_FLAG_READY)	
		{
			ps->flags &= ~GAME_PLAYER_FLAG_READY;
		} else {
			ps->flags |= GAME_PLAYER_FLAG_READY;

			// Check if all players ready
			u32		cnt		= get_count	();
			u32		ready	= 0;
			for		(u32 it=0; it<cnt; it++)	
			{
				ps		=	get_it	(it);
				if (ps->flags & GAME_PLAYER_FLAG_READY)	ready++;
			}

			if (ready == cnt)
			{
				OnRoundStart	();
			}
		}
	}
	Unlock	();
}
