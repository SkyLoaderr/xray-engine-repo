#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameRPoint.h"
#include "Scene.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraSpawnPoint::TfraSpawnPoint(TComponent* Owner)
        : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
    ebCurObj->Caption=NONE_CAPTION;
}
//---------------------------------------------------------------------------
void __fastcall TfraSpawnPoint::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawnPoint::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

AnsiString TfraSpawnPoint::GetCurrentEntity(BOOL bForceSelect)
{
	if ((bForceSelect)||(ebCurObj->Caption==NONE_CAPTION)) ebCurObjClick(0);
    return (ebCurObj->Caption!=NONE_CAPTION)?ebCurObj->Caption:AnsiString("");
}
void __fastcall TfraSpawnPoint::ebCurObjClick(TObject *Sender)
{
	LPCSTR N = TfrmChoseItem::SelectEntity((ebCurObj->Caption!=NONE_CAPTION)?ebCurObj->Caption.c_str():0);
    if (N) ebCurObj->Caption = N;
}
//---------------------------------------------------------------------------

