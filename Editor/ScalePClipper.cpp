#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ScalePClipper.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraScalePClipper *fraScalePClipper;
//---------------------------------------------------------------------------
__fastcall TfraScalePClipper::TfraScalePClipper(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etPClipper,eaScale,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraScalePClipper::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraScalePClipper::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

