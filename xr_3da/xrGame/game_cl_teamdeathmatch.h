#pragma once
#include "game_cl_deathmatch.h"

class game_cl_TeamDeathmatch :public game_cl_Deathmatch
{
typedef game_cl_Deathmatch inherited;

protected:
	virtual			void				TranslateGameMessage	(u32 msg, NET_Packet& P);
public :
	virtual			CUIGameCustom*		createGameUI			();
	virtual			void				GetMapEntities(xr_vector<SZoneMapEntityData>& dst);

};
