#include "stdafx.h"
#pragma hdrstop

#include "ui_DOTools.h"
#include "ui_tools.h"
#include "FrameDetObj.h"
#include "DetailObjects.h"
#include "ui_main.h"
#include "scene.h"
//---------------------------------------------------------------------------

TUI_DOTools::TUI_DOTools():TUI_CustomTools(OBJCLASS_DO){
    AddControlCB(new TUI_CustomControl	(estSelf,eaSelect,	this));
    AddControlCB(new TUI_ControlDOAdd	(estSelf,eaAdd,		this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaMove,	this));
}

void TUI_DOTools::OnActivate(){
    pFrame = new TfraDetailObject(0);
    ((TfraDetailObject*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_DOTools::OnDeactivate(){
    ((TfraDetailObject*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}
//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlDOAdd::TUI_ControlDOAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
	fraDO = 0;
}

void __fastcall TUI_ControlDOAdd::OnEnter()
{
    fraDO = (TfraDetailObject*)parent_tool->pFrame; VERIFY(fraDO);
}

void __fastcall TUI_ControlDOAdd::OnExit()
{
	fraDO = 0;
}

bool __fastcall TUI_ControlDOAdd::Start(TShiftState Shift){
/*	if (Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_DOCLUSTER); return 0;}
    Fvector p;
    if (UI->PickGround(p,UI->m_CurrentRStart,UI->m_CurrentRNorm)){
        char namebuffer[MAX_OBJ_NAME];
        CDOCluster* obj=0;
        Scene->GenObjectName(OBJCLASS_DOCLUSTER, namebuffer);
        obj = (CDOCluster*)NewObjectFromClassID(OBJCLASS_DOCLUSTER);
        strcpy(obj->GetName(),namebuffer);
        obj->Move( p );
        Scene->SelectObjects(false,OBJCLASS_DOCLUSTER);
//        if (obj->AppendCluster(fraDO->seClusterDensity->Value)){
//            Scene->AddObject(obj);
//            if (Shift.Contains(ssCtrl)) UI->Command(COMMAND_SHOWPROPERTIES);
//            if (!Shift.Contains(ssAlt)) ResetActionToSelect();
//        }else{
            _DELETE(obj);
//        }

    }
*/
    return false;
}


