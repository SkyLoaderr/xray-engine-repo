#include "stdafx.h"
#pragma hdrstop

#include "TextForm.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma link "mxPlacemnt"
#pragma link "ElACtrls"
#pragma resource "*.dfm"

TfrmText* TfrmText::form = 0;
//---------------------------------------------------------------------------
__fastcall TfrmText::TfrmText(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmText::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Shift.Contains(ssCtrl)&&(Key==VK_RETURN)) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmText::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebOkClick(TObject *Sender)
{
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

bool __fastcall TfrmText::Run(AnsiString& txt, LPCSTR caption, bool bReadOnly, int lim)
{
	VERIFY(!TfrmText::form);
	form 		= new TfrmText(0);
    form->Caption=caption;
    form->mmText->ReadOnly=bReadOnly;
	form->mmText->Text = txt;
    form->mmText->MaxLength = lim;
    bool bRes	= (form->ShowModal()==mrOk);
    if (bRes){
    	txt		= form->mmText->Text;
    }
	_DELETE	(form);
    return bRes;
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::mmTextChange(TObject *Sender)
{
    ebOk->Enabled 	= mmText->Modified;
}
//---------------------------------------------------------------------------

