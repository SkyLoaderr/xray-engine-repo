#include "stdafx.h"
#pragma hdrstop

#include "EScenePortalTools.h"
#include "ui_tools.h"
#include "FramePortal.h"

/* TODO 1 -oAlexMX -cTODO: Create tools as AI Map */

void EScenePortalTools::CreateControls()
{
//	inherited::CreateControls();
	// frame
    pFrame 			= xr_new<TfraPortal>((TComponent*)0);
}
//----------------------------------------------------
 
void EScenePortalTools::RemoveControls()
{
}
//----------------------------------------------------

