#pragma once
#include "game_cl_base.h"

class game_cl_Single :public game_cl_GameState
{
public :
				game_cl_Single();
	virtual			CUIGameCustom*		createGameUI			();
	virtual			char*				getTeamSection			(int Team);
};



