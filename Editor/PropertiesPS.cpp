#include "stdafx.h"
#pragma hdrstop

#include "PropertiesPS.h"
#include "SceneClassList.h"
#include "SceneObject.h"
#include "PSObject.h"
#include "ShaderFunction.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_check"
#pragma link "multi_edit"
#pragma link "RXCtrls"
#pragma link "FrameEmitter"
#pragma resource "*.dfm"

TfrmPropertiesPS* TfrmPropertiesPS::form = 0;
bool TfrmPropertiesPS::bLoadMode=false;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesPS::TfrmPropertiesPS(TComponent* Owner)
    : TForm(Owner)
{
	bLoadMode = false;
}
//---------------------------------------------------------------------------


void TfrmPropertiesPS::GetObjectsInfo(){
	bLoadMode = true;

	VERIFY(!m_Objects->empty());
	ObjectIt _F = m_Objects->begin();
    VERIFY( (*_F)->ClassID()==OBJCLASS_PS );

    CPSObject *_O = (CPSObject *)(*_F);
    fraEmitter->GetInfoFirst(_O->m_Emitter);
    _F++;
    for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_PS );
    	CPSObject *_O = (CPSObject *)(*_F);
	    fraEmitter->GetInfoFirst(_O->m_Emitter);
	}
	bLoadMode = false;
}

bool TfrmPropertiesPS::ApplyObjectsInfo(){
    VERIFY( !m_Objects->empty() );
	ObjectIt _F = m_Objects->begin();
	for(;_F!=m_Objects->end();_F++){
    	CPSObject *_O = (CPSObject *)(*_F);
	    fraEmitter->SetInfo(_O->m_Emitter);
	}
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPS::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesPS::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPS::OnModified(TObject *Sender)
{
	if (bLoadMode) return;
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPS::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPS::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesPS::Run(ObjectList* pObjects, bool& bChange)
{
	VERIFY(!form);
	form = new TfrmPropertiesPS(0);
    form->m_Objects = pObjects;
    VERIFY(form->m_Objects);
	form->GetObjectsInfo();
    int res = form->ShowModal();
    bChange = (res==mrOk);
    return res;
}

void __fastcall TfrmPropertiesPS::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    form = 0;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPS::ebBirthFuncMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Shift.Contains(ssShift)) fraEmitter->ebBirthFunc->Down = true;
    else{
		frmShaderFunctionRun(&m_BirthFunc);
    	fraEmitter->ebBirthFunc->Down = false;
    }
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPS::seEmitterKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN)	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPS::seEmitterLWChange(TObject *Sender,
      int Val)
{
	OnModified(Sender);
}
//---------------------------------------------------------------------------

