#include "stdafx.h"
#include "game_base.h"

game_PlayerState::game_PlayerState()
{
	team				=	-1;
	kills				=	0;
	deaths				=	0;
	money_total			=	0;
	money_for_round		=	0;
}

game_TeamState::game_TeamState()
{
	score				=	0;
	num_targets			=	0;
}
