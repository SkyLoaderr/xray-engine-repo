#include "stdafx.h"
#pragma hdrstop

#include "UI_GroupTools.h"
#include "ui_tools.h"
#include "FrameGroup.h"

//----------------------------------------------------------------------
TUI_GroupTools::TUI_GroupTools():TUI_CustomTools(OBJCLASS_GROUP,true){
    AddControlCB(xr_new<TUI_ControlGroupAdd>(estDefault,eaAdd,		this));
}

void TUI_GroupTools::OnActivate  (){
    pFrame = xr_new<TfraGroup>((TComponent*)0);
	TUI_CustomTools::OnActivate();
}
void TUI_GroupTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    xr_delete(pFrame);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlGroupAdd::TUI_ControlGroupAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlGroupAdd::Start(TShiftState Shift){
    return false;
}

void __fastcall TUI_ControlGroupAdd::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlGroupAdd::End(TShiftState _Shift)
{
    return true;
}

