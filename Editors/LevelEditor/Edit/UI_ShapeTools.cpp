#include "stdafx.h"
#pragma hdrstop

#include "ui_Shapetools.h"
#include "ui_tools.h"
#include "FrameShape.h"
#include "EShape.h"
#include "ui_main.h"
#include "scene.h"
//---------------------------------------------------------------------------

TUI_ShapeTools::TUI_ShapeTools():TUI_CustomTools(OBJCLASS_SHAPE)
{
    AddControlCB(new TUI_ControlShapeAdd(estSelf,eaAdd,	this));
}
void TUI_ShapeTools::OnActivate  ()
{
    pFrame = new TfraShape(0);
    ((TfraShape*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_ShapeTools::OnDeactivate()
{
    ((TfraShape*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}

//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlShapeAdd::TUI_ControlShapeAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}

bool __fastcall TUI_ControlShapeAdd::AfterAppendCallback(TShiftState Shift, CCustomObject* obj)
{
	CEditShape* shape 	= dynamic_cast<CEditShape*>(obj); R_ASSERT(shape);
	TfraShape* F 		= (TfraShape*)parent_tool->pFrame;
	if (F->ebTypeSphere->Down){		
    	Fsphere S;	S.identity();
    	shape->add_sphere(S);
        if (!Shift.Contains(ssAlt)) F->ebTypeSphere->Down = false;
        return true;
	}else if (F->ebTypeBox->Down){
    	Fmatrix M;	M.identity();
    	shape->add_box(M);
		if (!Shift.Contains(ssAlt)) F->ebTypeBox->Down = false;
        return true;
    }else{
    	ELog.DlgMsg(mtInformation,"Select shape type at first.");
    }
    return false;
}

bool __fastcall TUI_ControlShapeAdd::Start(TShiftState Shift)
{
	TfraShape* F 		= (TfraShape*)parent_tool->pFrame;
    if (F->ebAttachShape->Down){
		CEditShape* from = dynamic_cast<CEditShape*>(Scene.RayPick(UI.m_CurrentRStart, UI.m_CurrentRNorm, OBJCLASS_SHAPE, 0, false, 0));
        if (from){
            ObjectList lst;
            int cnt 		= Scene.GetQueryObjects(lst,OBJCLASS_SHAPE,1,1,0);
            if (1!=cnt)		ELog.DlgMsg(mtError,"Select one shape.");
            else{
                CEditShape* base = dynamic_cast<CEditShape*>(lst.back()); R_ASSERT(base);
                if (base!=from){
	                base->Attach(from);
    	            if (!Shift.Contains(ssAlt)){ 
        	            F->ebAttachShape->Down 	= false;
            	        ResetActionToSelect		();
                	}
                }
            }
        }
    }else
	    DefaultAddObject(Shift,0,AfterAppendCallback);
    return false;
}

