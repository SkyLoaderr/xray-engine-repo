#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "MoveSound.h"
#include "UI_Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraMoveSound *fraMoveSound;
//---------------------------------------------------------------------------
__fastcall TfraMoveSound::TfraMoveSound(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etSound,eaMove,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraMoveSound::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraMoveSound::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraMoveSound::sbLockClick(TObject *Sender)
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

