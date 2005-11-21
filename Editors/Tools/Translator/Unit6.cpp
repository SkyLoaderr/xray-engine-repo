//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop
#include "config.h"
#include "Unit6.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma resource "*.dfm"
TAddLangDialog *AddLangDialog;
//---------------------------------------------------------------------------
__fastcall TAddLangDialog::TAddLangDialog(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAddLangDialog::LangShortcutChange(TObject *Sender)
{
    Button1->Enabled = LangShortcut->Text.IsEmpty() == false;
}
//---------------------------------------------------------------------------
void __fastcall TAddLangDialog::Button1Click(TObject *Sender)
{
    char path[512];
    m_pConfig->GetString ("languages", "base_path", path, "");
    if (!*path)
       throw Exception ("Invalid base path");
    size_t len = strlen (path);
    if (path[len - 1] != '\\') strcat (path, "\\");
    strcat (path, LangShortcut->Text.c_str());
    CreateDir (AnsiString (path));

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
