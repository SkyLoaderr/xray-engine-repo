#include "Pch.h"
#pragma hdrstop

#include "PropertiesPClipper.h"
#include "UI_Main.h"
#include "PClipper.h"
#include "SceneClassList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXSpin"
#pragma link "multiobj"
#pragma resource "*.dfm"
TfrmPropertiesPClipper *frmPropertiesPClipper;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesPClipper::TfrmPropertiesPClipper(TComponent* Owner)
    : TForm(Owner)
{
}
//----------------------------------------------------
void __fastcall TfrmPropertiesPClipper::Run(ObjectList* pObjects)
{
    m_Objects = pObjects;
	GetObjectsInfo();
    frmPropertiesPClipper->ShowModal();
}
//---------------------------------------------------------------------------

void TfrmPropertiesPClipper::GetObjectsInfo(){
	_ASSERTE( !m_Objects->empty() );

	PClipper *_L = 0;
	ObjectIt _F = m_Objects->begin();

	_ASSERTE( (*_F)->ClassID()==OBJCLASS_PCLIPPER );
	_L = (PClipper *)(*_F);

	seRange->ObjFirsInit( _L->m_Density );
	_F++;

	for(;_F!=m_Objects->end();_F++){
		_ASSERTE( (*_F)->ClassID()==OBJCLASS_PCLIPPER );
		_L = (PClipper *)(*_F);
		seRange->ObjNextInit( _L->m_Density );
	}
}

void TfrmPropertiesPClipper::ApplyObjectsInfo(){
	_ASSERTE( !m_Objects->empty() );

	PClipper *_L = 0;
	ObjectIt _F = m_Objects->begin();

	for(;_F!=m_Objects->end();_F++){
		_ASSERTE( (*_F)->ClassID()==OBJCLASS_PCLIPPER );
		_L = (PClipper *)(*_F);
		seRange->ObjApply( _L->m_Density );
	}
    UI.UpdateScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPClipper::btApplyClick(TObject *Sender)
{
    ApplyObjectsInfo();
    GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPClipper::btOkClick(TObject *Sender)
{
    ApplyObjectsInfo();
}

void __fastcall TfrmPropertiesPClipper::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) btCancel->Click();
}
//---------------------------------------------------------------------------


