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

bool CActorMain::Command(int _Command, int p1, int p2)
{
	bool bRes = true;
	string256 filebuffer;
	switch (_Command){
    case COMMAND_SHOW_CLIPMAKER:
    	ATools->ShowClipMaker();
    	break;
    case COMMAND_MAKE_PREVIEW:  
    	ATools->MakePreview();
    	break;
    case COMMAND_LOAD:{
        AnsiString temp_fn	= AnsiString((char*)p1).LowerCase();
        if(!p1){
        	LPCSTR new_val;
            if (!TfrmChoseItem::SelectItem(smObject,new_val)) return false;
            temp_fn = AnsiString(new_val)+".object";
        }
        if( !temp_fn.IsEmpty() ){
            if (!Tools->IfModified()){
                bRes=false;
                break;
            }
            if (0!=temp_fn.AnsiCompareIC(m_LastFileName)&&EFS.CheckLocking(_objects_,temp_fn.c_str(),false,true)){
                bRes=false;
                break;
            }
            if (0==temp_fn.AnsiCompareIC(m_LastFileName)&&EFS.CheckLocking(_objects_,temp_fn.c_str(),true,false)){
                EFS.UnlockFile(_objects_,temp_fn.c_str());
            }
            Command( COMMAND_CLEAR );
            CTimer T;
            T.Start();                
            if (!Tools->Load(_objects_,temp_fn.c_str())){
                bRes=false;
                break;
            }
            m_LastFileName = temp_fn;
            ELog.Msg(mtInformation,"Object '%s' successfully loaded. Loading time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
            EPrefs.AppendRecentFile(m_LastFileName.c_str());
            Command	(COMMAND_UPDATE_CAPTION);
            Command	(COMMAND_UPDATE_PROPERTIES);
            // lock
            EFS.LockFile(_objects_,m_LastFileName.c_str());
            ATools->UndoClear();
            ATools->UndoSave();
        }
    	}break;
    case COMMAND_SAVE_BACKUP:{
    	AnsiString fn = AnsiString(Core.UserName)+"_backup.object";
        FS.update_path("$objects$",fn);
    	Command(COMMAND_SAVEAS,(int)fn.c_str());
    }break;
    case COMMAND_SAVEAS:{
        AnsiString temp_fn	= AnsiString((char*)p1).LowerCase();
        bRes 				= false;
        if(p1||EFS.GetSaveName(_objects_,temp_fn)){
            if (p1||(1==temp_fn.Pos(FS.get_path(_objects_)->m_Path))){
                if (!p1) temp_fn = AnsiString(temp_fn.c_str()+strlen(FS.get_path(_objects_)->m_Path)).LowerCase();
                bRes=Command(COMMAND_SAVE, (u32)temp_fn.c_str());
                // unlock
                EFS.UnlockFile(_objects_,m_LastFileName.c_str());
                m_LastFileName=temp_fn;
                Command(COMMAND_UPDATE_CAPTION);
                EFS.LockFile(_objects_,m_LastFileName.c_str());
                EPrefs.AppendRecentFile(m_LastFileName.c_str());
            }else{
            	ELog.DlgMsg	(mtError,"Invalid file path.");
            }
        }
    	}break;
	case COMMAND_SAVE:{
    	AnsiString temp_fn;
        if (p1)	temp_fn = (char*)p1;
        else	temp_fn = m_LastFileName;
        EFS.UnlockFile(_objects_,temp_fn.c_str());
        CTimer T;
        T.Start();
		if (Tools->Save(_objects_,temp_fn.c_str())){
            ELog.Msg(mtInformation,"Object '%s' successfully saved. Saving time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
        	Command(COMMAND_UPDATE_CAPTION);
			EPrefs.AppendRecentFile(temp_fn.c_str());
            AnsiString mfn;
            FS.update_path(mfn,_objects_,temp_fn.c_str());
//.            EFS.MarkFile(mfn.c_str(),false);
            EFS.BackupFile(_objects_,temp_fn.c_str());
        }else{
        	bRes=false;
        }
        EFS.LockFile(_objects_,temp_fn.c_str());
    	}break;
    case COMMAND_IMPORT:{
        AnsiString temp_fn;
        if(EFS.GetOpenName(_import_,temp_fn)){
            if (1==temp_fn.Pos(FS.get_path(_import_)->m_Path)){
                temp_fn = AnsiString(temp_fn.c_str()+strlen(FS.get_path(_import_)->m_Path)).LowerCase();
                if (!Tools->IfModified()){
                    bRes=false;
                    break;
                }
                Command( COMMAND_CLEAR );
                CTimer T;
                T.Start();
                if (!ATools->Import(_import_,temp_fn.c_str())){
                    bRes=false;
                    break;
                }
                m_LastFileName = temp_fn;
                ELog.Msg(mtInformation,"Object '%s' successfully imported. Loading time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
                if (Command( COMMAND_SAVEAS )){
                	AnsiString mfn;
                    FS.update_path(mfn,_import_,temp_fn.c_str());
                    EFS.MarkFile(mfn.c_str(),true);
                    EFS.BackupFile(_objects_,temp_fn.c_str());
                }else{
                    Command( COMMAND_CLEAR );
                }
    		}else{
            	ELog.DlgMsg	(mtError,"Invalid file path. ");
            }
        }
    	}break;
    case COMMAND_EXPORT_DM:{
    	AnsiString fn;
    	if (EFS.GetSaveName("$game_dm$",fn))
            if (ATools->ExportDM(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
            else		        		    	ELog.DlgMsg(mtError,"Export failed.");
    	}break;
    case COMMAND_EXPORT_OGF:{
    	AnsiString fn;
    	if (EFS.GetSaveName("$game_meshes$",fn,0,0))
            if (ATools->ExportOGF(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
            else		        		    	ELog.DlgMsg(mtError,"Export failed.");
    	}break;
    case COMMAND_EXPORT_OMF:{
    	AnsiString fn;
    	if (EFS.GetSaveName("$game_meshes$",fn,0,1))
            if (ATools->ExportOMF(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
            else		        		    	ELog.DlgMsg(mtError,"Export failed.");
    	}break;
	case COMMAND_CLEAR:
		{
            if (!Tools->IfModified()) return false;
			// unlock
			EFS.UnlockFile(_objects_,m_LastFileName.c_str());
			m_LastFileName="";
			Device.m_Camera.Reset();
            Tools->Clear();
			Command	(COMMAND_UPDATE_CAPTION);
	        Command	(COMMAND_UPDATE_PROPERTIES);
		    ATools->UndoClear();
		}
		break;
    case COMMAND_PREVIEW_OBJ_PREF:
    	ATools->SetPreviewObjectPrefs();
    	break;
    case COMMAND_SELECT_PREVIEW_OBJ:
		ATools->SelectPreviewObject(p1);
    	break;
    case COMMAND_RESET_ANIMATION:
    	break;
    case COMMAND_LOAD_FIRSTRECENT:
    	if (EPrefs.FirstRecentFile()){
        	bRes = Command(COMMAND_LOAD,(int)EPrefs.FirstRecentFile());
        }
    	break;
    case COMMAND_FILE_MENU:
		FHelper.ShowPPMenu(fraLeftBar->pmSceneFile,0);
    	break;
	case COMMAND_UNDO:
        if( !ATools->Undo() ) ELog.DlgMsg( mtInformation, "Undo buffer empty" );
        else				Command(COMMAND_CHANGE_ACTION, etaSelect);
		break;
	case COMMAND_REDO:
        if( !ATools->Redo() ) ELog.DlgMsg( mtInformation, "Redo buffer empty" );
        else				Command(COMMAND_CHANGE_ACTION, etaSelect);
		break;
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
    default:
    	return inherited::Command(_Command,p1,p2);
    }
    return 	bRes;
}

char* CActorMain::GetCaption()
{
	return GetEditFileName().IsEmpty()?"noname":GetEditFileName().c_str();
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
    	Tools->OnObjectsUpdate(); // обновить все что как-то связано с объектами
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
void CActorMain::ProgressInfo(LPCSTR text, bool bWarn)
{
	if (text){
		fraBottomBar->paStatus->Caption=fraBottomBar->sProgressTitle+" ("+text+")";
    	fraBottomBar->paStatus->Repaint();
	    ELog.Msg(bWarn?mtError:mtInformation,fraBottomBar->paStatus->Caption.c_str());
    }
}
void CActorMain::ProgressStart(float max_val, const char* text)
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
void CActorMain::ProgressEnd()
{
	VERIFY(m_bReady);
    fraBottomBar->sProgressTitle = "";
	fraBottomBar->paStatus->Caption="";
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->cgProgress->Visible=false;
}
void CActorMain::ProgressUpdate(float val)
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
void CActorMain::ProgressInc(const char* info, bool bWarn)
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
void CActorMain::OutCameraPos()
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
void CActorMain::OutUICursorPos()
{
	VERIFY(m_bReady);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void CActorMain::OutGridSize()
{
	VERIFY(m_bReady);
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

