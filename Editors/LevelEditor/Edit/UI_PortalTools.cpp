#include "stdafx.h"
#pragma hdrstop

#include "ui_Portaltools.h"
#include "scene.h"
#include "Portal.h"
#include "framePortal.h"
#include "topbar.h"
#include "ui_tools.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
TUI_PortalTools::TUI_PortalTools():TUI_CustomTools(OBJCLASS_PORTAL,true){
    AddControlCB(xr_new<TUI_ControlPortalAdd>(estDefault,eaAdd,		this));
}
void TUI_PortalTools::OnObjectsUpdate(){
    TfraPortal* fraPortal = (TfraPortal*)pFrame; VERIFY(fraPortal);
}
void TUI_PortalTools::OnActivate  (){
    pFrame = xr_new<TfraPortal>((TComponent*)0);
	TUI_CustomTools::OnActivate();
}
void TUI_PortalTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    xr_delete(pFrame);
}
//------------------------------------------------------------------------------
// add
//------------------------------------------------------------------------------
__fastcall TUI_ControlPortalAdd::TUI_ControlPortalAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

void __fastcall TUI_ControlPortalAdd::OnEnter()
{
}

void __fastcall TUI_ControlPortalAdd::OnExit()
{
}

bool __fastcall TUI_ControlPortalAdd::Start(TShiftState Shift)
{
    if (Shift==ssRBOnly){ UI.Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_PORTAL); return false;}
    return false;
}

void __fastcall TUI_ControlPortalAdd::Move(TShiftState _Shift)
{
}

bool __fastcall TUI_ControlPortalAdd::End(TShiftState _Shift)
{
    return true;
}

