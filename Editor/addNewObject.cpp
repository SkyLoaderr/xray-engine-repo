#include "pch.h"
#pragma hdrstop

#include "EditLibrary.h"
#include "addNewObject.h"
#include "scene.h"
#include "library.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "RXCtrls"
#pragma resource "*.dfm"
TfrmAddNewObject *frmAddNewObject;
//---------------------------------------------------------------------------
__fastcall TfrmAddNewObject::TfrmAddNewObject(TComponent* Owner)
    : TForm(Owner)
{
    edName->Text = "";
    lbMeshes->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAddNewObject::ebAddClick(TObject *Sender)
{
    if (!edName->Text.Length()){
        MessageDlg("Enter Object name!", mtError, TMsgDlgButtons() << mbOK, 0);
        return;
    }
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmAddNewObject::sbNewObjectClick(TObject *Sender)
{
   	char buffer[MAX_PATH]="";
    FS.m_Meshes.Update(buffer);
    odMesh->InitialDir = buffer;
    if (odMesh->Execute()){
        AnsiString new_name = ExtractFileName(odMesh->FileName);
        bool bEq = false;
        for (int i = lbMeshes->Items->Count-1; i >= 0 ; i--)
            if (lbMeshes->Items->Strings[i]==new_name){ bEq = true; break; }
        if (!bEq){
            lbMeshes->Items->Add(new_name);
            ebAdd->Enabled = !!edName->Text.Length() && !!lbMeshes->Items->Count;
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmAddNewObject::edNameChange(TObject *Sender)
{
    ebAdd->Enabled = !!edName->Text.Length() && !!lbMeshes->Items->Count;
}
//---------------------------------------------------------------------------

void __fastcall TfrmAddNewObject::ExtBtn1Click(TObject *Sender)
{
    for (int i = lbMeshes->Items->Count-1; i >= 0 ; i--)
        if (lbMeshes->Selected[i]) lbMeshes->Items->Delete(i);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAddNewObject::ExtBtn2Click(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

