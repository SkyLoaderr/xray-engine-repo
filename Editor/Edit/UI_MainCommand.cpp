//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "EditLibrary.h"
#include "EditShaders.h"
#include "ObjectList.h"
#include "EditorPref.h"
#include "main.h"
#include "sceneproperties.h"
#include "EditorPref.h"
#include "EditParticles.h"
#include "ImageEditor.h"

#include "UI_Main.h"
#include "Scene.h"
#include "builder.h"

bool TUI::Command( int _Command, int p1 ){
	char filebuffer[MAX_PATH]="";

    bool bRes = true;
    
	switch( _Command ){

	case COMMAND_EXIT:{
	    EEditorState est = GetEState();
    	switch(est){
        case esEditLibrary: 	if(frmEditLibrary) frmEditLibrary->FinalClose(); break;
        case esEditShaders: 	if(frmEditShaders) frmEditShaders->FinalClose(); break;
        case esEditParticles:	TfrmEditParticles::FinalClose(); break;
        case esEditImages:		TfrmImageLib::HideImageLib(); break;
        case esEditScene:		if (!Scene->IfModified()) return false; break;
        }
        SetStatus("Editor unloading...");
        Clear();
        SetStatus("");
		}break;
	case COMMAND_SHOWPROPERTIES:
        m_Tools->ShowProperties();
        break;
	case COMMAND_SHOWCONTEXTMENU:
    	frmMain->ShowContextMenu(EObjClass(p1));
        break;
	case COMMAND_EDITOR_PREF:
	    frmEditorPreferences->ShowModal();
        break;
	case COMMAND_OBJECT_LIST:
		if( !Scene->locked() ){
	        frmObjectListShow();
        }else{
			Log->DlgMsg( mtError, "Scene sharing violation" );
        }
        break;
	case COMMAND_CHANGE_TARGET:
	  	m_Tools->ChangeTarget(p1);
        break;
	case COMMAND_CHANGE_ACTION:
		m_Tools->ChangeAction(p1);
        break;
    case COMMAND_LIBRARY_EDITOR:
        if (Scene->ObjCount()||(GetEState()!=esEditScene)){
        	if (GetEState()==esEditLibrary)	frmEditLibraryEditLibrary();
            else							Log->DlgMsg(mtError, "Scene must be empty before editing library!");
        }else{
            frmEditLibraryEditLibrary();
        }
        break;
    case COMMAND_SHADER_EDITOR:
        if (Scene->ObjCount()||(GetEState()!=esEditScene)){
        	if (GetEState()==esEditShaders)	frmEditShadersEditShaders();
            else							Log->DlgMsg(mtError, "Scene must be empty before editing shaders!");
        }else{
			frmEditShadersEditShaders();
        }
        break;
    case COMMAND_PARTICLE_EDITOR:
        if (Scene->ObjCount()||(GetEState()!=esEditScene)){
        	if (GetEState()==esEditParticles)	TfrmEditParticles::ShowEditor();
            else								Log->DlgMsg(mtError, "Scene must be empty before editing particles!");
        }else{
			TfrmEditParticles::ShowEditor();
        }
        break;
    case COMMAND_IMAGE_EDITOR:
        if (Scene->ObjCount()||(GetEState()!=esEditScene)){
        	if (GetEState()==esEditImages)	TfrmImageLib::EditImageLib(AnsiString("Image Editor"));
            else							Log->DlgMsg(mtError, "Scene must be empty before editing images!");
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
		if( !Scene->locked() ){
        	if (p1)	strcpy( filebuffer, (char*)p1 );
            else	strcpy( filebuffer, m_LastFileName );
			if( p1 || FS.GetOpenName( &FS.m_Maps, filebuffer ) ){
                if (!Scene->IfModified()) return false;
                SetStatus("Level loading...");
            	Command( COMMAND_CLEAR );
	            BeginEState(esSceneLocked);
				Scene->Load( filebuffer );
                EndEState();
				strcpy(m_LastFileName,filebuffer);
                SetStatus("");
              	Scene->UndoClear();
				Scene->UndoSave();
                Scene->m_Modified = false;
			    UI->Command(COMMAND_UPDATE_CAPTION);
                Command(COMMAND_CHANGE_ACTION,eaSelect);
			}
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_RELOAD:
		if( !Scene->locked() ){
        	UI->Command(COMMAND_LOAD,(int)m_LastFileName);
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SAVE:
		if( !Scene->locked() ){
			if( m_LastFileName[0] ){
	            BeginEState(esSceneLocked);
                SetStatus("Level saving...");
				Scene->Save( m_LastFileName, false );
                SetStatus("");
                Scene->m_Modified = false;
			    UI->Command(COMMAND_UPDATE_CAPTION);
                EndEState();
			}else{
				bRes = Command( COMMAND_SAVEAS ); }
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SAVEAS:
		if( !Scene->locked() ){
			filebuffer[0] = 0;
			if( FS.GetSaveName( &FS.m_Maps, filebuffer ) ){
	            BeginEState(esSceneLocked);
				Scene->Save( filebuffer, false );
				strcpy(m_LastFileName,filebuffer);
	            EndEState();
			    bRes = UI->Command(COMMAND_UPDATE_CAPTION);
			}else 
            	bRes = false;
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_CLEAR:
		if( !Scene->locked() ){
            if (!Scene->IfModified()) return false;
            BeginEState(esSceneLocked);
//            m_pTexturizer->Reset();
			Device.m_Camera.Reset();
			Scene->Unload();
            Scene->m_LevelOp.Reset();
			m_LastFileName[0] = 0;
            EndEState();
           	Scene->UndoClear();
            Scene->m_Modified = false;
			UI->Command(COMMAND_UPDATE_CAPTION);
			Command(COMMAND_CHANGE_ACTION,eaSelect);
		    Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_VALIDATE_SCENE:
		if( !Scene->locked() ){
            Scene->Validate(true,false);
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_REFRESH_TEXTURES:
		Device.RefreshTextures(bool(p1));
		break;

    case COMMAND_UNLOAD_LIBMESHES:
    	Lib->UnloadMeshes();
    	break;
        
    case COMMAND_RELOAD_LIBRARY:
    	Lib->ReloadLibrary();
    	break;

    case COMMAND_REFRESH_LIBRARY:
    	Lib->RefreshLibrary();
    	break;

	case COMMAND_CUT:
		if( !Scene->locked() ){
        	BeginEState(esSceneLocked);
			Scene->CutSelection(CurrentClassID());
        	EndEState();
            fraLeftBar->miPaste->Enabled = true;
            frmMain->miPaste->Enabled = true;
			Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_COPY:
		if( !Scene->locked() ){
        	BeginEState(esSceneLocked);
			Scene->CopySelection(CurrentClassID());
        	EndEState();
            fraLeftBar->miPaste->Enabled = true;
            frmMain->miPaste->Enabled = true;
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_PASTE:
		if( !Scene->locked() ){
        	BeginEState(esSceneLocked);
			Scene->PasteSelection();
        	EndEState();
			Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_UNDO:
		if( !Scene->locked() ){
			if( !Scene->Undo() ) Log->DlgMsg( mtInformation, "Undo buffer empty" );
            m_Tools->Reset();
		    UI->Command(COMMAND_CHANGE_ACTION, eaSelect);
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_REDO:
		if( !Scene->locked() ){
			if( !Scene->Redo() ) Log->DlgMsg( mtInformation, "Redo buffer empty" );
            m_Tools->Reset();
		    UI->Command(COMMAND_CHANGE_ACTION, eaSelect);
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_OPTIONS:
		if( !Scene->locked() ){
            frmScenePropertiesRun(&Scene->m_BuildParams,false);
            Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_BUILD:
		if( !Scene->locked() ){
            if( !Builder->InProgress() )
                if (frmScenePropertiesRun(&Scene->m_BuildParams,true)==mrOk)
                    Builder->Execute( );
        }else{
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
        
	case COMMAND_MAKE_LTX:
		if( !Scene->locked() ){
            if( !Builder->InProgress() )
                if (frmScenePropertiesRun(&Scene->m_BuildParams,true)==mrOk)
                    Builder->MakeLTX( );
        }else{
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
    case COMMAND_MAKE_DETAILS:
		if( !Scene->locked() ){
            if( !Builder->InProgress() )
				Builder->MakeDetails(true);
        }else{
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
	case COMMAND_INVERT_SELECTION_ALL:
		if( !Scene->locked() ){
			Scene->InvertSelection(CurrentClassID());
			Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SELECT_ALL:
		if( !Scene->locked() ){
			Scene->SelectObjects(true,CurrentClassID());
			Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DESELECT_ALL:
		if( !Scene->locked() ){
			Scene->SelectObjects(false,CurrentClassID());
			Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DELETE_SELECTION:
		if( !Scene->locked() ){
			Scene->RemoveSelection( CurrentClassID() );
			Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_HIDE_UNSEL:
		if( !Scene->locked() ){
			Scene->ShowObjects( false, CurrentClassID(), true, false );
			Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_SEL:
		if( !Scene->locked() ){
			Scene->ShowObjects( bool(p1), CurrentClassID(), true, true );
			Scene->UndoSave();
		} else {
			Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_ALL:
		if( !Scene->locked() ){
			Scene->ShowObjects( bool(p1), CurrentClassID(), false );
			Scene->UndoSave();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_ZOOM_EXTENTS:
		if( !Scene->locked() ){
			Scene->ZoomExtents(p1);
		} else {
        	if (GetEState()==esEditLibrary){
            	frmEditLibrary->ZoomObject();   
            }else if (GetEState()==esEditShaders){
            	frmEditShaders->ZoomObject();
                }else{ 
                    Log->DlgMsg( mtError, "Scene sharing violation" );
                    bRes = false;
                }
        }
    	break;
    case COMMAND_SET_NUMERIC_POSITION:
    	m_Tools->SetNumPosition((SceneObject*)p1);
    	break;
    case COMMAND_SET_NUMERIC_ROTATION:
    	m_Tools->SetNumRotation((SceneObject*)p1);
    	break;
    case COMMAND_SET_NUMERIC_SCALE:
    	m_Tools->SetNumScale((SceneObject*)p1);
    	break;
    case COMMAND_LOCK_ALL:
		if( !Scene->locked() ){
			Scene->LockObjects(bool(p1),CurrentClassID(),false);
			Scene->UndoSave();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_LOCK_SEL:
		if( !Scene->locked() ){
			Scene->LockObjects(bool(p1),CurrentClassID(),true,true);
			Scene->UndoSave();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_LOCK_UNSEL:
		if( !Scene->locked() ){
			Scene->LockObjects(bool(p1),CurrentClassID(),true,false);
			Scene->UndoSave();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_RENDER_FOCUS:
		if (frmMain->Visible) 
        	m_D3DWindow->SetFocus();
    	break;
    case COMMAND_UPDATE_CAPTION:
    	frmMain->UpdateCaption();
    	break;
	case COMMAND_BREAK_LAST_OPERATION:{
		EEditorState est = GetEState();
		if (Builder&&(est==esBuildLevel)) Builder->BreakBuild();
    	}break;
    case COMMAND_GROUP_CREATE:
		if( !Scene->locked() ){
	    	Scene->GroupCreate(true);
			Scene->UndoSave();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
	    break;
    case COMMAND_GROUP_SAVE:
		if( !Scene->locked() ){
	    	Scene->GroupSave();
			Scene->UndoSave();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;    
    case COMMAND_GROUP_DESTROY:
		if( !Scene->locked() ){
	    	Scene->GroupDestroy();
			Scene->UndoSave();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_GROUP_DESTROYALL:
		if( !Scene->locked() ){
	    	Scene->UngroupAll();
			Scene->UndoSave();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_RESET_ANIMATION:
		if( !Scene->locked() ){
	    	Scene->ResetAnimation();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;    
    case COMMAND_CLEAN_LIBRARY:
		if( !Scene->locked() ){
	    	Lib->Clean();
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_EDIT_PREFERENCES:
	    frmEditorPreferences->Run();
    	break;
    case COMMAND_SET_SNAP_OBJECTS:
		if( !Scene->locked() ){
	    	int cnt=Scene->SetSnapList();
 			Scene->UndoSave();
        	Log->Msg( mtInformation, "%d snap object(s) selected.", cnt );
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
    case COMMAND_ADD_SNAP_OBJECTS:
		if( !Scene->locked() ){
	    	int cnt=Scene->AddToSnapList();
 			Scene->UndoSave();
        	Log->Msg( mtInformation, "%d object(s) appended.", cnt );
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_CLEAR_SNAP_OBJECTS:
		if( !Scene->locked() ){
	    	Scene->ClearSnapList();
 			Scene->UndoSave();
	       	Log->Msg( mtInformation, "Snap list empty.");
		}else{ 
        	Log->DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
   case COMMAND_UPDATE_TOOLBAR:
    	fraLeftBar->UpdateBar();
    	break;
 	default:
		Log->DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
		}

    RedrawScene();
    return bRes;
}


 