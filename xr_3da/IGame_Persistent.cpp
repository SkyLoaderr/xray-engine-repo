#include "stdafx.h"
#pragma hdrstop

#include "IGame_Persistent.h"
#include "environment.h"

ENGINE_API	IGame_Persistent*		g_pGamePersistent	= NULL;

IGame_Persistent::IGame_Persistent	()
{
	Device.seqAppCycleStart.Add		(this);
	Device.seqAppCycleEnd.Add		(this);
}

IGame_Persistent::~IGame_Persistent	()
{
	Device.seqAppCycleStart.Remove	(this);
	Device.seqAppCycleEnd.Remove	(this);
}

void IGame_Persistent::OnAppCycleStart()
{
#ifndef _EDITOR
	ObjectPool.load					();
#endif
	Environment.Load				();
}

void IGame_Persistent::OnAppCycleEnd()
{
#ifndef _EDITOR
	ObjectPool.unload				();
#endif
}
