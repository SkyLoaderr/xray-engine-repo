#include "stdafx.h"
#pragma hdrstop

#include "ESceneShapeTools.h"
#include "ESceneShapeControls.h"
#include "ui_leveltools.h"
#include "FrameShape.h"

void ESceneShapeTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    AddControl		(xr_new<TUI_ControlShapeAdd>(estDefault,etaAdd,	this));
	// frame
    pFrame 			= xr_new<TfraShape>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneShapeTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

