#pragma once

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
