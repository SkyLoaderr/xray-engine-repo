#include "stdafx.h"
#pragma hdrstop

#include "PropertiesRPoint.h"
#include "SpawnPoint.h"
#include "Scene.h"
#include "ChoseForm.h"
#include "PropertiesList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"

TfrmPropertiesSpawnPoint* TfrmPropertiesSpawnPoint::form = 0;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesSpawnPoint::TfrmPropertiesSpawnPoint(TComponent* Owner)
    : TForm(Owner)
{
	m_Props = TProperties::CreateForm(paProps,alClient,OnModified);
	DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------

void TfrmPropertiesSpawnPoint::GetObjectInfo()
{
/*
//p
    m_Props->BeginFillMode();
    TElTreeItem* M=0;
//	M = m_Props->AddItem(0,PROP_MARKER2,	"Entity",	m_SPObject->m_SpawnData->s_name);
	m_Props->AddItem	(0,PROP_TEXT,		"Name",		m_Props->MakeTextValue(m_SPObject->FName,sizeof(m_SPObject->FName),OnNameAfterEdit));
    m_SPObject->PropWrite(m_SPData);
    CStream F(m_SPData.pointer(),m_SPData.size());
    m_Props->AddItems	(M,F);
    m_Props->EndFillMode();
*/
}
//---------------------------------------------------------------------------

bool TfrmPropertiesSpawnPoint::ApplyObjectInfo(){
    CStream F(m_SPData.pointer(),m_SPData.size());
    m_SPObject->PropRead(F);
    return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::OnNameAfterEdit(TElTreeItem* item, PropItem* sender, LPVOID edit_val)
{
	TextValue* V = (TextValue*)sender;
	AnsiString* new_name = (AnsiString*)edit_val;
	if (Scene.FindObjectByName(new_name->c_str(),0)) *new_name = V->GetValue();
    else *new_name = new_name->LowerCase();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
//	if (Key==VK_ESCAPE) ebCancel->Click();
//	if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesSpawnPoint::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
    GetObjectInfo		();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::OnModified()
{
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesSpawnPoint::Run(ObjectList* pObjects, bool& bChange)
{
	VERIFY(!TfrmPropertiesSpawnPoint::form);
    if (pObjects->size()>1){
    	ELog.DlgMsg(mtError,"Unsupport multiple selection.");
    	return mrCancel;
    }
	form = new TfrmPropertiesSpawnPoint(0);
    form->m_SPObject = (CSpawnPoint*)pObjects->front();
    VERIFY(form->m_SPObject);
    int res = form->ShowModal();
    bChange = (res==mrOk);
    return res;
}

void __fastcall TfrmPropertiesSpawnPoint::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::fsStorageRestorePlacement(
      TObject *Sender)
{
	m_Props->RestoreColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::fsStorageSavePlacement(
      TObject *Sender)
{
	m_Props->SaveColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::FormDestroy(TObject *Sender)
{
	TProperties::DestroyForm(m_Props);
}
//---------------------------------------------------------------------------

