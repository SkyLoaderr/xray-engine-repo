#include "stdafx.h"

game_cl_GameState::game_cl_GameState()
{
}

void	game_cl_GameState::Create			(LPCSTR options)
{
}

void	game_cl_GameState::net_import_state	(NET_Packet& P)
{
	// Generic
	P.r_s32			(type);
	P.r_u16			(phase);
	P.r_s32			(round);
	P.r_s32			(fraglimit);
	P.r_s32			(timelimit);

	// Teams
	u16 t_count;
	P.r_u16			(t_count);
	teams.clear		();
	for (u16 t_it=0; t_it<t_count; t_it++)
	{
		game_TeamState	ts;
		P.r				(&ts,sizeof(game_TeamState));
		teams.push_back	(ts);
	}

	// Players
	u16	p_count;
	P.r_u16			(p_count);
	players.clear	();
	for (u16 p_it=0; p_it<p_count; p_it++)
	{
		u32				ID;
		Player			IP;
		P.r_u32			(ID);
		P.r_string		(IP.name);
		P.r				(&IP,sizeof(game_PlayerState));
		players.insert	(make_pair(ID,IP));
	}
}

void	game_cl_GameState::net_import_update(NET_Packet& P)
{
	// Read
	u32					ID;
	game_PlayerState	PS;
	P.r_u32				(ID);
	P.r					(&PS,sizeof(game_PlayerState));

	// Update
	map<u32,Player>::iterator I	= players.find(ID);
	if (I!=players.end())
	{
		Player& IP	= I->second;
		CopyMemory	(&IP,&PS,sizeof(PS));
	}
}

void	game_cl_GameState::net_signal		(NET_Packet& P)
{
}
