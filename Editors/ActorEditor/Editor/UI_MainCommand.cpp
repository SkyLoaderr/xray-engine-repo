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

#include "SoundManager.h"

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
            ::Render->PSystems.OnCreate	();
            Lib.OnCreate	();
            LALib.OnCreate	();
            SndLib.OnCreate	();
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
    	::Render->PSystems.OnDestroy();
		Tools.OnDestroy		();
		SndLib.OnDestroy	();
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
        Tools.UpdateProperties((bool)p1);
        break;
	case COMMAND_REFRESH_PROPERTIES:
        Tools.RefreshProperties();
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
    case COMMAND_RENDER_WIRE:
    	if (p1)	Device.dwFillMode 	= D3DFILL_WIREFRAME;
        else 	Device.dwFillMode 	= D3DFILL_SOLID;
	    break;
    case COMMAND_RENDER_FOCUS:
		if (frmMain->Visible&&m_bReady)
        	m_D3DWindow->SetFocus();
    	break;
    case COMMAND_UPDATE_CAPTION:
    	frmMain->UpdateCaption();
    	break;
	case COMMAND_BREAK_LAST_OPERATION:                                          	
        if (mrYes==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to break current action?")){
            NeedBreak	();
            ELog.Msg	(mtInformation,"Execution canceled.");
        }
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
    	DU.UpdateGrid(frmEditorPreferences->seGridNumberOfCells->Value,frmEditorPreferences->seGridSquareSize->Value);
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

    if (Key==VK_ESCAPE)   			COMMAND1(COMMAND_CHANGE_ACTION, eaSelect)
    if (Shift.Contains(ssCtrl)){
    }else{
        if (Shift.Contains(ssAlt)){
        }else{
            // simple press
        	if (Key=='S')			COMMAND1(COMMAND_CHANGE_ACTION, eaSelect)
        	else if (Key=='A')		COMMAND1(COMMAND_CHANGE_ACTION, eaAdd)
        	else if (Key=='T')		COMMAND1(COMMAND_CHANGE_ACTION, eaMove)
        	else if (Key=='Y')		COMMAND1(COMMAND_CHANGE_ACTION, eaRotate)
        	else if (Key=='H')		COMMAND1(COMMAND_CHANGE_ACTION, eaScale)
        	else if (Key=='Z')		COMMAND1(COMMAND_CHANGE_AXIS,   eAxisX)
        	else if (Key=='X')		COMMAND1(COMMAND_CHANGE_AXIS,   eAxisY)
        	else if (Key=='C')		COMMAND1(COMMAND_CHANGE_AXIS,   eAxisZ)
        	else if (Key=='V')		COMMAND1(COMMAND_CHANGE_AXIS,   eAxisZX)
        	else if (Key=='O')		COMMAND1(COMMAND_CHANGE_SNAP,   (int)fraTopBar->ebOSnap)
        	else if (Key=='G')		COMMAND1(COMMAND_CHANGE_SNAP,   (int)fraTopBar->ebGSnap)
        	else if (Key=='P')		COMMAND0(COMMAND_EDITOR_PREF)
            else if (Key==VK_OEM_4)	COMMAND1(COMMAND_GRID_SLOT_SIZE,false)
            else if (Key==VK_OEM_6)	COMMAND1(COMMAND_GRID_SLOT_SIZE,true)
        	else if (Key=='W')		COMMAND1(COMMAND_RENDER_WIRE, !(Device.dwFillMode==D3DFILL_WIREFRAME))
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
            if (Shift.Contains(ssAlt))  COMMAND0(COMMAND_SAVEAS)
            else                        COMMAND0(COMMAND_SAVE)
        }
        else if (Key=='O')   			COMMAND0(COMMAND_LOAD)
        else if (Key=='N')   			COMMAND0(COMMAND_CLEAR)
        else if (Key=='G')   			COMMAND0(COMMAND_TOGGLE_GRID)
        else if (Key=='F')				COMMAND0(COMMAND_TOGGLE_SAFE_RECT)
    }
    if (Key==VK_OEM_3)					COMMAND0(COMMAND_RENDER_FOCUS)
    return bExec;
}
//---------------------------------------------------------------------------

