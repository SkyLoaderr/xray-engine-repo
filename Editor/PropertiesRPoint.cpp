#include "stdafx.h"
#pragma hdrstop

#include "PropertiesRPoint.h"
#include "SceneClassList.h"
#include "RPoint.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmPropertiesRPoint* TfrmPropertiesRPoint::form = 0;
bool TfrmPropertiesRPoint::bLoadMode=false;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesRPoint::TfrmPropertiesRPoint(TComponent* Owner)
    : TForm(Owner)
{
	bLoadMode = false;
}
//---------------------------------------------------------------------------


void TfrmPropertiesRPoint::GetObjectsInfo(){
	bLoadMode = true;

	VERIFY(!m_Objects->empty());
	ObjectIt _F = m_Objects->begin();
    VERIFY( (*_F)->ClassID()==OBJCLASS_RPOINT );

    CRPoint *_S = (CRPoint *)(*_F);
	CRPoint *_N = _S;
	edName->Text= _S->GetName();
    seTeamID->ObjFirstInit(_S->m_dwTeamID);
    seHeading->ObjFirstInit(rad2deg(_S->m_fHeading));
    
    _F++;
    for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_RPOINT );
    	_N = (CRPoint *)(*_F);
	    seTeamID->ObjNextInit(_N->m_dwTeamID);
	    seHeading->ObjNextInit(rad2deg(_S->m_fHeading));
	}
    if (_S!=_N){ 
    	edName->Enabled = false;
        edName->Text	= "<Multiple selection>";
    }else
    	edName->Enabled = true;
    
	bLoadMode = false;
}

bool TfrmPropertiesRPoint::ApplyObjectsInfo(){
    VERIFY( !m_Objects->empty() );
	ObjectIt _F = m_Objects->begin();
    bool bMultiSel = (m_Objects->size()>1);
	for(;_F!=m_Objects->end();_F++){
    	CRPoint *_O = (CRPoint *)(*_F);
        if (!bMultiSel) _O->SetName(edName->Text.c_str());
        seTeamID->ObjApplyInt(int(_O->m_dwTeamID));
        // orientation
        float a = rad2deg(_O->m_fHeading);
        seHeading->ObjApplyFloat(a);
        _O->m_fHeading = deg2rad(a);
	}
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesRPoint::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesRPoint::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesRPoint::OnModified(TObject *Sender)
{
	if (bLoadMode) return;
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesRPoint::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesRPoint::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesRPoint::Run(ObjectList* pObjects, bool& bChange)
{
	VERIFY(!TfrmPropertiesRPoint::form);
	form = new TfrmPropertiesRPoint(0);
    form->m_Objects = pObjects;
    VERIFY(form->m_Objects);
	form->GetObjectsInfo();
    int res = form->ShowModal();
    bChange = (res==mrOk);
    return res;
}

void __fastcall TfrmPropertiesRPoint::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    form = 0;	
}
//---------------------------------------------------------------------------


