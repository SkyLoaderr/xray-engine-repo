#include "stdafx.h"
#pragma hdrstop

#include "ESceneAIMapTools.h"
#include "ui_tools.h"
#include "ui_aimaptools.h"
#include "FrameAIMap.h"

void ESceneAIMapTools::CreateControls()
{
	inherited::CreateControls();
	// node tools
    AddControl(xr_new<TUI_ControlAIMapNodeSelect>	(estAIMapNode,		eaSelect, 	this));
    AddControl(xr_new<TUI_ControlAIMapNodeAdd>		(estAIMapNode,		eaAdd, 		this));
    AddControl(xr_new<TUI_ControlAIMapNodeMove>		(estAIMapNode,		eaMove,		this));
    AddControl(xr_new<TUI_ControlAIMapNodeRotate>	(estAIMapNode,		eaRotate,	this));
	// frame
    pFrame 			= xr_new<TfraAIMap>((TComponent*)0,this);
}
//----------------------------------------------------
 
void ESceneAIMapTools::RemoveControls()
{
}
//----------------------------------------------------

