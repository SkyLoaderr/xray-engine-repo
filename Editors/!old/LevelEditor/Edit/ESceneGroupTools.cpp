#include "stdafx.h"
#pragma hdrstop

#include "ESceneGroupTools.h"
#include "ui_tools.h"
#include "FrameGroup.h"

void ESceneGroupTools::CreateControls()
{
	inherited::CreateControls();
	// frame
    pFrame 			= xr_new<TfraGroup>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneGroupTools::RemoveControls()
{
}
//----------------------------------------------------

