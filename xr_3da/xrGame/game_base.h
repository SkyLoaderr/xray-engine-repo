#pragma once

enum
{
	GAME_ANY					= 0,
	GAME_SINGLE					= 1,
	GAME_DEATHMATCH				= 2,
	GAME_CTF					= 3,
	GAME_ASSAULT				= 4,	// Team1 - assaulting, Team0 - Defending
	GAME_CS						= 5
};

enum
{
	GAME_PLAYER_FLAG_LOCAL				= (1<<0),
	GAME_PLAYER_FLAG_READY				= (1<<1),
	GAME_PLAYER_FLAG_CS_ON_BASE			= (1<<2),
	GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE	= (1<<3),
	GAME_PLAYER_FLAG_CS_HAS_ARTEFACT	= (1<<4),

	GAME_PLAYER_FLAG_FORCEDWORD			= u32(-1)
};

enum
{
	GAME_PHASE_INPROGRESS		= 0,
	GAME_PHASE_PENDING,

	GAME_PHASE_FORCEDWORD		= u32(-1)
};

#pragma pack(push,1)
struct	game_PlayerState
{
	s16			team;
	s16			kills;
	s16			deaths;
	s16			money_total;
	s16			money_for_round;
	u16			flags;

	game_PlayerState();
};

struct	game_TeamState
{
	u16			score;
	u16			num_targets;

	game_TeamState();
};
#pragma pack(pop)

class	game_GameState
{
public:
	s32								type;
	u16								phase;
	s32								round;
	u32								start_time;
	u32								buy_time;
	s32								fraglimit;
	s32								timelimit;
	vector<game_TeamState>			teams;
public:
	game_GameState();

	virtual		void				Create					(LPCSTR options)	=	0;
};
