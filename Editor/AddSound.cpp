#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "AddSound.h"
#include "UI_Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraAddSound *fraAddSound;
//---------------------------------------------------------------------------
__fastcall TfraAddSound::TfraAddSound(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etSound,eaAdd,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraAddSound::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAddSound::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

