#pragma once

class CGamePersistent: public pureAppCycleStart, public pureAppCycleEnd
{
public:
					CGamePersistent	();
					~CGamePersistent();

	virtual void	OnAppCycleStart	();
	virtual void	OnAppCycleEnd	();
};

extern CGamePersistent* GamePersistent;
