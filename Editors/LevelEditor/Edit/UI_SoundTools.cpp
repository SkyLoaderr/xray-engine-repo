#include "stdafx.h"
#pragma hdrstop

#include "ui_soundtools.h"
#include "ui_tools.h"
#include "FrameSound.h"
#include "ESound.h"

//---------------------------------------------------------------------------
TUI_SoundTools::TUI_SoundTools():TUI_CustomTools(OBJCLASS_SOUND)
{
    AddControlCB(xr_new<TUI_ControlSoundAdd>(estSelf,eaAdd,	this));
}
void TUI_SoundTools::OnActivate  ()
{
    pFrame = xr_new<TfraSound>((TComponent*)0);
    ((TfraSound*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_SoundTools::OnDeactivate()
{
    ((TfraSound*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    xr_delete(pFrame);
}
//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlSoundAdd::TUI_ControlSoundAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}

bool __fastcall TUI_ControlSoundAdd::AppendCallback(SBeforeAppendCallbackParams* p)
{
	p->data 		= (void*)((TfraSound*)parent_tool->pFrame)->GetCurrentSoundType(); 
    return (ESound::stUndefined!=(ESound::ESoundType)p->data);
}

bool __fastcall TUI_ControlSoundAdd::AfterAppendCallback(TShiftState Shift, CCustomObject* obj)
{
/*	ESound* snd 		= dynamic_cast<ESound*>(obj); R_ASSERT(snd);
	TfraSound* F 		= (TfraSound*)parent_tool->pFrame;
	if (F->ebTypeStaticSource->Down){
        return true;
	}else if (F->ebTypeEnvironment->Down){
        return true;
    }else{
    	ELog.DlgMsg(mtInformation,"Select append type at first.");
    }
*/    return false;
}

bool __fastcall TUI_ControlSoundAdd::Start(TShiftState Shift)
{
//	TfraSound* F 		= (TfraSound*)parent_tool->pFrame;
	DefaultAddObject	(Shift,AppendCallback);//,AfterAppendCallback);
    return false;
}

