//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_LevelMain.h"

#include "UI_LevelTools.h"
#include "EditLibrary.h"
#include "ImageEditor.h"
#include "leftbar.h"
#include "topbar.h"
#include "scene.h"
#include "sceneobject.h"
#include "Cursor3D.h"
#include "bottombar.h"
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "EditLightAnim.h"
#include "folderLib.h"
#include "sceneproperties.h"
#include "builder.h"
#include "SoundManager.h"
#include "NumericVector.h"
#include "EditorPreferences.h"

#ifdef _LEVEL_EDITOR
//.    if (m_Cursor->GetVisible()) RedrawScene();
#endif

CLevelMain*&	LUI=(CLevelMain*)UI;

CLevelMain::CLevelMain()
{
    m_Cursor        = xr_new<C3DCursor>();
}

CLevelMain::~CLevelMain()
{
    xr_delete(m_Cursor);
}

bool CLevelMain::CommandExt(int _Command, int p1, int p2)
{
	bool bRes = true;
	switch (_Command){
    case COMMAND_CHANGE_VIEW:
//    	Scene.ChangeView
    	break;
	case COMMAND_CHANGE_TARGET:
	  	LTools->SetTarget((EObjClass)p1);
        Command(COMMAND_UPDATE_PROPERTIES);
        break;
	case COMMAND_SHOW_OBJECTLIST:
        if (GetEState()==esEditScene) LTools->ShowObjectList();
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
    case COMMAND_FILE_MENU:
		FHelper.ShowPPMenu(fraLeftBar->pmSceneFile,0);
    	break;
	case COMMAND_LOAD:
		if( !Scene.locked() ){
            AnsiString temp_fn	= AnsiString((char*)p1).LowerCase();
			if( p1 || EFS.GetOpenName( _maps_, temp_fn ) ){
                if (1==temp_fn.Pos(FS.get_path(_maps_)->m_Path))
                    temp_fn = AnsiString(temp_fn.c_str()+strlen(FS.get_path(_maps_)->m_Path)).LowerCase();
                
                if (!Scene.IfModified()){
                	bRes=false;
                    break;
                }
                if (0!=temp_fn.AnsiCompareIC(m_LastFileName)&&EFS.CheckLocking(_maps_,temp_fn.c_str(),false,true)){
                	bRes=false;
                    break;
                }
                if (0==temp_fn.AnsiCompareIC(m_LastFileName)&&EFS.CheckLocking(_maps_,temp_fn.c_str(),true,false)){
	                EFS.UnlockFile(_maps_,temp_fn.c_str());
                }
                SetStatus("Level loading...");
            	Command			(COMMAND_CLEAR);
				if (Scene.Load	(_maps_, temp_fn.c_str(), false)){
                    m_LastFileName	= temp_fn;
                    ResetStatus		();
                    Scene.UndoClear	();
                    Scene.UndoSave	();
                    Scene.m_RTFlags.set(EScene::flRT_Unsaved|EScene::flRT_Modified,FALSE);
				    Command			(COMMAND_UPDATE_CAPTION);
    	            Command			(COMMAND_CHANGE_ACTION,etaSelect);
        	        // lock
	                EFS.LockFile	(_maps_,temp_fn.c_str());
    	            EPrefs.AppendRecentFile(temp_fn.c_str());
                }else{
					ELog.DlgMsg	( mtError, "Can't load map '%s'", temp_fn.c_str() );
                }
                // update props
		        Command			(COMMAND_UPDATE_PROPERTIES);
                RedrawScene		();
			}
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_RELOAD:
		if( !Scene.locked() ){
        	Command(COMMAND_LOAD,(int)m_LastFileName.c_str());
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SAVE:
		if( !Scene.locked() ){
			if( !m_LastFileName.IsEmpty() ){
                SetStatus		("Level saving...");
				Scene.Save		(_maps_, m_LastFileName.c_str(), false);
				ResetStatus		();
//                Scene.m_Modified = false;
			    Command(COMMAND_UPDATE_CAPTION);
			}else{
				bRes = Command( COMMAND_SAVEAS ); }
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

    case COMMAND_SAVE_BACKUP:{
    	AnsiString fn = AnsiString(Core.CompName)+"_"+Core.UserName+"_backup.level";
        FS.update_path(_maps_,fn);
    	Command(COMMAND_SAVEAS,(int)fn.c_str());
    }break;
	case COMMAND_SAVEAS:
		if( !Scene.locked() ){
        	AnsiString temp_fn	= AnsiString((char*)p1).LowerCase();
			if(p1 || EFS.GetSaveName( _maps_, temp_fn ) ){
                if (1==temp_fn.Pos(FS.get_path(_maps_)->m_Path))
                    temp_fn = AnsiString(temp_fn.c_str()+strlen(FS.get_path(_maps_)->m_Path)).LowerCase();

                SetStatus		("Level saving...");
				Scene.Save		(_maps_, temp_fn.c_str(), false);
				ResetStatus		();
//.                Scene.m_Modified = false;
				// unlock
    	        EFS.UnlockFile	(_maps_,m_LastFileName.c_str());
                // set new name
                EFS.LockFile	(_maps_,temp_fn.c_str());
				m_LastFileName 	= temp_fn;
			    bRes 			= Command(COMMAND_UPDATE_CAPTION);
                EPrefs.AppendRecentFile(temp_fn.c_str());
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
            // backup map
            if (!m_LastFileName.IsEmpty()&&Scene.IsModified()){
	           	EFS.BackupFile(_maps_,m_LastFileName.c_str());
            }
			// unlock
			EFS.UnlockFile(_maps_,m_LastFileName.c_str());
			Device.m_Camera.Reset	();
			Scene.Unload			();
            Scene.m_LevelOp.Reset	();
			m_LastFileName 			= "";
           	Scene.UndoClear			();
			Command(COMMAND_UPDATE_CAPTION);
			Command(COMMAND_CHANGE_TARGET,OBJCLASS_SCENEOBJECT,TRUE);
			Command(COMMAND_CHANGE_ACTION,etaSelect);
		    Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        Command(COMMAND_UPDATE_PROPERTIES);
		break;

    case COMMAND_LOAD_FIRSTRECENT:
    	if (EPrefs.FirstRecentFile())
        	bRes = Command(COMMAND_LOAD,(int)EPrefs.FirstRecentFile());
    	break;

	case COMMAND_CLEAR_COMPILER_ERROR:
    	Scene.ClearCompilerErrors();
        break;

    case COMMAND_IMPORT_COMPILER_ERROR:{
    	AnsiString fn;
    	if(EFS.GetOpenName("$logs$", fn, false, NULL, 0)){
        	Scene.LoadCompilerError(fn.c_str());
        }
    	}break;

	case COMMAND_VALIDATE_SCENE:
		if( !Scene.locked() ){
            Scene.Validate(true,false,true,true,true);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
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
			Scene.CutSelection(LTools->CurrentClassID());
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
			Scene.CopySelection(LTools->CurrentClassID());
            fraLeftBar->miPaste->Enabled = true;
            fraLeftBar->miPaste2->Enabled = true;
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_PASTE:
		if( !Scene.locked() ){
			Scene.PasteSelection();
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_LOAD_SELECTION:
		if( !Scene.locked() ){
        	AnsiString fn;
			if( EFS.GetOpenName( _maps_, fn ) ){
                SetStatus		("Fragment loading...");
				Scene.LoadSelection	(0,fn.c_str());
				ResetStatus		();
				Scene.UndoSave	();
                Command			(COMMAND_CHANGE_ACTION,etaSelect);
		        Command			(COMMAND_UPDATE_PROPERTIES);
                RedrawScene		();
			}
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
	    break;
        
	case COMMAND_SAVE_SELECTION:
		if( !Scene.locked() ){
        	AnsiString fn;
			if( EFS.GetSaveName	( _maps_, fn ) ){
                SetStatus		("Fragment saving...");
				Scene.SaveSelection(LTools->CurrentClassID(),0,fn.c_str());
				ResetStatus		();
			}
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;

	case COMMAND_UNDO:
		if( !Scene.locked() ){
			if( !Scene.Undo() ) ELog.DlgMsg( mtInformation, "Undo buffer empty" );
            else{
	            LTools->Reset();
			    Command(COMMAND_CHANGE_ACTION, etaSelect);
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
	            LTools->Reset();
			    Command(COMMAND_CHANGE_ACTION, etaSelect);
            }
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SCENE_SUMMARY:
		if( !Scene.locked() ){
        	Scene.ShowSummaryInfo();
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
    case COMMAND_MAKE_AIMAP:
		if( !Scene.locked() ){
			if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export ai-map?"))
                Builder.MakeAIMap( );
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
	case COMMAND_MAKE_GAME:
		if( !Scene.locked() ){
			if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export game?"))
                Builder.MakeGame( );
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
    case COMMAND_MAKE_DETAILS:
		if( !Scene.locked() ){
			if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export details?"))
	            Builder.MakeDetails();
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
	case COMMAND_MAKE_HOM:
		if( !Scene.locked() ){
			if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export HOM?"))
	            Builder.MakeHOM();
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
	case COMMAND_INVERT_SELECTION_ALL:
		if( !Scene.locked() ){
			Scene.InvertSelection(LTools->CurrentClassID());
//.			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SELECT_ALL:
		if( !Scene.locked() ){
			Scene.SelectObjects(true,LTools->CurrentClassID());
//.			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DESELECT_ALL:
		if( !Scene.locked() ){
			Scene.SelectObjects(false,LTools->CurrentClassID());
//			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DELETE_SELECTION:
		if( !Scene.locked() ){
			Scene.RemoveSelection( LTools->CurrentClassID() );
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_HIDE_UNSEL:
		if( !Scene.locked() ){
			Scene.ShowObjects( false, LTools->CurrentClassID(), true, false );
			Scene.UndoSave();
	        Command(COMMAND_UPDATE_PROPERTIES);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_SEL:
		if( !Scene.locked() ){
			Scene.ShowObjects( bool(p1), LTools->CurrentClassID(), true, true );
			Scene.UndoSave();
	        Command(COMMAND_UPDATE_PROPERTIES);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_ALL:
		if( !Scene.locked() ){
			Scene.ShowObjects( bool(p1), LTools->CurrentClassID(), false );
			Scene.UndoSave();
	        Command(COMMAND_UPDATE_PROPERTIES);
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
    case COMMAND_LOCK_ALL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),LTools->CurrentClassID(),false);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_LOCK_SEL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),LTools->CurrentClassID(),true,true);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_LOCK_UNSEL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),LTools->CurrentClassID(),true,false);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
/*    case COMMAND_RESET_ANIMATION:
		if( !Scene.locked() ){
	    	Scene.ResetAnimation();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
*/
		case COMMAND_SET_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	Scene.SetSnapList();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
    case COMMAND_ADD_SEL_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	Scene.AddSelToSnapList();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_DEL_SEL_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	Scene.DelSelFromSnapList();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_CLEAR_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	Scene.ClearSnapList(true);
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_SELECT_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	Scene.SelectSnapList();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
    case COMMAND_REFRESH_SNAP_OBJECTS:
    	fraLeftBar->UpdateSnapList();
    	break;
    case COMMAND_REFRESH_SOUND_ENVS:
    	::Sound->refresh_env_library();
//		::Sound->_restart();
    	break;
    case COMMAND_REFRESH_SOUND_ENV_GEOMETRY:
    	SndLib.RefreshEnvGeometry();
    	break;
    case COMMAND_MUTE_SOUND:
    	SndLib.MuteSounds(p1);
    	break;
    case COMMAND_MOVE_CAMERA_TO:{
    	Fvector pos=Device.m_Camera.GetPosition();
		if (NumericVectorRun("Move to",&pos,3))
        	Device.m_Camera.Set(Device.m_Camera.GetHPB(),pos);
    	}break;
	case COMMAND_SHOWCONTEXTMENU:
    	ShowContextMenu(EObjClass(p1));
        break;
//------        
    case COMMAND_REFRESH_UI_BAR:
        fraTopBar->RefreshBar	();
        fraLeftBar->RefreshBar	();
        fraBottomBar->RefreshBar();
        break;
    case COMMAND_RESTORE_UI_BAR:
        fraTopBar->fsStorage->RestoreFormPlacement();
        fraLeftBar->fsStorage->RestoreFormPlacement();
        fraBottomBar->fsStorage->RestoreFormPlacement();
        break;
    case COMMAND_SAVE_UI_BAR:
        fraTopBar->fsStorage->SaveFormPlacement();
        fraLeftBar->fsStorage->SaveFormPlacement();
        fraBottomBar->fsStorage->SaveFormPlacement();
        break;
	case COMMAND_UPDATE_TOOLBAR:
    	fraLeftBar->UpdateBar();
    	break;
    case COMMAND_UPDATE_CAPTION:
    	frmMain->UpdateCaption();
    	break;
//------
    case COMMAND_CREATE_SOUND_LIB:
    	SndLib		= xr_new<CLevelSoundManager>();
    	break;
    default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
    }
    return 	bRes;
}

char* CLevelMain::GetCaption()
{
	return m_LastFileName.IsEmpty()?"noname":m_LastFileName.c_str();
}

#define COMMAND0(cmd)		{Command(cmd);bExec=true;}
#define COMMAND1(cmd,p0)	{Command(cmd,p0);bExec=true;}

bool __fastcall CLevelMain::ApplyShortCutExt(WORD Key, TShiftState Shift)
{
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
        if (Shift.Contains(ssShift)){
            if (Key==VK_F5)    			COMMAND0(COMMAND_MAKE_GAME)
        }else{
            if (Key==VK_F5)    			COMMAND0(COMMAND_BUILD)                		
            else if (Key==VK_F7)   		COMMAND0(COMMAND_OPTIONS)                      
            else if (Key=='A')    		COMMAND0(COMMAND_SELECT_ALL)                   
            else if (Key=='T')    		COMMAND0(COMMAND_MOVE_CAMERA_TO)                   
            else if (Key=='I')    		COMMAND0(COMMAND_INVERT_SELECTION_ALL)         
            else if (Key=='1') 	 		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_GROUP)
            else if (Key=='2')			COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_PS)          
            else if (Key=='3')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_DO)          
            else if (Key=='4')			COMMAND1(COMMAND_CHANGE_TARGET,	OBJCLASS_AIMAP)		
            else if (Key=='W')			COMMAND0(COMMAND_SHOW_OBJECTLIST)              
        }
    }else{
        if (Shift.Contains(ssAlt)){
        	if (Key=='F')   			COMMAND0(COMMAND_FILE_MENU)                    
        }else{
            if (Key=='1')     			COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_SCENEOBJECT)      
            else if (Key=='2')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_LIGHT)       
            else if (Key=='3')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_SOUND_SRC)    
            else if (Key=='4')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_SOUND_ENV)    
            else if (Key=='5')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_GLOW)        
            else if (Key=='6')			COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_SHAPE)       
            else if (Key=='7')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_SPAWNPOINT)  
            else if (Key=='8')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_WAY)         
            else if (Key=='9')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_SECTOR)      
            else if (Key=='0')  		COMMAND1(COMMAND_CHANGE_TARGET, OBJCLASS_PORTAL)      
            // simple press
            else if (Key==VK_DELETE)	COMMAND0(COMMAND_DELETE_SELECTION)             
            else if (Key==VK_RETURN)	COMMAND0(COMMAND_SHOW_PROPERTIES)              
            else if (Key==VK_OEM_MINUS)	COMMAND1(COMMAND_HIDE_SEL, FALSE)              
            else if (Key==VK_OEM_PLUS)	COMMAND1(COMMAND_HIDE_UNSEL, FALSE)            
            else if (Key==VK_OEM_5)		COMMAND1(COMMAND_HIDE_ALL, TRUE)               
        }
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall CLevelMain::ApplyGlobalShortCutExt(WORD Key, TShiftState Shift)
{
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
        if (Shift.Contains(ssShift)){
            if (Key=='S')				COMMAND0(COMMAND_SAVE_SELECTION)
	        else if (Key=='O')   		COMMAND0(COMMAND_LOAD_SELECTION)
        }else{
            if (Key=='V')    			COMMAND0(COMMAND_PASTE)
            else if (Key=='C')    		COMMAND0(COMMAND_COPY)
            else if (Key=='X')    		COMMAND0(COMMAND_CUT)
            else if (Key=='Z')    		COMMAND0(COMMAND_UNDO)
            else if (Key=='Y')    		COMMAND0(COMMAND_REDO)
            else if (Key=='R')			COMMAND0(COMMAND_LOAD_FIRSTRECENT)
        }
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool CLevelMain::PickGround(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap, Fvector* hitnormal){
	VERIFY(m_bReady);
    // pick object geometry
    if ((bSnap==-1)||(Tools->GetSettings(etfOSnap)&&(bSnap==1))){
        bool bPickObject;
        SRayPickInfo pinf;
	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary:{
        	bPickObject = !!TfrmEditLibrary::RayPick(start,direction,&pinf);
        }break;
        case esEditScene:{
        	bPickObject = !!Scene.RayPickObject(pinf.inf.range, start,direction,OBJCLASS_SCENEOBJECT,&pinf,Scene.GetSnapList(false)); break;
        }
        default: return false;
        }
        if (bPickObject){
		    if (Tools->GetSettings(etfVSnap)&&bSnap){
                Fvector pn;
                float u = pinf.inf.u;
                float v = pinf.inf.v;
                float w = 1-(u+v);
				Fvector verts[3];
                pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
                if ((w>u) && (w>v)) pn.set(verts[0]);
                else if ((u>w) && (u>v)) pn.set(verts[1]);
                else pn.set(verts[2]);
                if (pn.distance_to(pinf.pt) < LTools->m_MoveSnap) hitpoint.set(pn);
                else hitpoint.set(pinf.pt);
            }else{
            	hitpoint.set(pinf.pt);
            }
            if (hitnormal){
	            Fvector verts[3];
    	        pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
        	    hitnormal->mknormal(verts[0],verts[1],verts[2]);
            }
			return true;
        }
    }

    // pick grid
	Fvector normal;
	normal.set( 0, 1, 0 );
	float clcheck = direction.dotproduct( normal );
	if( fis_zero( clcheck ) ) return false;
	float alpha = - start.dotproduct(normal) / clcheck;
	if( alpha <= 0 ) return false;

	hitpoint.x = start.x + direction.x * alpha;
	hitpoint.y = start.y + direction.y * alpha;
	hitpoint.z = start.z + direction.z * alpha;

    if (Tools->GetSettings(etfGSnap) && bSnap){
        hitpoint.x = snapto( hitpoint.x, LTools->m_MoveSnap );
        hitpoint.z = snapto( hitpoint.z, LTools->m_MoveSnap );
        hitpoint.y = 0.f;
    }
	if (hitnormal) hitnormal->set(0,1,0);
	return true;
}
//----------------------------------------------------

bool CLevelMain::SelectionFrustum(CFrustum& frustum)
{
	VERIFY(m_bReady);
    Fvector st,d,p[4];
    Ivector2 pt[4];

    float depth = 0;

    float x1=m_StartCp.x, x2=m_CurrentCp.x;
    float y1=m_StartCp.y, y2=m_CurrentCp.y;

	if(!(x1!=x2&&y1!=y2)) return false;

	pt[0].set(_min(x1,x2),_min(y1,y2));
	pt[1].set(_max(x1,x2),_min(y1,y2));
	pt[2].set(_max(x1,x2),_max(y1,y2));
	pt[3].set(_min(x1,x2),_max(y1,y2));

    SRayPickInfo pinf;
    for (int i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        if (EPrefs.bp_lim_depth){
			pinf.inf.range = Device.m_Camera._Zfar(); // max pick range
            if (Scene.RayPickObject(pinf.inf.range, st, d, OBJCLASS_SCENEOBJECT, &pinf, 0))
	            if (pinf.inf.range > depth) depth = pinf.inf.range;
        }
    }
    if (depth<Device.m_Camera._Znear()) depth = Device.m_Camera._Zfar();
    else depth += EPrefs.bp_depth_tolerance;

    for (i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        p[i].mad(st,d,depth);
    }

    frustum.CreateFromPoints(p,4,Device.m_Camera.GetPosition());

    Fplane P; P.build(p[0],p[1],p[2]);
    if (P.classify(st)>0) P.build(p[2],p[1],p[0]);
	frustum._add(P);

	return true;
}
//----------------------------------------------------
void CLevelMain::RealUpdateScene()
{
	if (GetEState()==esEditScene){
	    Scene.OnObjectsUpdate();
    	LTools->OnObjectsUpdate(); // �������� ��� ��� ���-�� ������� � ���������
	    RedrawScene();
    }
    m_Flags.set(flUpdateScene,FALSE);
}
//---------------------------------------------------------------------------


void CLevelMain::ShowContextMenu(int cls)
{
	VERIFY(m_bReady);
    POINT pt;
    GetCursorPos(&pt);
    fraLeftBar->miProperties->Enabled = false;
    if (Scene.SelectionCount( true, (EObjClass)cls )) fraLeftBar->miProperties->Enabled = true;
    RedrawScene(true);
    fraLeftBar->pmObjectContext->TrackButton = tbRightButton;
    fraLeftBar->pmObjectContext->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Common
//---------------------------------------------------------------------------
void CLevelMain::ResetStatus()
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=""){
	    fraBottomBar->paStatus->Caption=""; fraBottomBar->paStatus->Repaint();
    }
}
void CLevelMain::SetStatus(LPSTR s, bool bOutLog)
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=s){
	    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
    	if (bOutLog&&s&&s[0]) ELog.Msg(mtInformation,s);
    }
}
void CLevelMain::ProgressInfo(LPCSTR text, bool bWarn)
{
	if (text){
		fraBottomBar->paStatus->Caption=fraBottomBar->sProgressTitle+" ("+text+")";
    	fraBottomBar->paStatus->Repaint();
	    ELog.Msg(bWarn?mtError:mtInformation,fraBottomBar->paStatus->Caption.c_str());
    }
}
void CLevelMain::ProgressStart(float max_val, const char* text)
{
	VERIFY(m_bReady);
    fraBottomBar->sProgressTitle = text;
	fraBottomBar->paStatus->Caption=text;
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->fMaxVal=max_val;
	fraBottomBar->fStatusProgress=0;
	fraBottomBar->cgProgress->Progress=0;
	fraBottomBar->cgProgress->Visible=true;
    ELog.Msg(mtInformation,text);
}
void CLevelMain::ProgressEnd()
{
	VERIFY(m_bReady);
    fraBottomBar->sProgressTitle = "";
	fraBottomBar->paStatus->Caption="";
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->cgProgress->Visible=false;
}
void CLevelMain::ProgressUpdate(float val)
{
	VERIFY(m_bReady);
	fraBottomBar->fStatusProgress=val;
    if (fraBottomBar->fMaxVal>=0){
    	int new_val = (int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
        if (new_val!=fraBottomBar->cgProgress->Progress){
			fraBottomBar->cgProgress->Progress=(int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
    	    fraBottomBar->cgProgress->Repaint();
        }
    }
}
void CLevelMain::ProgressInc(const char* info, bool bWarn)
{
	VERIFY(m_bReady);
    ProgressInfo(info,bWarn);
	fraBottomBar->fStatusProgress++;
    if (fraBottomBar->fMaxVal>=0){
    	int val = (int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
        if (val!=fraBottomBar->cgProgress->Progress){
			fraBottomBar->cgProgress->Progress=val;
	        fraBottomBar->cgProgress->Repaint();
        }
    }
}
//---------------------------------------------------------------------------
void CLevelMain::OutCameraPos()
{
	VERIFY(m_bReady);
    AnsiString s;
	const Fvector& c 	= Device.m_Camera.GetPosition();
	s.sprintf("C: %3.1f, %3.1f, %3.1f",c.x,c.y,c.z);
//	const Fvector& hpb 	= Device.m_Camera.GetHPB();
//	s.sprintf(" Cam: %3.1f�, %3.1f�, %3.1f�",rad2deg(hpb.y),rad2deg(hpb.x),rad2deg(hpb.z));
    fraBottomBar->paCamera->Caption=s; fraBottomBar->paCamera->Repaint();
}
//---------------------------------------------------------------------------
void CLevelMain::OutUICursorPos()
{
	VERIFY(m_bReady);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void CLevelMain::OutGridSize()
{
	VERIFY(m_bReady);
    AnsiString s;
    s.sprintf("Grid: %1.1f",EPrefs.grid_cell_size);
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
void CLevelMain::OutInfo()
{
	fraBottomBar->paSel->Caption = Tools->GetInfo();
}
//---------------------------------------------------------------------------
void CLevelMain::RealQuit()
{
	frmMain->Close();
}
//---------------------------------------------------------------------------

