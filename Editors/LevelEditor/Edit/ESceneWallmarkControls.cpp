//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESceneWallmarkControls.h"

#include "ESceneWallmarkTools.h"
#include "ui_main.h"
#include "scene.h"
#include "ui_leveltools.h"
#include "ui_levelmain.h"

//------------------------------------------------------------------------------
// Node Add
//------------------------------------------------------------------------------
TUI_ControlWallmarkAdd::TUI_ControlWallmarkAdd(int st, int act, ESceneCustomMTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlWallmarkAdd::Start(TShiftState Shift)
{
    ESceneWallmarkTools* S 	= (ESceneWallmarkTools*)parent_tool;
    S->SelectObjects		(false);
    wm_cnt					= 0;
    if (S->AddWallmark(UI->m_CurrentRStart,UI->m_CurrentRNorm)){
    	wm_cnt++;
		if (!Shift.Contains(ssAlt)){ 
		    Scene->UndoSave		();
        	ResetActionToSelect	();
            return false;
        }else return true;
    }
    return false;
}
void TUI_ControlWallmarkAdd::Move(TShiftState _Shift)
{
}
bool TUI_ControlWallmarkAdd::End(TShiftState _Shift)
{
	if (!_Shift.Contains(ssAlt))ResetActionToSelect();
	if (wm_cnt)	    			Scene->UndoSave		();
	return true;
}


