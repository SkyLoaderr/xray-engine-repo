#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "MoveObject.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraMoveObject *fraMoveObject;
//---------------------------------------------------------------------------
__fastcall TfraMoveObject::TfraMoveObject(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etObject,eaMove,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraMoveObject::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraMoveObject::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraMoveObject::sbLockClick(TObject *Sender)
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

