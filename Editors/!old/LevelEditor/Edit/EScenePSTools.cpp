#include "stdafx.h"
#pragma hdrstop

#include "EScenePSTools.h"
#include "ui_tools.h"
#include "ui_PStools.h"
#include "FramePS.h"

void EScenePSTools::CreateControls()
{
	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlPSAdd>(estDefault,eaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraPS>((TComponent*)0);
}
//----------------------------------------------------
 
void EScenePSTools::RemoveControls()
{
}
//----------------------------------------------------

