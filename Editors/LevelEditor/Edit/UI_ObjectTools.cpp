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
TUI_ObjectTools::TUI_ObjectTools():TUI_CustomTools(OBJCLASS_SCENEOBJECT){
    AddControlCB(new TUI_ControlObjectAdd   (estSelf,eaAdd,		this));
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
    if (Shift==ssRBOnly){ UI.Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_SCENEOBJECT); return false;}
    TfraObject* fraObject = (TfraObject*)parent_tool->pFrame; VERIFY(fraObject);
	Fvector p,n;
	if(!UI.PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,1,&n)) return false;
    LPCSTR N = Lib.GetCurrentObject();
    if(!N){
    	fraObject->ebCurObjClick(0);
	    N = Lib.GetCurrentObject();

    }
    if(!N) return false;

    { // pick already executed (see top)
        char namebuffer[MAX_OBJ_NAME];
        Scene.GenObjectName(OBJCLASS_SCENEOBJECT, namebuffer, N);
        CSceneObject *obj = new CSceneObject(0,namebuffer);
        CEditableObject* ref = obj->SetReference(N);
        if (!ref){
        	ELog.DlgMsg(mtError,"TUI_ControlObjectAdd:: Can't load reference object.");
        	_DELETE(obj);
        	return false;
        }
        if (fraLeftBar->ebRandomAdd->Down){
            Fvector S;
            if (frmEditorPreferences->cbRandomRotation->Checked){
            	Fvector p;
                p.set(	deg2rad(Random.randF(frmEditorPreferences->seRandomRotateMinX->Value,frmEditorPreferences->seRandomRotateMaxX->Value)),
                 		deg2rad(Random.randF(frmEditorPreferences->seRandomRotateMinY->Value,frmEditorPreferences->seRandomRotateMaxY->Value)),
                        deg2rad(Random.randF(frmEditorPreferences->seRandomRotateMinZ->Value,frmEditorPreferences->seRandomRotateMaxZ->Value)));
                obj->PRotation = p;
            }
            if (frmEditorPreferences->cbRandomScale->Checked){
            	Fvector s;
                s.set(	Random.randF(frmEditorPreferences->seRandomScaleMinX->Value,frmEditorPreferences->seRandomScaleMaxX->Value),
                        Random.randF(frmEditorPreferences->seRandomScaleMinY->Value,frmEditorPreferences->seRandomScaleMaxY->Value),
                        Random.randF(frmEditorPreferences->seRandomScaleMinZ->Value,frmEditorPreferences->seRandomScaleMaxZ->Value));
                obj->PScale = s;
            }
            if (frmEditorPreferences->cbRandomSize->Checked){
            	Fvector s;
                s.x=s.y=s.z=Random.randF(frmEditorPreferences->seRandomSizeMin->Value,frmEditorPreferences->seRandomSizeMax->Value);
                obj->PScale = s;
            }
        }
        obj->MoveTo(p,n);
        
        Scene.SelectObjects(false,OBJCLASS_SCENEOBJECT);
        Scene.AddObject( obj );
        if (Shift.Contains(ssCtrl)) UI.Command(COMMAND_SHOWPROPERTIES);
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


