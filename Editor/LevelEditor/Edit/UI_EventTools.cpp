#include "stdafx.h"
#pragma hdrstop

#include "UI_EventTools.h"
#include "ui_tools.h"
#include "FrameEvent.h"
#include "event.h"
#include "Scene.h"
#include "ui_main.h"

//----------------------------------------------------------------------
TUI_EventTools::TUI_EventTools():TUI_CustomTools(OBJCLASS_EVENT){
    AddControlCB(new TUI_ControlEventAdd	(estSelf,eaAdd,		this));
}

void TUI_EventTools::OnActivate  (){
    pFrame = new TfraEvent(0);
    ((TfraEvent*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_EventTools::OnDeactivate(){
    ((TfraEvent*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}
//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlEventAdd::TUI_ControlEventAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlEventAdd::Start(TShiftState Shift){
	ObjectList lst; Scene.GetQueryObjects(lst,OBJCLASS_EVENT,1,1,-1);
	TfraEvent* frame=(TfraEvent*)parent_tool->pFrame;
	if (frame->ebFormMode->Down){
    	if (1!=lst.size()){
        	ELog.DlgMsg(mtInformation,"Select one EventObject.");
            return false;
        }
        Fvector pos,up;
	    if (UI.PickGround(pos,UI.m_CurrentRStart,UI.m_CurrentRNorm,1,&up)){
        	CEvent* obj = (CEvent*)lst.front(); R_ASSERT(obj);
	        CEvent::SForm* ev=obj->AppendForm(CEvent::efBox);
    	    ev->MoveTo(pos,up);
            Scene.UndoSave();
        }
        if (!Shift.Contains(ssAlt)) ResetActionToSelect();
    }else{
		CEvent* O = (CEvent*)DefaultAddObject(Shift); R_ASSERT(O);
    }
    return false;
}

