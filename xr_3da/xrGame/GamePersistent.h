#ifndef GamePersistentH
#define GamePersistentH
#pragma once

#include "..\IGame_Persistent.h"

class CGamePersistent				: 
	public IGame_Persistent, 
	public IEventReceiver
{
public:
	IReader*			pDemoFile;
	u32					uTime2Change;
	EVENT				eDemoStart;

						CGamePersistent			();
						~CGamePersistent		();

	virtual void		OnAppCycleStart			();
	virtual void		OnAppCycleEnd			();
	virtual void		OnFrame					();
	virtual void		OnEvent					(EVENT E, u64 P1, u64 P2);
};

#endif //GamePersistentH

