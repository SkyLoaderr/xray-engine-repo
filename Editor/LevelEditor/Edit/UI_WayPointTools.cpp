#include "stdafx.h"
#pragma hdrstop

#include "UI_WayPointTools.h"
#include "ui_tools.h"
#include "FrameWayPoint.h"
#include "WayPoint.h"
#include "ui_main.h"
#include "scene.h"
//---------------------------------------------------------------------------

TUI_WayPointTools::TUI_WayPointTools():TUI_CustomTools(OBJCLASS_WAYPOINT){
    AddControlCB(new TUI_ControlWayPointAdd	(estSelf,eaAdd,		this));
}

void TUI_WayPointTools::OnActivate(){
    pFrame = new TfraWayPoint(0);
    ((TfraWayPoint*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_WayPointTools::OnDeactivate(){
    ((TfraWayPoint*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}
//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlWayPointAdd::TUI_ControlWayPointAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlWayPointAdd::Start(TShiftState Shift){
    ObjectList lst; Scene.GetQueryObjects(lst,OBJCLASS_WAYPOINT,1,1,-1);
	CWayPoint* last_obj = (lst.size()==1)?(CWayPoint*)lst.front():0;

	CWayPoint* obj = (CWayPoint*)DefaultAddObject(Shift);

	if (obj&&Shift.Contains(ssAlt))
		if (((TfraWayPoint*)parent_tool->pFrame)->ebAutoLink->Down){
        	if (last_obj) last_obj->AddSingleLink(obj);
		}
    return false;
}

void __fastcall TUI_ControlWayPointAdd::OnEnter(){
}

