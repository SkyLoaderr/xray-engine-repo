#include "stdafx.h"
#pragma hdrstop

#include "TextForm.h"
//#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma link "mxPlacemnt"
#pragma link "ElACtrls"
#pragma link "ElStatBar"
#pragma link "ElXPThemedControl"
#pragma link "MxMenus"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfrmText::TfrmText(TComponent* Owner)
    : TForm(Owner)
{
	m_Text 			= 0;
    OnApplyClick 	= 0;
    OnCloseClick	= 0;
    OnCodeInsight	= 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmText::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    else if (Shift.Contains(ssCtrl)&&(Key==VK_RETURN)) ebOk->Click();
//.	else{
//.    	UI->ApplyGlobalShortCut(Key, Shift);
//.	}
}

//----------------------------------------------------
void __fastcall TfrmText::FormShow(TObject *Sender)
{
	ebApply->Visible 	= !!OnApplyClick;
    ebOk->Enabled       = false;
	// check window position
	CheckWindowPos		(this);
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
	*m_Text 				= mmText->Text;
    if (OnApplyClick) 		OnApplyClick();
	mmText->Modified 		= false;
}
//---------------------------------------------------------------------------

TfrmText* __fastcall TfrmText::CreateForm(AnsiString& txt, LPCSTR caption, u32 flags, int lim, TOnApplyClick on_apply, TOnCloseClick on_close, TOnCodeIndight on_insight)
{
	TfrmText* form			= xr_new<TfrmText>((TComponent*)0);
    form->Caption			= caption;
    form->m_Text			= &txt;
    form->mmText->ReadOnly	= flags&flReadOnly;
	form->mmText->Text 		= txt;
    form->mmText->MaxLength = lim;
    form->mmText->PopupMenu	= (flags&flOurPPMenu)?form->pmTextMenu:0;
    form->OnApplyClick 		= on_apply;
    form->OnCloseClick		= on_close;
    form->OnCodeInsight		= on_insight;

    form->Show();
    return form;
}
//---------------------------------------------------------------------------

bool __fastcall TfrmText::RunEditor(AnsiString& txt, LPCSTR caption, u32 flags, int lim, TOnApplyClick on_apply, TOnCloseClick on_close, TOnCodeIndight on_insight)
{
	TfrmText* form			= xr_new<TfrmText>((TComponent*)0);
	form 					= xr_new<TfrmText>((TComponent*)0);
    form->Caption			= caption;
    form->m_Text			= &txt;
    form->mmText->ReadOnly	= flags&flReadOnly;
	form->mmText->Text 		= txt;
    form->mmText->MaxLength = lim;
    form->mmText->PopupMenu	= (flags&flOurPPMenu)?form->pmTextMenu:0;
    form->OnApplyClick 		= on_apply;
    form->OnCloseClick		= on_close;
    form->OnCodeInsight		= on_insight;

	return (form->ShowModal()==mrOk);
}
//---------------------------------------------------------------------------

void TfrmText::SetText(AnsiString& text)
{
    m_Text					= &text;
	mmText->Text 			= text;
    mmText->Modified		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::DestroyForm(TfrmText* form)
{
	if (form) form->Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::mmTextChange(TObject *Sender)
{
    ebOk->Enabled 	= mmText->Modified;
}
//---------------------------------------------------------------------------


void __fastcall TfrmText::FormClose(TObject *Sender, TCloseAction &Action)
{
//	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebLoadClick(TObject *Sender)
{
	AnsiString fn;                   
	if (EFS.GetOpenName(_import_,fn,false,NULL,2)){
    	string4096 buf;
    	IReader* F 		= FS.r_open(fn.c_str());
        F->r_stringZ	(buf);
        mmText->Text	= buf;
        FS.r_close		(F);
	    ebOk->Enabled 	= true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebSaveClick(TObject *Sender)
{
	AnsiString fn;        
	if (EFS.GetSaveName(_import_,fn,NULL,2)){
    	CMemoryWriter F;
        F.w_stringZ	(mmText->Text.c_str());
        F.save_to	(fn.c_str());
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if (OnCloseClick) OnCloseClick(CanClose);
}
//---------------------------------------------------------------------------

void TfrmText::OutLineNumber()
{
    sbStatusPanel->Panels->Items[0]->Text = AnsiString().sprintf("%3d:%3d",mmText->CaretPos.y+1,mmText->CaretPos.x+1);
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::mmTextKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if ((Key==VK_SPACE)&&(Shift.Contains(ssCtrl))&&(Shift.Contains(ssShift))&&OnCodeInsight)
    {
    	AnsiString src_line, hint;
        src_line = mmText->Lines->Strings[mmText->CaretPos.y];
    	if (OnCodeInsight(src_line,hint)){
		    sbStatusPanel->Panels->Items[1]->Text = AnsiString("Hint: ")+hint;
            sbStatusPanel->Hint = hint;
        }else{
		    sbStatusPanel->Panels->Items[1]->Text = AnsiString("Error: ")+hint;
            sbStatusPanel->Hint = hint;
//        	ELog.DlgMsg(mtInformation,hint);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::ebClearClick(TObject *Sender)
{
	mmText->Clear();
}
//---------------------------------------------------------------------------

void TfrmText::InsertTextCP(const AnsiString& line, bool bCommas)
{
	AnsiString txt = (bCommas)?AnsiString("\"")+line+AnsiString("\""):line;
	if (!txt.IsEmpty()){
    	AnsiString h 	= mmText->Lines->Strings[mmText->CaretPos.y];
        h.Insert		(txt,mmText->CaretPos.x+1);
        mmText->Lines->Strings[mmText->CaretPos.y] = h;
    }
}
//---------------------------------------------------------------------------

void TfrmText::InsertLine(const AnsiString& line)
{
	if (!line.IsEmpty())
    	mmText->Lines->Insert(mmText->CaretPos.y,line);
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::tmIdleTimer(TObject *Sender)
{
    OutLineNumber();
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::FormActivate(TObject *Sender)
{
    tmIdle->Enabled		= true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmText::FormDeactivate(TObject *Sender)
{
    tmIdle->Enabled		= false;
}
//---------------------------------------------------------------------------

