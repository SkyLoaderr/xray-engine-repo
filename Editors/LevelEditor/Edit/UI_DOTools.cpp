#include "stdafx.h"
#pragma hdrstop

#include "ui_DOTools.h"
#include "ui_tools.h"
#include "FrameDetObj.h"
#include "DetailObjects.h"
#include "scene.h"
//---------------------------------------------------------------------------

TUI_DOTools::TUI_DOTools():TUI_CustomTools(OBJCLASS_DO,false)
{
    AddControlCB(xr_new<TUI_CustomControl>		(estDefault,	eaSelect, 	this));
}

void TUI_DOTools::OnActivate()
{
    pFrame = xr_new<TfraDetailObject>((TComponent*)0,dynamic_cast<EDetailManager*>(Scene.GetMTools(OBJCLASS_DO)));
    ((TfraDetailObject*)pFrame)->OnEnter();
	TUI_CustomTools::OnActivate();
}
void TUI_DOTools::OnDeactivate()
{
    ((TfraDetailObject*)pFrame)->OnExit();
	TUI_CustomTools::OnDeactivate();
    xr_delete(pFrame);
}
//--------------------------------------------------------------------------------------------------

