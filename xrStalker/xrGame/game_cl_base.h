#pragma once

#include "game_base.h"

class	CGameObject;

class	game_cl_GameState	: public game_GameState
{
public:
	struct Player : public game_PlayerState 
	{
		string64	name;
	};
	xr_map<u32,Player>				players;
	Player*							local_player;
	xr_vector<CGameObject*>			targets;
public:
									game_cl_GameState		();
	virtual							~game_cl_GameState		();

	virtual		void				Create					(LPSTR &options);
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_signal				(NET_Packet& P);
};
