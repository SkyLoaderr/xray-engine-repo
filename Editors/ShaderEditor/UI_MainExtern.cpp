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
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"

bool TUI::CommandExt(int _Command, int p1, int p2)
{
	bool bRes = true;
	string256 filebuffer;
	switch (_Command){
	case COMMAND_SAVE:
    	Tools.SEngine.Save();
    	Tools.SCompiler.Save();
    	Tools.SMaterial.Save();
		Command(COMMAND_UPDATE_CAPTION);
    	break;
    case COMMAND_SAVE_BACKUP:
		Command(COMMAND_SAVE);
    	break;
    case COMMAND_RELOAD:
    	if (Tools.ActiveEditor()==aeEngine){
	    	if (!Tools.SEngine.IfModified()) return false;
            if (ELog.DlgMsg(mtConfirmation,"Reload shaders?")==mrYes)
                Tools.SEngine.Reload();
    	}else if (Tools.ActiveEditor()==aeCompiler){
	    	if (!Tools.SCompiler.IfModified()) return false;
            if (ELog.DlgMsg(mtConfirmation,"Reload shaders?")==mrYes)
                Tools.SCompiler.Reload();
        }else if ((Tools.ActiveEditor()==aeMaterial)||(Tools.ActiveEditor()==aeMaterialPair)){
	    	if (!Tools.SMaterial.IfModified()) return false;
            if (ELog.DlgMsg(mtConfirmation,"Reload materials?")==mrYes)
                Tools.SMaterial.Reload();
        }
		Command(COMMAND_UPDATE_CAPTION);
    	break;
	case COMMAND_CLEAR:
        Device.m_Camera.Reset();
        Tools.ResetPreviewObject();
        Command(COMMAND_UPDATE_CAPTION);
		break;
	case COMMAND_RESET_ANIMATION:
   		break;
    case COMMAND_SELECT_PREVIEW_OBJ:
		Tools.SelectPreviewObject(p1);
    	break;
    default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
    }
    return 	bRes;
}

char* TUI::GetCaption()
{
	return "shaders&materials";
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


