//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "EditLibrary.h"
#include "ObjectList.h"
#include "EditorPref.h"
#include "main.h"
#include "sceneproperties.h"
#include "EditorPref.h"
#include "ImageEditor.h"

#include "UI_Main.h"
#include "builder.h"
#include "D3DUtils.h"

#include "Scene.h"
#include "PSLibrary.h"
#include "Library.h"
#include "UI_Tools.h"

bool TUI::Command( int _Command, int p1, int p2 ){
	char filebuffer[MAX_PATH]="";

    bool bRes = true;

	switch( _Command ){
	case COMMAND_INITIALIZE:{
		FS.OnCreate			();
		InitMath			();
        if (UI.OnCreate()){
            Tools.OnCreate	();
			Scene.OnCreate	();
            PSLib.OnCreate	();
            Lib.OnCreate	();

		    Command			(COMMAND_CLEAR);
			Command			(COMMAND_CHECK_TEXTURES);
			Command			(COMMAND_RENDER_FOCUS);
			Command			(COMMAND_CHANGE_TARGET, etObject);
			Command			(COMMAND_CHANGE_ACTION, eaSelect);
		    BeginEState		(esEditScene);
        }else{
        	bRes = false;
        }
    	}break;
	case COMMAND_DESTROY:
		Lib.OnDestroy	();
    	PSLib.OnDestroy	();
		Scene.OnDestroy	();
		Tools.OnDestroy	();
        UI.OnDestroy	();
    	break;
    case COMMAND_CHECK_MODIFIED:
    	bRes = Scene.IsModified();
		break;
	case COMMAND_EXIT:{
	    EEditorState est = GetEState();
    	switch(est){
        case esEditLibrary: 	if (!TfrmEditLibrary::FinalClose()) return false; break;
        case esEditImages:		if (!TfrmImageLib::HideImageLib()) return false; break;
        case esEditScene:		if (!Scene.IfModified()) return false; break;
        }
		}break;
	case COMMAND_SHOWPROPERTIES:
        Tools.ShowProperties();
        break;
	case COMMAND_SHOWCONTEXTMENU:
    	ShowContextMenu(EObjClass(p1));
        break;
	case COMMAND_EDITOR_PREF:
	    frmEditorPreferences->Run();
        break;
	case COMMAND_OBJECT_LIST:
		if( !Scene.locked() ){
	        frmObjectListShow();
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
        }
        break;
	case COMMAND_CHANGE_TARGET:
	  	Tools.ChangeTarget(p1);
        break;
	case COMMAND_CHANGE_ACTION:
		Tools.ChangeAction(p1);
        break;
    case COMMAND_LIBRARY_EDITOR:
        if (Scene.ObjCount()||(GetEState()!=esEditScene)){
        	if (GetEState()==esEditLibrary)	TfrmEditLibrary::ShowEditor();
            else							ELog.DlgMsg(mtError, "Scene must be empty before editing library!");
        }else{
            TfrmEditLibrary::ShowEditor();
        }
        break;
    case COMMAND_IMAGE_EDITOR:
        if (Scene.ObjCount()||(GetEState()!=esEditScene)){
        	if (GetEState()==esEditImages)	TfrmImageLib::EditImageLib(AnsiString("Image Editor"));
            else							ELog.DlgMsg(mtError, "Scene must be empty before editing images!");
        }else{
			TfrmImageLib::EditImageLib(AnsiString("Image Editor"));
        }
    	break;
	case COMMAND_CHECK_TEXTURES:
    	if (GetEState()==esEditScene) TfrmImageLib::CheckImageLib();
    	break;
	case COMMAND_CHANGE_AXIS:
    	fraTopBar->ChangeAxis(p1);
        break;
	case COMMAND_CHANGE_SNAP:
        ((TExtBtn*)p1)->Down = !((TExtBtn*)p1)->Down;
        break;
	case COMMAND_LOAD:
		if( !Scene.locked() ){
        	if (p1)	strcpy( filebuffer, (char*)p1 );
            else	strcpy( filebuffer, m_LastFileName );
			if( p1 || FS.GetOpenName( &FS.m_Maps, filebuffer ) ){
                if (!Scene.IfModified()) return false;
                SetStatus("Level loading...");
            	Command( COMMAND_CLEAR );
	            BeginEState(esSceneLocked);
				Scene.Load( filebuffer );
                EndEState();
				strcpy(m_LastFileName,filebuffer);
                SetStatus("");
              	Scene.UndoClear();
				Scene.UndoSave();
                Scene.m_Modified = false;
			    Command(COMMAND_UPDATE_CAPTION);
                Command(COMMAND_CHANGE_ACTION,eaSelect);
			}
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_RELOAD:
		if( !Scene.locked() ){
        	Command(COMMAND_LOAD,(int)m_LastFileName);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SAVE:
		if( !Scene.locked() ){
			if( m_LastFileName[0] ){
	            BeginEState(esSceneLocked);
                SetStatus("Level saving...");
				Scene.Save( m_LastFileName, false );
                SetStatus("");
                Scene.m_Modified = false;
			    Command(COMMAND_UPDATE_CAPTION);
                EndEState();
			}else{
				bRes = Command( COMMAND_SAVEAS ); }
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SAVEAS:
		if( !Scene.locked() ){
			filebuffer[0] = 0;
			if( FS.GetSaveName( &FS.m_Maps, filebuffer ) ){
	            BeginEState(esSceneLocked);
				Scene.Save( filebuffer, false );
				strcpy(m_LastFileName,filebuffer);
	            EndEState();
			    bRes = Command(COMMAND_UPDATE_CAPTION);
			}else
            	bRes = false;
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_CLEAR:
		if( !Scene.locked() ){
            if (!Scene.IfModified()) return false;
            BeginEState(esSceneLocked);
//            m_pTexturizer->Reset();
			Device.m_Camera.Reset();
			Scene.Unload();
            Scene.m_LevelOp.Reset();
			m_LastFileName[0] = 0;
            EndEState();
           	Scene.UndoClear();
            Scene.m_Modified = false;
			Command(COMMAND_UPDATE_CAPTION);
			Command(COMMAND_CHANGE_ACTION,eaSelect);
		    Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_VALIDATE_SCENE:
		if( !Scene.locked() ){
            Scene.Validate(true,false);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_REFRESH_TEXTURES:
		Device.RefreshTextures(bool(p1));
		break;

    case COMMAND_UNLOAD_LIBMESHES:
    	Lib.UnloadMeshes();
    	break;

    case COMMAND_RELOAD_LIBRARY:
    	Lib.ReloadLibrary();
    	break;

    case COMMAND_REFRESH_LIBRARY:
    	Lib.RefreshLibrary();
    	break;

	case COMMAND_CUT:
		if( !Scene.locked() ){
        	BeginEState(esSceneLocked);
			Scene.CutSelection(Tools.CurrentClassID());
        	EndEState();
            fraLeftBar->miPaste->Enabled = true;
            fraLeftBar->miPaste2->Enabled = true;
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_COPY:
		if( !Scene.locked() ){
        	BeginEState(esSceneLocked);
			Scene.CopySelection(Tools.CurrentClassID());
        	EndEState();
            fraLeftBar->miPaste->Enabled = true;
            fraLeftBar->miPaste2->Enabled = true;
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_PASTE:
		if( !Scene.locked() ){
        	BeginEState(esSceneLocked);
			Scene.PasteSelection();
        	EndEState();
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_UNDO:
		if( !Scene.locked() ){
			if( !Scene.Undo() ) ELog.DlgMsg( mtInformation, "Undo buffer empty" );
            Tools.Reset();
		    Command(COMMAND_CHANGE_ACTION, eaSelect);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_REDO:
		if( !Scene.locked() ){
			if( !Scene.Redo() ) ELog.DlgMsg( mtInformation, "Redo buffer empty" );
            Tools.Reset();
		    Command(COMMAND_CHANGE_ACTION, eaSelect);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_OPTIONS:
		if( !Scene.locked() ){
            frmScenePropertiesRun(&Scene.m_BuildParams,false);
            Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_BUILD:
		if( !Scene.locked() ){
            if( !Builder.InProgress() )
                if (frmScenePropertiesRun(&Scene.m_BuildParams,true)==mrOk)
                    Builder.Execute( );
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_MAKE_LTX:
		if( !Scene.locked() ){
            if( !Builder.InProgress() )
                if (frmScenePropertiesRun(&Scene.m_BuildParams,true)==mrOk)
                    Builder.MakeLTX( );
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
    case COMMAND_MAKE_DETAILS:
		if( !Scene.locked() ){
            if( !Builder.InProgress() )
				Builder.MakeDetails(true);
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
	case COMMAND_INVERT_SELECTION_ALL:
		if( !Scene.locked() ){
			Scene.InvertSelection(Tools.CurrentClassID());
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SELECT_ALL:
		if( !Scene.locked() ){
			Scene.SelectObjects(true,Tools.CurrentClassID());
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DESELECT_ALL:
		if( !Scene.locked() ){
			Scene.SelectObjects(false,Tools.CurrentClassID());
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DELETE_SELECTION:
		if( !Scene.locked() ){
			Scene.RemoveSelection( Tools.CurrentClassID() );
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_HIDE_UNSEL:
		if( !Scene.locked() ){
			Scene.ShowObjects( false, Tools.CurrentClassID(), true, false );
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_SEL:
		if( !Scene.locked() ){
			Scene.ShowObjects( bool(p1), Tools.CurrentClassID(), true, true );
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_ALL:
		if( !Scene.locked() ){
			Scene.ShowObjects( bool(p1), Tools.CurrentClassID(), false );
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_ZOOM_EXTENTS:
		if( !Scene.locked() ){
			Scene.ZoomExtents(p1);
		} else {
        	if (GetEState()==esEditLibrary){
            	TfrmEditLibrary::ZoomObject();
            }else{
                ELog.DlgMsg( mtError, "Scene sharing violation" );
                bRes = false;
            }
        }
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
    case COMMAND_LOCK_ALL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),Tools.CurrentClassID(),false);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_LOCK_SEL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),Tools.CurrentClassID(),true,true);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_LOCK_UNSEL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),Tools.CurrentClassID(),true,false);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_RENDER_FOCUS:
		if (frmMain->Visible&&g_bEditorValid)
        	m_D3DWindow->SetFocus();
    	break;
    case COMMAND_UPDATE_CAPTION:
    	frmMain->UpdateCaption();
    	break;
	case COMMAND_BREAK_LAST_OPERATION:
    	NeedBreak();
    	break;
    case COMMAND_GROUP_CREATE:
		if( !Scene.locked() ){
	    	Scene.GroupCreate(true);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
	    break;
    case COMMAND_GROUP_SAVE:
		if( !Scene.locked() ){
	    	Scene.GroupSave();
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
    case COMMAND_GROUP_DESTROY:
		if( !Scene.locked() ){
	    	Scene.GroupDestroy();
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_GROUP_DESTROYALL:
		if( !Scene.locked() ){
	    	Scene.UngroupAll();
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_RESET_ANIMATION:
		if( !Scene.locked() ){
	    	Scene.ResetAnimation();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
    case COMMAND_SET_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	int cnt=Scene.SetSnapList();
 			Scene.UndoSave();
        	ELog.Msg( mtInformation, "%d snap object(s) selected.", cnt );
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
    case COMMAND_ADD_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	int cnt=Scene.AddToSnapList();
 			Scene.UndoSave();
        	ELog.Msg( mtInformation, "%d object(s) appended.", cnt );
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_CLEAR_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	Scene.ClearSnapList();
 			Scene.UndoSave();
	       	ELog.Msg( mtInformation, "Snap list empty.");
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
	case COMMAND_UPDATE_TOOLBAR:
    	fraLeftBar->UpdateBar();
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
        float& val = frmEditorPreferences->seGridSquareSize->Value;
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
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
		}

    RedrawScene();
    return bRes;
}

void TUI::ApplyShortCut(WORD Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE)   		Command(COMMAND_CHANGE_ACTION, eaSelect);
    if (Shift.Contains(ssCtrl)){
        if (Key=='V')    		Command(COMMAND_PASTE);
        else if (Key=='C')    	Command(COMMAND_COPY);
        else if (Key=='X')    	Command(COMMAND_CUT);
        else if (Key=='Z')    	Command(COMMAND_UNDO);
        else if (Key=='Y')    	Command(COMMAND_REDO);
        else if (Key==VK_F5)    Command(COMMAND_BUILD);
        else if (Key==VK_F7)    Command(COMMAND_OPTIONS);
        else if (Key=='A')    	Command(COMMAND_SELECT_ALL);
        else if (Key=='I')    	Command(COMMAND_INVERT_SELECTION_ALL);
        else if (Key=='O')   	Command(COMMAND_LOAD);
        else if (Key=='N')   	Command(COMMAND_CLEAR);
        else if (Key=='S'){ 	if (Shift.Contains(ssAlt))  Command(COMMAND_SAVEAS);
					            else                        Command(COMMAND_SAVE);}
       	else if (Key=='1') 	 	Command(COMMAND_CHANGE_TARGET, etEvent);
		else if (Key=='2')		Command(COMMAND_CHANGE_TARGET, etPS);
    }else{
        if (Shift.Contains(ssAlt)){
        }else{
            if (Key=='1')     	Command(COMMAND_CHANGE_TARGET, etObject);
        	else if (Key=='2')  Command(COMMAND_CHANGE_TARGET, etLight);
        	else if (Key=='3')  Command(COMMAND_CHANGE_TARGET, etSound);
        	else if (Key=='4')  Command(COMMAND_CHANGE_TARGET, etOccluder);
        	else if (Key=='5')  Command(COMMAND_CHANGE_TARGET, etGlow);
        	else if (Key=='6')  Command(COMMAND_CHANGE_TARGET, etDO);
        	else if (Key=='7')  Command(COMMAND_CHANGE_TARGET, etRPoint);
        	else if (Key=='8')  Command(COMMAND_CHANGE_TARGET, etAITPoint);
        	else if (Key=='9')  Command(COMMAND_CHANGE_TARGET, etSector);
        	else if (Key=='0')  Command(COMMAND_CHANGE_TARGET, etPortal);
            // simple press
        	else if (Key=='S')	Command(COMMAND_CHANGE_ACTION, eaSelect);
        	else if (Key=='A')	Command(COMMAND_CHANGE_ACTION, eaAdd);
        	else if (Key=='T')	Command(COMMAND_CHANGE_ACTION, eaMove);
        	else if (Key=='Y')	Command(COMMAND_CHANGE_ACTION, eaRotate);
        	else if (Key=='H')	Command(COMMAND_CHANGE_ACTION, eaScale);
        	else if (Key=='Z')	Command(COMMAND_CHANGE_AXIS,   eAxisX);
        	else if (Key=='X')	Command(COMMAND_CHANGE_AXIS,   eAxisY);
        	else if (Key=='C')	Command(COMMAND_CHANGE_AXIS,   eAxisZ);
        	else if (Key=='V')	Command(COMMAND_CHANGE_AXIS,   eAxisZX);
        	else if (Key=='O')	Command(COMMAND_CHANGE_SNAP,   (int)fraTopBar->ebOSnap);
        	else if (Key=='G')	Command(COMMAND_CHANGE_SNAP,   (int)fraTopBar->ebGSnap);
        	else if (Key=='P')	Command(COMMAND_EDITOR_PREF);
        	else if (Key=='W')	Command(COMMAND_OBJECT_LIST);
        	else if (Key==VK_DELETE)Command(COMMAND_DELETE_SELECTION);
        	else if (Key==VK_RETURN)Command(COMMAND_SHOWPROPERTIES);
            else if (Key==VK_OEM_4)	Command(COMMAND_GRID_SLOT_SIZE,false);
            else if (Key==VK_OEM_6)	Command(COMMAND_GRID_SLOT_SIZE,true);
        }
    }
}
//---------------------------------------------------------------------------

void TUI::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    if (Shift.Contains(ssCtrl)){
        if (Key=='S'){
            if (Shift.Contains(ssAlt))  Command(COMMAND_SAVEAS);
            else                        Command(COMMAND_SAVE);
        }
        else if (Key=='O')   			Command(COMMAND_LOAD);
        else if (Key=='N')   			Command(COMMAND_CLEAR);
    }
    if (Key==VK_OEM_3)		  			Command(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------


