#include "stdafx.h"
#pragma hdrstop

#include "PropertiesOccluder.h"
#include "UI_Main.h"
#include "Occluder.h"
#include "SceneClassList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXSpin"
#pragma link "multi_edit"
#pragma link "CloseBtn"
#pragma link "RXCtrls"
#pragma resource "*.dfm"
TfrmPropertiesOccluder *frmPropertiesOccluder=0;
//---------------------------------------------------------------------------
void frmPropertiesOccluderRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesOccluder = new TfrmPropertiesOccluder(0);
    frmPropertiesOccluder->Run(pObjects,bChange);
    _DELETE(frmPropertiesOccluder);
}
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesOccluder::TfrmPropertiesOccluder(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesOccluder::FormShow(TObject *Sender)
{
	sePointCount->MinValue = MIN_OCCLUDER_POINTS;
	sePointCount->MaxValue = MAX_OCCLUDER_POINTS;
}
//----------------------------------------------------
void __fastcall TfrmPropertiesOccluder::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
	GetObjectsInfo();
    bChange = (ShowModal()==mrOk);
}
//---------------------------------------------------------------------------

void TfrmPropertiesOccluder::GetObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	COccluder *_L = 0;
	ObjectIt _F = m_Objects->begin();

	VERIFY( (*_F)->ClassID()==OBJCLASS_OCCLUDER );
	_L = (COccluder*)(*_F);

	sePointCount->ObjFirstInit( _L->m_Points.size() );
	_F++;

	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_OCCLUDER );
		_L = (COccluder*)(*_F);
        sePointCount->ObjNextInit( _L->m_Points.size() );
	}
}

void TfrmPropertiesOccluder::ApplyObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	COccluder *_L = 0;
	ObjectIt _F = m_Objects->begin();

	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_OCCLUDER );
		_L = (COccluder*)(*_F);
        _L->ResizePoints(sePointCount->Value);
	}
    UI->UpdateScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesOccluder::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesOccluder::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesOccluder::ebOkClick(TObject *Sender)
{
    ApplyObjectsInfo();
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

