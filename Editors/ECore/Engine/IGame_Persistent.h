#ifndef IGame_PersistentH
#define IGame_PersistentH
#pragma once

#include "Environment.h"
#ifndef _EDITOR
	#include "IGame_ObjectPool.h"
#endif

class IMainUI;
//-----------------------------------------------------------------------------------------------------------
class ENGINE_API IGame_Persistent	: 
#ifndef _EDITOR
	public DLL_Pure,
#endif
	public pureAppStart, 
	public pureAppEnd,
	public pureAppActivate, 
	public pureAppDeactivate,
	public pureFrame
{
public:
#ifndef _EDITOR
	IGame_ObjectPool				ObjectPool;
#endif
	IMainUI*						m_pMainUI;	

	CEnvironment					Environment;
	BOOL							bDedicatedServer	;

	virtual	void					OnAppStart			();
	virtual void					OnAppEnd			();
	virtual	void					OnAppActivate		();
	virtual void					OnAppDeactivate		();
	virtual void					OnFrame				();

	virtual void					RegisterModel		(IRender_Visual* V)
#ifndef _EDITOR
     = 0;
#else
	{}
#endif
	virtual	float					MtlTransparent		(u32 mtl_idx)
#ifndef _EDITOR
	= 0;
#else
	{return 1.f;}
#endif

	IGame_Persistent				();
	virtual ~IGame_Persistent		();

	virtual void					Statistics			(CGameFont* F)
#ifndef _EDITOR
     = 0;
#else
	{}
#endif
};

class IMainUI
{
public:
	virtual			~IMainUI						()							{};
	virtual void	Activate						(bool bActive)				=0; 
	virtual	bool	IsActive						()							=0; 
};

extern ENGINE_API	IGame_Persistent*	g_pGamePersistent;
#endif //IGame_PersistentH

