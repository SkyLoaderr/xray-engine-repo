#include "stdafx.h"
#pragma hdrstop

#include "IGame_Persistent.h"
#include "environment.h"

ENGINE_API	IGame_Persistent*		g_pGamePersistent	= NULL;

IGame_Persistent::IGame_Persistent	()
{
	Device.seqAppStart.Add			(this);
	Device.seqAppEnd.Add			(this);
	Device.seqFrame.Add				(this,REG_PRIORITY_HIGH+1);
}

IGame_Persistent::~IGame_Persistent	()
{
	Device.seqFrame.Remove			(this);
	Device.seqAppStart.Remove		(this);
	Device.seqAppEnd.Remove			(this);
}

void IGame_Persistent::OnAppStart	()
{
#ifndef _EDITOR
	ObjectPool.load					();
#endif
	Environment.load				();

	if (strstr(Core.Params,"-dedicated"))	bDedicatedServer	= TRUE;
	else									bDedicatedServer	= FALSE;
}

void IGame_Persistent::OnAppEnd		()
{
#ifndef _EDITOR
	ObjectPool.unload				();
#endif
}

void IGame_Persistent::OnFrame		()
{
	Environment.OnFrame				();
}
