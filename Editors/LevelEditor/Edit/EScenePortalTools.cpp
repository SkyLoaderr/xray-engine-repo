#include "stdafx.h"
#pragma hdrstop

#include "EScenePortalTools.h"
#include "ui_tools.h"
#include "FramePortal.h"
#include "ui_portaltools.h"

/* TODO 1 -oAlexMX -cTODO: Create tools as AI Map */

void EScenePortalTools::CreateControls()
{
//	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlPortalSelect>(estDefault,eaSelect,	this));
	// frame
    pFrame 			= xr_new<TfraPortal>((TComponent*)0);
}
//----------------------------------------------------
 
void EScenePortalTools::RemoveControls()
{
}
//----------------------------------------------------

