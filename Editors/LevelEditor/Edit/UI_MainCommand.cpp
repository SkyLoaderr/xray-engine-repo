//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ui_main.h"
#include "UI_Tools.h"

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"

#include "EditorPref.h"
#include "main.h"
#include "ImageEditor.h"
#include "d3dutils.h"

#include "PSLibrary.h"
#include "Library.h"
#include "LightAnimLibrary.h"

bool TUI::Command( int _Command, int p1, int p2 ){
	if ((_Command!=COMMAND_INITIALIZE)&&!m_bReady) return false;
	string256 filebuffer;

    bool bRes = true;

	switch( _Command ){
	case COMMAND_INITIALIZE:{
		Engine.Initialize	();
        // make interface
	    fraBottomBar		= xr_new<TfraBottomBar>((TComponent*)0);
    	fraLeftBar  		= xr_new<TfraLeftBar>((TComponent*)0);
	    fraTopBar   		= xr_new<TfraTopBar>((TComponent*)0);
		//----------------
        if (UI.OnCreate()){
            PSLib.OnCreate	();
            Lib.OnCreate	();
            LALib.OnCreate	();
			if (!Tools.OnCreate()){
                bRes=false;
            	break;
            }

		    Command			(COMMAND_CLEAR);
			Command			(COMMAND_RENDER_FOCUS);
			Command			(COMMAND_CHANGE_ACTION, eaSelect);
        }else{
        	bRes = false;
        }
    	}break;
	case COMMAND_DESTROY:
		Command				(COMMAND_CLEAR);
        LALib.OnDestroy		();
    	PSLib.OnDestroy		();
		Tools.OnDestroy		();
		Lib.OnDestroy		();
        UI.OnDestroy		();
		Engine.Destroy		();
		//----------------
        xr_delete(fraLeftBar);
	    xr_delete(fraTopBar);
    	xr_delete(fraBottomBar);
		//----------------
    	break;
	case COMMAND_QUIT:
    	Quit();
    	break;
	case COMMAND_EDITOR_PREF:
	    frmEditorPreferences->Run();
        break;
	case COMMAND_CHANGE_ACTION:
		Tools.ChangeAction((EAction)p1);
        break;
    case COMMAND_IMAGE_EDITOR:
    	TfrmImageLib::EditImageLib(AnsiString("Image Editor"));
    	break;
	case COMMAND_CHECK_TEXTURES:
    	TfrmImageLib::ImportTextures();
    	break;
	case COMMAND_REFRESH_TEXTURES:
		Device.RefreshTextures(0);
		break;
	case COMMAND_RELOAD_TEXTURES:
    	Device.ReloadTextures();
    	break;
	case COMMAND_CHANGE_AXIS:
    	fraTopBar->ChangeAxis(p1);
        break;
	case COMMAND_CHANGE_SNAP:
        ((TExtBtn*)p1)->Down = !((TExtBtn*)p1)->Down;
        break;
    case COMMAND_UNLOAD_TEXTURES:
		Device.UnloadTextures();
    	break;
    case COMMAND_EVICT_OBJECTS:
    	Lib.EvictObjects();
    	break;
    case COMMAND_EVICT_TEXTURES:
    	Device.Shader.Evict();
    	break;
    case COMMAND_CHECK_MODIFIED:
    	bRes = Tools.IsModified();
		break;
	case COMMAND_EXIT:
    	bRes = Tools.IfModified();
		break;
	case COMMAND_SHOW_PROPERTIES:
        Tools.ShowProperties();
        break;
	case COMMAND_UPDATE_PROPERTIES:
        Tools.UpdateProperties();
        break;
	case COMMAND_SHOWCONTEXTMENU:
    	ShowContextMenu(EObjClass(p1));
        break;
	case COMMAND_ZOOM_EXTENTS:
		Tools.ZoomObject(p1);
    	break;
    case COMMAND_SET_NUMERIC_POSITION:
    	Tools.SetNumPosition((CCustomObject*)p1);
    	break;
    case COMMAND_SET_NUMERIC_ROTATION:
    	Tools.SetNumRotation((CCustomObject*)p1);
    	break;
    case COMMAND_SET_NUMERIC_SCALE:
    	Tools.SetNumScale((CCustomObject*)p1);
    	break;
    case COMMAND_RENDER_FOCUS:
		if (frmMain->Visible&&m_bReady)
        	m_D3DWindow->SetFocus();
    	break;
    case COMMAND_UPDATE_CAPTION:
    	frmMain->UpdateCaption();
    	break;
	case COMMAND_BREAK_LAST_OPERATION:
    	NeedBreak();
		ELog.DlgMsg(mtInformation,"Execution canceled.");
    	break;
	case COMMAND_UPDATE_TOOLBAR:
    	fraLeftBar->UpdateBar();
    	break;
    case COMMAND_TOGGLE_SAFE_RECT:
    	psDeviceFlags.set(rsDrawSafeRect,!psDeviceFlags.is(rsDrawSafeRect));
        frmMain->paWindowResize(0);
    	break;
    case COMMAND_TOGGLE_GRID:
    	psDeviceFlags.set(rsDrawGrid,!psDeviceFlags.is(rsDrawGrid));
    	break;
	case COMMAND_UPDATE_GRID:
    	DU::UpdateGrid(frmEditorPreferences->seGridNumberOfCells->Value,frmEditorPreferences->seGridSquareSize->Value);
	    OutGridSize();
    	break;
    case COMMAND_GRID_NUMBER_OF_SLOTS:
    	if (p1)	frmEditorPreferences->seGridNumberOfCells->Value += frmEditorPreferences->seGridNumberOfCells->Increment;
        else	frmEditorPreferences->seGridNumberOfCells->Value -= frmEditorPreferences->seGridNumberOfCells->Increment;
        Command(COMMAND_UPDATE_GRID);
    	break;
    case COMMAND_GRID_SLOT_SIZE:{
    	float step = frmEditorPreferences->seGridSquareSize->Increment;
        float val = frmEditorPreferences->seGridSquareSize->Value;
    	if (p1){
	    	if (val<1) step/=10.f;
        	frmEditorPreferences->seGridSquareSize->Value += step;
        }else{
	    	if (fsimilar(val,1.f)||(val<1)) step/=10.f;
        	frmEditorPreferences->seGridSquareSize->Value -= step;
        }
        Command(COMMAND_UPDATE_GRID);
    	}break;
 	default:
    	bRes = CommandExt(_Command,p1,p2);
	}

    RedrawScene();
    return bRes;
}
//---------------------------------------------------------------------------
char* TUI::GetTitle()
{
	return _EDITOR_NAME_;
}

