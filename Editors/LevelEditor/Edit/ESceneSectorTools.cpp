#include "stdafx.h"
#pragma hdrstop

#include "ESceneSectorTools.h"
#include "ui_sectortools.h"
#include "ui_tools.h"
#include "FrameSector.h"

/* TODO 1 -oAlexMX -cTODO: Create tools as AI Map */

void ESceneSectorTools::CreateControls()
{
//	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlSectorSelect>(estDefault,eaSelect,	this));
    AddControl		(xr_new<TUI_ControlSectorAdd>	(estDefault,eaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraSector>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneSectorTools::RemoveControls()
{
}
//----------------------------------------------------

