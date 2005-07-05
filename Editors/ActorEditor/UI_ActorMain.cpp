//---------------------------------------------------------------------------
#include "stdafx.h"              
#pragma hdrstop

#include "UI_ActorMain.h"
#include "UI_ActorTools.h"
#include "topbar.h"
#include "leftbar.h"
#include "D3DUtils.h"
#include "bottombar.h"
#include "main.h"
#include "xr_input.h"
#include "ChoseForm.h"           

//---------------------------------------------------------------------------
CActorMain*&	AUI=(CActorMain*)UI;
//---------------------------------------------------------------------------

CActorMain::CActorMain()
{               
}
//---------------------------------------------------------------------------

CActorMain::~CActorMain()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// actor commands
//---------------------------------------------------------------------------
CCommandVar CActorTools::CommandLoad(CCommandVar p1, CCommandVar p2)
{
    xr_string temp_fn	= p1.IsString()?xr_string(p1):xr_string(""); 
    if(!p1.IsString()){
        LPCSTR 			new_val;
        AnsiString last_nm = ChangeFileExt(m_LastFileName,"");
        if (!TfrmChoseItem::SelectItem(smObject,new_val,1,last_nm.c_str()))
        	return  	FALSE;
        temp_fn 		= xr_string(new_val)+".object";
    }
    if( temp_fn.size() ){
		xr_strlwr				(temp_fn);
        if (!IfModified())		return FALSE;

        ExecCommand				(COMMAND_CLEAR);
        
        BOOL bReadOnly 			= !FS.can_modify_file(_objects_,temp_fn.c_str());
        
        if (EFS.CheckLocking(_objects_,temp_fn.c_str(),false,true)) return FALSE;

        m_Flags.set				(flReadOnlyMode,bReadOnly);
        if (bReadOnly){
        	if (EFS.GetLockOwner(_objects_,temp_fn.c_str()).size()){
				Log				("#!Object opened in readonly mode. Locked by user:",EFS.GetLockOwner(_objects_,temp_fn.c_str()).c_str());
            }else{
				Log				("#!You don't have permisions to modify object:",temp_fn.c_str());
            }
        }
        // set enable ...
		m_Props->SetReadOnly	(bReadOnly);
        fraLeftBar->SetReadOnly	(bReadOnly);
        
        CTimer T;
        T.Start();     
        if (!Load(_objects_,temp_fn.c_str())){
            return FALSE;
        }
        m_LastFileName 			= temp_fn.c_str();
        ELog.Msg(mtInformation,"Object '%s' successfully loaded. Loading time - %3.2f(s).",m_LastFileName.c_str(),T.GetElapsed_sec());
        EPrefs.AppendRecentFile(m_LastFileName.c_str());
        ExecCommand	(COMMAND_UPDATE_CAPTION);
        ExecCommand	(COMMAND_UPDATE_PROPERTIES);
        // lock
        if (!bReadOnly)			EFS.LockFile(_objects_,m_LastFileName.c_str());
        UndoClear();
        UndoSave();
    }
    return TRUE;
}
CCommandVar CActorTools::CommandSaveBackup(CCommandVar p1, CCommandVar p2)
{
	xr_string fn = xr_string(Core.UserName)+"_backup.object";
    FS.update_path(fn,"$objects$",fn.c_str());
    ExecCommand(COMMAND_SAVE,fn);
    return TRUE;
}
CCommandVar CActorTools::CommandSave(CCommandVar p1, CCommandVar p2)
{
	if (p2==1){
        xr_string temp_fn	= ATools->m_LastFileName.c_str();
        if (EFS.GetSaveName	( _objects_, temp_fn ))
            return 			ExecCommand(COMMAND_SAVE,temp_fn,0);
    }else{
        if (p1.IsInteger())
            return 				ExecCommand(COMMAND_SAVE,xr_string(ATools->m_LastFileName.c_str()),0);
        xr_string temp_fn		= xr_string(p1);
        if (temp_fn.empty()){
			return 				ExecCommand(COMMAND_SAVE,temp_fn,1);
        }else{
            xr_strlwr			(temp_fn);
            if (0==temp_fn.find(FS.get_path(_objects_)->m_Path))
                temp_fn 		= xr_string(temp_fn.c_str()+strlen(FS.get_path(_objects_)->m_Path));
            EFS.UnlockFile		(_objects_,temp_fn.c_str());
            CTimer T;
            T.Start();
            CCommandVar			res;
            if (Tools->Save(_objects_,temp_fn.c_str())){
                ELog.Msg		(mtInformation,"Object '%s' successfully saved. Saving time - %3.2f(s).",m_LastFileName.c_str(),T.GetElapsed_sec());
                EFS.BackupFile	(_objects_,temp_fn.c_str());
                m_LastFileName	= temp_fn.c_str();
                EPrefs.AppendRecentFile	(m_LastFileName.c_str());
                ExecCommand		(COMMAND_UPDATE_CAPTION);
                res				= TRUE;
            }else{
                res				= FALSE;
            }
            EFS.LockFile		(_objects_,temp_fn.c_str());
            return 				res;
        }
    }
    return 					FALSE;
}     
CCommandVar CActorTools::CommandImport(CCommandVar p1, CCommandVar p2)
{
    xr_string		temp_fn			= p1.IsString()?xr_string(p1):xr_string("");
    if(p1.IsString()||EFS.GetOpenName(_import_,temp_fn)){
        if (0==temp_fn.find(FS.get_path(_import_)->m_Path)){
            temp_fn = xr_string(temp_fn.c_str()+strlen(FS.get_path(_import_)->m_Path));
            xr_strlwr(temp_fn);
            if (!Tools->IfModified())
                return	FALSE;
            ExecCommand( COMMAND_CLEAR );
            CTimer T;
            T.Start();
            if (!ATools->Import(_import_,temp_fn.c_str()))
                return	FALSE;
            m_LastFileName = temp_fn.c_str();
            ELog.Msg(mtInformation,"Object '%s' successfully imported. Loading time - %3.2f(s).",m_LastFileName.c_str(),T.GetElapsed_sec());
            if (ExecCommand( COMMAND_SAVE )){
                xr_string mfn;
                FS.update_path(mfn,_import_,temp_fn.c_str());
                EFS.MarkFile(mfn.c_str(),true);
                EFS.BackupFile(_objects_,temp_fn.c_str());
            }else{
                ExecCommand( COMMAND_CLEAR );
            }
            return TRUE;
        }else{
            ELog.Msg	(mtError,"Invalid file path. ");
        }
    }
    return FALSE;
}
CCommandVar CActorTools::CommandExportDM(CCommandVar p1, CCommandVar p2)
{
	CCommandVar res 				= FALSE;
    xr_string fn=p1.IsString()?xr_string(p1):xr_string("");
    if (p1.IsString()||EFS.GetSaveName("$game_dm$",fn)){
        if (0!=(res=ExportDM(fn.c_str())))	ELog.Msg(mtInformation,"Export complete.");
        else        		    			ELog.Msg(mtError,"Export failed.");
    }
    return res;
}
CCommandVar CActorTools::CommandExportOBJ(CCommandVar p1, CCommandVar p2)
{
	CCommandVar res 				= FALSE;
    xr_string fn=p1.IsString()?xr_string(p1):xr_string("");
    if (p1.IsString()||EFS.GetSaveName("$import$",fn,0,5)){
        if (0!=(res=ExportOBJ(fn.c_str())))	ELog.Msg(mtInformation,"Export complete.");
        else        		    			ELog.Msg(mtError,"Export failed.");
    }
    return res;
}
CCommandVar CActorTools::CommandExportOGF(CCommandVar p1, CCommandVar p2)
{
	CCommandVar res 				= FALSE;
    xr_string fn=p1.IsString()?xr_string(p1):xr_string("");
    if (p1.IsString()||EFS.GetSaveName("$game_meshes$",fn,0,0)){
        if (0!=(res=ATools->ExportOGF(fn.c_str())))	ELog.Msg(mtInformation,"Export complete.");
        else		        		    			ELog.Msg(mtError,"Export failed.");
    }
    return res;
}
CCommandVar CActorTools::CommandExportOMF(CCommandVar p1, CCommandVar p2)
{
	CCommandVar res 				= FALSE;
    xr_string fn=p1.IsString()?xr_string(p1):xr_string("");
    if (p1.IsString()||EFS.GetSaveName("$game_meshes$",fn,0,1)){
        if (0!=(res=ExportOMF(fn.c_str())))	ELog.Msg(mtInformation,"Export complete.");
        else        		    			ELog.Msg(mtError,"Export failed.");
    }
    return res;
}
CCommandVar CActorTools::CommandExportCPP(CCommandVar p1, CCommandVar p2)
{
	CCommandVar res 				= FALSE;
    xr_string fn=p1.IsString()?xr_string(p1):xr_string("");
    if (p1.IsString()||EFS.GetSaveName(_import_,fn,0,7)){
        if (0!=(res=ExportCPP(fn.c_str())))	ELog.Msg(mtInformation,"Export complete.");
        else        		    			ELog.Msg(mtError,"Export failed.");
    }
    return res;
}
CCommandVar CActorTools::CommandClear(CCommandVar p1, CCommandVar p2)
{
    if (!IfModified())	return FALSE;
    // unlock
    EFS.UnlockFile	(_objects_,m_LastFileName.c_str());
    m_LastFileName	= "";
    Device.m_Camera.Reset();
    Clear			();
    ExecCommand		(COMMAND_UPDATE_CAPTION);
    ExecCommand		(COMMAND_UPDATE_PROPERTIES);
    UndoClear		();
    return TRUE;
}
CCommandVar CActorTools::CommandUndo(CCommandVar p1, CCommandVar p2)
{
    if(!Undo())	ELog.Msg( mtInformation, "Undo buffer empty" );
    else		return ExecCommand(COMMAND_CHANGE_ACTION, etaSelect);
    return FALSE;
}
CCommandVar CActorTools::CommandRedo(CCommandVar p1, CCommandVar p2)
{
    if(!Redo())	ELog.Msg( mtInformation, "Redo buffer empty" );
    else		return ExecCommand(COMMAND_CHANGE_ACTION, etaSelect);
    return FALSE;
}
CCommandVar CActorTools::CommandOptimizeMotions(CCommandVar p1, CCommandVar p2)
{
    OptimizeMotions();
    return TRUE;
}
CCommandVar CActorTools::CommandMakeThumbnail(CCommandVar p1, CCommandVar p2)
{
	MakeThumbnail();
    return TRUE;
}
CCommandVar CActorTools::CommandBatchConvert(CCommandVar p1, CCommandVar p2)
{
	CCommandVar res 				= FALSE;
    xr_string fn;
    if (EFS.GetOpenName("$import$",fn,false,0,6)){
        if (0!=(res=BatchConvert(fn.c_str())))	ELog.Msg(mtInformation,"Convert complete.");
        else		        		    		ELog.Msg(mtError,"Convert failed.");
    }
    return res;
}

