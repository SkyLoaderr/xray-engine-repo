#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_main.h"
#include "MovePClipper.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraMovePClipper *fraMovePClipper;
//---------------------------------------------------------------------------
__fastcall TfraMovePClipper::TfraMovePClipper(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etPClipper,eaMove,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraMovePClipper::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraMovePClipper::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraMovePClipper::sbLockClick(TObject *Sender)
{
    TExtBtn* btn = dynamic_cast<TExtBtn*>(Sender);
    if (btn&&btn->Down){
        if ((btn==sbLockX)||(btn==sbLockZ)){
            sbLockY->Down=false;
        }else if (Sender==sbLockY){
            sbLockX->Down=false;
            sbLockZ->Down=false;
        }
    }
}
//---------------------------------------------------------------------------

