#include "stdafx.h"
#pragma hdrstop

#include "UI_OccluderTools.h"
#include "ui_tools.h"
#include "library.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "FrameOccluder.h"
#include "ui_main.h"
#include "occluder.h"
#include "topbar.h"
#include "Cursor3D.h"

//----------------------------------------------------------------------
TUI_OccluderTools::TUI_OccluderTools():TUI_CustomTools(OBJCLASS_OCCLUDER){
    AddControlCB(new TUI_ControlOccluderSelect(estSelf,eaSelect,	this));
    AddControlCB(new TUI_ControlOccluderAdd (estSelf,eaAdd,		this));
    AddControlCB(new TUI_ControlOccluderMove(estSelf,eaMove,	this));
    AddControlCB(new TUI_CustomControl		(estSelf,eaRotate,	this));
    AddControlCB(new TUI_CustomControl		(estSelf,eaScale,	this));
}
//------------------------------------------------------------------------------
void TUI_OccluderTools::OnActivate  (){
    pFrame = new TfraOccluder(0);
    ((TfraOccluder*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_OccluderTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    ((TfraOccluder*)pFrame)->fsStorage->SaveFormPlacement();
    _DELETE(pFrame);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlOccluderAdd::TUI_ControlOccluderAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
    pFrame = 0;
}
void TUI_ControlOccluderAdd::OnEnter(){
    UI->m_Cursor->Style(csPoint);
    UI->m_Cursor->Visible(true);
    pFrame = (TfraOccluder*)parent_tool->pFrame; VERIFY(pFrame);
}
void TUI_ControlOccluderAdd::OnExit (){
    UI->m_Cursor->Visible(false);
    UI->UpdateScene();
}
bool __fastcall TUI_ControlOccluderAdd::Start(TShiftState Shift){
	if (pFrame->ebModePlane->Down){
		SceneObject* obj = DefaultAddObject(Shift);
    }
    return false;
}

void __fastcall TUI_ControlOccluderAdd::Move(TShiftState _Shift){
}

bool __fastcall TUI_ControlOccluderAdd::End(TShiftState Shift){
    return false;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlOccluderMove::TUI_ControlOccluderMove(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
    pFrame 	= 0;
}
void TUI_ControlOccluderMove::OnEnter(){
    pFrame 	= (TfraOccluder*)parent_tool->pFrame; VERIFY(pFrame);
    obj     = 0;
}
void TUI_ControlOccluderMove::OnExit (){
	pFrame 	= 0;
}
bool __fastcall TUI_ControlOccluderMove::Start(TShiftState Shift){
	if (pFrame->ebModePoint->Down){
	    MovingStart(Shift);
		obj = (COccluder*)Scene->GetQueryObject(OBJCLASS_OCCLUDER);
    	return true;
    }else{
	    return MovingStart(Shift);
    }
}
void __fastcall TUI_ControlOccluderMove::Move(TShiftState _Shift){
	if (pFrame->ebModePlane->Down){
		MovingProcess(_Shift);
    }else{
		Fvector amount;
		if (DefaultMovingProcess(_Shift,amount)){
    	    if (obj) obj->MovePoints(amount);
        }
    }
}

bool __fastcall TUI_ControlOccluderMove::End(TShiftState Shift){
    return MovingEnd(Shift);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlOccluderSelect::TUI_ControlOccluderSelect(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
    pFrame 	= 0;
}
void TUI_ControlOccluderSelect::OnEnter(){
    pFrame 	= (TfraOccluder*)parent_tool->pFrame; VERIFY(pFrame);
    obj     = 0;
}
void TUI_ControlOccluderSelect::OnExit (){
	pFrame = 0;
}
bool __fastcall TUI_ControlOccluderSelect::Start(TShiftState Shift){
	if (pFrame->ebModePlane->Down){
	    return SelectStart(Shift);
    }else{
        obj 	= (COccluder*)Scene->GetQueryObject(OBJCLASS_OCCLUDER);
  		if (obj){
	        bBoxSelection	= !obj->SelectPoint(UI->m_CurrentRStart,UI->m_CurrentRNorm,Shift.Contains(ssCtrl));

    	    if( bBoxSelection ){
        	    UI->EnableSelectionRect( true );
            	UI->UpdateSelectionRect(UI->m_StartCp,UI->m_CurrentCp);
	            return true;
    	    }
        }

        UI->RedrawScene();
    	return false;
    }
}
void __fastcall TUI_ControlOccluderSelect::Move(TShiftState Shift){
	SelectProcess(Shift);
}

bool __fastcall TUI_ControlOccluderSelect::End(TShiftState Shift){
	if (pFrame->ebModePlane->Down){
    	return SelectEnd(Shift);
    }else{
        if (bBoxSelection){
            UI->EnableSelectionRect( false );
            bBoxSelection = false;
  			if (obj){
				CFrustum frustum;
			    if (UI->SelectionFrustum(frustum))
	                obj->SelectPoint(frustum,Shift.Contains(ssCtrl));
            }
        }
     	return true;
    }
}

