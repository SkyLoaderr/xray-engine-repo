#pragma once
#include "game_cl_base.h"

class game_cl_Single :public game_cl_GameState
{
public :
		virtual		LPCSTR				type_name				() const {return "single";};

};


