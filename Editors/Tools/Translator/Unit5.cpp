//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit5.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma resource "*.dfm"
TDiffStrDialog *DiffStrDialog;
//---------------------------------------------------------------------------
__fastcall TDiffStrDialog::TDiffStrDialog(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void  TDiffStrDialog::InitializeList (xml_diff_strings *list)
{
    FilesList->Clear ();
    StringsList->Clear ();
    m_List = list;

    xml_diff_strings_i fs = m_List->begin (), fe = m_List->end ();
    for (; fs != fe; ++fs)
        FilesList->Items->Add ((AnsiString)(*fs).first.c_str ());
    FilesList->ItemIndex = 0;
    FilesList->Selected[0] = true;
    FilesListClick (this);
}
//---------------------------------------------------------------------------
void __fastcall TDiffStrDialog::FilesListClick(TObject *Sender)
{
    StringsList->Clear ();
    string str = FilesList->Items->Strings[FilesList->ItemIndex].c_str ();
    xml_string_id_i s = (*m_List)[str].begin (), e = (*m_List)[str].end ();
    for (; s != e; ++s)
        StringsList->Items->Add ((AnsiString)(*s).c_str ());
    StringsList->ItemIndex = 0;
    StringsList->Selected[0] = true;
}
//---------------------------------------------------------------------------
void __fastcall TDiffStrDialog::FormDestroy(TObject *Sender)
{
    delete m_List;
}
//---------------------------------------------------------------------------
void __fastcall TDiffStrDialog::GotoButtonClick(TObject *Sender)
{
    if (GotoButton->Enabled == false)
       return;
    if (!FilesList->Items->Count)
       {
       ModalResult = mrCancel;
       return;
       }     
    m_File = FilesList->Items->Strings[FilesList->ItemIndex].c_str();
    m_String = StringsList->Items->Strings[StringsList->ItemIndex].c_str ();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

