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
	public pureFrame,
    public pureDeviceCreate,
    public pureDeviceDestroy
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

	virtual void					RegisterModel	(IRender_Visual* V)
#ifndef _EDITOR
     = 0;
#else
	{}
#endif

	IGame_Persistent				();
	virtual ~IGame_Persistent		();

    void							OnDeviceCreate	();
    void							OnDeviceDestroy	();
};

extern ENGINE_API	IGame_Persistent*	g_pGamePersistent;
#endif //IGame_PersistentH


