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

#include "UI_Main.h"

bool TUI::Command( int _Command, int p1, int p2 ){
	if ((_Command!=COMMAND_INITIALIZE)&&!m_bReady) return false;

    bool bRes = true;

	switch( _Command ){
	case COMMAND_INITIALIZE:{
		Engine.Initialize	();
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
	case COMMAND_EXIT:
    	bRes = Tools.IfModified();
        break;
	case COMMAND_EDITOR_PREF:
	    frmEditorPreferences->ShowModal();
        break;
	case COMMAND_LOAD_MOTIONS:{
    	if (!Tools.CurrentObject()){
        	ELog.DlgMsg(mtError,"Scene empty. Load object first.");
        	bRes=false;
        	break;
        }
		AnsiString fn;
		if (Engine.FS.GetOpenName(Engine.FS.m_SMotions,fn)){
        	Tools.LoadMotions(fn.c_str());
            fraLeftBar->UpdateMotionList();
        }
        }break;
	case COMMAND_SAVE_MOTIONS:{
    	if (!Tools.CurrentObject()){
        	ELog.DlgMsg(mtError,"Scene empty. Load object first.");
        	bRes=false;
        	break;
        }
		AnsiString fn;
		if (Engine.FS.GetSaveName(Engine.FS.m_SMotions,fn)) Tools.SaveMotions(fn.c_str());
        }break;
    case COMMAND_SAVEAS:{
		AnsiString fn = m_LastFileName;
		if (Engine.FS.GetSaveName(Engine.FS.m_Objects,fn)) bRes=Command(COMMAND_SAVE, (DWORD)fn.c_str());
        if (bRes){
        	strcpy(m_LastFileName,fn.c_str());
        	Command(COMMAND_UPDATE_CAPTION);
        }
    	}break;
	case COMMAND_SAVE:{
    	AnsiString fn;
        if (p1)	fn = (char*)p1;
        else	fn = m_LastFileName;
		if (Tools.Save(fn.c_str())){
        	Command(COMMAND_UPDATE_CAPTION);
			fraLeftBar->AppendRecentFile(fn.c_str());
        }else{
        	bRes=false;
        }
    	}break;
    case COMMAND_IMPORT:{
    	AnsiString fn;
    	if (Engine.FS.GetOpenName(Engine.FS.m_Import,fn)){
            if (!Tools.IfModified()){
                bRes=false;
                break;
            }
			Command( COMMAND_CLEAR );
	    	if (!Tools.Load(fn.c_str())){
            	ELog.DlgMsg(mtError,"Can't load file '%s'",fn.c_str());
            	bRes=false;
            	break;
            }
			strcpy(m_LastFileName,ExtractFileName(fn).c_str());
			if (Command( COMMAND_SAVEAS )){
	            Engine.FS.MarkFile(fn.c_str());
			    fraLeftBar->UpdateMotionList();
            }else{
            	Command( COMMAND_CLEAR );
            }
        }
    	}break;
    case COMMAND_EXPORT:{
    	AnsiString fn;
    	if (Engine.FS.GetSaveName(Engine.FS.m_GameMeshes,fn))
            if (Tools.Export(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
            else			            	ELog.DlgMsg(mtError,"Export failed.");
    	}break;
    case COMMAND_LOAD:{
    	AnsiString fn;
        if (p1)	fn = (char*)p1;
        else	fn = m_LastFileName;
        if( p1 || Engine.FS.GetOpenName( Engine.FS.m_Objects, fn ) ){
            if (!Tools.IfModified()){
                bRes=false;
                break;
            }
            if ((0!=stricmp(fn.c_str(),m_LastFileName))&&Engine.FS.CheckLocking(0,fn.c_str(),false,true)){
                bRes=false;
                break;
            }
            if ((0==stricmp(fn.c_str(),m_LastFileName))&&Engine.FS.CheckLocking(0,fn.c_str(),true,false)){
                Engine.FS.UnlockFile(0,fn.c_str());
            }
			Command( COMMAND_CLEAR );
	    	if (!Tools.Load(fn.c_str())){
            	ELog.DlgMsg(mtError,"Can't load file '%s'",fn.c_str());
            	bRes=false;
            	break;
            }
			strcpy(m_LastFileName,fn.c_str());
			fraLeftBar->AppendRecentFile(m_LastFileName);
		    fraLeftBar->UpdateMotionList();
        	Command(COMMAND_UPDATE_CAPTION);
        }
    	}break;
	case COMMAND_CLEAR:
		{
			m_LastFileName[0]=0;
			Device.m_Camera.Reset();
            Tools.Clear();
			Command(COMMAND_UPDATE_CAPTION);
		}
		break;

	case COMMAND_REFRESH_TEXTURES:
		Device.RefreshTextures(0);
		break;

	case COMMAND_ZOOM_EXTENTS:
		Tools.ZoomObject();
    	break;
    case COMMAND_RENDER_FOCUS:
		if (frmMain->Visible&&m_bReady)
        	m_D3DWindow->SetFocus();
    	break;
    case COMMAND_UPDATE_CAPTION:
    	frmMain->UpdateCaption();
   	 	break;
   	case COMMAND_UPDATE_TOOLBAR:
    	fraLeftBar->UpdateBar();
    	break;
	case COMMAND_RESET_ANIMATION:
   		break;
    case COMMAND_MAKE_PREVIEW:
    	Tools.MakePreview();
    	break;
    case COMMAND_PREVIEW_OBJ_PREF:
    	Tools.SetPreviewObjectPrefs();
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
	case COMMAND_CHANGE_AXIS:
    	fraTopBar->ChangeAxis(p1);
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
        else if (Key=='O')			Command(COMMAND_LOAD);
    }else{
        if (Shift.Contains(ssAlt)){
        }else{
        	if (Key=='P')			Command(COMMAND_EDITOR_PREF);
            else if (Key==VK_OEM_4)	Command(COMMAND_GRID_SLOT_SIZE,false);
            else if (Key==VK_OEM_6)	Command(COMMAND_GRID_SLOT_SIZE,true);
        	else if (Key=='S')		Command(COMMAND_CHANGE_ACTION, eaSelect);
        	else if (Key=='A')		Command(COMMAND_CHANGE_ACTION, eaAdd);
        	else if (Key=='T')		Command(COMMAND_CHANGE_ACTION, eaMove);
        	else if (Key=='Y')		Command(COMMAND_CHANGE_ACTION, eaRotate);
        	else if (Key=='H')		Command(COMMAND_CHANGE_ACTION, eaScale);
        	else if (Key=='Z')		Command(COMMAND_CHANGE_AXIS,   eAxisX);
        	else if (Key=='X')		Command(COMMAND_CHANGE_AXIS,   eAxisY);
        	else if (Key=='C')		Command(COMMAND_CHANGE_AXIS,   eAxisZ);
        	else if (Key=='V')		Command(COMMAND_CHANGE_AXIS,   eAxisZX);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TUI::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);
    if (Shift.Contains(ssCtrl)){
        if (Key=='S')				Command(COMMAND_SAVE);
        else if (Key=='O')			Command(COMMAND_LOAD);
    }
    if (Key==VK_OEM_3)		  		Command(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------
char* TUI::GetCaption()
{
	VERIFY(m_bReady);
	return GetEditFileName()[0]?GetEditFileName():"noname";
}

char* TUI::GetTitle()
{
	VERIFY(m_bReady);
	return "Actor Editor";
}

LPSTR GetNameByClassID(EObjClass cls_id){
	return 0;
}

