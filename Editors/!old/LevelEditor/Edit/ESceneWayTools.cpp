#include "stdafx.h"
#pragma hdrstop

#include "ESceneWayTools.h"
#include "ESceneWayControls.h"
#include "ui_leveltools.h"
#include "FrameWayPoint.h"

void ESceneWayTools::CreateControls()
{
	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlWayPointAdd>(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraWayPoint>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneWayTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

