#include "stdafx.h"
#pragma hdrstop

#include "EScenePSTools.h"
#include "ui_leveltools.h"
#include "EScenePSControls.h"
#include "FramePS.h"

void EScenePSTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    AddControl		(xr_new<TUI_ControlPSAdd>(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraPS>((TComponent*)0);
}
//----------------------------------------------------
 
void EScenePSTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

