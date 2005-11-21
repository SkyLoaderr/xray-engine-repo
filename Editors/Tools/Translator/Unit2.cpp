//---------------------------------------------------------------------------
#include <vcl.h>
#include <shlobj.h>
#pragma hdrstop

#pragma link "shell32.lib"

#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ToolEdit"
#pragma resource "*.dfm"
TConfigDialog *ConfigDialog;
//---------------------------------------------------------------------------
__fastcall TConfigDialog::TConfigDialog(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TConfigDialog::TextChange(TObject *Sender)
{
    if (Lang1->Text.IsEmpty () == false && Lang2->Text.IsEmpty () == false &&
        Path1->Text.IsEmpty () == false && ImportPath->Text.IsEmpty () == false)
       OkButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TConfigDialog::OkButtonClick(TObject *Sender)
{
    AnsiString str = Path1->Text + (AnsiString)"\\" + Lang1->Text;
    SHCreateDirectoryEx (NULL, str.c_str (), NULL);
    str = Path1->Text + (AnsiString)"\\" + Lang2->Text;
    SHCreateDirectoryEx (NULL, str.c_str (), NULL);
    str = ImportPath->Text + (AnsiString)"\\" + Lang1->Text;
    SHCreateDirectoryEx (NULL, str.c_str (), NULL);
    str = ImportPath->Text + (AnsiString)"\\" + Lang2->Text;
    SHCreateDirectoryEx (NULL, str.c_str (), NULL);
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

