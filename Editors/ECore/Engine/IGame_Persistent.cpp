#include "stdafx.h"
#pragma hdrstop

#include "IGame_Persistent.h"
#include "environment.h"

ENGINE_API	IGame_Persistent*		g_pGamePersistent	= NULL;

IGame_Persistent::IGame_Persistent	()
{
	Device.seqAppCycleStart.Add		(this);
	Device.seqAppCycleEnd.Add		(this);
	Device.seqFrame.Add				(this,REG_PRIORITY_HIGH+1);
	Device.seqDevCreate.Add			(this);
	Device.seqDevDestroy.Add		(this);
}

IGame_Persistent::~IGame_Persistent	()
{
	Device.seqDevCreate.Remove		(this);
	Device.seqDevDestroy.Remove		(this);
	Device.seqFrame.Remove			(this);
	Device.seqAppCycleStart.Remove	(this);
	Device.seqAppCycleEnd.Remove	(this);
}

void IGame_Persistent::OnAppCycleStart()
{
#ifndef _EDITOR
	ObjectPool.load					();
#endif
	Environment.load				();

	if (strstr(Core.Params,"-dedicated"))	bDedicatedServer	= TRUE;
	else									bDedicatedServer	= FALSE;
}

void IGame_Persistent::OnAppCycleEnd()
{
#ifndef _EDITOR
	ObjectPool.unload				();
#endif
}

void IGame_Persistent::OnFrame		()
{
	Environment.OnFrame				();
}

void IGame_Persistent::OnDeviceCreate()
{
	Environment.OnDeviceCreate		();
}

void IGame_Persistent::OnDeviceDestroy()
{
	Environment.OnDeviceDestroy		();
}

