//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("Unit1.cpp", Form1);
USEFORM("Unit2.cpp", ConfigDialog);
USEFORM("Unit3.cpp", LangDialog);
USEFORM("Unit4.cpp", DiffFilesDialog);
USEFORM("Unit5.cpp", DiffStrDialog);
USEFORM("Unit6.cpp", AddLangDialog);
USEFORM("Unit7.cpp", StatsDialog);
USEFORM("Unit8.cpp", RemovedStringsDialog);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->CreateForm(__classid(TForm1), &Form1);
         Application->CreateForm(__classid(TConfigDialog), &ConfigDialog);
         Application->CreateForm(__classid(TLangDialog), &LangDialog);
         Application->CreateForm(__classid(TDiffFilesDialog), &DiffFilesDialog);
         Application->CreateForm(__classid(TDiffStrDialog), &DiffStrDialog);
         Application->CreateForm(__classid(TAddLangDialog), &AddLangDialog);
         Application->CreateForm(__classid(TStatsDialog), &StatsDialog);
         Application->CreateForm(__classid(TRemovedStringsDialog), &RemovedStringsDialog);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
