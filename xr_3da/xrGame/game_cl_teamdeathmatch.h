#pragma once
#include "game_cl_base.h"

class game_cl_TeamDeathmatch :public game_cl_GameState
{
typedef game_cl_GameState inherited;

protected:
	virtual			void				TranslateGameMessage	(u32 msg, NET_Packet& P);
public :
		virtual		LPCSTR				type_name				() const {return "teamdeathmatch";};

};
