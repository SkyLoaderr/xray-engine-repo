#include "stdafx.h"
#pragma hdrstop

#include "StatisticForm.h"
#include "main.h"
#include "FileSystem.h"
#include "UI_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfrmStatistic* TfrmStatistic::frmStatistic = 0;
//---------------------------------------------------------------------------
__fastcall TfrmStatistic::TfrmStatistic(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStatistic->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfrmStatistic::ShowStatistic(){
	if (!g_bEditorValid) return;
	if(!frmStatistic) frmStatistic = new TfrmStatistic(0);
    frmStatistic->Show();
    UI->RedrawScene();
    UI->Command(COMMAND_RENDER_FOCUS);
}
void __fastcall TfrmStatistic::HideStatistic(){
	if (frmStatistic)
		frmStatistic->Close();
}
void __fastcall TfrmStatistic::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
	frmStatistic = 0;
}
//---------------------------------------------------------------------------


