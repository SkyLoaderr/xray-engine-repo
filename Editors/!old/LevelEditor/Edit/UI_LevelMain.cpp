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
#include "SoundManager_LE.h"
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

//------------------------------------------------------------------------------
// Tools commands
//------------------------------------------------------------------------------
void CLevelTools::CommandChangeTarget(u32 p1, u32 p2, u32& res)
{
    SetTarget	((EObjClass)p1,p2);
    ExecCommand	(COMMAND_UPDATE_PROPERTIES);
}
void CLevelTools::CommandShowObjectList(u32 p1, u32 p2, u32& res)
{
    if (LUI->GetEState()==esEditScene) ShowObjectList();
}

//------------------------------------------------------------------------------
// Main commands
//------------------------------------------------------------------------------
void CommandLibraryEditor(u32 p1, u32 p2, u32& res)
{
    if (Scene->ObjCount()||(LUI->GetEState()!=esEditScene)){
        if (LUI->GetEState()==esEditLibrary)	TfrmEditLibrary::ShowEditor();
        else									ELog.DlgMsg(mtError, "Scene must be empty before editing library!");
    }else{
        TfrmEditLibrary::ShowEditor();
    }
}
void CommandLAnimEditor(u32 p1, u32 p2, u32& res)
{
    TfrmEditLightAnim::ShowEditor();
}
void CommandFileMenu(u32 p1, u32 p2, u32& res)
{
    FHelper.ShowPPMenu(fraLeftBar->pmSceneFile,0);
}
void CommandLoad(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        xr_string temp_fn	= AnsiString((char*)p1).LowerCase().c_str();
        if( p1 || EFS.GetOpenName( _maps_, temp_fn ) ){
            if (0==temp_fn.find(FS.get_path(_maps_)->m_Path)){
                temp_fn = xr_string(temp_fn.c_str()+strlen(FS.get_path(_maps_)->m_Path));
                xr_strlwr(temp_fn);
            }
                
            if (!Scene->IfModified()){
                res=FALSE;
                return;
            }
            if (0!=temp_fn.compare(LTools->m_LastFileName.c_str())&&EFS.CheckLocking(_maps_,temp_fn.c_str(),false,true)){
                res=FALSE;
                return;
            }
            if (0==temp_fn.compare(LTools->m_LastFileName.c_str())&&EFS.CheckLocking(_maps_,temp_fn.c_str(),true,false)){
                EFS.UnlockFile(_maps_,temp_fn.c_str());
            }
            UI->SetStatus("Level loading...");
            ExecCommand				(COMMAND_CLEAR);
            if (Scene->Load	(_maps_, temp_fn.c_str(), false)){
                LTools->m_LastFileName	= temp_fn.c_str();
                UI->ResetStatus		();
                Scene->UndoClear	();
                Scene->UndoSave		();
                Scene->m_RTFlags.set(EScene::flRT_Unsaved|EScene::flRT_Modified,FALSE);
                ExecCommand			(COMMAND_UPDATE_CAPTION);
                ExecCommand			(COMMAND_CHANGE_ACTION,etaSelect);
                // lock
                EFS.LockFile		(_maps_,temp_fn.c_str());
                EPrefs.AppendRecentFile(temp_fn.c_str());
            }else{
                ELog.DlgMsg	( mtError, "Can't load map '%s'", temp_fn.c_str() );
            }
            // update props
            ExecCommand			(COMMAND_UPDATE_PROPERTIES);
            UI->RedrawScene		();
        }
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandReload(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        AnsiString temp_fn	= p1?AnsiString((char*)p1).LowerCase():Tools->m_LastFileName;
        AnsiString save_fn	= Tools->m_LastFileName;
        ExecCommand			(COMMAND_LOAD,(int)temp_fn.c_str());
        Tools->m_LastFileName	= save_fn;
        ExecCommand			(COMMAND_UPDATE_CAPTION);
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res 				= FALSE;
    }
}
void CommandSave(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if( !Tools->m_LastFileName.IsEmpty() ){
            UI->SetStatus	("Level saving...");
            Scene->Save		(_maps_, Tools->m_LastFileName.c_str(), false);
            UI->ResetStatus	();
//                Scene->m_Modified = false;
            ExecCommand(COMMAND_UPDATE_CAPTION);
        }else{
            res 			= ExecCommand( COMMAND_SAVEAS ); }
    } else {
        ELog.DlgMsg			( mtError, "Scene sharing violation" );
        res					= FALSE;
    }
}
void CommandSaveBackup(u32 p1, u32 p2, u32& res)
{
    xr_string 	fn;
    fn				= xr_string(Core.CompName)+"_"+Core.UserName+"_backup.level";
    FS.update_path	(fn,_maps_,fn.c_str());
    ExecCommand	  	(COMMAND_SAVEAS,(int)fn.c_str());
}
void CommandSaveAs(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        xr_string temp_fn	= AnsiString((char*)p1).LowerCase().c_str();
        if(p1 || EFS.GetSaveName( _maps_, temp_fn ) ){
            if (0==temp_fn.find(FS.get_path(_maps_)->m_Path)){
                temp_fn 	= xr_string(temp_fn.c_str()+strlen(FS.get_path(_maps_)->m_Path));
                xr_strlwr	(temp_fn);
            }

            UI->SetStatus	("Level saving...");
            Scene->Save		(_maps_, temp_fn.c_str(), false);
            UI->ResetStatus	();
//.                Scene->m_Modified = false;
            // unlock
            EFS.UnlockFile	(_maps_,Tools->m_LastFileName.c_str());
            // set new name
            EFS.LockFile	(_maps_,temp_fn.c_str());
            Tools->m_LastFileName 	= temp_fn.c_str();
            res 			= ExecCommand(COMMAND_UPDATE_CAPTION);
            EPrefs.AppendRecentFile(temp_fn.c_str());
        }else
            res 			= FALSE;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res 				= FALSE;
    }
}
void CommandClear(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if (!Scene->IfModified()){ 
        	res		= FALSE;
        	return;
        }
        // backup map
        if (!Tools->m_LastFileName.IsEmpty()&&Scene->IsModified()){
            EFS.BackupFile(_maps_,Tools->m_LastFileName.c_str());
        }
        // unlock
        EFS.UnlockFile(_maps_,Tools->m_LastFileName.c_str());
        Device.m_Camera.Reset	();
        Scene->Unload			();
        Scene->m_LevelOp.Reset	();
        Tools->m_LastFileName 			= "";
        Scene->UndoClear			();
        ExecCommand(COMMAND_UPDATE_CAPTION);
        ExecCommand(COMMAND_CHANGE_TARGET,OBJCLASS_SCENEOBJECT);
        ExecCommand(COMMAND_CHANGE_ACTION,etaSelect,estDefault);
        Scene->UndoSave();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res 					= FALSE;
    }
    ExecCommand(COMMAND_UPDATE_PROPERTIES);
}
void CommandLoadFirstRecent(u32 p1, u32 p2, u32& res)
{
    if (EPrefs.FirstRecentFile())
        res = ExecCommand(COMMAND_LOAD,(int)EPrefs.FirstRecentFile());
}

