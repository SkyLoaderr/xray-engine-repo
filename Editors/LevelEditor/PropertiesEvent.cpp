#include "stdafx.h"
#pragma hdrstop

#include "PropertiesEvent.h"
#include "Event.h"
#include "PropertiesList.h"
#include "EventOneAction.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmPropertiesEvent *frmPropertiesEvent=0;
//---------------------------------------------------------------------------
int frmPropertiesEventRun(ObjectList* pObjects, bool& bChange){
	VERIFY(pObjects);
    if (pObjects->size()!=1){
    	ELog.DlgMsg(mtError,"Multiple selection not supported.");
    	return mrCancel;
    }
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

	ObjectIt _F = m_Objects->begin();
	CEvent * E = (CEvent*)(*_F);

    CEvent::ActionVec& lst=E->m_Actions;
    for (CEvent::ActionIt it=lst.begin(); it!=lst.end(); it++){
		m_ActionForms.push_back(new TfrmOneEventAction(0));
		m_ActionForms.back()->Parent = sbActions;
	    m_ActionForms.back()->ShowIndex(this,it);
    }
}

bool TfrmPropertiesEvent::ApplyObjectsInfo(){
    VERIFY( !m_Objects->empty() );

    if (m_Objects->size()==1){
	    CEvent *E 			= (CEvent*)(*m_Objects->begin());
        CEvent::ActionVec& lst=E->m_Actions;
        lst.clear			();
		for (ActionFormIt it=m_ActionForms.begin(); it!=m_ActionForms.end(); it++){
        	lst.push_back	(CEvent::SAction());
			CEvent::SAction& A=lst.back();
        	(*it)->ApplyAction(&A);
        }
    }
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesEvent::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Shift.Contains(ssCtrl)&&(Key==VK_RETURN)) ebOk->Click();
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

void __fastcall TfrmPropertiesEvent::RemoveAction(TfrmOneEventAction* action)
{
	m_ActionForms.erase(find(m_ActionForms.begin(),m_ActionForms.end(),action));
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEvent::ebAppendActionClick(TObject *Sender)
{
	m_ActionForms.push_back(new TfrmOneEventAction(0));
	m_ActionForms.back()->Parent = sbActions;
    m_ActionForms.back()->ShowIndex(this);
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEvent::ebMultiClearClick(TObject *Sender)
{
	for (ActionFormIt it=m_ActionForms.begin(); it!=m_ActionForms.end(); it++)
    	_DELETE(*it);
    m_ActionForms.clear();
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEvent::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	for (ActionFormIt it=m_ActionForms.begin(); it!=m_ActionForms.end(); it++)
    	_DELETE(*it);
}
//---------------------------------------------------------------------------

