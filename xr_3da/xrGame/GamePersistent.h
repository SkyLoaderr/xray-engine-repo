#ifndef GamePersistentH
#define GamePersistentH
#pragma once

#include "../IGame_Persistent.h"
class CMainUI;
class CUICursor;

class CGamePersistent: 
	public IGame_Persistent, 
	public IEventReceiver
{

public:
	IReader*			pDemoFile;
	u32					uTime2Change;
	EVENT				eDemoStart;

						CGamePersistent			();
	virtual				~CGamePersistent		();

	virtual void		OnAppStart				();
	virtual void		OnAppEnd				();
	virtual void		OnFrame					();
	virtual void		OnEvent					(EVENT E, u64 P1, u64 P2);

	virtual void		RegisterModel			(IRender_Visual* V);
	virtual	float		MtlTransparent			(u32 mtl_idx);
	virtual	void		Statistics				(CGameFont* F);
};

#endif //GamePersistentH

