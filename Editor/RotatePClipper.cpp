#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "RotatePClipper.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraRotatePClipper *fraRotatePClipper;
//---------------------------------------------------------------------------
__fastcall TfraRotatePClipper::TfraRotatePClipper(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etPClipper,eaRotate,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraRotatePClipper::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraRotatePClipper::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

