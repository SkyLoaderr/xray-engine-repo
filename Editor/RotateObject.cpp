#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "RotateObject.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraRotateObject *fraRotateObject;
//---------------------------------------------------------------------------
__fastcall TfraRotateObject::TfraRotateObject(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etObject,eaRotate,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraRotateObject::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraRotateObject::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

