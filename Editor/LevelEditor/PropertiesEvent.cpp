#include "stdafx.h"
#pragma hdrstop

#include "PropertiesEvent.h"
#include "SceneClassList.h"
#include "Event.h"
#include "PropertiesList.h"
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
    m_Props = TfrmProperties::CreateProperties(paProps,alClient);
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesEvent::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	TfrmProperties::DestroyProperties(m_Props);
}
//---------------------------------------------------------------------------

void TfrmPropertiesEvent::GetObjectsInfo(){
	VERIFY(!m_Objects->empty());

	ObjectIt _F = m_Objects->begin();
	CEvent * E = (CEvent*)(*_F);

    m_Props->Enabled = true;
    m_Props->BeginFillMode();
    TElTreeItem* M=0;
    TElTreeItem* N=0;
    M = m_Props->AddItem(0,PROP_MARKER,	"Actions");
        m_Props->AddItem(M,PROP_FLAG,	"Enabled",	m_Props->MakeFlagValue(&F.m_Flags,CEditFlare::flFlare));
    for (CEditFlare::FlareIt it=F.m_Flares.begin(); it!=F.m_Flares.end(); it++){
        AnsiString nm; nm.sprintf("Flare %d",it-F.m_Flares.begin());
    N = m_Props->AddItem(M,PROP_MARKER,	nm.c_str());
        m_Props->AddItem(N,PROP_TEXTURE,"Texture",	&it->texture);
        m_Props->AddItem(N,PROP_FLOAT,	"Radius", 	m_Props->MakeFloatValue(&it->fRadius,0.f,10.f));
        m_Props->AddItem(N,PROP_FLOAT,	"Opacity", 	m_Props->MakeFloatValue(&it->fOpacity,0.f,1.f));
        m_Props->AddItem(N,PROP_FLOAT,	"Position",	m_Props->MakeFloatValue(&it->fPosition,-10.f,10.f));
    }
	m_Props->EndFillMode();
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

