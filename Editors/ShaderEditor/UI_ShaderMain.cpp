//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_ShaderMain.h"
#include "UI_ShaderTools.h"
#include "topbar.h"
#include "leftbar.h"
#include "D3DUtils.h"
#include "bottombar.h"
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"

//---------------------------------------------------------------------------
CShaderMain*&	PUI=(CShaderMain*)UI;
//---------------------------------------------------------------------------

CShaderMain::CShaderMain()
{
}
//---------------------------------------------------------------------------

CShaderMain::~CShaderMain()
{
}
//---------------------------------------------------------------------------

void CShaderTools::CommandSave(u32 p1, u32 p2, u32& res)
{
    Save			(0,0);
    ExecCommand		(COMMAND_UPDATE_CAPTION);
}
void CShaderTools::CommandSaveBackup(u32 p1, u32 p2, u32& res)
{
    ExecCommand		(COMMAND_SAVE);
}
void CShaderTools::CommandReload(u32 p1, u32 p2, u32& res)
{
    Reload			();
    ExecCommand		(COMMAND_UPDATE_CAPTION);
}
void CShaderTools::CommandClear(u32 p1, u32 p2, u32& res)
{
    Device.m_Camera.Reset();
    ExecCommand		(COMMAND_UPDATE_CAPTION);
}
void CShaderTools::CommandUpdateList(u32 p1, u32 p2, u32& res)
{
	UpdateList		();
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


void CShaderMain::RegisterCommands()
{
	inherited::RegisterCommands();
    // tools
	RegisterCommand(COMMAND_SAVE,              	SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(STools,CShaderTools::CommandSave)));
	RegisterCommand(COMMAND_SAVE_BACKUP,		SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(STools,CShaderTools::CommandSaveBackup)));
	RegisterCommand(COMMAND_RELOAD,				SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(STools,CShaderTools::CommandReload)));
	RegisterCommand(COMMAND_CLEAR,				SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(STools,CShaderTools::CommandClear)));
    RegisterCommand(COMMAND_UPDATE_LIST,	    SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_C(STools,CShaderTools::CommandUpdateList)));
    RegisterCommand(COMMAND_REFRESH_UI_BAR,		SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandRefreshUIBar)));
    RegisterCommand(COMMAND_RESTORE_UI_BAR,     SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandRestoreUIBar)));
    RegisterCommand(COMMAND_SAVE_UI_BAR,        SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandSaveUIBar)));
	RegisterCommand(COMMAND_UPDATE_TOOLBAR,     SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandUpdateToolBar)));
    RegisterCommand(COMMAND_UPDATE_CAPTION,     SECommand("",MAKE_EMPTY_SHORTCUT,BIND_CMD_EVENT_S(CommandUpdateCaption)));
}

char* CShaderMain::GetCaption()
{
	return (LPSTR)STools->CurrentToolsName();// "shaders&materials";
}           

bool __fastcall CShaderMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
    if (inherited::ApplyShortCut(Key,Shift)) return true;
	bool bExec = false;
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall CShaderMain::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
    if (inherited::ApplyGlobalShortCut(Key,Shift)) return true;
	bool bExec = false;
    return bExec;
}
//---------------------------------------------------------------------------

void CShaderMain::RealUpdateScene()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Common
//---------------------------------------------------------------------------
void CShaderMain::ResetStatus()
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=""){
	    fraBottomBar->paStatus->Caption=""; fraBottomBar->paStatus->Repaint();
    }
}
void CShaderMain::SetStatus(LPSTR s, bool bOutLog)
{
	VERIFY(m_bReady);
    if (fraBottomBar->paStatus->Caption!=s){
	    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
    	if (bOutLog&&s&&s[0]) ELog.Msg(mtInformation,s);
    }
}
void CShaderMain::ProgressDraw()
{
	fraBottomBar->RedrawBar();
}
//---------------------------------------------------------------------------
void CShaderMain::OutCameraPos()
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
void CShaderMain::OutUICursorPos()
{
	VERIFY(fraBottomBar);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void CShaderMain::OutGridSize()
{
	VERIFY(fraBottomBar);
    AnsiString s;
    s.sprintf("Grid: %1.1f",EPrefs.grid_cell_size);
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
void CShaderMain::OutInfo()
{
	fraBottomBar->paSel->Caption = Tools->GetInfo();
}
//---------------------------------------------------------------------------
void CShaderMain::RealQuit()
{
	frmMain->Close();
}
//---------------------------------------------------------------------------

