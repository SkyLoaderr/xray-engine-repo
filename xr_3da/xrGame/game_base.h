#pragma once

enum
{
	GAME_ANY			= 0,
	GAME_SINGLE			= 1,
	GAME_DEATHMATCH		= 2,
	GAME_CTF			= 3,
	GAME_ASSAULT		= 4,	// Team1 - assaulting, Team0 - Defending
	GAME_CS				= 5
};

#pragma pack(push,1)
struct	game_PlayerState
{
	s16			team;
	s16			kills;
	s16			deaths;
	s16			money_total;
	s16			money_for_round;

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

	s32								round;
	u32								fraglimit;
	u32								timelimit;
	vector<game_TeamState>			teams;
public:
	game_GameState();

	virtual		void				Create					(LPCSTR options)	=	0;
};
