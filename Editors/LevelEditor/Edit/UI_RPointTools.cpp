#include "stdafx.h"
#pragma hdrstop

#include "ui_RPointtools.h"
#include "ui_tools.h"
#include "FrameRPoint.h"
#include "Scene.h"
#include "SpawnPoint.h"
//---------------------------------------------------------------------------

TUI_SpawnPointTools::TUI_SpawnPointTools():TUI_CustomTools(OBJCLASS_SPAWNPOINT){
    AddControlCB(new TUI_ControlSpawnPointAdd(estSelf,eaAdd,	this));
}
void TUI_SpawnPointTools::OnActivate  (){
    pFrame = new TfraSpawnPoint(0);
    ((TfraSpawnPoint*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_SpawnPointTools::OnDeactivate(){
    ((TfraSpawnPoint*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}

//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlSpawnPointAdd::TUI_ControlSpawnPointAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlSpawnPointAdd::AppendCallback(SAppendCallbackParams* p)
{
	p->prefix 	= ((TfraSpawnPoint*)parent_tool->pFrame)->GetCurrentEntity();
	p->data 	= p->prefix.c_str();
    return !p->prefix.IsEmpty();
}

bool __fastcall TUI_ControlSpawnPointAdd::Start(TShiftState Shift)
{
    DefaultAddObject(Shift,AppendCallback);
    return false;
}

