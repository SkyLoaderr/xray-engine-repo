#include "stdafx.h"
#pragma hdrstop

#include "UI_AIMapTools.h"
#include "ESceneAIMapTools.h"
#include "ui_main.h"
#include "FrameAIMap.h"
#include "scene.h"
#include "TopBar.h"
#include "ui_tools.h"

//------------------------------------------------------------------------------
// Node Add
//------------------------------------------------------------------------------
TUI_ControlAIMapNodeAdd::TUI_ControlAIMapNodeAdd(int st, int act, ESceneCustomMTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlAIMapNodeAdd::Start(TShiftState Shift)
{
	append_nodes = 0;
	Fvector p;
    if (parent_tool->PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar())){
    	parent_tool->SelectObjects(false);
        ESceneAIMapTools* S = (ESceneAIMapTools*)parent_tool;
	    append_nodes		= S->AddNode(p,((TfraAIMap*)parent_tool->pFrame)->ebIgnoreConstraints->Down,((TfraAIMap*)parent_tool->pFrame)->ebAutoLink->Down,S->m_BrushSize);
		if (!Shift.Contains(ssAlt)){ 
		    if (append_nodes) Scene.UndoSave();
        	ResetActionToSelect();
            return false;
        }else return true;
    }
    return false;
}
void TUI_ControlAIMapNodeAdd::Move(TShiftState _Shift)
{
	Fvector p;
    if (parent_tool->PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar())){
        ESceneAIMapTools* S = (ESceneAIMapTools*)parent_tool;
	    append_nodes+=S->AddNode(p,((TfraAIMap*)parent_tool->pFrame)->ebIgnoreConstraints->Down,((TfraAIMap*)parent_tool->pFrame)->ebAutoLink->Down,S->m_BrushSize);
    }
}
bool TUI_ControlAIMapNodeAdd::End(TShiftState _Shift)
{
	if (!_Shift.Contains(ssAlt)) ResetActionToSelect();
    if (append_nodes) Scene.UndoSave();
	return true;
}

//------------------------------------------------------------------------------
// Node Select
//------------------------------------------------------------------------------
TUI_ControlAIMapNodeSelect::TUI_ControlAIMapNodeSelect(int st, int act, ESceneCustomMTools* parent):TUI_CustomControl(st,act,parent)
{
}
bool TUI_ControlAIMapNodeSelect::Start(TShiftState Shift)
{
	if (CheckSnapList(Shift)) return false;
    if (!(Shift.Contains(ssCtrl)||Shift.Contains(ssAlt))) parent_tool->SelectObjects(false);

    SAINode* N 		= ((ESceneAIMapTools*)parent_tool)->PickNode(UI.m_CurrentRStart,UI.m_CurrentRNorm,UI.ZFar());
    bBoxSelection   = (N && (Shift.Contains(ssCtrl)||Shift.Contains(ssAlt))) || !N;

    if( bBoxSelection ){
        UI.EnableSelectionRect( true );
        UI.UpdateSelectionRect(UI.m_StartCp,UI.m_CurrentCp);
        if(N){
        	if (Shift.Contains(ssCtrl)) N->flags.invert(SAINode::flSelected);
			else N->flags.set(SAINode::flSelected,!Shift.Contains(ssAlt));
        }
	    ((ESceneAIMapTools*)parent_tool)->UpdateHLSelected();
        return true;
    } else {
        if(N){
        	if (Shift.Contains(ssCtrl)) N->flags.invert(SAINode::flSelected);
            else N->flags.set(SAINode::flSelected,!Shift.Contains(ssAlt));
        }
    }
    ((ESceneAIMapTools*)parent_tool)->UpdateHLSelected();
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

//------------------------------------------------------------------------------------
// Node Move
//------------------------------------------------------------------------------------
TUI_ControlAIMapNodeMove::TUI_ControlAIMapNodeMove(int st, int act, ESceneCustomMTools* parent):TUI_CustomControl(st,act,parent)
{
}
bool TUI_ControlAIMapNodeMove::Start(TShiftState Shift)
{
    if(parent_tool->SelectionCount(true)==0) return false;

    if (fraTopBar->ebAxisY->Down){
        m_MovingXVector.set(0,0,0);
        m_MovingYVector.set(0,1,0);
    }else{
        m_MovingXVector.set(0,0,0);
        m_MovingYVector.set(0,0,0);
    }
    m_MovingReminder.set(0,0,0);
    return true;
}

void __fastcall TUI_ControlAIMapNodeMove::Move(TShiftState _Shift)
{
	Fvector amount;
	if (DefaultMovingProcess(_Shift,amount)){
       	AINodeVec& lst = ((ESceneAIMapTools*)parent_tool)->Nodes();
        for(AINodeIt _F = lst.begin();_F!=lst.end();_F++)
            if((*_F)->flags.is(SAINode::flSelected)){ 
            	(*_F)->Pos.add(amount);
            	(*_F)->Plane.build((*_F)->Pos,(*_F)->Plane.n);
            }
    }
}

bool __fastcall TUI_ControlAIMapNodeMove::End(TShiftState _Shift)
{
	return MovingEnd(_Shift);
}

//------------------------------------------------------------------------------------
// Rotate Node
//------------------------------------------------------------------------------------
TUI_ControlAIMapNodeRotate::TUI_ControlAIMapNodeRotate(int st, int act, ESceneCustomMTools* parent):TUI_CustomControl(st,act,parent)
{
}                                           
bool __fastcall TUI_ControlAIMapNodeRotate::Start(TShiftState Shift)
{
    if(parent_tool->SelectionCount(true)==0) return false;

    m_RotateVector.set(0,0,0);
    if (fraTopBar->ebAxisX->Down) m_RotateVector.set(1,0,0);
    else if (fraTopBar->ebAxisY->Down) m_RotateVector.set(0,0,0);
    else if (fraTopBar->ebAxisZ->Down) m_RotateVector.set(0,0,1);
	m_fRotateSnapAngle = 0;
    return true;
}

void __fastcall TUI_ControlAIMapNodeRotate::Move(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)){
        float amount = -UI.m_DeltaCpH.x * UI.m_MouseSR;

        if( fraTopBar->ebASnap->Down ) CHECK_SNAP(m_fRotateSnapAngle,amount,UI.anglesnap());

        Fmatrix R;
        if 	(fis_zero(m_RotateVector.x)) 	R.rotateZ(amount);
        else								R.rotateX(amount);
        
       	AINodeVec& lst = ((ESceneAIMapTools*)parent_tool)->Nodes();
        for(AINodeIt _F = lst.begin();_F!=lst.end();_F++)
            if((*_F)->flags.is(SAINode::flSelected)){
            	R.transform_dir((*_F)->Plane.n);
            	(*_F)->Plane.build((*_F)->Pos,(*_F)->Plane.n);
            }
    }
}
bool __fastcall TUI_ControlAIMapNodeRotate::End(TShiftState _Shift)
{
	return RotateEnd(_Shift);
}

