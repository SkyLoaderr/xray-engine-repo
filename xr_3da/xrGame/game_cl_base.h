#pragma once

#include "game_base.h"

class CGameObject;
class	game_cl_GameState	: public game_GameState
{
public:
	struct Player : public game_PlayerState 
	{
		string64	name;
	};
	map<u32,Player>			players;
	Player*					local_player;
	vector<CGameObject*>	targets;
public:
	virtual		void				Create					(LPCSTR options);
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_signal				(NET_Packet& P);
};
