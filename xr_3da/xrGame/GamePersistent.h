#pragma once

class CGamePersistent				: public IGame_Persistant
{
public:
					CGamePersistent	();
					~CGamePersistent();

	virtual void	OnAppCycleStart	();
	virtual void	OnAppCycleEnd	();
};
