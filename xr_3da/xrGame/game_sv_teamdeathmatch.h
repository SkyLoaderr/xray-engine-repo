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
	virtual		void				OnRoundStart			();							// ����� ������
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);

	virtual		u8					AutoTeam				( );
	virtual		u32					RP_2_Use				(CSE_Abstract* E);
};
