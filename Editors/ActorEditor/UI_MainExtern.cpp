//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "UI_Tools.h"
#include "topbar.h"
#include "leftbar.h"
#include "EditorPref.h"
#include "D3DUtils.h"
#include "bottombar.h"
#include "main.h"
#include "xr_input.h"

bool TUI::CommandExt(int _Command, int p1, int p2)
{
	bool bRes = true;
	string256 filebuffer;
	switch (_Command){
    case COMMAND_MAKE_PREVIEW:  
    	Tools.MakePreview();
    	break;
	case COMMAND_LOAD_MOTIONS:{
    	if (!Tools.CurrentObject()){
        	ELog.DlgMsg(mtError,"Scene empty. Load object first.");
        	bRes=false;
        	break;
        }
		AnsiString fn;
		if (EFS.GetOpenName("$smotions$",fn)){
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
		if (EFS.GetSaveName("$smotions$",fn)) Tools.SaveMotions(fn.c_str());
        }break;
    case COMMAND_SAVE_BACKUP:{
    	AnsiString fn = AnsiString(Core.UserName)+"_backup.object";
        FS.update_path("$objects$",fn);
    	Command(COMMAND_SAVEAS,(int)fn.c_str());
    }break;
    case COMMAND_SAVEAS:{
		AnsiString fn = ChangeFileExt(m_LastFileName,".object");
		if (p1||EFS.GetSaveName("$objects$",fn,NULL,0)){ 
        	if (p1) fn= (LPCSTR)p1;
        	bRes=Command(COMMAND_SAVE, (u32)fn.c_str());
        }else{
        	bRes=false;
        }
        if (bRes){
			// unlock
   	        EFS.UnlockFile(0,m_LastFileName);
        	strcpy(m_LastFileName,fn.c_str());
        	Command(COMMAND_UPDATE_CAPTION);
            EFS.LockFile(0,m_LastFileName);
            fraLeftBar->AppendRecentFile(m_LastFileName);
        }
    	}break;
	case COMMAND_SAVE:{
    	AnsiString fn;
        if (p1)	fn = (char*)p1;
        else	fn = m_LastFileName;
        EFS.UnlockFile(0,m_LastFileName);
		if (Tools.Save(fn.c_str())){
        	Command(COMMAND_UPDATE_CAPTION);
			fraLeftBar->AppendRecentFile(fn.c_str());
        }else{
        	bRes=false;
        }
        EFS.LockFile(0,m_LastFileName);
    	}break;
    case COMMAND_IMPORT:{
    	AnsiString fn;
    	if (EFS.GetOpenName("$import$",fn)){
            if (!Tools.IfModified()){
                bRes=false;
                break;
            }
			Command( COMMAND_CLEAR );
	    	if (!Tools.Load(fn.c_str())){
            	bRes=false;
            	break;
            }
			strcpy(m_LastFileName,ExtractFileName(fn).c_str());
			if (Command( COMMAND_SAVEAS )){
	            EFS.MarkFile(fn.c_str(),true);
			    fraLeftBar->UpdateMotionList();
            }else{
            	Command( COMMAND_CLEAR );
            }
        }
    	}break;
    case COMMAND_EXPORT_SKELETON:{
    	AnsiString fn;
    	if (EFS.GetSaveName("$game_meshes$",fn))
            if (Tools.ExportSkeleton(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
            else			        		    	ELog.DlgMsg(mtError,"Export failed.");
    	}break;
    case COMMAND_EXPORT_OBJECT:{
    	AnsiString fn;
    	if (EFS.GetSaveName("$game_meshes$",fn))
            if (Tools.ExportObject(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
            else			        		    	ELog.DlgMsg(mtError,"Export failed.");
    	}break;
    case COMMAND_LOAD:{
    	AnsiString fn;
        if (p1)	fn = (char*)p1;
        else	fn = m_LastFileName;
        if( p1 || EFS.GetOpenName("$objects$", fn ) ){
            if (!Tools.IfModified()){
                bRes=false;
                break;
            }
            if ((0!=stricmp(fn.c_str(),m_LastFileName))&&EFS.CheckLocking(0,fn.c_str(),false,true)){
                bRes=false;
                break;
            }
            if ((0==stricmp(fn.c_str(),m_LastFileName))&&EFS.CheckLocking(0,fn.c_str(),true,false)){
                EFS.UnlockFile(0,fn.c_str());
            }
			Command( COMMAND_CLEAR );
	    	if (!Tools.Load(fn.c_str())){
            	bRes=false;
            	break;
            }
			strcpy(m_LastFileName,fn.c_str());
			fraLeftBar->AppendRecentFile(m_LastFileName);
		    fraLeftBar->UpdateMotionList();
        	Command(COMMAND_UPDATE_CAPTION);
			// lock
			EFS.LockFile(0,m_LastFileName);
        }
    	}break;
	case COMMAND_CLEAR:
		{
            if (!Tools.IfModified()) return false;
			// unlock
			EFS.UnlockFile(0,m_LastFileName);
			m_LastFileName[0]=0;
			Device.m_Camera.Reset();
            Tools.Clear();
			Command(COMMAND_UPDATE_CAPTION);
		}
		break;
    case COMMAND_PREVIEW_OBJ_PREF:
    	Tools.SetPreviewObjectPrefs();
    	break;
    case COMMAND_SELECT_PREVIEW_OBJ:
		Tools.SelectPreviewObject(p1);
    	break;
    case COMMAND_RESET_ANIMATION:
    	break;
    case COMMAND_LOAD_FIRSTRECENT:
    	if (fraLeftBar->FirstRecentFile()){
        	bRes = Command(COMMAND_LOAD,(int)fraLeftBar->FirstRecentFile());
        }
    	break;
    default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
    }
    return 	bRes;
}

char* TUI::GetCaption()
{
	return GetEditFileName()[0]?GetEditFileName():"noname";
}

bool __fastcall TUI::ApplyShortCutExt(WORD Key, TShiftState Shift)
{
	bool bExec = false;
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall TUI::ApplyGlobalShortCutExt(WORD Key, TShiftState Shift)
{
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
		if (Key=='R')					{Command(COMMAND_LOAD_FIRSTRECENT);     bExec=true;}
    }
    return bExec;
}
//---------------------------------------------------------------------------

void TUI::ShowContextMenu(int cls)
{
/*	VERIFY(m_bReady);
    POINT pt;
    GetCursorPos(&pt);
    fraLeftBar->miProperties->Enabled = false;
    if (Scene.SelectionCount( true, (EObjClass)cls )) fraLeftBar->miProperties->Enabled = true;
    RedrawScene(true);
    fraLeftBar->pmObjectContext->TrackButton = tbRightButton;
    fraLeftBar->pmObjectContext->Popup(pt.x,pt.y);
*/
}
//---------------------------------------------------------------------------

void TUI::RealUpdateScene()
{
/*	if (GetEState()==esEditScene){
	    Scene.OnObjectsUpdate();
    	Tools.OnObjectsUpdate(); // обновить все что как-то связано с объектами
	    RedrawScene();
    }
    m_Flags.set(flUpdateScene,FALSE);
*/
}
//---------------------------------------------------------------------------


