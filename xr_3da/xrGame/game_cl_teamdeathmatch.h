#pragma once
#include "game_cl_base.h"

class game_cl_TeamDeathmatch :public game_cl_GameState
{
public :
		virtual		LPCSTR				type_name				() const {return "teamdeathmatch";};

};
