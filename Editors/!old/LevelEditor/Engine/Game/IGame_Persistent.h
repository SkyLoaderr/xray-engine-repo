#ifndef IGame_PersistentH
#define IGame_PersistentH
#pragma once

#include "Environment.h"
#ifndef _EDITOR
	#include "IGame_ObjectPool.h"
#endif

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API IGame_Persistent	: 
#ifndef _EDITOR
	public DLL_Pure,
#endif
	public pureAppCycleStart, 
	public pureAppCycleEnd,
	public pureFrame
{
public:
#ifndef _EDITOR
	IGame_ObjectPool				ObjectPool;
#endif
	CEnvironment					Environment;
	BOOL							bDedicatedServer	;

	virtual	void					OnAppCycleStart		();
	virtual void					OnAppCycleEnd		();
	virtual void					OnFrame				();

	virtual void					RegisterModel		(IRender_Visual* V)
#ifndef _EDITOR
     = 0;
#else
	{}
#endif

	IGame_Persistent				();
	virtual ~IGame_Persistent		();

    virtual void					OnDeviceCreate		();
    virtual void					OnDeviceDestroy		();
	virtual void					Statistics			(CGameFont* F)
#ifndef _EDITOR
     = 0;
#else
	{}
#endif
};

extern ENGINE_API	IGame_Persistent*	g_pGamePersistent;
#endif //IGame_PersistentH

