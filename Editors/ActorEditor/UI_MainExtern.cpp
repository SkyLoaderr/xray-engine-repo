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
#include "ChoseForm.h"

bool TUI::CommandExt(int _Command, int p1, int p2)
{
	bool bRes = true;
	string256 filebuffer;
	switch (_Command){
    case COMMAND_MAKE_PREVIEW:  
    	Tools.MakePreview();
    	break;
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
                AppendRecentFile(m_LastFileName.c_str());
            }else{
            	ELog.DlgMsg	(mtError,"Invalid file path.");
            }
        }
    	}break;
	case COMMAND_SAVE:{
    	AnsiString temp_fn;
        if (p1)	temp_fn = (char*)p1;
        else	temp_fn = m_LastFileName;
        EFS.UnlockFile(_objects_,m_LastFileName.c_str());
        CTimer T;
        T.Start();
		if (Tools.Save(_objects_,temp_fn.c_str())){
            ELog.Msg(mtInformation,"Object '%s' successfully saved. Saving time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
        	Command(COMMAND_UPDATE_CAPTION);
			AppendRecentFile(temp_fn.c_str());
        }else{
        	bRes=false;
        }
        EFS.LockFile(_objects_,m_LastFileName.c_str());
    	}break;
    case COMMAND_IMPORT:{
        AnsiString temp_fn;
        if(EFS.GetOpenName(_import_,temp_fn)){
            if (1==temp_fn.Pos(FS.get_path(_import_)->m_Path)){
                temp_fn = AnsiString(temp_fn.c_str()+strlen(FS.get_path(_import_)->m_Path)).LowerCase();
                if (!Tools.IfModified()){
                    bRes=false;
                    break;
                }
                Command( COMMAND_CLEAR );
                CTimer T;
                T.Start();
                if (!Tools.Import(_import_,temp_fn.c_str())){
                    bRes=false;
                    break;
                }
                m_LastFileName = temp_fn;
                ELog.Msg(mtInformation,"Object '%s' successfully imported. Loading time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
                if (Command( COMMAND_SAVEAS )){
                	AnsiString mfn;
                    FS.update_path(mfn,_import_,temp_fn.c_str());
                    EFS.MarkFile(mfn.c_str(),true);
    //.			    fraLeftBar->UpdateMotionList();
                }else{
                    Command( COMMAND_CLEAR );
                }
    		}else{
            	ELog.DlgMsg	(mtError,"Invalid file path. ");
            }
        }
    	}break;
    case COMMAND_EXPORT_OGF:{
    	AnsiString fn;
    	if (EFS.GetSaveName("$game_meshes$",fn))
            if (Tools.ExportOGF(fn.c_str()))	ELog.DlgMsg(mtInformation,"Export complete.");
            else		        		    	ELog.DlgMsg(mtError,"Export failed.");
    	}break;
    case COMMAND_LOAD:{
        AnsiString temp_fn	= AnsiString((char*)p1).LowerCase();
        if(!p1){
        	LPCSTR new_val;
            if (!TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,new_val)) return false;
            temp_fn = AnsiString(new_val)+".object";
        }
        if( !temp_fn.IsEmpty() ){
            if (!Tools.IfModified()){
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
            if (!Tools.Load(_objects_,temp_fn.c_str())){
                bRes=false;
                break;
            }
            m_LastFileName = temp_fn;
            ELog.Msg(mtInformation,"Object '%s' successfully loaded. Loading time - %3.2f(s).",m_LastFileName,T.GetElapsed_sec());
            AppendRecentFile(m_LastFileName.c_str());
//.		    fraLeftBar->UpdateMotionList();
            Command	(COMMAND_UPDATE_CAPTION);
            Command	(COMMAND_UPDATE_PROPERTIES);
            // lock
            EFS.LockFile(_objects_,m_LastFileName.c_str());
            Tools.UndoClear();
            Tools.UndoSave();
        }
    	}break;
	case COMMAND_CLEAR:
		{
            if (!Tools.IfModified()) return false;
			// unlock
			EFS.UnlockFile(_objects_,m_LastFileName.c_str());
			m_LastFileName="";
			Device.m_Camera.Reset();
            Tools.Clear();
			Command	(COMMAND_UPDATE_CAPTION);
	        Command	(COMMAND_UPDATE_PROPERTIES);
		    Tools.UndoClear();
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
    	if (FirstRecentFile()){
        	bRes = Command(COMMAND_LOAD,(int)FirstRecentFile());
        }
    	break;
    case COMMAND_FILE_MENU:
		FHelper.ShowPPMenu(fraLeftBar->pmSceneFile,0);
    	break;
	case COMMAND_UNDO:
        if( !Tools.Undo() ) ELog.DlgMsg( mtInformation, "Undo buffer empty" );
        else				Command(COMMAND_CHANGE_ACTION, eaSelect);
		break;
	case COMMAND_REDO:
        if( !Tools.Redo() ) ELog.DlgMsg( mtInformation, "Redo buffer empty" );
        else				Command(COMMAND_CHANGE_ACTION, eaSelect);
		break;
    default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
    }
    return 	bRes;
}

char* TUI::GetCaption()
{
	return GetEditFileName().IsEmpty()?"noname":GetEditFileName().c_str();
}

bool __fastcall TUI::ApplyShortCutExt(WORD Key, TShiftState Shift)
{
	bool bExec = false;
    if (Shift.Empty()){
    	if (Key=='B') 					{ Tools.SelectListItem(BONES_PREFIX,0,true,false,true); bExec=true;}
        else if (Key=='M') 				{ Tools.SelectListItem(MOTIONS_PREFIX,0,true,false,true); bExec=true;}
        else if (Key=='O') 				{ Tools.SelectListItem(OBJECT_PREFIX,0,true,false,true); bExec=true;}
        else if (Key=='F') 				{ Tools.SelectListItem(SURFACES_PREFIX,0,true,false,true); bExec=true;}
    }else if (Shift.Contains(ssAlt)){
		if (Key=='F')   				COMMAND0(COMMAND_FILE_MENU);
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall TUI::ApplyGlobalShortCutExt(WORD Key, TShiftState Shift)
{
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
		if (Key=='R')					COMMAND0(COMMAND_LOAD_FIRSTRECENT)
        else if (Key=='Z')    			COMMAND0(COMMAND_UNDO)
        else if (Key=='Y')    			COMMAND0(COMMAND_REDO)
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


