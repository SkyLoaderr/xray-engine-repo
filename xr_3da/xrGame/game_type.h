#pragma once

enum
{
	GAME_ANY			= 0,
	GAME_SINGLE			= 1,
	GAME_DEATHMATCH		= 2,
	GAME_CTF			= 3,
	GAME_ASSAULT		= 4,	// Team0 - assaulting, Team1 - Defending
	GAME_CS				= 5
};

enum
{
	GAME_FLAG_CHEATS	= (1<<0)
};


#ifndef _EDITOR
extern DWORD	GAME;
extern int		g_team;	
extern int		g_fraglimit;
extern int		g_timelimit;
extern DWORD	g_flags;
#endif

// Assault - team0 must capture target (setted as team1)
