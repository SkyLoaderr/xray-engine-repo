#include "stdafx.h"
#pragma hdrstop

#ifndef _ECOREB

#include "ui_main.h"
#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"

case COMMAND_REFRESH_UI_BAR:
    fraTopBar->RefreshBar	();
    fraLeftBar->RefreshBar	();
    fraBottomBar->RefreshBar();
    break;

    TMenuItem *MI 	= xr_new<TMenuItem>((TComponent*)0);
    MI->Caption 	= name;
    MI->OnClick 	= miRecentFilesClick;
    MI->Tag			= 0x1001;
    fraLeftBar->miRecentFiles->Insert(0,MI);

    fraLeftBar->miRecentFiles->Enabled = true;
        
void __fastcall TUI::miRecentFilesClick(TObject *Sender)
{
	TMenuItem* MI = dynamic_cast<TMenuItem*>(Sender); R_ASSERT(MI&&(MI->Tag==0x1001));
    AnsiString fn = MI->Caption;
    Command(COMMAND_LOAD,(u32)fn.c_str());
}
//---------------------------------------------------------------------------

bool TUI::OnCreate()
{
// create base class
	Device.InitTimer();

// forms
    fraBottomBar	= xr_new<TfraBottomBar>((TComponent*)0);
    fraLeftBar  	= xr_new<TfraLeftBar>((TComponent*)0);
    fraTopBar   	= xr_new<TfraTopBar>((TComponent*)0);

    fraBottomBar->Parent    = paBottomBar;
    fraTopBar->Parent       = paTopBar;
    fraLeftBar->Parent      = paLeftBar;
    if (paLeftBar->Tag > 0) paLeftBar->Parent = paTopBar;
    else paLeftBar->Parent 	= frmMain;

    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
    fraTopBar->fsStorage->RestoreFormPlacement();
//-
    
    m_D3DWindow 	= frmMain->D3DWindow;
    VERIFY(m_D3DWindow);
    Device.Initialize();
    
#ifdef _LEVEL_EDITOR
    m_Cursor        = xr_new<C3DCursor>();
#endif
	// Creation
	XRC.ray_options	(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL);

    pInput			= xr_new<CInput>(FALSE,mouse_device_key);
    UI.IR_Capture	();

    m_bReady		= true;

    EFS.WriteAccessLog(_EDITOR_FILE_NAME_,"Started");

    if (!CreateMailslot()){
    	ELog.DlgMsg	(mtError,"Can't create mail slot.\nIt's possible two Editors started.");
        return 		false;
    }

    if (!FS.path_exist(_local_root_)){
    	ELog.DlgMsg	(mtError,"Undefined Editor local directory.");
        return 		false;
    }

	BeginEState		(esEditScene);

    return true;
}

void TUI::OnDestroy()
{
    EFS.WriteAccessLog(_EDITOR_FILE_NAME_,"Finished");

	VERIFY(m_bReady);
	m_bReady		= false;
	UI.IR_Release	();
    xr_delete		(pInput);
    EndEState		();

    Device.ShutDown	();
    
    //----------------
    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    fraTopBar->fsStorage->SaveFormPlacement();

    fraTopBar->Parent       = 0;
    fraLeftBar->Parent      = 0;
    fraBottomBar->Parent    = 0;

	xr_delete(fraLeftBar);
    xr_delete(fraTopBar);
    xr_delete(fraBottomBar);
}
 
#endif
