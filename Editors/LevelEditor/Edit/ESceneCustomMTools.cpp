#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomMTools.h"

ESceneCustomMTools::ESceneCustomMTools(ObjClassID cls)
{
    ClassID				= cls;
    // controls
    sub_target			= 0;
    pCurControl 		= 0;
    pFrame				= 0;
    action				= -1;
    m_bEnabled			= TRUE;
}

ESceneCustomMTools::~ESceneCustomMTools()
{
}
 
void ESceneCustomMTools::OnCreate()
{
    OnDeviceCreate		();
    CreateControls		();
}
void ESceneCustomMTools::OnDestroy()
{
    OnDeviceDestroy		();
    RemoveControls		();
}

void ESceneCustomMTools::FillProp(LPCSTR pref, PropItemVec& items)
{
    PropValue* V		= PHelper().CreateBOOL	(items, "Common\\Enabled",		&m_bEnabled);
//    P->OnChangeEvent.bind	(this,&EDetailManager::OnDensityChange);
//COMMAND_REFRESH_UI_BAR
}

