//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop       

#include "UI_ParticleMain.h"
#include "UI_ParticleTools.h"
#include "topbar.h"
#include "leftbar.h"           
#include "EditorPreferences.h"
#include "D3DUtils.h"
#include "bottombar.h"                  
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"

//---------------------------------------------------------------------------
CParticleMain*&	PUI=(CParticleMain*)UI;
//---------------------------------------------------------------------------

CParticleMain::CParticleMain()  
{
}
//---------------------------------------------------------------------------

CParticleMain::~CParticleMain()
{
}
//---------------------------------------------------------------------------

void CParticleTools::CommandSelectPreviewObj(u32 p1, u32 p2, u32& res)
{
    SelectPreviewObject(p1);
}
void CParticleTools::CommandEditPreviewProps(u32 p1, u32 p2, u32& res)
{
    EditPreviewPrefs();
}
void CParticleTools::CommandSave(u32 p1, u32 p2, u32& res)
{
    Save(0,0);
    ExecCommand(COMMAND_UPDATE_CAPTION);
}
void CParticleTools::CommandSaveBackup(u32 p1, u32 p2, u32& res)
{
    ExecCommand(COMMAND_SAVE);
}
void CParticleTools::CommandReload(u32 p1, u32 p2, u32& res)
{
    if (!IfModified()){ 
    	res = FALSE;
    	return;
    }
    Reload();
    ExecCommand(COMMAND_UPDATE_CAPTION);
}
void CParticleTools::CommandClear(u32 p1, u32 p2, u32& res)
{
    Device.m_Camera.Reset();
    ResetPreviewObject();
    ExecCommand(COMMAND_UPDATE_CAPTION);
}
void CParticleTools::CommandPlayCurrent(u32 p1, u32 p2, u32& res)
{
    PlayCurrent();
}
void CParticleTools::CommandStopCurrent(u32 p1, u32 p2, u32& res)
{
    StopCurrent(p1);
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

void CParticleMain::RegisterCommands()
{
	inherited::RegisterCommands();
    // tools
	RegisterCommand(COMMAND_SELECT_PREVIEW_OBJ, SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(PTools,CParticleTools::CommandSelectPreviewObj)));
	RegisterCommand(COMMAND_EDIT_PREVIEW_PROPS, SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(PTools,CParticleTools::CommandEditPreviewProps)));
	RegisterCommand(COMMAND_SAVE,              	SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(PTools,CParticleTools::CommandSave)));
	RegisterCommand(COMMAND_SAVE_BACKUP,        SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(PTools,CParticleTools::CommandSaveBackup)));
	RegisterCommand(COMMAND_RELOAD,             SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(PTools,CParticleTools::CommandReload)));
	RegisterCommand(COMMAND_CLEAR,             	SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(PTools,CParticleTools::CommandClear)));
	RegisterCommand(COMMAND_PLAY_CURRENT,       SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(PTools,CParticleTools::CommandPlayCurrent)));
	RegisterCommand(COMMAND_STOP_CURRENT,       SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(PTools,CParticleTools::CommandStopCurrent)));
	RegisterCommand(COMMAND_REFRESH_UI_BAR,     SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandRefreshUIBar)));
	RegisterCommand(COMMAND_RESTORE_UI_BAR,     SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandRestoreUIBar)));
	RegisterCommand(COMMAND_SAVE_UI_BAR,     	SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandSaveUIBar)));
	RegisterCommand(COMMAND_UPDATE_TOOLBAR,     SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandUpdateToolBar)));
	RegisterCommand(COMMAND_UPDATE_CAPTION,     SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandUpdateCaption)));
}

char* CParticleMain::GetCaption()
{
	return "particles";
}

bool __fastcall CParticleMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
    if (inherited::ApplyShortCut(Key,Shift)) return true;
	bool bExec = false;
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall CParticleMain::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    if (inherited::ApplyGlobalShortCut(Key,Shift)) return true;
	bool bExec = false;
    if (Shift.Empty()){
        if (Key==VK_F5)    	COMMAND0(COMMAND_PLAY_CURRENT)
        else if (Key==VK_F6)COMMAND1(COMMAND_STOP_CURRENT,FALSE)
        else if (Key==VK_F7)COMMAND1(COMMAND_STOP_CURRENT,TRUE)
    }
    return bExec;
}
//---------------------------------------------------------------------------

void CParticleMain::RealUpdateScene()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Common
//---------------------------------------------------------------------------
void CParticleMain::ResetStatus()
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=""){
	    fraBottomBar->paStatus->Caption=""; fraBottomBar->paStatus->Repaint();
    }
}
void CParticleMain::SetStatus(LPSTR s, bool bOutLog)
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=s){
	    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
    	if (bOutLog&&s&&s[0]) ELog.Msg(mtInformation,s);
    }
}
void CParticleMain::PBDraw()
{
	SPBItem* pbi 	= PBLast();
	if (pbi){
        AnsiString 	txt;
        float 		p,m;
        pbi->GetInfo(txt,p,m);
        // status line
        fraBottomBar->paStatus->Caption			= txt;
        fraBottomBar->paStatus->Repaint			();
        // progress
    	int val = (int)((p/m)*100);
        if (val!=fraBottomBar->cgProgress->Progress){
			fraBottomBar->cgProgress->Progress	= val;
	        fraBottomBar->cgProgress->Repaint	();
        }
    	if (false==fraBottomBar->cgProgress->Visible) 
        	fraBottomBar->cgProgress->Visible 	= true;
    }else{
    	if (fraBottomBar->cgProgress->Visible){
            // status line
            fraBottomBar->paStatus->Caption			= "";
            fraBottomBar->paStatus->Repaint			();
	        // progress
	        fraBottomBar->cgProgress->Progress	= 0;
        	fraBottomBar->cgProgress->Visible 	= false;
        }
    }
}
//---------------------------------------------------------------------------
void CParticleMain::OutCameraPos()
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
void CParticleMain::OutUICursorPos()
{
	VERIFY(m_bReady);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void CParticleMain::OutGridSize()
{
	VERIFY(fraBottomBar);
    AnsiString s;
    s.sprintf("Grid: %1.1f",EPrefs.grid_cell_size);
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
void CParticleMain::OutInfo()
{
	fraBottomBar->paSel->Caption = Tools->GetInfo();
}
//---------------------------------------------------------------------------
void CParticleMain::RealQuit()
{
	frmMain->Close();
}
//---------------------------------------------------------------------------

