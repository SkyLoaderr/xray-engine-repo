#pragma once

#include "IGame_ObjectPool.h"
#include "environment.h"

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API IGame_Persistant	: 
	public DLL_Pure,
	public pureAppCycleStart, 
	public pureAppCycleEnd
{
public:
	IGame_ObjectPool				ObjectPool;
	CEnvironment					Environment;

	virtual	void					OnAppCycleStart	();
	virtual void					OnAppCycleEnd	();

	IGame_Persistant				();
	virtual ~IGame_Persistant		();
};

extern ENGINE_API	IGame_Persistant*	g_pGamePersistent;
