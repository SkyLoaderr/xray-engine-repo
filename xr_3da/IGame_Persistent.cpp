#include "stdafx.h"
#pragma hdrstop

#include "IGame_Persistent.h"
#include "environment.h"

ENGINE_API	IGame_Persistant*		g_pGamePersistent	= NULL;

IGame_Persistant::IGame_Persistant	()
{
	Device.seqAppCycleStart.Add		(this);
	Device.seqAppCycleEnd.Add		(this);
}

IGame_Persistant::~IGame_Persistant	()
{
	Device.seqAppCycleStart.Remove	(this);
	Device.seqAppCycleEnd.Remove	(this);
}

void IGame_Persistant::OnAppCycleStart()
{
	ObjectPool.load					();
	Environment.load				();
}

void IGame_Persistant::OnAppCycleEnd()
{
	ObjectPool.unload				();
}
