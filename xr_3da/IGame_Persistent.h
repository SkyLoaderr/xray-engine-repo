#ifndef IGame_PersistentH
#define IGame_PersistentH
#pragma once

#ifdef _EDITOR
	#include "Environment.h"
#else
	#include "IGame_ObjectPool.h"
#endif

// refs
class ENGINE_API CEnvironment;

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API IGame_Persistent	: 
#ifndef _EDITOR
	public DLL_Pure,
#endif
	public pureAppCycleStart, 
	public pureAppCycleEnd
{
public:
#ifndef _EDITOR
	IGame_ObjectPool				ObjectPool;
#endif
	CEnvironment					Environment;

	virtual	void					OnAppCycleStart	();
	virtual void					OnAppCycleEnd	();

	IGame_Persistent				();
	virtual ~IGame_Persistent		();
};

extern ENGINE_API	IGame_Persistent*	g_pGamePersistent;
#endif //IGame_PersistentH

