#pragma once

struct	game_PlayerState
{
	u16			team;
	s16			kills;
	s16			deaths;
	s16			money_total;
	s16			money_for_round;
};

struct	game_TeamState
{
	u16			num_artifacts;
};

class	game_GameState
{
	u32								round;
	vector<game_TeamState>			teams;
public:
	// Main
	virtual		void				Lock					();
	virtual		void				Unlock					();
	virtual		game_PlayerState*	get_it					(DWORD it);
	virtual		game_PlayerState*	get_id					(DWORD id);
	virtual		u32					get_count				();

	// Utilities
	virtual		u32					get_alive_count			(u32 team);

	// Events
	virtual		void				OnStartRound			();							// старт раунда
	virtual		void				OnTeamScore				(u32 team);					// команда выиграла
	virtual		void				OnTeamsInDraw			();							// ничья
	virtual		void				OnTargetTouched			(u32 id_who, u32 id_target);
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);
	virtual		void				OnTimeElapsed			();

	// Main
	virtual		void				Update					();
	virtual		void				net_Export_Update		(NET_Packet& P);
	virtual		void				net_Import_Update		(NET_Packet& P);
	virtual		void				net_Export_Update		(NET_Packet& P);
	virtual		void				net_Import_Update		(NET_Packet& P);
};

class cl_GameState
{
public:
	string64	name;
	struct Player
	map<DWORD,Item>	players;
public:
	void			p_add		(NET_Packet& P	);
	void			p_remove	(NET_Packet& P	);
	Item*			p_access	(DWORD ID		);

	void			

	CGameState		(void);
	~CGameState		(void);
};
