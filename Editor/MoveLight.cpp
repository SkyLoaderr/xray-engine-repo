#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "MoveLight.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraMoveLight *fraMoveLight;
//---------------------------------------------------------------------------
__fastcall TfraMoveLight::TfraMoveLight(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etLight,eaMove,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraMoveLight::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}

void __fastcall TfraMoveLight::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TfraMoveLight::sbLockClick(TObject *Sender)
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

