#include "stdafx.h"
#pragma hdrstop

#include "EventOneAction.h"
#include "PropertiesEvent.h"
#include "ChoseForm.h"
#include "TextForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "multi_edit"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfrmOneEventAction::TfrmOneEventAction(TComponent* Owner)
    : TForm(Owner)
{
	ebCLSID->Caption=NONE_CAPTION;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEventAction::ApplyAction(CEvent::SAction* action)
{
    if (action){
    	if (ebTypeEnter->Down)		action->type = CEvent::etEnter;
        else if (ebTypeLeave->Down)	action->type = CEvent::etLeave;
        action->clsid = TEXT2CLSID(ebCLSID->Caption.c_str());
        action->count = seCount->Value;
        action->event = edEvent->Text;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEventAction::ShowIndex(TfrmPropertiesEvent* parent,CEvent::SAction* action){
	m_Parent = parent;
    // fill action
    if (action){
	    bLoadMode = true;
        switch(action->type){
        case CEvent::etEnter: ebTypeEnter->Down=true; break;
        case CEvent::etLeave: ebTypeLeave->Down=true; break;
        }
        string16 buf; CLSID2TEXT(action->clsid,buf);
        ebCLSID->Caption= buf;
        seCount->Value 	= (signed short)action->count;  
        edEvent->Text 	= action->event;
	    bLoadMode = false;
    }
    Visible=true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEventAction::HideIndex(){
    Visible=false;
}
//---------------------------------------------------------------------------


void __fastcall TfrmOneEventAction::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    m_Parent->RemoveAction(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEventAction::ebMultiRemoveClick(TObject *Sender)
{
	m_Parent->OnModified(Sender);
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEventAction::ebCLSIDClick(TObject *Sender)
{
	THROW2("SelectCLSID");
	/*
	if (TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,N,1)){
    	ebCLSID->Caption = N;
        m_Parent->OnModified(Sender);
    }
    */
}          
//---------------------------------------------------------------------------

void __fastcall TfrmOneEventAction::OnModified(TObject *Sender)
{
	if (bLoadMode) return;
	m_Parent->OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEventAction::ebExpandTextClick(TObject *Sender)
{
	AnsiString txt=edEvent->Text;
	if (TfrmText::Run(txt, "Event Text")){
		edEvent->Text = txt;
    }
}
//---------------------------------------------------------------------------

