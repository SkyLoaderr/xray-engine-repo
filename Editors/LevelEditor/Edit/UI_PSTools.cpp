#include "stdafx.h"
#pragma hdrstop

#include "UI_PSTools.h"
#include "ui_tools.h"
#include "PSLibrary.h"
#include "EParticlesObject.h"
#include "scene.h"
#include "FramePS.h"
#include "ui_main.h"

//----------------------------------------------------------------------
TUI_PSTools::TUI_PSTools():TUI_CustomTools(OBJCLASS_PS,true)
{
    AddControlCB(xr_new<TUI_ControlPSAdd>(estDefault,eaAdd,		this));
}
void TUI_PSTools::OnActivate  ()
{
    pFrame = xr_new<TfraPS>((TComponent*)0);
	TUI_CustomTools::OnActivate();
}
void TUI_PSTools::OnDeactivate()
{
	TUI_CustomTools::OnDeactivate();
    xr_delete(pFrame);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlPSAdd::TUI_ControlPSAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlPSAdd::AfterAppendCallback(TShiftState Shift, CCustomObject* obj)
{
	EParticlesObject* pg= dynamic_cast<EParticlesObject*>(obj); R_ASSERT(pg);
    LPCSTR ref_name		= ::Render->PSystems.GetCurrentPED();
    if (!ref_name)		return false;
	if (!pg->Compile(ref_name)){
    	ELog.DlgMsg(mtInformation,"Can't compile particle system '%s'.",ref_name);
        return false;
    }
    return true;
}

bool __fastcall TUI_ControlPSAdd::Start(TShiftState Shift)
{
	DefaultAddObject	(Shift,0,AfterAppendCallback);
    return false;
}

void __fastcall TUI_ControlPSAdd::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlPSAdd::End(TShiftState _Shift)
{
    return true;
}


