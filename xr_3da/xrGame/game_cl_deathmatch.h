#pragma once
#include "game_cl_base.h"

class game_cl_Deathmatch :public game_cl_GameState
{
typedef game_cl_GameState inherited;

protected:
	virtual			void				TranslateGameMessage	(u32 msg, NET_Packet& P);

public :
	s32								fraglimit; //dm,tdm,ah
	s32								timelimit; //dm
	u32								damageblocklimit;//dm,tdm
	xr_vector<game_TeamState>		teams;//dm,tdm,ah

	virtual		CUIGameCustom*		createGameUI			();
	virtual		void				net_import_state	(NET_Packet& P);

};
