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
void CActorTools::CommandLoad(u32 p1, u32 p2, u32& res)
{
    AnsiString temp_fn	= AnsiString((char*)p1).LowerCase();
    if(!p1){
        LPCSTR new_val;
        AnsiString last_nm = ChangeFileExt(m_LastFileName,"");
        if (!TfrmChoseItem::SelectItem(smObject,new_val,1,last_nm.c_str())){ 
        	res	= FALSE;
        	return;
        }
        temp_fn = AnsiString(new_val)+".object";
    }
    if( !temp_fn.IsEmpty() ){
        if (!IfModified()){
            res	= FALSE;
            return;
        }
        if (0!=temp_fn.AnsiCompareIC(m_LastFileName)&&EFS.CheckLocking(_objects_,temp_fn.c_str(),false,true)){
            res	= FALSE;
            return;
        }
        if (0==temp_fn.AnsiCompareIC(m_LastFileName)&&EFS.CheckLocking(_objects_,temp_fn.c_str(),true,false)){
            EFS.UnlockFile(_objects_,temp_fn.c_str());
        }
        ExecCommand	(COMMAND_CLEAR);
        CTimer T;
        T.Start();     
        if (!Load(_objects_,temp_fn.c_str())){
            res	= FALSE;
            return;
        }
        m_LastFileName = temp_fn;
        ELog.Msg(mtInformation,"Object '%s' successfully loaded. Loading time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
        EPrefs.AppendRecentFile(m_LastFileName.c_str());
        ExecCommand	(COMMAND_UPDATE_CAPTION);
        ExecCommand	(COMMAND_UPDATE_PROPERTIES);
        // lock
        EFS.LockFile(_objects_,m_LastFileName.c_str());
        UndoClear();
        UndoSave();
    }
}
void CActorTools::CommandSaveBackup(u32 p1, u32 p2, u32& res)
{
    std::string fn = std::string(Core.UserName)+"_backup.object";
    FS.update_path(fn,"$objects$",fn.c_str());
    ExecCommand(COMMAND_SAVEAS,(int)fn.c_str());
}
void CActorTools::CommandSaveAs(u32 p1, u32 p2, u32& res)
{
    std::string temp_fn	= AnsiString((char*)p1).LowerCase().c_str();
    res 				= FALSE;
    if(p1||EFS.GetSaveName(_objects_,temp_fn)){
        if (p1||(0==temp_fn.find(FS.get_path(_objects_)->m_Path))){
            if (!p1){ 
                temp_fn = std::string(temp_fn.c_str()+strlen(FS.get_path(_objects_)->m_Path));
                xr_strlwr(temp_fn);
            }
            res=ExecCommand			(COMMAND_SAVE, (u32)temp_fn.c_str());
            // unlock
            EFS.UnlockFile			(_objects_,m_LastFileName.c_str());
            m_LastFileName			= temp_fn.c_str();
            ExecCommand				(COMMAND_UPDATE_CAPTION);
            EFS.LockFile			(_objects_,m_LastFileName.c_str());
            EPrefs.AppendRecentFile	(m_LastFileName.c_str());
        }else{
            ELog.DlgMsg				(mtError,"Invalid file path.");
        }
    }
}     
void CActorTools::CommandSave(u32 p1, u32 p2, u32& res)
{
    AnsiString temp_fn;
    if (p1)	temp_fn = (char*)p1;
    else	temp_fn = m_LastFileName;
    EFS.UnlockFile(_objects_,temp_fn.c_str());
    CTimer T;
    T.Start();
    if (Tools->Save(_objects_,temp_fn.c_str())){
        ELog.Msg(mtInformation,"Object '%s' successfully saved. Saving time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
        ExecCommand(COMMAND_UPDATE_CAPTION);
        EPrefs.AppendRecentFile(temp_fn.c_str());
        std::string mfn;
        FS.update_path(mfn,_objects_,temp_fn.c_str());
//.            EFS.MarkFile(mfn.c_str(),false);
        EFS.BackupFile(_objects_,temp_fn.c_str());
    }else{
        res	= FALSE;
    }
    EFS.LockFile(_objects_,temp_fn.c_str());
}
void CActorTools::CommandImport(u32 p1, u32 p2, u32& res)
{
    std::string temp_fn;
    if(EFS.GetOpenName(_import_,temp_fn)){
        if (0==temp_fn.find(FS.get_path(_import_)->m_Path)){
            temp_fn = std::string(temp_fn.c_str()+strlen(FS.get_path(_import_)->m_Path));
            xr_strlwr(temp_fn);
            if (!Tools->IfModified()){
                res=FALSE;
                return;
            }
            ExecCommand( COMMAND_CLEAR );
            CTimer T;
            T.Start();
            if (!ATools->Import(_import_,temp_fn.c_str())){
                res=FALSE;
                return;
            }
            m_LastFileName = temp_fn.c_str();
            ELog.Msg(mtInformation,"Object '%s' successfully imported. Loading time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
            if (ExecCommand( COMMAND_SAVEAS )){
                std::string mfn;
                FS.update_path(mfn,_import_,temp_fn.c_str());
                EFS.MarkFile(mfn.c_str(),true);
                EFS.BackupFile(_objects_,temp_fn.c_str());
            }else{
                ExecCommand( COMMAND_CLEAR );
            }
        }else{
            ELog.DlgMsg	(mtError,"Invalid file path. ");
        }
    }
}
void CActorTools::CommandExportDM(u32 p1, u32 p2, u32& res)
{
    std::string fn;
    if (EFS.GetSaveName("$game_dm$",fn)){
        if (ExportDM(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
        else        		    	ELog.DlgMsg(mtError,"Export failed.");
    }
}
void CActorTools::CommandExportOBJ(u32 p1, u32 p2, u32& res)
{
    std::string fn;
    if (EFS.GetSaveName("$import$",fn,0,5)){
    	
        if (ExportOBJ(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
        else        		    	ELog.DlgMsg(mtError,"Export failed.");
    }
}
void CActorTools::CommandExportOGF(u32 p1, u32 p2, u32& res)
{
    std::string fn;
    if (EFS.GetSaveName("$game_meshes$",fn,0,0)){
        if (ATools->ExportOGF(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
        else		        		    	ELog.DlgMsg(mtError,"Export failed.");
    }
}
void CActorTools::CommandExportOMF(u32 p1, u32 p2, u32& res)
{
    std::string fn;
    if (EFS.GetSaveName("$game_meshes$",fn,0,1)){
        if (ExportOMF(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
        else        		    	ELog.DlgMsg(mtError,"Export failed.");
    }
}
void CActorTools::CommandClear(u32 p1, u32 p2, u32& res)
{
    if (!IfModified()){ 
    	res = FALSE;
    	return;
    }
    // unlock
    EFS.UnlockFile(_objects_,m_LastFileName.c_str());
    m_LastFileName="";
    Device.m_Camera.Reset();
    Clear		();
    ExecCommand	(COMMAND_UPDATE_CAPTION);
    ExecCommand	(COMMAND_UPDATE_PROPERTIES);
    UndoClear	();
}
void CActorTools::CommandUndo(u32 p1, u32 p2, u32& res)
{
    if(!Undo())	ELog.DlgMsg( mtInformation, "Undo buffer empty" );
    else		ExecCommand(COMMAND_CHANGE_ACTION, etaSelect);
}
void CActorTools::CommandRedo(u32 p1, u32 p2, u32& res)
{
    if(!Redo())	ELog.DlgMsg( mtInformation, "Redo buffer empty" );
    else		ExecCommand(COMMAND_CHANGE_ACTION, etaSelect);
}
void CActorTools::CommandOptimizeMotions(u32 p1, u32 p2, u32& res)
{
    OptimizeMotions();
}
void CActorTools::CommandMakeThumbnail(u32 p1, u32 p2, u32& res)
{
	MakeThumbnail();
}
void CActorTools::CommandBatchConvert(u32 p1, u32 p2, u32& res)
{
    std::string fn;
    if (EFS.GetOpenName("$import$",fn,false,0,6)){
        if (BatchConvert(fn.c_str()))	ELog.DlgMsg(mtInformation,"Convert complete.");
        else		        		    ELog.DlgMsg(mtError,"Convert failed.");
    }
}

//---------------------------------------------------------------------------
// Common command
//---------------------------------------------------------------------------
void CommandShowClipMaker(u32 p1, u32 p2, u32& res)
{
    ATools->ShowClipMaker();
}
void CommandMakePreview(u32 p1, u32 p2, u32& res)
{
    ATools->MakePreview();
}
void CommandPreviewObjPref(u32 p1, u32 p2, u32& res)
{
    ATools->SetPreviewObjectPrefs();
}
void CommandSelectPreviewObj(u32 p1, u32 p2, u32& res)
{
    ATools->SelectPreviewObject(p1);
}
void CommandLoadFirstRecent(u32 p1, u32 p2, u32& res)
{
    if (EPrefs.FirstRecentFile()){
        res = ExecCommand(COMMAND_LOAD,(int)EPrefs.FirstRecentFile());
    }
}
void CommandFileMenu(u32 p1, u32 p2, u32& res)
{
    FHelper.ShowPPMenu(fraLeftBar->pmSceneFile,0);
}
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

void CActorMain::RegisterCommands()
{
	inherited::RegisterCommands();
    // tools
	RegisterCommand(COMMAND_CLEAR,              xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandClear)));
    RegisterCommand(COMMAND_LOAD,               xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandLoad)));
    RegisterCommand(COMMAND_SAVE_BACKUP,        xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandSaveBackup)));
    RegisterCommand(COMMAND_SAVEAS,             xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandSaveAs)));
	RegisterCommand(COMMAND_SAVE,               xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandSave)));
    RegisterCommand(COMMAND_IMPORT,             xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandImport)));
    RegisterCommand(COMMAND_EXPORT_DM,          xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandExportDM)));
    RegisterCommand(COMMAND_EXPORT_OBJ,			xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandExportOBJ)));
    RegisterCommand(COMMAND_EXPORT_OGF,         xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandExportOGF)));
    RegisterCommand(COMMAND_EXPORT_OMF,         xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandExportOMF)));
	RegisterCommand(COMMAND_UNDO,               xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandUndo)));
	RegisterCommand(COMMAND_REDO,               xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandRedo)));
    RegisterCommand(COMMAND_OPTIMIZE_MOTIONS,   xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(ATools,CActorTools::CommandOptimizeMotions)));
    RegisterCommand(COMMAND_MAKE_THUMBNAIL, 	xr_new<SECommand>("Make Thumbnail","",true,BIND_CMD_EVENT_C(ATools,CActorTools::CommandMakeThumbnail)));
    RegisterCommand(COMMAND_BATCH_CONVERT,		xr_new<SECommand>("Batch Convert","",true,BIND_CMD_EVENT_C(ATools,CActorTools::CommandBatchConvert)));
    // ui
    RegisterCommand(COMMAND_SHOW_CLIPMAKER,  	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandShowClipMaker)));
    RegisterCommand(COMMAND_MAKE_PREVIEW,       xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandMakePreview)));
    RegisterCommand(COMMAND_PREVIEW_OBJ_PREF,   xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandPreviewObjPref)));
    RegisterCommand(COMMAND_SELECT_PREVIEW_OBJ, xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandSelectPreviewObj)));
    RegisterCommand(COMMAND_LOAD_FIRSTRECENT,   xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandLoadFirstRecent)));
    RegisterCommand(COMMAND_FILE_MENU,          xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandFileMenu)));
    RegisterCommand(COMMAND_REFRESH_UI_BAR,     xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandRefreshUIBar)));
    RegisterCommand(COMMAND_RESTORE_UI_BAR,     xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandRestoreUIBar)));
    RegisterCommand(COMMAND_SAVE_UI_BAR,        xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandSaveUIBar)));
	RegisterCommand(COMMAND_UPDATE_TOOLBAR,     xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandUpdateToolBar)));
    RegisterCommand(COMMAND_UPDATE_CAPTION,     xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandUpdateCaption)));
}                                                                    

