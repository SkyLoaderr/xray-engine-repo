#pragma once

class CGamePersistent				: public g_pGameLevel
{
public:
					CGamePersistent	();
					~CGamePersistent();

	virtual void	OnAppCycleStart	();
	virtual void	OnAppCycleEnd	();
};
