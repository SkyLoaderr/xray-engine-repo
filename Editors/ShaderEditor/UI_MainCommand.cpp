//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "EditorPref.h"
#include "main.h"
#include "UI_Tools.h"
#include "Library.h"
#include "D3DUtils.h"
#include "ImageEditor.h"

#include "UI_Main.h"

bool TUI::Command( int _Command, int p1, int p2 ){
	if ((_Command!=COMMAND_INITIALIZE)&&!m_bReady) return false;

    bool bRes = true;

	switch( _Command ){
	case COMMAND_INITIALIZE:{
		Engine.Initialize	();
		InitMath			();
        // make interface
	    fraBottomBar		= new TfraBottomBar(0);
    	fraLeftBar  		= new TfraLeftBar(0);
	    fraTopBar   		= new TfraTopBar(0);
		//----------------
        if (UI.OnCreate()){
			if (!Tools.OnCreate()){
                bRes=false;
            	break;
            }
            Lib.OnCreate	();

		    Command			(COMMAND_CLEAR);
			Command			(COMMAND_RENDER_FOCUS);
		    BeginEState		(esEditScene);
        }else{
        	bRes = false;
        }
    	}break;
	case COMMAND_DESTROY:
		Tools.OnDestroy	();
		Lib.OnDestroy	();
        UI.OnDestroy	();
        Engine.Destroy	();
		//----------------
        _DELETE(fraLeftBar);
	    _DELETE(fraTopBar);
    	_DELETE(fraBottomBar);
		//----------------
    	break;
    case COMMAND_EVICT_OBJECTS:
    	Lib.EvictObjects();
    	break;
    case COMMAND_EVICT_TEXTURES:
    	Device.Shader.Evict();
    	break;
	case COMMAND_QUIT:
    	Quit();
        break;
	case COMMAND_EXIT:
    	bRes = Tools.IfModified();
        break;
	case COMMAND_EDITOR_PREF:
	    frmEditorPreferences->ShowModal();
        break;
	case COMMAND_SAVE:
    	// engine shaders
    	Tools.SEngine.Save();
    	// compiler
    	Tools.SCompiler.Save();
		Command(COMMAND_UPDATE_CAPTION);
    	break;
    case COMMAND_RELOAD:
    	if (Tools.ActiveEditor()==aeEngine){
	    	if (!Tools.SEngine.IfModified()) return false;
            if (ELog.DlgMsg(mtConfirmation,"Reload shaders?")==mrYes)
                Tools.SEngine.Reload();
    	}else if (Tools.ActiveEditor()==aeCompiler){
	    	if (!Tools.SCompiler.IfModified()) return false;
            if (ELog.DlgMsg(mtConfirmation,"Reload shaders?")==mrYes)
                Tools.SCompiler.Reload();
        }
		Command(COMMAND_UPDATE_CAPTION);
    	break;
	case COMMAND_CLEAR:
		{
			Device.m_Camera.Reset();
            Tools.ResetPreviewObject();
			Command(COMMAND_UPDATE_CAPTION);
		}
		break;

	case COMMAND_REFRESH_TEXTURES:
		Device.RefreshTextures(0);
		break;

    case COMMAND_CHECK_TEXTURES:
    	TfrmImageLib::ImportTextures();
    	break;

	case COMMAND_IMAGE_EDITOR:
    	TfrmImageLib::EditImageLib(AnsiString("Image Editor"));
    	break;

	case COMMAND_ZOOM_EXTENTS:
		Tools.ZoomObject();
    	break;
    case COMMAND_RENDER_FOCUS:
		if (frmMain->Visible&&m_bReady) m_D3DWindow->SetFocus();
    	break;
    case COMMAND_UPDATE_CAPTION:
    	frmMain->UpdateCaption();
   	 	break;
   	case COMMAND_UPDATE_TOOLBAR:
    	fraLeftBar->UpdateBar();
    	break;
	case COMMAND_RESET_ANIMATION:
   		break;
    case COMMAND_SELECT_PREVIEW_OBJ:
		Tools.SelectPreviewObject(p1);
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
    case COMMAND_CHECK_MODIFIED:
    	bRes = Tools.IsModified();
		break;
    case COMMAND_CHANGE_ACTION:
    	Tools.ChangeAction(p1);
		break;
    case COMMAND_UNDO:
    case COMMAND_REDO:
    	// fake
    	break;
 	default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
		}

    RedrawScene();
    return bRes;
}

void __fastcall TUI::ApplyShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);
    if (Shift.Contains(ssCtrl)){
		if (Key=='S') 				Command(COMMAND_SAVE);
    }else{
        if (Shift.Contains(ssAlt)){
        }else{
        	if (Key=='P')			Command(COMMAND_EDITOR_PREF);
            else if (Key==VK_OEM_4)	Command(COMMAND_GRID_SLOT_SIZE,false);
            else if (Key==VK_OEM_6)	Command(COMMAND_GRID_SLOT_SIZE,true);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TUI::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);
    if (Shift.Contains(ssCtrl)){
        if (Key=='S')				Command(COMMAND_SAVE);
    }
    if (Key==VK_OEM_3)		  		Command(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------
char* TUI::GetCaption()
{
 	return "shaders";
}
char* TUI::GetTitle()
{
	return "Shader Editor";
}


