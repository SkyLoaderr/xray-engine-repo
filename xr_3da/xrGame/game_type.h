#pragma once

enum
{
	GAME_SINGLE,
	GAME_DEATHMATCH,
	GAME_CTF,
	GAME_ASSAULT
};

extern DWORD	GAME;
extern int		g_fraglimit;
extern int		g_timelimit;
