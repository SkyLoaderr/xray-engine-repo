#include "stdafx.h"
#pragma hdrstop

#include "PropertiesEvent.h"
#include "SceneClassList.h"
#include "Event.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "ElTree"
#pragma link "multi_check"
#pragma resource "*.dfm"
TfrmPropertiesEvent *frmPropertiesEvent=0;
//---------------------------------------------------------------------------
int frmPropertiesEventRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesEvent = new TfrmPropertiesEvent(0);
    int res = frmPropertiesEvent->Run(pObjects,bChange);
    _DELETE(frmPropertiesEvent);
    return res;
}

//---------------------------------------------------------------------------
__fastcall TfrmPropertiesEvent::TfrmPropertiesEvent(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void TfrmPropertiesEvent::GetObjectsInfo(){
	VERIFY(!m_Objects->empty());

	ObjectIt _F = m_Objects->begin();	VERIFY( (*_F)->ClassID()==OBJCLASS_EVENT );
	CEvent *_E = (CEvent*)(*_F);

    if (m_Objects->size()>1) gbEventParams->Hide();
    else{
    	gbEventParams->Show();
		cbTargetClass->ItemIndex = cbTargetClass->Items->IndexOf(_E->sTargetClass);
        edOnEnter->Text 	= _E->sOnEnter;
        edOnExit->Text 		= _E->sOnExit;
        edOnExit->Text 		= _E->sOnExit;
        cbExecuteOnce->Checked = _E->bExecuteOnce;
    }
/*
	_F++;
	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_SECTOR );
    	_O = (CSector *)(*_F);
	}
*/
}

bool TfrmPropertiesEvent::ApplyObjectsInfo(){
    VERIFY( !m_Objects->empty() );

    if (m_Objects->size()==1){
	    CEvent *_E 			= (CEvent*)(*m_Objects->begin());
        _E->sTargetClass    = cbTargetClass->Text;
        _E->sOnEnter	    = edOnEnter->Text;
        _E->sOnExit		    = edOnExit->Text;
        _E->bExecuteOnce	= cbExecuteOnce->Checked;
    }
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesEvent::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesEvent::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEvent::OnModified(TObject *Sender)
{
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEvent::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEvent::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesEvent::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
    VERIFY(m_Objects);
	GetObjectsInfo();
    bChange = (ShowModal()==mrOk);
    return mrOk;
}

void __fastcall TfrmPropertiesEvent::cbTargetClassChange(TObject *Sender)
{
	OnModified(Sender);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEvent::edOnEnterChange(TObject *Sender)
{
	OnModified(Sender);	
}
//---------------------------------------------------------------------------

