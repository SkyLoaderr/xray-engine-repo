#include "stdafx.h"
#pragma hdrstop

#include "ESceneDOTools.h"
#include "FrameDetObj.h"
#include "UI_LevelTools.h"

void EDetailManager::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
	// frame
    pFrame 			= xr_new<TfraDetailObject>((TComponent*)0,this);
}
//----------------------------------------------------
 
void EDetailManager::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

 