//---------------------------------------------------------------------------
// Common command
//---------------------------------------------------------------------------
CCommandVar CommandShowClipMaker(CCommandVar p1, CCommandVar p2)
{
    ATools->ShowClipMaker();
    return TRUE;
}
CCommandVar CommandMakePreview(CCommandVar p1, CCommandVar p2)
{
    ATools->MakePreview();
    return TRUE;
}
CCommandVar CommandPreviewObjPref(CCommandVar p1, CCommandVar p2)
{
    ATools->SetPreviewObjectPrefs();
    return TRUE;
}
CCommandVar CommandSelectPreviewObj(CCommandVar p1, CCommandVar p2)
{
    ATools->SelectPreviewObject(p1);
    return TRUE;
}
CCommandVar CommandLoadFirstRecent(CCommandVar p1, CCommandVar p2)
{
    if (EPrefs.FirstRecentFile())
        return ExecCommand(COMMAND_LOAD,xr_string(EPrefs.FirstRecentFile()));
    return FALSE;
}
CCommandVar CommandFileMenu(CCommandVar p1, CCommandVar p2)
{
    FHelper.ShowPPMenu(fraLeftBar->pmSceneFile,0);
    return TRUE;
}
CCommandVar CommandRefreshUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->RefreshBar	();
    fraLeftBar->RefreshBar	();
    fraBottomBar->RefreshBar();
    return TRUE;
}
CCommandVar CommandRestoreUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->fsStorage->RestoreFormPlacement();
    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
    return TRUE;
}
CCommandVar CommandSaveUIBar(CCommandVar p1, CCommandVar p2)
{
    fraTopBar->fsStorage->SaveFormPlacement();
    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    return TRUE;
}
CCommandVar CommandUpdateToolBar(CCommandVar p1, CCommandVar p2)
{
    fraLeftBar->UpdateBar();
    return TRUE;
}
CCommandVar CommandUpdateCaption(CCommandVar p1, CCommandVar p2)
{
    frmMain->UpdateCaption();
    return TRUE;
}

