#include "stdafx.h"
#pragma hdrstop

#include "ESceneSpawnTools.h"
#include "ui_leveltools.h"
#include "ui_SpawnTools.h"
#include "FrameSpawn.h"
#include "PropertiesListHelper.h"

void ESceneSpawnTools::CreateControls()
{
	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlSpawnAdd>(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraSpawn>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneSpawnTools::RemoveControls()
{
}
//----------------------------------------------------

void ESceneSpawnTools::FillProp(LPCSTR pref, PropItemVec& items)
{
    PHelper.CreateFlag<Flags32>(items, FHelper.PrepareKey(pref,"Common\\Show Spawn Type"),	&m_Flags,		flShowSpawnType);
    PHelper.CreateFlag<Flags32>(items, FHelper.PrepareKey(pref,"Common\\Trace Visibility"),	&m_Flags,		flPickSpawnType);
	inherited::FillProp(pref, items);
}
//------------------------------------------------------------------------------

