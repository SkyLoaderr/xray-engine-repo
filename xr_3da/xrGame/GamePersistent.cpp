#include "stdafx.h"
#include "gamepersistent.h"

CGamePersistent* GamePersistent=0;

CGamePersistent::CGamePersistent(void)
{
	Device.seqAppCycleStart.Add	(this);
	Device.seqAppCycleEnd.Add	(this);
}

CGamePersistent::~CGamePersistent(void)
{
	Device.seqAppCycleStart.Remove(this);
	Device.seqAppCycleEnd.Remove(this);
}

void CGamePersistent::OnAppCycleStart()
{
	// load game materials
	FILE_NAME		fn_mtl;
	if (Engine.FS.Exist(fn_mtl, Path.GameData, "gamemtl.xr"))
		GMLib.Load	(fn_mtl);
}

void CGamePersistent::OnAppCycleEnd()
{
	GMLib.Unload	();
}
