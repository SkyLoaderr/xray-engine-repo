//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "ChoseLand.h"
#include "Library.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmChoseLand *frmChoseLand;
//---------------------------------------------------------------------------
__fastcall TfrmChoseLand::TfrmChoseLand(TComponent* Owner)
    : TForm(Owner)
{
    pCurNode = 0;
    tvLands->Items->Clear();
    Lib.SetLands();
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseLand::AddFolder(char* name)
{
    pCurNode = tvLands->Items->Add(0,name);
    pCurNode->ImageIndex = 3;
    pCurNode->SelectedIndex = 4;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseLand::AddSubItem(char* name, void* obj)
{
    TTreeNode* n = tvLands->Items->AddChildObject(pCurNode,name,obj);
    n->ImageIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseLand::sbSelectClick(TObject *Sender)
{
    ModalResult = mrOk;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseLand::sbCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
    Close();
}
//---------------------------------------------------------------------------
