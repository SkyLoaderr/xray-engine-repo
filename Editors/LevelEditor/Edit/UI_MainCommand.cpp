//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "EditLibrary.h"
#include "EditLightAnim.h"
#include "LightAnimLibrary.h"
#include "ObjectList.h"
#include "EditorPref.h"
#include "main.h"
#include "sceneproperties.h"
#include "EditorPref.h"
#include "ImageEditor.h"
#include "d3dutils.h"

#include "builder.h"

#include "Scene.h"
#include "PSLibrary.h"
#include "Library.h"
#include "UI_Tools.h"
#include "folderlib.h"
#include "ui_main.h"

bool TUI::Command( int _Command, int p1, int p2 ){
	if ((_Command!=COMMAND_INITIALIZE)&&!m_bReady) return false;
	string256 filebuffer;

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
            Tools.OnCreate	();
			Scene.OnCreate	();
            PSLib.OnCreate	();
            Lib.OnCreate	();
            LALib.OnCreate	();

		    BeginEState		(esEditScene);

		    Command			(COMMAND_CLEAR);
			Command			(COMMAND_RENDER_FOCUS);
			Command			(COMMAND_CHANGE_TARGET, etObject);
			Command			(COMMAND_CHANGE_ACTION, eaSelect);
//			Command			(COMMAND_CHECK_TEXTURES);
        }else{
        	bRes = false;
        }
    	}break;
	case COMMAND_DESTROY:
		Command				(COMMAND_CLEAR);
		Scene.OnDestroy		();
        LALib.OnDestroy		();
    	PSLib.OnDestroy		();
		Lib.OnDestroy		();
		Tools.OnDestroy		();
        UI.OnDestroy		();
		Engine.Destroy		();
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
    case COMMAND_CHECK_MODIFIED:
    	bRes = Scene.IsModified();
		break;
	case COMMAND_QUIT:
    	Quit();
    	break;
	case COMMAND_EXIT:{
	    EEditorState est = GetEState();
    	switch(est){
        case esEditLightAnim: 	bRes = TfrmEditLightAnim::FinalClose(); break;
        case esEditLibrary: 	bRes = TfrmEditLibrary::FinalClose(); 	break;
        case esEditScene:		bRes = Scene.IfModified(); 				break;
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
    case COMMAND_LANIM_EDITOR:
    	TfrmEditLightAnim::ShowEditor();
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
    case COMMAND_FILE_MENU:
		FOLDER::ShowPPMenu(fraLeftBar->pmSceneFile,0);
    	break;
	case COMMAND_LOAD:
		if( !Scene.locked() ){
        	if (p1)	strcpy( filebuffer, (char*)p1 );
            else	strcpy( filebuffer, m_LastFileName );
			if( p1 || Engine.FS.GetOpenName( Engine.FS.m_Maps, filebuffer, sizeof(filebuffer) ) ){
                if (!Scene.IfModified()){
                	bRes=false;
                    break;
                }
                if ((0!=stricmp(filebuffer,m_LastFileName))&&Engine.FS.CheckLocking(0,filebuffer,false,true)){
                	bRes=false;
                    break;
                }
                if ((0==stricmp(filebuffer,m_LastFileName))&&Engine.FS.CheckLocking(0,filebuffer,true,false)){
	                Engine.FS.UnlockFile(0,filebuffer);
                }
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
                // lock
                Engine.FS.LockFile(0,filebuffer);
                fraLeftBar->AppendRecentFile(filebuffer);
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

    case COMMAND_SAVE_BACKUP:{
    	AnsiString fn = AnsiString(Engine.FS.m_UserName)+"_backup.level";
        Engine.FS.m_Maps.Update(fn);
    	Command(COMMAND_SAVEAS,(int)fn.c_str());
    }break;
	case COMMAND_SAVEAS:
		if( !Scene.locked() ){
			filebuffer[0] = 0;
			if(p1 || Engine.FS.GetSaveName( Engine.FS.m_Maps, filebuffer, sizeof(filebuffer) ) ){
            	if (p1)	strcpy(filebuffer,(LPCSTR)p1);
	            BeginEState(esSceneLocked);
                SetStatus("Level saving...");
				Scene.Save( filebuffer, false );
                SetStatus("");
                Scene.m_Modified = false;
				// unlock
    	        Engine.FS.UnlockFile(0,m_LastFileName);
                // set new name
                Engine.FS.LockFile(0,filebuffer);
				strcpy(m_LastFileName,filebuffer);
			    bRes = Command(COMMAND_UPDATE_CAPTION);
	            EndEState();
                fraLeftBar->AppendRecentFile(filebuffer);
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
			// unlock
			Engine.FS.UnlockFile(0,m_LastFileName);
			Device.m_Camera.Reset();
			Scene.Unload();
            Scene.m_LevelOp.Reset();
			m_LastFileName[0] = 0;
            EndEState();
           	Scene.UndoClear();
            Scene.m_Modified = false;
			Command(COMMAND_UPDATE_CAPTION);
			Command(COMMAND_CHANGE_TARGET,etObject);
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

    case COMMAND_UNLOAD_TEXTURES:
		Device.UnloadTextures();
    	break;

    case COMMAND_REFRESH_LIBRARY:
        if (!Scene.ObjCount()&&!Scene.locked()){
	    	Lib.RefreshLibrary();
        }else{
            ELog.DlgMsg(mtError, "Scene must be empty before refreshing library!");
			bRes = false;
        }
    	break;

    case COMMAND_RELOAD_OBJECTS:
    	Lib.ReloadObjects();
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
            else{
	            Tools.Reset();
			    Command(COMMAND_CHANGE_ACTION, eaSelect);
            }
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_REDO:
		if( !Scene.locked() ){
			if( !Scene.Redo() ) ELog.DlgMsg( mtInformation, "Redo buffer empty" );
            else{
	            Tools.Reset();
			    Command(COMMAND_CHANGE_ACTION, eaSelect);
            }
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_OPTIONS:
		if( !Scene.locked() ){
            if (mrOk==frmScenePropertiesRun(&Scene.m_LevelOp.m_BuildParams,false))
            	Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_BUILD:
		if( !Scene.locked() ){
            if (frmScenePropertiesRun(&Scene.m_LevelOp.m_BuildParams,true)==mrOk)
                Builder.Compile( );
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_MAKE_GAME:
		if( !Scene.locked() ){
            if (frmScenePropertiesRun(&Scene.m_LevelOp.m_BuildParams,true)==mrOk)
                Builder.MakeGame( );
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
    case COMMAND_MAKE_DETAILS:
		if( !Scene.locked() ){
            Builder.MakeDetails();
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
    case COMMAND_LOAD_FIRSTRECENT:
    	if (fraLeftBar->FirstRecentFile()){
        	bRes = Command(COMMAND_LOAD,(int)fraLeftBar->FirstRecentFile());
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
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
		}

    RedrawScene();
    return bRes;
}

void TUI::ApplyShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);
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
       	else if (Key=='1') 	 	Command(COMMAND_CHANGE_TARGET, etGroup);
		else if (Key=='2')		Command(COMMAND_CHANGE_TARGET, etPS);
		else if (Key=='R')		Command(COMMAND_LOAD_FIRSTRECENT);
    }else{
        if (Shift.Contains(ssAlt)){
        	if (Key=='F')   	Command(COMMAND_FILE_MENU);
        }else{
            if (Key=='1')     	Command(COMMAND_CHANGE_TARGET, etObject);
        	else if (Key=='2')  Command(COMMAND_CHANGE_TARGET, etLight);
        	else if (Key=='3')  Command(COMMAND_CHANGE_TARGET, etSound);
        	else if (Key=='4')  Command(COMMAND_CHANGE_TARGET, etEvent);
        	else if (Key=='5')  Command(COMMAND_CHANGE_TARGET, etGlow);
        	else if (Key=='6')  Command(COMMAND_CHANGE_TARGET, etDO);
        	else if (Key=='7')  Command(COMMAND_CHANGE_TARGET, etSpawnPoint);
        	else if (Key=='8')  Command(COMMAND_CHANGE_TARGET, etWay);
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
            else if (Key==VK_OEM_MINUS)	Command(COMMAND_HIDE_SEL, FALSE);
            else if (Key==VK_OEM_PLUS)	Command(COMMAND_HIDE_UNSEL, FALSE);
            else if (Key==VK_OEM_5)		Command(COMMAND_HIDE_ALL, TRUE);
        	else if (Key=='N'){
            	switch (Tools.GetAction()){
            	case eaMove: 	UI.Command(COMMAND_SET_NUMERIC_POSITION); 	break;
			    case eaRotate: 	UI.Command(COMMAND_SET_NUMERIC_ROTATION); 	break;
			    case eaScale: 	UI.Command(COMMAND_SET_NUMERIC_SCALE); 		break;
            	}
            }
        }
    }
}
//---------------------------------------------------------------------------

void TUI::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);
    if (Shift.Contains(ssCtrl)){
        if (Key=='S'){
            if (Shift.Contains(ssAlt))  Command(COMMAND_SAVEAS);
            else                        Command(COMMAND_SAVE);
        }
        else if (Key=='O')   			Command(COMMAND_LOAD);
        else if (Key=='N')   			Command(COMMAND_CLEAR);
    }
    if ((Key==VK_OEM_3)||(Key==VK_SHIFT))Command(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------

char* TUI::GetCaption()
{
	return GetEditFileName()[0]?GetEditFileName():"noname";
}
char* TUI::GetTitle()
{
	return "Level Editor";
}