CCommandVar CommandChangeTarget(CCommandVar p1, CCommandVar p2)
{
	if (p1.IsString()){
        ATools->SelectListItem(xr_string(p1).c_str(),	0,true,false,true);
    }else{
        switch (p1){
        case 0: ATools->SelectListItem(BONES_PREFIX,	0,true,false,true); 	break;
        case 1: ATools->SelectListItem(MOTIONS_PREFIX,	0,true,false,true); 	break;
        case 2: ATools->SelectListItem(OBJECT_PREFIX,	0,true,false,true); 	break;
        case 3: ATools->SelectListItem(SURFACES_PREFIX,	0,true,false,true); 	break;
        }
    }
    return TRUE;
}

void CActorMain::RegisterCommands()
{
	inherited::RegisterCommands();
    // tools
	REGISTER_CMD_CE	(COMMAND_CLEAR,             "File\\Clear Scene", 			ATools,CActorTools::CommandClear,			true);
    REGISTER_CMD_CE	(COMMAND_LOAD,              "File\\Load", 					ATools,CActorTools::CommandLoad,			true);
    REGISTER_CMD_C	(COMMAND_SAVE_BACKUP,       ATools,							CActorTools::CommandSaveBackup);
    REGISTER_SUB_CMD_CE (COMMAND_SAVE, 			"File",							ATools,CActorTools::CommandSave,			true);
    	APPEND_SUB_CMD	("Save",				0,								0);
    	APPEND_SUB_CMD	("Save As",				0,								1);
    REGISTER_SUB_CMD_END;
    REGISTER_CMD_CE	(COMMAND_IMPORT,            "File\\Import",					ATools,CActorTools::CommandImport,			true);
    REGISTER_CMD_CE	(COMMAND_EXPORT_DM,         "File\\Export DM",				ATools,CActorTools::CommandExportDM,		true);
    REGISTER_CMD_CE	(COMMAND_EXPORT_OBJ,		"File\\Export OBJ",				ATools,CActorTools::CommandExportOBJ,		true);
    REGISTER_CMD_CE	(COMMAND_EXPORT_OGF,        "File\\Export OGF",				ATools,CActorTools::CommandExportOGF,		true);
    REGISTER_CMD_CE	(COMMAND_EXPORT_OMF,        "File\\Export OMF",				ATools,CActorTools::CommandExportOMF,		true);
    REGISTER_CMD_CE	(COMMAND_EXPORT_CPP,		"File\\Export CPP",				ATools,CActorTools::CommandExportCPP,		true);
	REGISTER_CMD_CE	(COMMAND_UNDO,              "Edit\\Undo",					ATools,CActorTools::CommandUndo,			false);
	REGISTER_CMD_CE	(COMMAND_REDO,              "Edit\\Redo",					ATools,CActorTools::CommandRedo,			false);
    REGISTER_CMD_C	(COMMAND_OPTIMIZE_MOTIONS,  ATools,							CActorTools::CommandOptimizeMotions);
    REGISTER_CMD_CE	(COMMAND_MAKE_THUMBNAIL, 	"Make Thumbnail",				ATools,CActorTools::CommandMakeThumbnail, 	false);
    REGISTER_CMD_CE	(COMMAND_BATCH_CONVERT,		"File\\Batch Convert",			ATools,CActorTools::CommandBatchConvert, 	false);
    // ui
    REGISTER_CMD_S	(COMMAND_SHOW_CLIPMAKER,  	CommandShowClipMaker);
    REGISTER_CMD_S	(COMMAND_MAKE_PREVIEW,      CommandMakePreview);
    REGISTER_CMD_S	(COMMAND_PREVIEW_OBJ_PREF,  CommandPreviewObjPref);
    REGISTER_CMD_S	(COMMAND_SELECT_PREVIEW_OBJ,CommandSelectPreviewObj);
    REGISTER_CMD_SE	(COMMAND_LOAD_FIRSTRECENT,  "File\\Load First Recent",		CommandLoadFirstRecent, 					true);
    REGISTER_CMD_SE	(COMMAND_FILE_MENU,         "File Menu",					CommandFileMenu, 							true);
    REGISTER_CMD_S	(COMMAND_REFRESH_UI_BAR,    CommandRefreshUIBar);
    REGISTER_CMD_S	(COMMAND_RESTORE_UI_BAR,    CommandRestoreUIBar);
    REGISTER_CMD_S	(COMMAND_SAVE_UI_BAR,       CommandSaveUIBar);
	REGISTER_CMD_S	(COMMAND_UPDATE_TOOLBAR,    CommandUpdateToolBar);
    REGISTER_CMD_S	(COMMAND_UPDATE_CAPTION,    CommandUpdateCaption);
    REGISTER_SUB_CMD_SE (COMMAND_CHANGE_TARGET, "Change Target",				CommandChangeTarget,						true);
    	APPEND_SUB_CMD	(BONES_PREFIX,			xr_string(BONES_PREFIX),		0);
    	APPEND_SUB_CMD	(MOTIONS_PREFIX,		xr_string(MOTIONS_PREFIX),		0);
    	APPEND_SUB_CMD	(OBJECT_PREFIX,			xr_string(OBJECT_PREFIX),		0);
    	APPEND_SUB_CMD	(SURFACES_PREFIX,		xr_string(SURFACES_PREFIX),		0);
    REGISTER_SUB_CMD_END;
}                                                                    

