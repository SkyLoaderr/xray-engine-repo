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
	m_Text 			= 0;
    OnApplyClick 	= 0;
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
	ebApply->Visible 	= !!OnApplyClick;
    ebOk->Enabled       = false;
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebOkClick(TObject *Sender)
{
	ebApplyClick	(Sender);
    Close			();
    ModalResult 	= mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebApplyClick(TObject *Sender)
{
	*m_Text = form->mmText->Text;
    if (OnApplyClick) OnApplyClick();
}
//---------------------------------------------------------------------------

bool __fastcall TfrmText::Run(AnsiString& txt, LPCSTR caption, bool bReadOnly, int lim, bool bModal, TOnApplyClick on_apply)
{
	VERIFY(!TfrmText::form);
	form 					= xr_new<TfrmText>((TComponent*)0);
    form->Caption			= caption;
    form->m_Text			= &txt;
    form->mmText->ReadOnly	= bReadOnly;
	form->mmText->Text 		= txt;
    form->mmText->MaxLength = lim;
    form->OnApplyClick 		= on_apply;
    
    if (bModal){
	    return (form->ShowModal()==mrOk);
    }else{
	    form->Show();
        return true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::mmTextChange(TObject *Sender)
{
    ebOk->Enabled 	= mmText->Modified;
}
//---------------------------------------------------------------------------


void __fastcall TfrmText::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
    form = 0;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebLoadClick(TObject *Sender)
{
	AnsiString fn;
	if (Engine.FS.GetOpenName(Engine.FS.m_Import,fn,false,NULL,4)){
    	string4096 buf;
    	CStream* F = Engine.FS.Open(fn.c_str());
        F->RstringZ	(buf);
        mmText->Text= buf;
        Engine.FS.Close(F);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebSaveClick(TObject *Sender)
{
	AnsiString fn;
	if (Engine.FS.GetSaveName(Engine.FS.m_Import,fn,NULL,4)){
    	CFS_Memory F;
        F.WstringZ	(mmText->Text.c_str());
        F.SaveTo	(fn.c_str(),0);
    }
}
//---------------------------------------------------------------------------

