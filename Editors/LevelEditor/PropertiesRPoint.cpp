#include "stdafx.h"
#pragma hdrstop

#include "PropertiesRPoint.h"
#include "SceneClassList.h"
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
bool TfrmPropertiesSpawnPoint::bLoadMode=false;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesSpawnPoint::TfrmPropertiesSpawnPoint(TComponent* Owner)
    : TForm(Owner)
{
	m_Props = TfrmProperties::CreateProperties(paProps,alClient,OnModified);
	bLoadMode = false;
    bMultiSel = false;
	DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------


void TfrmPropertiesSpawnPoint::GetObjectsInfo(){
	bLoadMode = true;

	VERIFY(!m_Objects->empty());
	ObjectIt _F = m_Objects->begin();
    VERIFY( (*_F)->ClassID==OBJCLASS_SPAWNPOINT );

    {
        CSpawnPoint* _S 		= (CSpawnPoint*)(*_F);
        CSpawnPoint* _N 		= _S;
        UpdateProps(_S);
	}

    _F++;
    for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID==OBJCLASS_SPAWNPOINT );
    	CSpawnPoint *_N = (CSpawnPoint*)(*_F);
	    bMultiSel = true;
	}
	bLoadMode = false;
}

bool TfrmPropertiesSpawnPoint::ApplyObjectsInfo(){
    return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::OnNameAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	TextValue* V = (TextValue*)sender;
	AnsiString* new_name = (AnsiString*)edit_val;
	if (Scene.FindObjectByName(new_name->c_str(),0)) *new_name = V->val;
    else *new_name = new_name->LowerCase();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::UpdateProps(CSpawnPoint* S)
{
    m_Props->BeginFillMode();
    TElTreeItem* M=0;
//    TElTreeItem* N=0;
//    M = m_Props->AddItem(0,PROP_MARKER,	"Source");
//	m_Props->AddItem(M,PROP_FLAG,	"Enabled",	m_Props->MakeFlagValue(&F.m_Flags,CEditFlare::flSource));
	M = m_Props->AddItem(0,PROP_TEXT,	"Name",	m_Props->MakeTextValue(S->FName,sizeof(S->FName),OnNameAfterEdit));
    M->Enabled = false;
    m_Props->EndFillMode();
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
    GetObjectsInfo		();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::OnMModified(TObject* Sender)
{
	if (bLoadMode) return;
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::OnModified()
{
	if (bLoadMode) return;
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSpawnPoint::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
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
	form = new TfrmPropertiesSpawnPoint(0);
    form->m_Objects = pObjects;
    VERIFY(form->m_Objects);
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
	TfrmProperties::DestroyProperties(m_Props);
}
//---------------------------------------------------------------------------

