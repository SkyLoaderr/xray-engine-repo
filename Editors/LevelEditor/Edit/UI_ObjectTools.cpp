#include "stdafx.h"
#pragma hdrstop

#include "UI_ObjectTools.h"
#include "ui_tools.h"
#include "library.h"
#include "scene.h"
#include "SceneObject.h"
//#include "EditObject.h"
#include "FrameObject.h"
#include "leftbar.h"
#include "EditorPref.h"
#include "ui_main.h"

//----------------------------------------------------------------------
TUI_ObjectTools::TUI_ObjectTools():TUI_CustomTools(OBJCLASS_SCENEOBJECT,true){
    AddControlCB(xr_new<TUI_ControlObjectAdd >(estDefault,eaAdd,		this));
}

void TUI_ObjectTools::OnActivate  ()
{
    pFrame = xr_new<TfraObject>((TComponent*)0);
	TUI_CustomTools::OnActivate();
    ((TfraObject*)pFrame)->OnEnter();
}
void TUI_ObjectTools::OnDeactivate()
{
    ((TfraObject*)pFrame)->OnExit();
	TUI_CustomTools::OnDeactivate();
    xr_delete(pFrame);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlObjectAdd::TUI_ControlObjectAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}

bool __fastcall TUI_ControlObjectAdd::Start(TShiftState Shift)
{
    if (Shift==ssRBOnly){ UI.Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_SCENEOBJECT); return false;}
    TfraObject* fraObject = (TfraObject*)parent_tool->pFrame; VERIFY(fraObject);
	Fvector p,n;
	if(!UI.PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,1,&n)) return false;
    LPCSTR N = ((TfraObject*)parent_tool->pFrame)->Current();
    if(!N){
    	ELog.DlgMsg(mtInformation,"Nothing selected.");
     	return false;
    }

    { // pick already executed (see top)
        string256 namebuffer;
        Scene.GenObjectName(OBJCLASS_SCENEOBJECT, namebuffer, N);
        CSceneObject *obj = xr_new<CSceneObject>((LPVOID)0,namebuffer);
        CEditableObject* ref = obj->SetReference(N);
        if (!ref){
        	ELog.DlgMsg(mtError,"TUI_ControlObjectAdd:: Can't load reference object.");
        	xr_delete(obj);
        	return false;
        }
        if (fraLeftBar->ebRandomAdd->Down){
            Fvector S;
            if (frmEditPrefs->cbRandomRotation->Checked){
            	Fvector p;
                p.set(	deg2rad(Random.randF(frmEditPrefs->seRandomRotateMinX->Value,frmEditPrefs->seRandomRotateMaxX->Value)),
                 		deg2rad(Random.randF(frmEditPrefs->seRandomRotateMinY->Value,frmEditPrefs->seRandomRotateMaxY->Value)),
                        deg2rad(Random.randF(frmEditPrefs->seRandomRotateMinZ->Value,frmEditPrefs->seRandomRotateMaxZ->Value)));
                obj->PRotation = p;
            }
            if (frmEditPrefs->cbRandomScale->Checked){
            	Fvector s;
                s.set(	Random.randF(frmEditPrefs->seRandomScaleMinX->Value,frmEditPrefs->seRandomScaleMaxX->Value),
                        Random.randF(frmEditPrefs->seRandomScaleMinY->Value,frmEditPrefs->seRandomScaleMaxY->Value),
                        Random.randF(frmEditPrefs->seRandomScaleMinZ->Value,frmEditPrefs->seRandomScaleMaxZ->Value));
                obj->PScale = s;
            }
            if (frmEditPrefs->cbRandomSize->Checked){
            	Fvector s;
                s.x=s.y=s.z=Random.randF(frmEditPrefs->seRandomSizeMin->Value,frmEditPrefs->seRandomSizeMax->Value);
                obj->PScale = s;
            }
        }
        obj->MoveTo(p,n);

        Scene.SelectObjects(false,OBJCLASS_SCENEOBJECT);
        Scene.AppendObject( obj );
        if (Shift.Contains(ssCtrl)) UI.Command(COMMAND_SHOW_PROPERTIES);
        if (!Shift.Contains(ssAlt)) ResetActionToSelect();
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


