#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "AddPClipper.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "multiobj"
#pragma resource "*.dfm"
TfraAddPClipper *fraAddPClipper;
//---------------------------------------------------------------------------
__fastcall TfraAddPClipper::TfraAddPClipper(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etPClipper,eaAdd,this);
    paFixed->Visible = false;
    cbFixed->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TfraAddPClipper::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAddPClipper::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAddPClipper::cbFixedClick(TObject *Sender)
{
    paFixed->Visible = cbFixed->Checked;
}
//---------------------------------------------------------------------------

