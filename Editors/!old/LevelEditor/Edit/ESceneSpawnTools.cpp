#include "stdafx.h"
#pragma hdrstop

#include "ESceneSpawnTools.h"
#include "ui_leveltools.h"
#include "ESceneSpawnControls.h"
#include "FrameSpawn.h"
#include "PropertiesListHelper.h"
#include "Scene.h"
#include "ChoseForm.h"

void __stdcall  FillSpawnItems	(ChooseItemVec& lst, void* param)
{
	LPCSTR type					= (LPCSTR)param;
	LPCSTR gcs					= pSettings->r_string(type,"GroupControlSection");
    ObjectList objects;
    Scene->GetQueryObjects		(objects,OBJCLASS_SPAWNPOINT,-1,-1,-1);
    
    for (ObjectIt it=objects.begin(); it!=objects.end(); it++)
        if ((*it)->OnChooseQuery(gcs))	lst.push_back(SChooseItem((*it)->Name,""));
}

ESceneSpawnTools::ESceneSpawnTools	():ESceneCustomOTools(OBJCLASS_SPAWNPOINT)
{
	m_Flags.zero();
    TfrmChoseItem::AppendEvents	(smSpawnItem,		"Select Spawn Item",		FillSpawnItems,		0,false);
}

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
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneSpawnTools::FillProp(LPCSTR pref, PropItemVec& items)
{            
	PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Show Spawn Type"),	&m_Flags,		flShowSpawnType);
    PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Trace Visibility"),	&m_Flags,		flPickSpawnType);
	inherited::FillProp	(pref, items);
}
//------------------------------------------------------------------------------