//---------------------------------------------------------------------------
bool TUI::ApplyShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);

    if (ApplyGlobalShortCut(Key,Shift))	return true;
	if (ApplyShortCutExt(Key,Shift)) 	return true;

	bool bExec = false;

    if (Key==VK_ESCAPE)   		{Command(COMMAND_CHANGE_ACTION, eaSelect);					bExec=true;}
    if (Shift.Contains(ssCtrl)){
    }else{
        if (Shift.Contains(ssAlt)){
        }else{
            // simple press
        	if (Key=='S')		{Command(COMMAND_CHANGE_ACTION, eaSelect);					bExec=true;}
        	else if (Key=='A')	{Command(COMMAND_CHANGE_ACTION, eaAdd);                     bExec=true;}
        	else if (Key=='T')	{Command(COMMAND_CHANGE_ACTION, eaMove);					bExec=true;}
        	else if (Key=='Y')	{Command(COMMAND_CHANGE_ACTION, eaRotate);                  bExec=true;}
        	else if (Key=='H')	{Command(COMMAND_CHANGE_ACTION, eaScale);                   bExec=true;}
        	else if (Key=='Z')	{Command(COMMAND_CHANGE_AXIS,   eAxisX);                    bExec=true;}
        	else if (Key=='X')	{Command(COMMAND_CHANGE_AXIS,   eAxisY);                    bExec=true;}
        	else if (Key=='C')	{Command(COMMAND_CHANGE_AXIS,   eAxisZ);                    bExec=true;}
        	else if (Key=='V')	{Command(COMMAND_CHANGE_AXIS,   eAxisZX);                   bExec=true;}
        	else if (Key=='O')	{Command(COMMAND_CHANGE_SNAP,   (int)fraTopBar->ebOSnap);   bExec=true;}
        	else if (Key=='G')	{Command(COMMAND_CHANGE_SNAP,   (int)fraTopBar->ebGSnap);   bExec=true;}
        	else if (Key=='P')	{Command(COMMAND_EDITOR_PREF);                              bExec=true;}
        }
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool TUI::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);
	if (ApplyGlobalShortCutExt(Key,Shift)) return true;
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
        if (Key=='S'){
            if (Shift.Contains(ssAlt))  {Command(COMMAND_SAVEAS);               bExec=true;}
            else                        {Command(COMMAND_SAVE);                 bExec=true;}
        }
        else if (Key=='O')   			{Command(COMMAND_LOAD);                 bExec=true;}
        else if (Key=='N')   			{Command(COMMAND_CLEAR);                bExec=true;}
        else if (Key=='G')   			{Command(COMMAND_TOGGLE_GRID);          bExec=true;}
        else if (Key=='F')				{Command(COMMAND_TOGGLE_SAFE_RECT);     bExec=true;}
    }
    if (Key==VK_OEM_3)					{Command(COMMAND_RENDER_FOCUS);        bExec=true;}
    return bExec;
}
//---------------------------------------------------------------------------

