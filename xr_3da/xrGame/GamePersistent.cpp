#include "stdafx.h"
#include "gamepersistent.h"

CGamePersistent::CGamePersistent(void)
{
}

CGamePersistent::~CGamePersistent(void)
{
}

void CGamePersistent::OnAppCycleStart()
{
	__super::OnAppCycleStart	();

	// load game materials
	string256		fn_mtl;
	if (FS.exist(fn_mtl, "$game_data$","gamemtl.xr"))
		GMLib.Load	(fn_mtl);
}

void CGamePersistent::OnAppCycleEnd()
{
	__super::OnAppCycleEnd		();

	GMLib.Unload				();
}
