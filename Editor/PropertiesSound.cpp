//---------------------------------------------------------------------------
#include "Pch.h"
#pragma hdrstop

#include "PropertiesSound.h"
#include "UI_Main.h"
#include "Sound.h"
#include "SceneClassList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXSpin"
#pragma link "multiobj"
#pragma resource "*.dfm"
TfrmPropertiesSound *frmPropertiesSound;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesSound::TfrmPropertiesSound(TComponent* Owner)
    : TForm(Owner)
{
}
//----------------------------------------------------
void __fastcall TfrmPropertiesSound::Run(ObjectList* pObjects)
{
    m_Objects = pObjects;
	GetObjectsInfo();
    frmPropertiesSound->ShowModal();
}
//---------------------------------------------------------------------------

void TfrmPropertiesSound::GetObjectsInfo(){
	_ASSERTE( !m_Objects->empty() );

	CSound *_L = 0;
	ObjectIt _F = m_Objects->begin();

	_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOUND );
	_L = (CSound *)(*_F);

	seRange->ObjFirsInit( _L->m_Range );
	_F++;

	for(;_F!=m_Objects->end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOUND );
		_L = (CSound *)(*_F);

		seRange->ObjNextInit( _L->m_Range );
	}
}

void TfrmPropertiesSound::ApplyObjectsInfo(){
	_ASSERTE( !m_Objects->empty() );

	CSound *_L = 0;
	ObjectIt _F = m_Objects->begin();

	for(;_F!=m_Objects->end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOUND );
		_L = (CSound *)(*_F);

		seRange->ObjApply( _L->m_Range );
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSound::btApplyClick(TObject *Sender)
{
    ApplyObjectsInfo();
    GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSound::btOkClick(TObject *Sender)
{
    ApplyObjectsInfo();
}

void __fastcall TfrmPropertiesSound::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) btCancel->Click();
}
//---------------------------------------------------------------------------

