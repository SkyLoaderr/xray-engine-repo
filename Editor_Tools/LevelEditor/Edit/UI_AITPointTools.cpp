#include "stdafx.h"
#pragma hdrstop

#include "ui_AITPointTools.h"
#include "ui_tools.h"
#include "FrameAITPoint.h"
#include "AITraffic.h"
#include "ui_main.h"
#include "scene.h"
//---------------------------------------------------------------------------

TUI_AITPointTools::TUI_AITPointTools():TUI_CustomTools(OBJCLASS_AITPOINT){
    AddControlCB(new TUI_CustomControl		(estSelf,eaSelect,	this));
    AddControlCB(new TUI_ControlAITPointAdd	(estSelf,eaAdd,		this));
    AddControlCB(new TUI_CustomControl		(estSelf,eaMove,	this));
}

void TUI_AITPointTools::OnActivate(){
    pFrame = new TfraAITPoint(0);
    ((TfraAITPoint*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_AITPointTools::OnDeactivate(){
    ((TfraAITPoint*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}
//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlAITPointAdd::TUI_ControlAITPointAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
	last_obj = 0;
}

bool __fastcall TUI_ControlAITPointAdd::Start(TShiftState Shift){
	CAITPoint* obj = (CAITPoint*)DefaultAddObject(Shift);
	if (obj&&Shift.Contains(ssAlt))
		if (((TfraAITPoint*)parent_tool->pFrame)->ebAutoLink->Down){
        	if (last_obj) obj->AddLink(last_obj);
        	last_obj = obj;
		}
    return false;
}

void __fastcall TUI_ControlAITPointAdd::OnEnter(){
	last_obj = 0;
}

