#include "stdafx.h"
#pragma hdrstop

#include "ui_main.h"
#include "FrameGroup.h"
#include "GroupObject.h"
#include "Scene.h"
#include "escenegrouptools.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraGroup::TfraGroup(TComponent* Owner, ESceneGroupTools* gt)
        : TForm(Owner)
{
	ParentTools = gt;	VERIFY(ParentTools);
}
//---------------------------------------------------------------------------
void __fastcall TfraGroup::PaneMinClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ExpandClick(TObject *Sender)
{
    PanelMaximizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebMakeGroupClick(TObject *Sender)
{
    ParentTools->GroupObjects	();
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebUngroupClick(TObject *Sender)
{
    ParentTools->UngroupObjects	();
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebCenterToGroupClick(TObject *Sender)
{
    ParentTools->CenterToGroup	();
}
//---------------------------------------------------------------------------


