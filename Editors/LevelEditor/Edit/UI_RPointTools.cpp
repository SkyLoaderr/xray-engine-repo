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

bool __fastcall TUI_ControlSpawnPointAdd::Start(TShiftState Shift){
    AnsiString ref_name = ((TfraSpawnPoint*)parent_tool->pFrame)->GetCurrentEntity();
    if (ref_name.IsEmpty()) return false;
    DefaultAddObject(Shift,ref_name.c_str(),ref_name.c_str());
    return false;
}

