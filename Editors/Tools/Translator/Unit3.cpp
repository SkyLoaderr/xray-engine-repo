//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop
#include "config.h"
#include "Unit3.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLangDialog *LangDialog;
//---------------------------------------------------------------------------
__fastcall TLangDialog::TLangDialog(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void    TLangDialog::InitializeData  (vsConfig *pConfig)
{
    if (!pConfig)
       throw Exception ("Configuration interface is invalid !!!");
    char buf[512];
    pConfig->GetString("languages", "base_path", buf, "");
    if (!*buf)
       throw Exception ("Parameter [base_path] not found in file translator.ini");

    WIN32_FIND_DATA fdata;
    if (buf[strlen (buf)] != '\\') buf[strlen (buf)] = '\\';
    strcat (buf, "*.*");
    HANDLE hFind = FindFirstFile (buf, &fdata);
    bool res = hFind != INVALID_HANDLE_VALUE;
    while (res)
          {
          if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && *fdata.cFileName != '.')
             {
             char drv[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
             _splitpath (fdata.cFileName, drv, dir, fname, ext);
             SourceList->Items->Add (fname);
             DestList->Items->Add (fname);
             }
          res = FindNextFile (hFind, &fdata);
          }
    FindClose (hFind);          

    if (SourceList->Count <= 1 || DestList->Count <= 1)
       throw Exception ("Number languages must be greater then 1!");
    SourceList->Selected[0] = true;
    DestList->Selected[1] = true;
}
//---------------------------------------------------------------------------
void __fastcall TLangDialog::SourceListClick(TObject *Sender)
{
    if (SourceList->Items->Strings[SourceList->ItemIndex] == DestList->Items->Strings[DestList->ItemIndex])
       Button1->Enabled = false;
    else
       Button1->Enabled = true;
}
//---------------------------------------------------------------------------
