#include "stdafx.h"
#pragma hdrstop

#include "UI_ObjectTools.h"
#include "ui_tools.h"
#include "library.h"
#include "scene.h"
#include "EditObject.h"
#include "FrameObject.h"
#include "ui_main.h"
#include "leftbar.h"
#include "EditorPref.h"

//----------------------------------------------------------------------
TUI_ObjectTools::TUI_ObjectTools():TUI_CustomTools(OBJCLASS_EDITOBJECT){
    AddControlCB(new TUI_CustomControl		(estSelf,eaSelect,	this));
    AddControlCB(new TUI_ControlObjectAdd   (estSelf,eaAdd,		this));
    AddControlCB(new TUI_CustomControl		(estSelf,eaMove,	this));
    AddControlCB(new TUI_CustomControl		(estSelf,eaRotate,	this));
    AddControlCB(new TUI_CustomControl		(estSelf,eaScale,	this));

//    AddControlCB(new TUI_ControlTexturingSelect (estTexturing,eaSelect));
//    AddControlCB(new TUI_ControlTexturingSelect (estGizmo,eaSelect));
//    AddControlCB(new TUI_ControlGizmoMove       (estGizmo,eaMove));
//    AddControlCB(new TUI_ControlGizmoRotate     (estGizmo,eaRotate));
//    AddControlCB(new TUI_ControlGizmoScale      (estGizmo,eaScale));
}

void TUI_ObjectTools::OnActivate  (){
    pFrame = new TfraObject(0);
	TUI_CustomTools::OnActivate();
}
void TUI_ObjectTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlObjectAdd::TUI_ControlObjectAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlObjectAdd::Start(TShiftState Shift){
    if (Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_EDITOBJECT); return false;}
    TfraObject* fraObject = (TfraObject*)parent_tool->pFrame; VERIFY(fraObject);
	Fvector p;
	if(!UI->PickGround(p,UI->m_CurrentRStart,UI->m_CurrentRNorm)) return false;
    CLibObject* LO = Lib->GetCurrentObject();
    if(!LO){ 
    	fraObject->ebCurObjClick(0);
	    LO = Lib->GetCurrentObject();
    }
	if(LO!=0){
        if (UI->PickGround(p,UI->m_CurrentRStart,UI->m_CurrentRNorm)){
            char namebuffer[MAX_OBJ_NAME];
            Scene->GenObjectName( OBJCLASS_EDITOBJECT, namebuffer, LO->GetRefName() );
            CEditObject *obj = new CEditObject( namebuffer );
            obj->LibReference( LO->GetReference() );
            //obj->CloneFromLib( LO->GetReference() );
            if (fraLeftBar->ebRandomAdd->Down){
                Fvector S;
                if (frmEditorPreferences->cbRandomRotation->Checked){
                    obj->Rotate().set(deg2rad(Random.randF(frmEditorPreferences->seRandomRotateMinX->Value,frmEditorPreferences->seRandomRotateMaxX->Value)),
                                    deg2rad(Random.randF(frmEditorPreferences->seRandomRotateMinY->Value,frmEditorPreferences->seRandomRotateMaxY->Value)),
                                    deg2rad(Random.randF(frmEditorPreferences->seRandomRotateMinZ->Value,frmEditorPreferences->seRandomRotateMaxZ->Value)));
                }
                if (frmEditorPreferences->cbRandomScale->Checked){
                    obj->Scale().set(Random.randF(frmEditorPreferences->seRandomScaleMinX->Value,frmEditorPreferences->seRandomScaleMaxX->Value),
                                    Random.randF(frmEditorPreferences->seRandomScaleMinY->Value,frmEditorPreferences->seRandomScaleMaxY->Value),
                                    Random.randF(frmEditorPreferences->seRandomScaleMinZ->Value,frmEditorPreferences->seRandomScaleMaxZ->Value));
                }
                if (frmEditorPreferences->cbRandomSize->Checked){
                    obj->Scale().x=obj->Scale().y=obj->Scale().z=Random.randF(frmEditorPreferences->seRandomSizeMin->Value,frmEditorPreferences->seRandomSizeMax->Value);
                }
            }
            obj->Move(p);
            Scene->SelectObjects(false,OBJCLASS_EDITOBJECT);
            Scene->AddObject( obj );
            if (Shift.Contains(ssCtrl)) UI->Command(COMMAND_SHOWPROPERTIES);
            if (!Shift.Contains(ssAlt)) ResetActionToSelect();
        }
    }
    return false;
}

void __fastcall TUI_ControlObjectAdd::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlObjectAdd::End(TShiftState _Shift)
{
    return true;
}


