#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "MoveScene.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraMoveScene *fraMoveScene;
//---------------------------------------------------------------------------
__fastcall TfraMoveScene::TfraMoveScene(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etScene,eaMove,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraMoveScene::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}

void __fastcall TfraMoveScene::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraMoveScene::sbLockClick(TObject *Sender)
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

