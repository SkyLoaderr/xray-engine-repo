#include "stdafx.h"
#pragma hdrstop

#include "ESceneDOTools.h"
#include "FrameDetObj.h"

void EDetailManager::CreateControls()
{
	inherited::CreateControls();
	// frame
    pFrame 			= xr_new<TfraDetailObject>((TComponent*)0,this);
}
//----------------------------------------------------
 
void EDetailManager::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

 