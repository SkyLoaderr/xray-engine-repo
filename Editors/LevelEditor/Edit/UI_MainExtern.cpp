//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_MainExtern.h"

#include "UI_Tools.h"
#include "EditLibrary.h"
#include "ImageEditor.h"
#include "topbar.h"
#include "leftbar.h"
#include "scene.h"
#include "sceneobject.h"
#include "EditorPref.h"
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

bool TUI::CommandExt(int _Command, int p1, int p2)
{
	bool bRes = true;
	switch (_Command){
    case COMMAND_CHANGE_VIEW:
//    	Scene.ChangeView
    	break;
	case COMMAND_CHANGE_TARGET:
	  	Tools.ChangeTarget(p1,p2);
        Command(COMMAND_UPDATE_PROPERTIES);
        break;
	case COMMAND_SHOW_OBJECTLIST:
        if (GetEState()==esEditScene) Tools.ShowObjectList();
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
    	            Command			(COMMAND_CHANGE_ACTION,eaSelect);
        	        // lock
	                EFS.LockFile	(_maps_,temp_fn.c_str());
    	            AppendRecentFile(temp_fn.c_str());
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
                AppendRecentFile(temp_fn.c_str());
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
			Command(COMMAND_CHANGE_ACTION,eaSelect);
		    Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        Command(COMMAND_UPDATE_PROPERTIES);
		break;

    case COMMAND_LOAD_FIRSTRECENT:
    	if (FirstRecentFile())
        	bRes = Command(COMMAND_LOAD,(int)FirstRecentFile());
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
			Scene.CutSelection(Tools.CurrentClassID());
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
			Scene.CopySelection(Tools.CurrentClassID());
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
                Command			(COMMAND_CHANGE_ACTION,eaSelect);
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
				Scene.SaveSelection(Tools.CurrentClassID(),0,fn.c_str());
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
    case COMMAND_MAKE_SKYDOME:
		if( !Scene.locked() ){
			if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export skydome?"))
                Builder.MakeSkydome( );
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
			Scene.InvertSelection(Tools.CurrentClassID());
//.			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SELECT_ALL:
		if( !Scene.locked() ){
			Scene.SelectObjects(true,Tools.CurrentClassID());
//.			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DESELECT_ALL:
		if( !Scene.locked() ){
			Scene.SelectObjects(false,Tools.CurrentClassID());
//			Scene.UndoSave();
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
	        Command(COMMAND_UPDATE_PROPERTIES);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_SEL:
		if( !Scene.locked() ){
			Scene.ShowObjects( bool(p1), Tools.CurrentClassID(), true, true );
			Scene.UndoSave();
	        Command(COMMAND_UPDATE_PROPERTIES);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_ALL:
		if( !Scene.locked() ){
			Scene.ShowObjects( bool(p1), Tools.CurrentClassID(), false );
			Scene.UndoSave();
	        Command(COMMAND_UPDATE_PROPERTIES);
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
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
    default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
    }
    return 	bRes;
}

char* TUI::GetCaption()
{
	return m_LastFileName.IsEmpty()?"noname":m_LastFileName.c_str();
}

#define COMMAND0(cmd)		{Command(cmd);bExec=true;}
#define COMMAND1(cmd,p0)	{Command(cmd,p0);bExec=true;}

bool __fastcall TUI::ApplyShortCutExt(WORD Key, TShiftState Shift)
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
            else if (Key=='N'){
                switch (Tools.GetAction()){
                case eaMove: 			COMMAND0(COMMAND_SET_NUMERIC_POSITION); break;
                case eaRotate: 			COMMAND0(COMMAND_SET_NUMERIC_ROTATION); break;
                case eaScale: 			COMMAND0(COMMAND_SET_NUMERIC_SCALE);	break;
                }
            }
        }
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall TUI::ApplyGlobalShortCutExt(WORD Key, TShiftState Shift)
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

bool TUI::PickGround(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap, Fvector* hitnormal){
	VERIFY(m_bReady);
    // pick object geometry
    if ((bSnap==-1)||(fraTopBar->ebOSnap->Down&&(bSnap==1))){
        bool bPickObject;
        SRayPickInfo pinf;
	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary:{
        	bPickObject = !!TfrmEditLibrary::RayPick(start,direction,&pinf);
        }break;
        case esEditScene:{
        	bPickObject = !!Scene.RayPickObject(start,direction,OBJCLASS_SCENEOBJECT,&pinf,Scene.GetSnapList(false)); break;
        }
        default: return false;
        }
        if (bPickObject){
		    if (fraTopBar->ebVSnap->Down&&bSnap){
                Fvector pn;
                float u = pinf.inf.u;
                float v = pinf.inf.v;
                float w = 1-(u+v);
				Fvector verts[3];
                pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
                if ((w>u) && (w>v)) pn.set(verts[0]);
                else if ((u>w) && (u>v)) pn.set(verts[1]);
                else pn.set(verts[2]);
                if (pn.distance_to(pinf.pt) < movesnap()) hitpoint.set(pn);
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

    if (fraTopBar->ebGSnap->Down && bSnap){
        hitpoint.x = snapto( hitpoint.x, movesnap() );
        hitpoint.z = snapto( hitpoint.z, movesnap() );
        hitpoint.y = 0.f;
    }
	if (hitnormal) hitnormal->set(0,1,0);
	return true;
}
//----------------------------------------------------

bool TUI::SelectionFrustum(CFrustum& frustum){
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
        if (frmEditPrefs->cbBoxPickLimitedDepth->Checked){
			pinf.inf.range = Device.m_Camera.m_Zfar; // max pick range
            if (Scene.RayPickObject(st, d, OBJCLASS_SCENEOBJECT, &pinf, 0))
	            if (pinf.inf.range > depth) depth = pinf.inf.range;
        }
    }
    if (depth<Device.m_Camera.m_Znear) depth = Device.m_Camera.m_Zfar;
    else depth += frmEditPrefs->seBoxPickDepthTolerance->Value;

    for (i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        p[i].mad(st,d,depth);
    }

    frustum.CreateFromPoints(p,4,Device.m_Camera.m_Position);

    Fplane P; P.build(p[0],p[1],p[2]);
    if (P.classify(st)>0) P.build(p[2],p[1],p[0]);
	frustum._add(P);

	return true;
}
//----------------------------------------------------
void TUI::RealUpdateScene()
{
	if (GetEState()==esEditScene){
	    Scene.OnObjectsUpdate();
    	Tools.OnObjectsUpdate(); // �������� ��� ��� ���-�� ������� � ���������
	    RedrawScene();
    }
    m_Flags.set(flUpdateScene,FALSE);
}
//---------------------------------------------------------------------------


void TUI::ShowContextMenu(int cls)
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

