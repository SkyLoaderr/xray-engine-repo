#include "stdafx.h"
#include "game_sv_cs.h"

game_sv_CS::game_sv_CS()
{
	round	= -1;
}

void	game_sv_CS::Create			(LPCSTR options)
{
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)
}

void	game_sv_CS::OnRoundStart	()
{
	__super::OnRoundStart	();
	round	++;
	if (0==round)	
	{
		// give $1000 to everybody
		Lock	();
		u32		cnt = get_count();
		for		(u32 it=0; it<cnt; it++)	
		{
			game_PlayerState*	ps	=	get_it	(it);
			ps->money_total			=	1000;
			ps->money_for_round		=	0;
		}
		Unlock	();
	} else {
		// sum-up money for round with total
		Lock	();
		u32		cnt = get_count();
		for		(u32 it=0; it<cnt; it++)	
		{
			game_PlayerState*	ps	=	get_it	(it);
			ps->money_total			=	ps->money_total + ps->money_for_round;
			ps->money_for_round		=	0;
		}
		Unlock	();
	}
}

void	game_sv_CS::OnRoundEnd		()
{
}

void	game_sv_CS::OnTeamScore		(u32 team)
{
	// Increment/decrement money
	Lock	();
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; it++)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->money_for_round		+=	(s32(team)==ps->team)?+2000:+500;
	}
	Unlock	();
}

void	game_sv_CS::OnTeamsInDraw	()
{
	// Give $1000 to everybody
	Lock	();
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; it++)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->money_for_round		+=	+1000;
	}
	Unlock	();
}

void	game_sv_CS::OnPlayerKillPlayer	(u32 id_killer, u32 id_killed)
{
	Lock	();
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	ps_killed->deaths				+=	1;
	if (ps_killer->team == ps_killed->team)	
	{
		// Teammate killed - no frag, chop money
		ps_killer->money_for_round	-=	500;
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

void	game_sv_CS::OnTimelimitExceed	()
{
	Lock	();
	// Artifacts count
	// if ()
	// ���� � ������ ������� ����������, �� �����.
	R_ASSERT(teams.size() == 2);
	if(teams[0].num_targets == teams[1].num_targets) OnTeamsInDraw();
	else OnTeamScore((teams[0].num_targets > teams[1].num_targets) ? 0 : 1);
	Unlock	();
}

BOOL	game_sv_CS::OnTargetTouched	(u32 id_who, u32 eid_target)
{
	return FALSE;
}

void	game_sv_CS::Update			()
{
	if ((Device.TimerAsync()-start_time)>timelimit)
		OnTimelimitExceed	();
}

void	game_sv_Deathmatch::OnPlayerReady			(u32 id)
{
	if	(GAME_PHASE_INPROGRESS == phase) return;

	Lock	();
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