char* CActorMain::GetCaption()
{
	return ATools->GetEditFileName().IsEmpty()?"noname":ATools->GetEditFileName().c_str();
}

bool __fastcall CActorMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
    if (inherited::ApplyShortCut(Key,Shift)) return true;
	bool bExec = false;
    if (Shift.Empty()){
    	if (Key=='B') 					{ ATools->SelectListItem(BONES_PREFIX,0,true,false,true); bExec=true;}
        else if (Key=='M') 				{ ATools->SelectListItem(MOTIONS_PREFIX,0,true,false,true); bExec=true;}
        else if (Key=='O') 				{ ATools->SelectListItem(OBJECT_PREFIX,0,true,false,true); bExec=true;}
        else if (Key=='F') 				{ ATools->SelectListItem(SURFACES_PREFIX,0,true,false,true); bExec=true;}
    }else if (Shift.Contains(ssAlt)){
		if (Key=='F')   				COMMAND0(COMMAND_FILE_MENU);
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall CActorMain::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    if (inherited::ApplyGlobalShortCut(Key,Shift)) return true;
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
		if (Key=='R')					COMMAND0(COMMAND_LOAD_FIRSTRECENT)
        else if (Key=='Z')    			COMMAND0(COMMAND_UNDO)
        else if (Key=='Y')    			COMMAND0(COMMAND_REDO)
    }
    return bExec;
}
//---------------------------------------------------------------------------

void CActorMain::RealUpdateScene()
{
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

