#include "stdafx.h"
#pragma hdrstop

#include "LogForm.h"
#include "main.h"
#include "FileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfrmLog *TfrmLog::form=0;
//---------------------------------------------------------------------------
__fastcall TfrmLog::TfrmLog(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLog::CreateLog(){
	VERIFY(!form);
	form = new TfrmLog(0);
}
void __fastcall TfrmLog::DestroyLog(){
	_DELETE(form);
}
void __fastcall TfrmLog::ShowLog(){
	VERIFY(form);
	form->Show();
}
void __fastcall TfrmLog::HideLog(){
	VERIFY(form);
	form->Hide();
}

void __fastcall TfrmLog::ebClearClick(TObject *Sender)
{
	lbLog->Items->Clear();		
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::AddMessage(TMsgDlgType mt, const AnsiString& msg){
	VERIFY(form);
	form->lbLog->Items->AddObject(msg,(TObject*)mt);
    form->lbLog->ItemIndex = form->lbLog->Items->Count-1;
    if ((mt==mtError)&&!form->Visible) form->Show();
}
//---------------------------------------------------------------------------

#define MSG_ERROR 	0x00C4C4FF
#define MSG_INFO  	0x00E6FFE7
#define MSG_DEF  	0x00E8E8E8

void __fastcall TfrmLog::lbLogDrawItem(TWinControl *Control, int Index,
      TRect &Rect, TOwnerDrawState State)
{
	TListBox* lb = ((TListBox *)Control);
	TCanvas *pCanvas = lb->Canvas;
    if (!State.Contains(odSelected)){
		pCanvas->Brush->Color 	= TColor(MSG_DEF);
	    TMsgDlgType mt 			= (TMsgDlgType)lb->Items->Objects[Index];
	    switch(mt){
	    case mtError: 			pCanvas->Brush->Color=TColor(MSG_ERROR);	break;
	    case mtInformation:
	    case mtConfirmation: 	pCanvas->Brush->Color=TColor(MSG_INFO);	break;
    	}
    }
	pCanvas->FillRect(Rect);
  	int     Offset = 2;   // default text offset width
	pCanvas->TextOut(Rect.Left + Offset, Rect.Top, lb->Items->Strings[Index]);
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (Key==VK_ESCAPE)	Close();
    else{
    	frmMain->ApplyGlobalShortCut(Key, Shift);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::ebClearSelectedClick(TObject *Sender)
{
	for (int i = 0; i < lbLog->Items->Count; i++){ 
    	if (lbLog->Selected[i]){
        	lbLog->Items->Delete(i);
            i--;
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmLog::ebCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------


