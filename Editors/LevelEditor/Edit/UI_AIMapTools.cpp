#include "stdafx.h"
#pragma hdrstop

#include "UI_AIMapTools.h"
#include "ui_tools.h"
#include "scene.h"
#include "FrameAIMap.h"
#include "ui_main.h"
#include "ESceneAIMapTools.h"
#include "TopBar.h"

//----------------------------------------------------------------------
TUI_AIMapTools::TUI_AIMapTools():TUI_CustomTools(OBJCLASS_AIMAP,false)
{
	// node tools
    AddControlCB(xr_new<TUI_ControlAIMapNodeSelect>		(estAIMapNode,		eaSelect, 	this));
    AddControlCB(xr_new<TUI_ControlAIMapNodeAdd>		(estAIMapNode,		eaAdd, 		this));
    // emitter tools
    AddControlCB(xr_new<TUI_ControlAIMapEmitterSelect>	(estAIMapEmitter,	eaSelect, 	this));
    AddControlCB(xr_new<TUI_ControlAIMapEmitterAdd>		(estAIMapEmitter,	eaAdd, 		this));
    AddControlCB(xr_new<TUI_ControlAIMapEmitterMove>	(estAIMapEmitter,	eaMove,		this));
}
void TUI_AIMapTools::OnActivate  ()
{
    pFrame = xr_new<TfraAIMap>((TComponent*)0);
    TfraAIMap* fm = (TfraAIMap*)pFrame;
    fm->fsStorage->RestoreFormPlacement();
    // fill object list
	TUI_CustomTools::OnActivate();
}
void TUI_AIMapTools::OnDeactivate()
{
	TUI_CustomTools::OnDeactivate();
    ((TfraAIMap*)pFrame)->fsStorage->SaveFormPlacement();
    xr_delete(pFrame);
}
//------------------------------------------------------------------------------
// Node Add
//------------------------------------------------------------------------------
TUI_ControlAIMapNodeAdd::TUI_ControlAIMapNodeAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlAIMapNodeAdd::Start(TShiftState Shift)
{
	Fvector p;
    if (Scene.m_AIMap->PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar())){
    	Scene.m_AIMap->SelectObjects(false);
	    Scene.m_AIMap->AddNode(p,((TfraAIMap*)parent_tool->pFrame)->ebIgnoreConstraints->Down,((TfraAIMap*)parent_tool->pFrame)->ebAutoLink->Down);
		if (!Shift.Contains(ssAlt)){ 
        	ResetActionToSelect();
            return false;
        }else return true;
    }
    return false;
}
void TUI_ControlAIMapNodeAdd::Move(TShiftState _Shift)
{
	Fvector p;
    if (Scene.m_AIMap->PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar())){
	    Scene.m_AIMap->AddNode(p,((TfraAIMap*)parent_tool->pFrame)->ebIgnoreConstraints->Down,((TfraAIMap*)parent_tool->pFrame)->ebAutoLink->Down);
    }
}
bool TUI_ControlAIMapNodeAdd::End(TShiftState _Shift)
{
	if (!_Shift.Contains(ssAlt)) ResetActionToSelect();
	return true;
}

