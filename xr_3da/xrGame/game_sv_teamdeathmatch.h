#pragma once

#include "game_sv_deathmatch.h"

class	game_sv_TeamDeathmatch			: public game_sv_Deathmatch
{
private:
	typedef game_sv_Deathmatch inherited;
public:
	virtual		void				Create					(LPSTR &options);

	// Events	
	virtual		void				OnPlayerConnect			(u32 id_who);

	virtual		void				OnPlayerChangeTeam		(u32 id_who, s16 team);

	virtual		u8					AutoTeam				( );
};
