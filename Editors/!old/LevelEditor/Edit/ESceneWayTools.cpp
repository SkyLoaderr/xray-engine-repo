#include "stdafx.h"
#pragma hdrstop

#include "ESceneWayTools.h"
#include "ui_waypointtools.h"
#include "ui_tools.h"
#include "FrameWayPoint.h"

void ESceneWayTools::CreateControls()
{
	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlWayPointAdd>(estDefault,eaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraWayPoint>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneWayTools::RemoveControls()
{
}
//----------------------------------------------------

