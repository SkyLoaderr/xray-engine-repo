#include "stdafx.h"
#pragma hdrstop

#include "ui_RPointtools.h"
#include "ui_tools.h"
#include "FrameRPoint.h"
#include "UI_Main.h"
#include "Scene.h"
#include "RPoint.h"
//---------------------------------------------------------------------------

TUI_RPointTools::TUI_RPointTools():TUI_CustomTools(OBJCLASS_RPOINT){
    AddControlCB(new TUI_CustomControl	(estSelf,eaSelect,	this));
    AddControlCB(new TUI_ControlRPointAdd(estSelf,eaAdd,	this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaMove,	this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaRotate,	this));
}
void TUI_RPointTools::OnActivate  (){
    pFrame = new TfraRPoint(0);
    ((TfraRPoint*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_RPointTools::OnDeactivate(){
    ((TfraRPoint*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}

//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlRPointAdd::TUI_ControlRPointAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlRPointAdd::Start(TShiftState Shift){
	SceneObject* obj = DefaultAddObject(Shift);
	((CRPoint*)obj)->m_dwTeamID = ((TfraRPoint*)parent_tool->pFrame)->seTeamID->Value;
    return false;
}

