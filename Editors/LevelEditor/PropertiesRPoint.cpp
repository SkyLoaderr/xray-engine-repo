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

void TfrmPropertiesSpawnPoint::GetObjectsInfo()
{
	VERIFY( !m_Objects->empty() );

	ObjectIt _F 		= m_Objects->begin();

    PropValueVec values;
	for(;_F!=m_Objects->end();_F++){
		CSpawnPoint *P 	= dynamic_cast<CSpawnPoint*>(*_F); R_ASSERT(P);
        P->FillProp		(GetClassNameByClassID(P->ClassID),values);
	}
	m_Props->AssignValues(values,true);
}
//---------------------------------------------------------------------------

void TfrmPropertiesSpawnPoint::CancelObjectsInfo()
{
	m_Props->ResetValues();
}
//---------------------------------------------------------------------------

void TfrmPropertiesSpawnPoint::ApplyObjectsInfo()
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (m_Props->IsFocused()) return;
	if (Key==VK_ESCAPE) ebCancel->Click();
	if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesSpawnPoint::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
    GetObjectsInfo		();
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::OnModified()
{
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::ebOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::ebCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

int __fastcall TfrmPropertiesSpawnPoint::Run(ObjectList* pObjects, bool& bChange)
{
	VERIFY(!TfrmPropertiesSpawnPoint::form);
    VERIFY(pObjects);
	form = new TfrmPropertiesSpawnPoint(0);
    form->m_Objects = pObjects;
    int res = form->ShowModal();
    bChange = (res==mrOk);
    return res;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    switch (ModalResult){
    case mrOk: 		ApplyObjectsInfo();		break;
    case mrCancel: 	CancelObjectsInfo();	break;
    default: THROW2("Invalid case");
    }

	// free resources
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