char* CActorMain::GetCaption()
{
	return ATools->GetEditFileName().IsEmpty()?"noname":ATools->GetEditFileName().c_str();
}

bool __fastcall CActorMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
    return inherited::ApplyShortCut(Key,Shift);
}
//---------------------------------------------------------------------------

bool __fastcall CActorMain::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    return inherited::ApplyGlobalShortCut(Key,Shift);
}
//---------------------------------------------------------------------------

void CActorMain::RealUpdateScene()
{
	inherited::RealUpdateScene	();
/*	if (GetEState()==esEditScene){
	    Scene.OnObjectsUpdate();
    	Tools->OnObjectsUpdate(); // �������� ��� ��� ���-�� ������� � ���������
	    RedrawScene();
    }
    m_Flags.set(flUpdateScene,FALSE);
*/
}
//---------------------------------------------------------------------------

void CActorMain::ResetStatus()
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=""){
	    fraBottomBar->paStatus->Caption=""; fraBottomBar->paStatus->Repaint();
    }
}
void CActorMain::SetStatus(LPSTR s, bool bOutLog)
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=s){
	    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
    	if (bOutLog&&s&&s[0]) ELog.Msg(mtInformation,s);
    }
}

void CActorMain::ProgressDraw()
{
	fraBottomBar->RedrawBar();
}
//---------------------------------------------------------------------------
void CActorMain::OutCameraPos()
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
void CActorMain::OutUICursorPos()
{
	VERIFY(fraBottomBar);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void CActorMain::OutGridSize()
{
	VERIFY(fraBottomBar);
    AnsiString s;
    s.sprintf("Grid: %1.1f",EPrefs.grid_cell_size);
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
void CActorMain::OutInfo()
{
	fraBottomBar->paSel->Caption = Tools->GetInfo();
}
//---------------------------------------------------------------------------
void CActorMain::RealQuit()
{
	frmMain->Close();
}
//---------------------------------------------------------------------------

