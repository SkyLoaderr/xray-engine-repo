#include "stdafx.h"
#pragma hdrstop

#include "PropertiesNavPoint.h"
#include "UI_Main.h"
#include "NavPoint.h"
#include "SceneClassList.h"
#include "AI_NavigationPoint.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXSpin"
#pragma link "multiobj"
#pragma link "RXCtrls"
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfrmPropertiesNavPoint *frmPropertiesNavPoint;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesNavPoint::TfrmPropertiesNavPoint(TComponent* Owner)
    : TForm(Owner)
{
}
//----------------------------------------------------
void __fastcall TfrmPropertiesNavPoint::Run(ObjectList* pObjects)
{
    m_Objects = pObjects;
	GetObjectsInfo();
    frmPropertiesNavPoint->ShowModal();
}
//---------------------------------------------------------------------------

void TfrmPropertiesNavPoint::GetObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	CNavPoint *_L = 0;
	ObjectIt _F = m_Objects->begin();

	VERIFY( (*_F)->ClassID()==OBJCLASS_NAVPOINT );
	_L = (CNavPoint *)(*_F);

    cbTypeStandart->ObjFirsInit( TCheckBoxState(_L->m_Type==NP_Standart) );
    cbTypeItem->ObjFirsInit( TCheckBoxState(_L->m_Type==NP_Item) );

	_F++;
	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_NAVPOINT );
		_L = (CNavPoint *)(*_F);
        cbTypeStandart->ObjNextInit( TCheckBoxState(_L->m_Type==NP_Standart) );
        cbTypeItem->ObjNextInit( TCheckBoxState(_L->m_Type==NP_Item) );
	}
}

void TfrmPropertiesNavPoint::ApplyObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	CNavPoint *_L = 0;
	ObjectIt _F = m_Objects->begin();

	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_NAVPOINT );
		_L = (CNavPoint *)(*_F);
        if(cbTypeStandart->Checked)     _L->m_Type = NP_Standart;
        else if(cbTypeItem->Checked)    _L->m_Type = NP_Item;
	}
    UI->UpdateScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesNavPoint::btApplyClick(TObject *Sender)
{
    ApplyObjectsInfo();
    GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesNavPoint::btOkClick(TObject *Sender)
{
    ApplyObjectsInfo();
}

void __fastcall TfrmPropertiesNavPoint::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) btCancel->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesNavPoint::cbTypeStandartClick(
      TObject *Sender)
{
    cbTypeItem->Checked = !cbTypeStandart->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesNavPoint::cbTypeItemClick(TObject *Sender)
{
    cbTypeStandart->Checked = !cbTypeItem->Checked;
}
//---------------------------------------------------------------------------

