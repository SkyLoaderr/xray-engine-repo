//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma resource "*.dfm"
TForm1 *Form1;

using namespace std;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
#ifndef _DEBUG
    DumpButton->Visible = false;
#endif
    char buf[256];
    DWORD size;
    GetUserName (buf, &size);
    buf[size] = 0;
    m_UserName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
    m_pConfig = new vsConfig ();
    if (FAILED (m_pConfig->LoadFromFile ("translator.ini")) ||
        m_pConfig->IsSectionExist("languages") == false)
       {
       if (ConfigDialog->ShowModal() == mrCancel)
          {
          Application->Terminate();
          return;
          }
       m_pConfig->SetString ("languages", "base_path", ConfigDialog->Path1->Text.c_str ());
       m_pConfig->SetString ("languages", "import_path", ConfigDialog->ImportPath->Text.c_str ());
       m_pConfig->Save ("translator.ini");
       }

    LangDialog->InitializeData(m_pConfig);
    AddLangDialog->InitializeData(m_pConfig);

    if (LangDialog->ShowModal () == mrCancel)
       {
       Application->Terminate();
       return;
       }
    ChangeLanguage ();
}
//---------------------------------------------------------------------------
void            TForm1::ChangeLanguage  ()
{
    char path[512];
    m_pConfig->GetString ("languages", "base_path", path, "");
    if (!*path)
       throw Exception ("Invalid base path");
    size_t len = strlen (path);
    if (path[len - 1] != '\\') strcat (path, "\\");
    m_SourceLangDB = Database_Import (path, LangDialog->SourceList->Items->Strings[LangDialog->SourceList->ItemIndex].c_str ());
    m_DestLangDB = Database_Import (path, LangDialog->DestList->Items->Strings[LangDialog->DestList->ItemIndex].c_str ());
    m_pConfig->GetString ("languages", "import_path", path, "");
    if (!*path)
       throw Exception ("Invalid import path");
    len = strlen (path);
    if (path[len - 1] != '\\') strcat (path, "\\");
    m_SourceLangI = Database_Import (path, LangDialog->SourceList->Items->Strings[LangDialog->SourceList->ItemIndex].c_str ());

    RxLabel1->Caption = m_SourceLangDB->m_Name.c_str ();
    RxLabel2->Caption = m_DestLangDB->m_Name.c_str ();
    CheckDatabase (m_SourceLangDB, m_SourceLangI);
    RemoveStrings ();
    m_DestLangDB->AddDifferentFiles (m_SourceLangDB, NULL);
    m_DestLangDB->AddDifferentStrings(m_SourceLangDB, NULL);
    m_SourceLangDB->FillListBox (FilesList);

    m_SourceLangDB->Export ();
    FilesList->ItemIndex = 0;
    FilesList->Selected[0] = true;
    FilesListClick (this);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy (TObject *Sender)
{
    if (m_pConfig)
       {
       m_pConfig->Save ();
       delete m_pConfig;
       }
    //if (Application->MessageBoxA ("Save databases ?", "Warning", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
//       {
//       m_SourceLangDB->Save ();
//       m_DestLangDB->Save ();
//       }
    delete m_SourceLangDB;
    delete m_DestLangDB;
    delete m_SourceLangI;
}
//---------------------------------------------------------------------------
void            TForm1::CheckDatabase   (CDatabase *db, CDatabase *imp)
{
    vector<string> list;
    db->AddDifferentFiles (imp, &list);
    if (list.size ())
       {
       DiffFilesDialog->FilesList->Clear ();
       vector<string>::iterator s = list.begin (), e = list.end ();
       for (; s != e; ++s)
           DiffFilesDialog->FilesList->Items->Add ((*s).c_str());
       DiffFilesDialog->ShowModal ();
       }
    xml_diff_strings *diff_str = new xml_diff_strings;
    db->AddDifferentStrings (imp, diff_str);
    if (diff_str->size ())
       {
       DiffStrDialog->GotoButton->Enabled = false;
       DiffStrDialog->InitializeList (diff_str);
       DiffStrDialog->ShowModal ();
       DiffStrDialog->GotoButton->Enabled = true;
       }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FilesListClick(TObject *Sender)
{
    string name = FilesList->Items->Strings[FilesList->ItemIndex].c_str ();

    if (m_DestLangDB->IsFileLocked (name))
       {
       if (m_DestLangDB->IsOurOwnerFile (name) == false)
          {
          DestText->ReadOnly  = true;
          DestText->Color = clBtnFace;
          LockMenu->Items->Items[0]->Enabled = false;
          LockMenu->Items->Items[1]->Enabled = false;
          }
       else
          {
          DestText->ReadOnly  = false;
          DestText->Color = clWhite;
          LockMenu->Items->Items[0]->Enabled = false;
          LockMenu->Items->Items[1]->Enabled = true;
          }
       }
    else
       {
       DestText->ReadOnly  = true;
       DestText->Color = clBtnFace;
       LockMenu->Items->Items[0]->Enabled = true;
       LockMenu->Items->Items[1]->Enabled = false;
       }


    if (DiffButton->Down == false)
       {
       if (NotransButton->Down == false)
          {
          m_SourceLangDB->FillStrings (name, StringsList);
          if (!StringsList->Items->Count) return;
          }
       else
          {
          StringsList->Clear ();
          vector<string> list;
          m_DestLangDB->GetNoTranslatedStrings(name, &list);
          if (!list.size()) return;
          vector<string>::iterator s = list.begin (), e = list.end ();
          for (; s != e; ++s)
              StringsList->Items->Add ((*s).c_str());
          }
       }
    else
       {
       StringsList->Clear ();
       vector<string> list;
       m_SourceLangDB->GetDifferentStrings (m_SourceLangI, name, &list);
       if (!list.size()) return;
       vector<string>::iterator s = list.begin (), e = list.end ();
       for (; s != e; ++s)
           StringsList->Items->Add ((*s).c_str());
       }

    StringsList->ItemIndex = 0;
    StringsList->Selected[0] = true;
    StringsListClick (Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::StringsListClick(TObject *Sender)
{
    m_bChange = true;
    SrcText->Lines->Clear ();
    DestText->Lines->Clear ();

    string str = m_SourceLangI->GetString (FilesList->Items->Strings[FilesList->ItemIndex].c_str (),
                                           StringsList->Items->Strings[StringsList->ItemIndex].c_str ());

    char *ptr = (char*)str.c_str (), *next = strstr (ptr, "\\n");
    while (next)
          {
          *next = 0;
          SrcText->Lines->Add (ptr);
          *next = '\\';
          ptr = next + 2;
          next = strstr (ptr, "\\n");
          }
    SrcText->Lines->Add (ptr);
    str = m_DestLangDB->GetString (FilesList->Items->Strings[FilesList->ItemIndex].c_str (),
                                   StringsList->Items->Strings[StringsList->ItemIndex].c_str ());
    ptr = (char*)str.c_str (), next = strstr (ptr, "\\n");
    while (next)
          {
          *next = 0;
          DestText->Lines->Add (ptr);
          *next = '\\';
          ptr = next + 2;
          next = strstr (ptr, "\\n");
          }
    DestText->Lines->Add (ptr);
    m_bChange = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ToolButton5Click(TObject *Sender)
{
    if (DiffStrDialog->ShowModal() == mrCancel) return;
    int index = FilesList->Items->IndexOf (DiffStrDialog->m_File.c_str ());
    if (index == -1)
       {
       Application->MessageBox ("File not found !!!", "Error", MB_OK | MB_ICONERROR);
       return;
       }
    FilesList->ItemIndex = index;
    FilesList->Selected[index] = true;
    FilesListClick (Sender);
    //m_SourceLangDB->FillStrings (FilesList->Items->Strings[FilesList->ItemIndex].c_str (), StringsList);
    index = StringsList->Items->IndexOf (DiffStrDialog->m_String.c_str ());
    if (index == -1) return;
    StringsList->ItemIndex = index;
    StringsList->Selected[index] = true;
    StringsListClick (this);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DestTextExit(TObject *Sender)
{
    if (m_bChange == true) return;
    if (DestText->Modified == false || StringsList->ItemIndex == -1 || FilesList->ItemIndex == -1) return;
    string text = "";
    for (int a = 0; a < DestText->Lines->Count; a++)
        {
        text += DestText->Lines->Strings[a].c_str ();
        if (a < (DestText->Lines->Count - 1))
           text += "\\n";
        }
    if (text.empty() == true)
       {
       text = "This string was added automatically";
       DestText->Lines->Strings[0] = text.c_str ();
       }
    m_DestLangDB->UpdateString (FilesList->Items->Strings[FilesList->ItemIndex].c_str (),
                                StringsList->Items->Strings[StringsList->ItemIndex].c_str (),
                                text);

    text = "";
    for (int a = 0; a < SrcText->Lines->Count; a++)
        {
        text += SrcText->Lines->Strings[a].c_str ();
        if (a < (SrcText->Lines->Count - 1))
           text += "\\n";
        }
    m_SourceLangDB->UpdateString (FilesList->Items->Strings[FilesList->ItemIndex].c_str (),
                                  StringsList->Items->Strings[StringsList->ItemIndex].c_str (),
                                  text);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DiffButtonClick(TObject *Sender)
{
    StringsList->Clear ();
    FilesList->Clear ();
    if (DiffButton->Down == true)
       {
       vector<string> diff_list;
       m_SourceLangDB->GetDifferentFiles (m_SourceLangI, FilesList);
       if (!FilesList->Count) return;
       FilesList->ItemIndex = 0;
       FilesList->Selected[0] = true;
       FilesListClick (Sender);
       }
    else
       {
       m_SourceLangDB->FillListBox (FilesList);
       FilesList->ItemIndex = 0;
       FilesList->Selected[0] = true;
       FilesListClick (this);
       }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ToolButton3Click(TObject *Sender)
{
    if (Application->MessageBoxA ("Are you sure to export the destination language database ?", "Warning", MB_YESNO + MB_ICONEXCLAMATION) == IDNO)
       return;
    m_DestLangDB->Export();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ToolButton6Click(TObject *Sender)
{
    if (LangDialog->ShowModal () == mrCancel)
       return;

    if (Application->MessageBoxA ("Save databases ?", "Warning", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
       {
       m_SourceLangDB->Save ();
       m_DestLangDB->Save ();
       }
    delete m_SourceLangDB;
    delete m_DestLangDB;
    delete m_SourceLangI;
    m_SourceLangDB = NULL;
    m_DestLangDB = NULL;
    m_SourceLangI = NULL;
    ChangeLanguage ();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ToolButton8Click(TObject *Sender)
{
    if (AddLangDialog->ShowModal() == mrCancel)
       return;
    LangDialog->SourceList->Items->Add (AddLangDialog->LangShortcut->Text);
    LangDialog->DestList->Items->Add (AddLangDialog->LangShortcut->Text);
    ToolButton6Click (Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ToolButton10Click(TObject *Sender)
{
    DB_STAT src, dest;
    m_SourceLangDB->GetStatistic (&src);
    m_DestLangDB->GetStatistic (&dest);
    StatsDialog->SrcFiles->Caption = src.m_iFiles;
    StatsDialog->SrcStrings->Caption = src.m_iStrings;
    StatsDialog->SrcWords->Caption = src.m_iWords;
    StatsDialog->DestFiles->Caption = dest.m_iFiles;
    StatsDialog->DestStrings->Caption = dest.m_iStrings;
    StatsDialog->DestWords->Caption = dest.m_iWords;
    StatsDialog->ShowModal ();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckButtonClick(TObject *Sender)
{
    bool need_dialog = false;
    RemovedStringsDialog->Clear ();
    vector<string> file_list, string_list;
    m_DestLangDB->GetFilesList(&file_list);
    for (size_t a = 0; a < file_list.size (); a++)
        {
        string str = file_list[a];
        m_DestLangDB->GetStringList (str, &string_list);
        if (string_list.size ())
           {
           vector<string>::iterator s = string_list.begin(), e = string_list.end ();
           for (; s != e; ++s)
               {
               if (m_SourceLangDB->IsStringExist (str, *s) == false)
                  {
                  string rem = *s;
                  m_DestLangDB->RemoveString (str, *s);
                  need_dialog = true;
                  RemovedStringsDialog->Add (str, rem);
                  }
               }
           }
        }
    if (need_dialog == true)
       RemovedStringsDialog->ShowModal ();
}
//---------------------------------------------------------------------------
void            TForm1::RemoveStrings   ()
{
    bool need_dialog = false;
    RemovedStringsDialog->Clear ();
    vector<string> file_list, string_list;
    m_SourceLangDB->GetFilesList(&file_list);
    for (size_t a = 0; a < file_list.size (); a++)
        {
        string str = file_list[a];
        m_SourceLangDB->GetStringList (str, &string_list);
        if (string_list.size ())
           {
           vector<string>::iterator s = string_list.begin(), e = string_list.end ();
           for (; s != e; ++s)
               if (m_SourceLangI->IsStringExist (str, *s) == false)
                  {
                  string rem = *s;
                  m_SourceLangDB->RemoveString (str, *s);
                  need_dialog = true;
                  RemovedStringsDialog->Add (str, rem);
                  }
           }
        }
    if (need_dialog == true)
       RemovedStringsDialog->ShowModal ();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NotransButtonClick(TObject *Sender)
{
    DiffButton->Down = false;
    StringsList->Clear ();
    FilesList->Clear ();
    if (NotransButton->Down == true)
       {
       vector<string> diff_list;
       m_DestLangDB->GetNoTranslatedFiles (FilesList);
       if (!FilesList->Count) return;
       FilesList->ItemIndex = 0;
       FilesList->Selected[0] = true;
       FilesListClick (Sender);
       }
    else
       {
       m_SourceLangDB->FillListBox (FilesList);
       FilesList->ItemIndex = 0;
       FilesList->Selected[0] = true;
       FilesListClick (this);
       }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DumpButtonClick(TObject *Sender)
{
    m_SourceLangDB->Dump ();
    m_DestLangDB->Dump ();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Shift.Contains (ssAlt) == false) return;
    if (Key == VK_UP)
       {
       if (StringsList->Items->Count && StringsList->ItemIndex != 0)
          {
          StringsList->ItemIndex = StringsList->ItemIndex - 1;
          StringsList->Selected[StringsList->ItemIndex] = true;
          StringsListClick (StringsList);
          }
       return;
       }
    if (Key == VK_DOWN)
       {
       if (StringsList->Items->Count && StringsList->ItemIndex < (StringsList->Items->Count - 1))
          {
          StringsList->ItemIndex = StringsList->ItemIndex + 1;
          StringsList->Selected[StringsList->ItemIndex] = true;
          StringsListClick (StringsList);
          }
       return;
       }
    if (Key == VK_PRIOR)
       {
       if (FilesList->Items->Count && FilesList->ItemIndex != 0)
          {
          FilesList->ItemIndex = FilesList->ItemIndex - 1;
          FilesList->Selected[FilesList->ItemIndex] = true;
          FilesListClick (FilesList);
          }
       return;
       }

    if (Key == VK_NEXT)
       {
       if (FilesList->Items->Count && FilesList->ItemIndex < (FilesList->Items->Count - 1))
          {
          FilesList->ItemIndex = FilesList->ItemIndex + 1;
          FilesList->Selected[FilesList->ItemIndex] = true;
          FilesListClick (FilesList);
          }
       }

}
//---------------------------------------------------------------------------
void __fastcall TForm1::SaveAllButtonClick(TObject *Sender)
{
    if (Application->MessageBoxA ("Do you wish to store databases ?", "Save", MB_YESNO + MB_ICONEXCLAMATION) == IDYES)
       {
       m_SourceLangDB->Save ();
       m_DestLangDB->Save ();
       }
    if (Application->MessageBoxA ("Do you wish to export destination language strings ?", "Export", MB_YESNO + MB_ICONEXCLAMATION) == IDYES)
       m_DestLangDB->Export();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DestTextEnter(TObject *Sender)
{
    m_bChange = true;
    if (DestText->Lines->Strings[0] == "This string was added automatically")
       DestText->Lines->Strings[0] = "";
    m_bChange = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DestTextChange(TObject *Sender)
{
    if (m_bChange == true) return;
    if (DestText->Modified == false || StringsList->ItemIndex == -1 || FilesList->ItemIndex == -1) return;
    string text = "";
    for (int a = 0; a < DestText->Lines->Count; a++)
        {
        text += DestText->Lines->Strings[a].c_str ();
        if (a < (DestText->Lines->Count - 1))
           text += "\\n";
        }
    m_DestLangDB->UpdateString (FilesList->Items->Strings[FilesList->ItemIndex].c_str (),
                                StringsList->Items->Strings[StringsList->ItemIndex].c_str (),
                                text);

    text = "";
    for (int a = 0; a < SrcText->Lines->Count; a++)
        {
        text += SrcText->Lines->Strings[a].c_str ();
        if (a < (SrcText->Lines->Count - 1))
           text += "\\n";
        }
    m_SourceLangDB->UpdateString (FilesList->Items->Strings[FilesList->ItemIndex].c_str (),
                                  StringsList->Items->Strings[StringsList->ItemIndex].c_str (),
                                  text);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FilesListDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
{
    TCanvas *pCanvas = ((TListBox*)Control)->Canvas;
    string name = ((TListBox *)Control)->Items->Strings[Index].c_str ();
    TColor color = clWhite;
    if (State.Contains (odSelected) == true)
       {
       pCanvas->FillRect(Rect);
       pCanvas->TextOut(Rect.Left + 2, Rect.Top, name.c_str ());
       return;
       }
    if (m_DestLangDB)
       {
       bool locked = m_DestLangDB->IsFileLocked (name);
       if (locked == true)
          {
          if (m_DestLangDB->IsOurOwnerFile (name) == true)
             color = clGreen;
          else
             color = clRed;
          }
       pCanvas->Brush->Color = color;
       pCanvas->FillRect(Rect);
       }
    //pCanvas->Font->Color = color;
    pCanvas->TextOut(Rect.Left + 2, Rect.Top, name.c_str ());
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LockFileMenuItemClick(TObject *Sender)
{
    string name = FilesList->Items->Strings[FilesList->ItemIndex].c_str ();
    if (m_DestLangDB->IsFileLocked (name) == false)
       {
       DestText->ReadOnly  = false;
       DestText->Color = clWhite;
       LockMenu->Items->Items[0]->Enabled = false;
       LockMenu->Items->Items[1]->Enabled = true;
       m_DestLangDB->LockFile (name);
       }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::UnlockFileMenuItemClick(TObject *Sender)
{
    string name = FilesList->Items->Strings[FilesList->ItemIndex].c_str ();
    if (m_DestLangDB->IsFileLocked (name) == true && m_DestLangDB->IsOurOwnerFile (name))
       {
       m_DestLangDB->UnlockFile (name);
       if (Application->MessageBox ("Do you with to save file ?", "Warning", MB_YESNO | MB_ICONSTOP) == IDYES)
          {
          m_DestLangDB->ExportFile (name);
          m_SourceLangDB->ExportFile (name);
          }
       DestText->ReadOnly  = true;
       DestText->Color = clBtnFace;
       LockMenu->Items->Items[0]->Enabled = true;
       LockMenu->Items->Items[1]->Enabled = false;
       }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerTimer(TObject *Sender)
{
    if (!m_DestLangDB) return;
    if (m_DestLangDB->IsLockStatusChanged () == true)
       FilesList->Invalidate ();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (m_DestLangDB && m_DestLangDB->IsLockedOurFilesPres () == true)
       if (Application->MessageBoxA ("Some files are locked. Unlock it and exit?", "Warning", MB_YESNO | MB_ICONSTOP) == IDYES)
          {
          m_DestLangDB->UnlockAllFiles ();
          }
       else
          Action = caNone;     
}
//---------------------------------------------------------------------------

