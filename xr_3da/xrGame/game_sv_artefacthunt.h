#pragma once

#include "game_sv_teamdeathmatch.h"

class	game_sv_ArtefactHunt			: public game_sv_TeamDeathmatch
{
private:
	typedef game_sv_TeamDeathmatch inherited;

protected:
	float		m_fFriendlyFireModifier;
public:
	virtual		void				Create					(LPSTR &options);

	// Events	
	virtual		void				OnRoundStart			();							// старт раунда
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);

	virtual		u32					RP_2_Use				(CSE_Abstract* E);

	virtual		void				LoadTeams				();

	virtual		void				OnPlayerBuyFinished		(u32 id_who, NET_Packet& P);
	
	bool							IsBuyableItem			(CSE_Abstract* pItem);
	BOOL							CheckUpgrades			(CSE_Abstract* pItem, u8 IItem);	
	void							RemoveItemFromActor		(CSE_Abstract* pItem);

};
