#pragma once

enum
{
	GAME_ANY							= 0,
	GAME_SINGLE							= 1,
	GAME_DEATHMATCH						= 2,
//	GAME_CTF							= 3,
//	GAME_ASSAULT						= 4,	// Team1 - assaulting, Team0 - Defending
	GAME_CS								= 5,
	GAME_TEAMDEATHMATCH					= 6,
	GAME_ARTEFACTHUNT					= 7,

	//identifiers in range [100...254] are registered for script game type
	GAME_DUMMY							= 255	// temporary game type
};

enum
{
	GAME_PLAYER_FLAG_LOCAL				= (1<<0),
	GAME_PLAYER_FLAG_READY				= (1<<1),
	GAME_PLAYER_FLAG_VERY_VERY_DEAD		= (1<<2),
	GAME_PLAYER_FLAG_CS_SPECTATOR		= (1<<3),

	GAME_PLAYER_FLAG_SCRIPT_BEGINS_FROM	= (1<<4),
	GAME_PLAYER_FLAG_FORCEDWORD			= u32(-1)
};

enum
{
	GAME_PHASE_NONE						= 0,
	GAME_PHASE_INPROGRESS,
	GAME_PHASE_PENDING,
	GAME_PHASE_TEAM1_SCORES,
	GAME_PHASE_TEAM2_SCORES,
	GAME_PHASE_TEAMS_IN_A_DRAW,

	GAME_PHASE_SCRIPT_BEGINS_FROM,
	GAME_PHASE_FORCEDWORD		= u32(-1)
};