void CommandClearCompilerError(u32 p1, u32 p2, u32& res)
{
    Tools->ClearErrors();
}

void CommandImportCompilerError(u32 p1, u32 p2, u32& res)
{
    xr_string fn;
    if(EFS.GetOpenName("$logs$", fn, false, NULL, 0)){
        Scene->LoadCompilerError(fn.c_str());
    }
}
void CommandExportCompilerError(u32 p1, u32 p2, u32& res)
{
    xr_string fn;
    if(EFS.GetSaveName("$logs$", fn, NULL, 0)){
        Scene->SaveCompilerError(fn.c_str());
    }
}
void CommandValidateScene(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->Validate(true,true,true,true,true);
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandRefreshLibrary(u32 p1, u32 p2, u32& res)
{
    if (!Scene->ObjCount()&&!Scene->locked()){
        Lib.RefreshLibrary();
    }else{
        ELog.DlgMsg(mtError, "Scene must be empty before refreshing library!");
        res = FALSE;
    }
}

void CommandReloadObject(u32 p1, u32 p2, u32& res)
{
    Lib.ReloadObjects();
}

void CommandCut(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->CutSelection(LTools->CurrentClassID());
        fraLeftBar->miPaste->Enabled = true;
        fraLeftBar->miPaste2->Enabled = true;
        Scene->UndoSave();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandCopy(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->CopySelection(LTools->CurrentClassID());
        fraLeftBar->miPaste->Enabled = true;
        fraLeftBar->miPaste2->Enabled = true;
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandPaste(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->PasteSelection();
        Scene->UndoSave();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandLoadSelection(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        xr_string fn;
        if( EFS.GetOpenName( _maps_, fn ) ){
            UI->SetStatus		("Fragment loading...");
            Scene->LoadSelection(0,fn.c_str());
            UI->ResetStatus		();
            Scene->UndoSave		();
            ExecCommand			(COMMAND_CHANGE_ACTION,etaSelect);
            ExecCommand			(COMMAND_UPDATE_PROPERTIES);
            UI->RedrawScene		();
        }               	
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}        
void CommandSaveSelection(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        xr_string fn;
        if( EFS.GetSaveName		( _maps_, fn ) ){
            UI->SetStatus		("Fragment saving...");
            Scene->SaveSelection(LTools->CurrentClassID(),0,fn.c_str());
            UI->ResetStatus		();
        }
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandUndo(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if( !Scene->Undo() ) ELog.DlgMsg( mtInformation, "Undo buffer empty" );
        else{
            LTools->Reset();
            ExecCommand(COMMAND_CHANGE_ACTION, etaSelect);
        }
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandRedo(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if( !Scene->Redo() ) ELog.DlgMsg( mtInformation, "Redo buffer empty" );
        else{
            LTools->Reset();
            ExecCommand(COMMAND_CHANGE_ACTION, etaSelect);
        }
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandSceneSummary(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->ShowSummaryInfo();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandOptions(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if (mrOk==frmScenePropertiesRun(&Scene->m_LevelOp.m_BuildParams,false))
            Scene->UndoSave();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandBuild(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if (frmScenePropertiesRun(&Scene->m_LevelOp.m_BuildParams,true)==mrOk)
            Builder.Compile( );
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandMakeAIMap(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export ai-map?"))
            Builder.MakeAIMap( );
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandMakeGame(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export game?"))
            Builder.MakeGame( );
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandMakeDetails(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export details?"))
            Builder.MakeDetails();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandMakeHOM(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        if (mrYes==ELog.DlgMsg(mtConfirmation, TMsgDlgButtons()<<mbYes<<mbNo, "Are you sure to export HOM?"))
            Builder.MakeHOM();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandInvertSelectionAll(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->InvertSelection(LTools->CurrentClassID());
//.			Scene->UndoSave();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandSelectAll(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->SelectObjects(true,LTools->CurrentClassID());
//.			Scene->UndoSave();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandDeselectAll(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->SelectObjects(false,LTools->CurrentClassID());
//			Scene->UndoSave();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandDeleteSelection(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->RemoveSelection( LTools->CurrentClassID() );
        Scene->UndoSave();
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}

void CommandHideUnsel(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->ShowObjects( false, LTools->CurrentClassID(), true, false );
        Scene->UndoSave();
        ExecCommand(COMMAND_UPDATE_PROPERTIES);
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandHideSel(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->ShowObjects( bool(p1), LTools->CurrentClassID(), true, true );
        Scene->UndoSave();
        ExecCommand(COMMAND_UPDATE_PROPERTIES);
    } else {
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandHideAll(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->ShowObjects( bool(p1), LTools->CurrentClassID(), false );
        Scene->UndoSave();
        ExecCommand(COMMAND_UPDATE_PROPERTIES);
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandLockAll(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->LockObjects(bool(p1),LTools->CurrentClassID(),false);
        Scene->UndoSave();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandLockSel(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->LockObjects(bool(p1),LTools->CurrentClassID(),true,true);
        Scene->UndoSave();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandLockUnsel(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->LockObjects(bool(p1),LTools->CurrentClassID(),true,false);
        Scene->UndoSave();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandSetSnapObjects(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->SetSnapList();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandAddSelSnapObjects(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->AddSelToSnapList();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandDelSelSnapObjects(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->DelSelFromSnapList();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandClearSnapObjects(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->ClearSnapList(true);
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandSelectSnapObjects(u32 p1, u32 p2, u32& res)
{
    if( !Scene->locked() ){
        Scene->SelectSnapList();
    }else{
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        res = FALSE;
    }
}
void CommandRefreshSnapObjects(u32 p1, u32 p2, u32& res)
{
    fraLeftBar->UpdateSnapList();
}
void CommandRefreshSoundEnvs(u32 p1, u32 p2, u32& res)
{
    ::Sound->refresh_env_library();
//		::Sound->_restart();
}
void CommandRefreshSoundEnvGeometry(u32 p1, u32 p2, u32& res)
{
    LSndLib->RefreshEnvGeometry();
}
void CommandMoveCameraTo(u32 p1, u32 p2, u32& res)
{
    Fvector pos=Device.m_Camera.GetPosition();
    if (NumericVectorRun("Move to",&pos,3))
        Device.m_Camera.Set(Device.m_Camera.GetHPB(),pos);
}
void CommandShowContextMenu(u32 p1, u32 p2, u32& res)
{
    LUI->ShowContextMenu(EObjClass(p1));
}
//------        
void CommandRefreshUIBar(u32 p1, u32 p2, u32& res)
{
    fraTopBar->RefreshBar	();
    fraLeftBar->RefreshBar	();
    fraBottomBar->RefreshBar();
}
void CommandRestoreUIBar(u32 p1, u32 p2, u32& res)
{
    fraTopBar->fsStorage->RestoreFormPlacement();
    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
}
void CommandSaveUIBar(u32 p1, u32 p2, u32& res)
{
    fraTopBar->fsStorage->SaveFormPlacement();
    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
}
void CommandUpdateToolBar(u32 p1, u32 p2, u32& res)
{
    fraLeftBar->UpdateBar();
}
void CommandUpdateCaption(u32 p1, u32 p2, u32& res)
{
    frmMain->UpdateCaption();
}
//------
void CommandCreateSoundLib(u32 p1, u32 p2, u32& res)
{
    SndLib		= xr_new<CLevelSoundManager>();
}

void CLevelMain::RegisterCommands()
{
	inherited::RegisterCommands();
    // tools                                                                                            
	REGISTER_SUB_CMD_CE	(COMMAND_CHANGE_TARGET,             "Change Target", 	LTools,CLevelTools::CommandChangeTarget);
		APPEND_SUB_CMD	("Object", 							OBJCLASS_SCENEOBJECT,	0);
		APPEND_SUB_CMD	("Light", 							OBJCLASS_LIGHT, 		0);
		APPEND_SUB_CMD	("Sound Source",					OBJCLASS_SOUND_SRC, 	0);
		APPEND_SUB_CMD	("Sound Env", 		                OBJCLASS_SOUND_ENV, 	0);
		APPEND_SUB_CMD	("Glow", 			                OBJCLASS_GLOW, 			0);
		APPEND_SUB_CMD	("Shape", 			                OBJCLASS_SHAPE, 		0);
		APPEND_SUB_CMD	("Spawn Point", 	                OBJCLASS_SPAWNPOINT, 	0);
		APPEND_SUB_CMD	("Way", 			                OBJCLASS_WAY, 			0);
		APPEND_SUB_CMD	("Way Point", 		                OBJCLASS_WAY, 			1);
		APPEND_SUB_CMD	("Sector", 			                OBJCLASS_SECTOR, 		0);
		APPEND_SUB_CMD	("Portal", 			                OBJCLASS_PORTAL, 		0);
		APPEND_SUB_CMD	("Group", 			                OBJCLASS_GROUP, 		0);
		APPEND_SUB_CMD	("Particle System",                 OBJCLASS_PS, 			0);
		APPEND_SUB_CMD	("Detail Objects", 	                OBJCLASS_DO, 			0);
		APPEND_SUB_CMD	("AI Map", 			                OBJCLASS_AIMAP, 		0);
		APPEND_SUB_CMD	("Static Wallmark",                 OBJCLASS_WM, 			0);
    REGISTER_SUB_CMD_END;    

	REGISTER_CMD_C	    (COMMAND_SHOW_OBJECTLIST,           LTools,CLevelTools::CommandShowObjectList);
	// common
	REGISTER_CMD_S	    (COMMAND_LIBRARY_EDITOR,           	CommandLibraryEditor);
	REGISTER_CMD_S	    (COMMAND_LANIM_EDITOR,            	CommandLAnimEditor);
	REGISTER_CMD_S	    (COMMAND_FILE_MENU,              	CommandFileMenu);
	REGISTER_CMD_S	    (COMMAND_LOAD,              		CommandLoad);
	REGISTER_CMD_S	    (COMMAND_RELOAD,              		CommandReload);
	REGISTER_CMD_S	    (COMMAND_SAVE,              		CommandSave);
	REGISTER_CMD_S	    (COMMAND_SAVE_BACKUP,              	CommandSaveBackup);
	REGISTER_CMD_S	    (COMMAND_SAVEAS,              		CommandSaveAs);
	REGISTER_CMD_S	    (COMMAND_CLEAR,              		CommandClear);
	REGISTER_CMD_S	    (COMMAND_LOAD_FIRSTRECENT,          CommandLoadFirstRecent);
	REGISTER_CMD_S	    (COMMAND_CLEAR_COMPILER_ERROR,      CommandClearCompilerError);
	REGISTER_CMD_S	    (COMMAND_IMPORT_COMPILER_ERROR,     CommandImportCompilerError);
	REGISTER_CMD_S	    (COMMAND_EXPORT_COMPILER_ERROR,     CommandExportCompilerError);
	REGISTER_CMD_S	    (COMMAND_VALIDATE_SCENE,            CommandValidateScene);
	REGISTER_CMD_S	    (COMMAND_REFRESH_LIBRARY,           CommandRefreshLibrary);
	REGISTER_CMD_S	    (COMMAND_RELOAD_OBJECTS,            CommandReloadObject);
	REGISTER_CMD_S	    (COMMAND_CUT,              			CommandCut);
	REGISTER_CMD_S	    (COMMAND_COPY,              		CommandCopy);
	REGISTER_CMD_S	    (COMMAND_PASTE,              		CommandPaste);
	REGISTER_CMD_S	    (COMMAND_LOAD_SELECTION,            CommandLoadSelection);
	REGISTER_CMD_S	    (COMMAND_SAVE_SELECTION,            CommandSaveSelection);
	REGISTER_CMD_S	    (COMMAND_UNDO,              		CommandUndo);
	REGISTER_CMD_S	    (COMMAND_REDO,              		CommandRedo);
	REGISTER_CMD_S	    (COMMAND_SCENE_SUMMARY,             CommandSceneSummary);
	REGISTER_CMD_S	    (COMMAND_OPTIONS,              		CommandOptions);
	REGISTER_CMD_S	    (COMMAND_BUILD,              		CommandBuild);
	REGISTER_CMD_S	    (COMMAND_MAKE_AIMAP,              	CommandMakeAIMap);
	REGISTER_CMD_S	    (COMMAND_MAKE_GAME,              	CommandMakeGame);
	REGISTER_CMD_S	    (COMMAND_MAKE_DETAILS,              CommandMakeDetails);
	REGISTER_CMD_S	    (COMMAND_MAKE_HOM,              	CommandMakeHOM);
	REGISTER_CMD_S	    (COMMAND_INVERT_SELECTION_ALL,      CommandInvertSelectionAll);
	REGISTER_CMD_S	    (COMMAND_SELECT_ALL,              	CommandSelectAll);
	REGISTER_CMD_S	    (COMMAND_DESELECT_ALL,              CommandDeselectAll);
	REGISTER_CMD_S	    (COMMAND_DELETE_SELECTION,          CommandDeleteSelection);
	REGISTER_CMD_S	    (COMMAND_HIDE_UNSEL,              	CommandHideUnsel);
	REGISTER_CMD_S	    (COMMAND_HIDE_SEL,              	CommandHideSel);
	REGISTER_CMD_S	    (COMMAND_HIDE_ALL,              	CommandHideAll);
	REGISTER_CMD_S	    (COMMAND_LOCK_ALL,              	CommandLockAll);
	REGISTER_CMD_S	    (COMMAND_LOCK_SEL,             		CommandLockSel);
	REGISTER_CMD_S	    (COMMAND_LOCK_UNSEL,              	CommandLockUnsel);
	REGISTER_CMD_S	    (COMMAND_SET_SNAP_OBJECTS,          CommandSetSnapObjects);
	REGISTER_CMD_S	    (COMMAND_ADD_SEL_SNAP_OBJECTS,      CommandAddSelSnapObjects);
	REGISTER_CMD_S	    (COMMAND_DEL_SEL_SNAP_OBJECTS,      CommandDelSelSnapObjects);
	REGISTER_CMD_S	    (COMMAND_CLEAR_SNAP_OBJECTS,        CommandClearSnapObjects);
	REGISTER_CMD_S	    (COMMAND_SELECT_SNAP_OBJECTS,       CommandSelectSnapObjects);
	REGISTER_CMD_S	    (COMMAND_REFRESH_SNAP_OBJECTS,      CommandRefreshSnapObjects);
	REGISTER_CMD_S	    (COMMAND_REFRESH_SOUND_ENVS,        CommandRefreshSoundEnvs);
	REGISTER_CMD_S	    (COMMAND_REFRESH_SOUND_ENV_GEOMETRY,CommandRefreshSoundEnvGeometry);
	REGISTER_CMD_S	    (COMMAND_MOVE_CAMERA_TO,            CommandMoveCameraTo);
	REGISTER_CMD_S	    (COMMAND_SHOWCONTEXTMENU,           CommandShowContextMenu);
	REGISTER_CMD_S	    (COMMAND_REFRESH_UI_BAR,            CommandRefreshUIBar);
	REGISTER_CMD_S	    (COMMAND_RESTORE_UI_BAR,            CommandRestoreUIBar);
	REGISTER_CMD_S	    (COMMAND_SAVE_UI_BAR,              	CommandSaveUIBar);
	REGISTER_CMD_S	    (COMMAND_UPDATE_TOOLBAR,            CommandUpdateToolBar);
	REGISTER_CMD_S	    (COMMAND_UPDATE_CAPTION,            CommandUpdateCaption);
	REGISTER_CMD_S	    (COMMAND_CREATE_SOUND_LIB,          CommandCreateSoundLib);
}                                                                            

char* CLevelMain::GetCaption()
{
	return Tools->m_LastFileName.IsEmpty()?"noname":Tools->m_LastFileName.c_str();
}

bool __fastcall CLevelMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
    if (inherited::ApplyShortCut(Key,Shift)) return true;

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
            else if (Key=='W')			COMMAND0(COMMAND_SHOW_OBJECTLIST)              
        }
    }else{
        if (Shift.Contains(ssAlt)){
        	if (Key=='F')   			COMMAND0(COMMAND_FILE_MENU)                    
        }else{
            // simple press
            if (Key==VK_DELETE)			COMMAND0(COMMAND_DELETE_SELECTION)             
            else if (Key==VK_RETURN)	COMMAND0(COMMAND_SHOW_PROPERTIES)              
            else if (Key==VK_OEM_MINUS)	COMMAND1(COMMAND_HIDE_SEL, FALSE)              
            else if (Key==VK_OEM_PLUS)	COMMAND1(COMMAND_HIDE_UNSEL, FALSE)            
            else if (Key==VK_OEM_5)		COMMAND1(COMMAND_HIDE_ALL, TRUE)               
        }
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall CLevelMain::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    if (inherited::ApplyGlobalShortCut(Key,Shift)) return true;
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
        	bPickObject = !!Scene->RayPickObject(pinf.inf.range, start,direction,OBJCLASS_SCENEOBJECT,&pinf,Scene->GetSnapList(false)); break;
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
                pinf.e_obj->GetFaceWorld(pinf.s_obj->_Transform(),pinf.e_mesh,pinf.inf.id,verts);
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
    	        pinf.e_obj->GetFaceWorld(pinf.s_obj->_Transform(),pinf.e_mesh,pinf.inf.id,verts);
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
            if (Scene->RayPickObject(pinf.inf.range, st, d, OBJCLASS_SCENEOBJECT, &pinf, 0))
	            if (pinf.inf.range > depth) depth = pinf.inf.range;
        }
    }
    if (depth<Device.m_Camera._Znear()) depth = Device.m_Camera._Zfar();
    else depth += EPrefs.bp_depth_tolerance;

    for (i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        p[i].mad(st,d,depth);
    }

    Fvector pos = Device.m_Camera.GetPosition();
    frustum.CreateFromPoints(p,4,pos);

    Fplane P; P.build(p[0],p[1],p[2]);
    if (P.classify(st)>0) P.build(p[2],p[1],p[0]);
	frustum._add(P);

	return true;
}
//----------------------------------------------------
void CLevelMain::RealUpdateScene()
{
	if (GetEState()==esEditScene){
	    Scene->OnObjectsUpdate();
    	LTools->OnObjectsUpdate(); // обновить все что как-то связано с объектами
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
    if (Scene->SelectionCount( true, (EObjClass)cls )) fraLeftBar->miProperties->Enabled = true;
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
void CLevelMain::ProgressDraw()
{
	fraBottomBar->RedrawBar();
}
//---------------------------------------------------------------------------
void CLevelMain::OutCameraPos()
{
	VERIFY(m_bReady);
    AnsiString s;
	const Fvector& c 	= Device.m_Camera.GetPosition();
	s.sprintf("C: %3.1f, %3.1f, %3.1f",c.x,c.y,c.z);
//	const Fvector& hpb 	= Device.m_Camera.GetHPB();
//	s.sprintf(" Cam: %3.1f°, %3.1f°, %3.1f°",rad2deg(hpb.y),rad2deg(hpb.x),rad2deg(hpb.z));
    fraBottomBar->paCamera->Caption=s; fraBottomBar->paCamera->Repaint();
}
//---------------------------------------------------------------------------
void CLevelMain::OutUICursorPos()
{
	VERIFY(fraBottomBar);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void CLevelMain::OutGridSize()
{
	VERIFY(fraBottomBar);
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

