#include "stdafx.h"
#pragma hdrstop

#include "UI_WayPointTools.h"
#include "ui_tools.h"
#include "FrameWayPoint.h"
#include "WayPoint.h"
#include "scene.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
__fastcall TUI_ControlWayPointAdd::TUI_ControlWayPointAdd(int st, int act, ESceneCustomMTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlWayPointAdd::Start(TShiftState Shift)
{
	ObjectList lst; Scene.GetQueryObjects(lst,OBJCLASS_WAY,1,1,-1);
	TfraWayPoint* frame=(TfraWayPoint*)parent_tool->pFrame;
	if (CWayObject::IsPointMode()){
    	if (1!=lst.size()){
        	ELog.DlgMsg(mtInformation,"Select one WayObject.");
            return false;
        }
        Fvector p;
	    if (UI.PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,1)){
        	CWayObject* obj = (CWayObject*)lst.front(); R_ASSERT(obj);
	        CWayPoint* last_wp=obj->GetFirstSelected();
	        CWayPoint* wp=obj->AppendWayPoint();
    	    wp->MoveTo(p);
			if (frame->ebAutoLink->Down){
	        	if (last_wp) last_wp->AddSingleLink(wp);
            }
            Scene.UndoSave();
        }
        if (!Shift.Contains(ssAlt)) ResetActionToSelect();
    }else{
		CWayObject* O = (CWayObject*)DefaultAddObject(Shift,0); R_ASSERT(O);
    }
    return false;
}

void __fastcall TUI_ControlWayPointAdd::OnEnter(){
}

