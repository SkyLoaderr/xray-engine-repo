#include "stdafx.h"
#pragma hdrstop

#include "ui_Shapetools.h"
#include "ui_tools.h"
#include "FrameShape.h"
#include "Scene.h"
#include "EShape.h"
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

bool __fastcall TUI_ControlShapeAdd::AfterAppendCallback(CCustomObject* obj)
{
	CEditShape* shape 	= dynamic_cast<CEditShape*>(obj); R_ASSERT(shape);
	TfraShape* F 		= (TfraShape*)parent_tool->pFrame;
	if (F->ebTypeSphere->Down){		
    	Fsphere S;	S.identity();
    	shape->add_sphere(S);
        F->ebTypeSphere->Down 	= false;
        return true;
	}else if (F->ebTypeBox->Down){
    	Fmatrix M;	M.identity();
    	shape->add_box(M);
        F->ebTypeBox->Down		= false;
        return true;
    }else{
    	ELog.DlgMsg(mtInformation,"Select shape type at first.");
    }
    return false;
}

bool __fastcall TUI_ControlShapeAdd::Start(TShiftState Shift)
{
    DefaultAddObject(Shift,0,AfterAppendCallback);
    return false;
}

