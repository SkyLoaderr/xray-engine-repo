#include "stdafx.h"
#include "game_sv_cs.h"

game_sv_CS::game_sv_CS()
{
	round	= -1;
}

void	game_sv_CS::Create			(LPCSTR options)
{
}

void	game_sv_CS::OnRoundStart	()
{
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
	// Artifacts count
	// if ()
}

BOOL	game_sv_CS::OnTargetTouched	(u32 id_who, u32 eid_target)
{
	return FALSE;
}

void	game_sv_CS::Update			()
{
}
