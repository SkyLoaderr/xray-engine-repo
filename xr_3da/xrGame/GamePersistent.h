#pragma once

class CGamePersistent				: 
	public IGame_Persistant, 
	public pureFrame, 
	public IEventReceiver
{
public:
	IReader*		pDemoFile;
	u32				uTime2Change;
	EVENT			eDemoStart;

					CGamePersistent	();
					~CGamePersistent();

	virtual void	OnAppCycleStart	();
	virtual void	OnAppCycleEnd	();
	virtual void	OnFrame			();
	virtual void	OnEvent			(EVENT E, u64 P1, u64 P2);
};