//------------------------------------------------------------------------------
// Emitter Add
//------------------------------------------------------------------------------
TUI_ControlAIMapEmitterAdd::TUI_ControlAIMapEmitterAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}
bool __fastcall TUI_ControlAIMapEmitterAdd::Start(TShiftState Shift)
{
	Fvector p;
    if (Scene.m_AIMap->PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar())){
    	Scene.m_AIMap->SelectObjects(false);
	    Scene.m_AIMap->AddEmitter(p);
		if (!Shift.Contains(ssAlt)) ResetActionToSelect();
    }
    return false;
}
//------------------------------------------------------------------------------
// Node Select
//------------------------------------------------------------------------------
TUI_ControlAIMapNodeSelect::TUI_ControlAIMapNodeSelect(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}
bool TUI_ControlAIMapNodeSelect::Start(TShiftState Shift)
{
    if (!(Shift.Contains(ssCtrl)||Shift.Contains(ssAlt))) Scene.m_AIMap->SelectObjects(false);

    SAINode* N 		= Scene.m_AIMap->PickNode(UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar());
    bBoxSelection   = (N && (Shift.Contains(ssCtrl)||Shift.Contains(ssAlt))) || !N;

    if( bBoxSelection ){
        UI.EnableSelectionRect( true );
        UI.UpdateSelectionRect(UI.m_StartCp,UI.m_CurrentCp);
        if(N){
        	if (Shift.Contains(ssCtrl)) N->flags.invert(SAINode::flSelected);
			else N->flags.set(SAINode::flSelected,!Shift.Contains(ssAlt));
        }
	    Scene.m_AIMap->UpdateHLSelected();
        return true;
    } else {
        if(N){
        	if (Shift.Contains(ssCtrl)) N->flags.invert(SAINode::flSelected);
            else N->flags.set(SAINode::flSelected,!Shift.Contains(ssAlt));
        }
    }
    Scene.m_AIMap->UpdateHLSelected();
    return false;
}
void __fastcall TUI_ControlAIMapNodeSelect::Move(TShiftState _Shift)
{
	SelectProcess(_Shift);
}
bool __fastcall TUI_ControlAIMapNodeSelect::End(TShiftState _Shift)
{
    return SelectEnd(_Shift);
}
//------------------------------------------------------------------------------
// Emitter Select
//------------------------------------------------------------------------------
TUI_ControlAIMapEmitterSelect::TUI_ControlAIMapEmitterSelect(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}
bool TUI_ControlAIMapEmitterSelect::Start(TShiftState Shift)
{
    if (!Shift.Contains(ssCtrl)) Scene.m_AIMap->SelectObjects(false);

    SAIEmitter* E 	= Scene.m_AIMap->PickEmitter(UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar());
    bBoxSelection   = (E && Shift.Contains(ssCtrl)) || !E;

    if(bBoxSelection){
        UI.EnableSelectionRect( true );
        UI.UpdateSelectionRect(UI.m_StartCp,UI.m_CurrentCp);
        if(E){
        	if (Shift.Contains(ssCtrl)) E->flags.invert(SAIEmitter::flSelected);
			else E->flags.set(SAIEmitter::flSelected,!Shift.Contains(ssAlt));
        }
        return true;
    }else{
        if(E){
        	if (Shift.Contains(ssCtrl)) E->flags.invert(SAIEmitter::flSelected);
			else E->flags.set(SAIEmitter::flSelected,!Shift.Contains(ssAlt));
        }
    }
    return false;
}
void __fastcall TUI_ControlAIMapEmitterSelect::Move(TShiftState _Shift)
{
	SelectProcess(_Shift);
}
bool __fastcall TUI_ControlAIMapEmitterSelect::End(TShiftState _Shift)
{
    return SelectEnd(_Shift);
}

//------------------------------------------------------------------------------------
// Emitter Move
//------------------------------------------------------------------------------------
TUI_ControlAIMapEmitterMove::TUI_ControlAIMapEmitterMove(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}
bool TUI_ControlAIMapEmitterMove::Start(TShiftState Shift)
{
    if(Scene.m_AIMap->SelectionCount(true)==0) return false;

    if (Shift.Contains(ssCtrl)){
		Fvector p;
    	if (Scene.m_AIMap->PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar())){
        	AIEmitterVec& lst = Scene.m_AIMap->Emitters();
            for(AIEmitterIt _F = lst.begin();_F!=lst.end();_F++)
                if(_F->flags.is(SAIEmitter::flSelected)) _F->pos.set(p);
			Scene.UndoSave();
        }
        return false;
    }else{
        if (fraTopBar->ebAxisY->Down){
            m_MovingXVector.set(0,0,0);
            m_MovingYVector.set(0,1,0);
        }else{
            m_MovingXVector.set( Device.m_Camera.GetRight() );
            m_MovingXVector.y = 0;
            m_MovingYVector.set( Device.m_Camera.GetDirection() );
            m_MovingYVector.y = 0;
            m_MovingXVector.normalize_safe();
            m_MovingYVector.normalize_safe();
        }
        m_MovingReminder.set(0,0,0);
    }
    return true;
}

void __fastcall TUI_ControlAIMapEmitterMove::Move(TShiftState _Shift)
{
	Fvector amount;
	if (DefaultMovingProcess(_Shift,amount)){
       	AIEmitterVec& lst = Scene.m_AIMap->Emitters();
        for(AIEmitterIt _F = lst.begin();_F!=lst.end();_F++)
            if(_F->flags.is(SAIEmitter::flSelected)) _F->pos.add(amount);
    }
}

bool __fastcall TUI_ControlAIMapEmitterMove::End(TShiftState _Shift)
{
	return MovingEnd(_Shift);
}

