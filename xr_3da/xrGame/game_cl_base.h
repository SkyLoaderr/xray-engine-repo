#pragma once

#include "game_base.h"

class	game_cl_GameState	: public game_GameState
{
public:
	struct Player : public game_PlayerState 
	{
		string64	name;
	};
	map<DWORD,Player>		players;
public:
	virtual		void				Create					(LPCSTR options);
};
