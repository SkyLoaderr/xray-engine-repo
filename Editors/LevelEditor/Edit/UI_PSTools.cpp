#include "stdafx.h"
#pragma hdrstop

#include "UI_PSTools.h"
#include "ui_tools.h"
#include "PSLibrary.h"
#include "PSObject.h"
#include "scene.h"
#include "FramePS.h"
#include "ui_main.h"

//----------------------------------------------------------------------
TUI_PSTools::TUI_PSTools():TUI_CustomTools(OBJCLASS_PS){
    AddControlCB(new TUI_ControlPSAdd   (estSelf,eaAdd,		this));
}
void TUI_PSTools::OnActivate  (){
    pFrame = new TfraPS(0);
	TUI_CustomTools::OnActivate();
}
void TUI_PSTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlPSAdd::TUI_ControlPSAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlPSAdd::Start(TShiftState Shift){
    if (Shift==ssRBOnly){ UI.Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_PS); return false;}
	Fvector p,up;
	if(!UI.PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm,1,&up)) return false;

    TfraPS* fraPS = (TfraPS*)parent_tool->pFrame; VERIFY(fraPS);
    PS::SDef* PS = PSLib.GetCurrentPS();
    if(!PS){
    	fraPS->ebCurObjClick(0);
        PS = PSLib.GetCurrentPS();
    }
    if(!PS) return false;

    if (UI.PickGround(p,UI.m_CurrentRStart,UI.m_CurrentRNorm)){
        char namebuffer[MAX_OBJ_NAME];
        Scene.GenObjectName( OBJCLASS_PS, namebuffer, PS->m_Name );
        CPSObject *obj = new CPSObject( 0, namebuffer );
        obj->Compile(PS);
        obj->MoveTo(p,up);
        Scene.SelectObjects(false,OBJCLASS_PS);
        Scene.AddObject( obj );
        if (Shift.Contains(ssCtrl)) UI.Command(COMMAND_SHOWPROPERTIES);
        if (!Shift.Contains(ssAlt)) ResetActionToSelect();
    }
    return false;
}

void __fastcall TUI_ControlPSAdd::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlPSAdd::End(TShiftState _Shift)
{
    return true;
}


