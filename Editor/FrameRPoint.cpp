#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameRPoint.h"
#include "Scene.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "RXCtrls"
#pragma link "multi_edit"
#pragma link "Placemnt"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraRPoint::TfraRPoint(TComponent* Owner)
        : TFrame(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfraRPoint::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraRPoint::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------



