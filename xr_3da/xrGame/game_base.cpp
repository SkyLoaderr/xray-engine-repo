#include "stdafx.h"
#include "game_base.h"


void game_sv_GameState::net_Export_State	(NET_Packet& P)
{
}

void game_sv_GameState::net_Export_Update	(NET_Packet& P, DWORD id)
{
	Lock		();
	game_PlayerState* A		= get_id		(id);
	if (A)
	{
		P->w_u32	(id);
		P->w		(A,sizeof(*A));
	}
	Unlock		();
}
