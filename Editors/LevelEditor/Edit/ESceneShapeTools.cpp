#include "stdafx.h"
#pragma hdrstop

#include "ESceneShapeTools.h"
#include "ui_shapetools.h"
#include "ui_tools.h"
#include "FrameShape.h"

void ESceneShapeTools::CreateControls()
{
	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlShapeAdd>(estDefault,eaAdd,	this));
	// frame
    pFrame 			= xr_new<TfraShape>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneShapeTools::RemoveControls()
{
}
//----------------------------------------------------

