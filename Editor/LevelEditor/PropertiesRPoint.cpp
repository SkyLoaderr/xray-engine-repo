#include "stdafx.h"
#pragma hdrstop

#include "PropertiesRPoint.h"
#include "SceneClassList.h"
#include "RPoint.h"
#include "Scene.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma resource "*.dfm"

TfrmPropertiesRPoint* TfrmPropertiesRPoint::form = 0;
bool TfrmPropertiesRPoint::bLoadMode=false;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesRPoint::TfrmPropertiesRPoint(TComponent* Owner)
    : TForm(Owner)
{
	bLoadMode = false;
    bMultiSel = false;
    ebEntityRefs->Caption = NONE_CAPTION;
    lbType->Font->Style.Clear();
    bTypeChanged	= false;
    bEntityChanged	= false;

    ebTypePlayer->Tag = CRPoint::etPlayer;
    ebTypeEntity->Tag = CRPoint::etEntity;
}
//---------------------------------------------------------------------------


void TfrmPropertiesRPoint::GetObjectsInfo(){
	bLoadMode = true;

	VERIFY(!m_Objects->empty());
	ObjectIt _F = m_Objects->begin();
    VERIFY( (*_F)->ClassID==OBJCLASS_RPOINT );

	CRPoint::EType T;
    {
        CRPoint *_S = (CRPoint *)(*_F);
        CRPoint *_N = _S;
        edName->Text= _S->Name;
        seTeamID->ObjFirstInit(_S->m_dwTeamID);
        seSquadID->ObjFirstInit(_S->m_dwSquadID);
        seGroupID->ObjFirstInit(_S->m_dwGroupID);
        seHeading->ObjFirstInit(rad2deg(_S->PRotate.y));
        cbActive->ObjFirstInit((TCheckBoxState)_S->m_Flags.bActive);
        SetType(_N->m_Type);
        T = _N->m_Type;
		ebEntityRefs->Caption = _N->m_EntityRefs[0]?_N->m_EntityRefs:NONE_CAPTION;
	}

    _F++;
    for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID==OBJCLASS_RPOINT );
    	CRPoint *_N = (CRPoint *)(*_F);
	    seTeamID->ObjNextInit(_N->m_dwTeamID);
	    seSquadID->ObjNextInit(_N->m_dwSquadID);
    	seGroupID->ObjNextInit(_N->m_dwGroupID);
	    seHeading->ObjNextInit(rad2deg(_N->PRotate.y));
	    cbActive->ObjNextInit((TCheckBoxState)_N->m_Flags.bActive);
        if (T!=_N->m_Type) lbType->Font->Style = TFontStyles()<<fsItalic;
		if (ebEntityRefs->Caption!=_N->m_EntityRefs) ebEntityRefs->Caption=MULTIPLESEL_CAPTION;
	    bMultiSel = true;
	}
    if (bMultiSel){
    	edName->Enabled = false;
        edName->Text	= MULTIPLESEL_CAPTION;
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
        if (!bMultiSel){
        	if (Scene.FindObjectByName(edName->Text.c_str(),_O)){
            	ELog.DlgMsg(mtError,"Duplicate object name already exists: '%s'",edName->Text.c_str());
            	return false;
            }
	        _O->Name = edName->Text.c_str();
        }
        seTeamID->ObjApplyInt(int(_O->m_dwTeamID));
        seSquadID->ObjApplyInt(int(_O->m_dwSquadID));
        seGroupID->ObjApplyInt(int(_O->m_dwGroupID));
        int f=_O->m_Flags.bActive; cbActive->ObjApply(f); _O->m_Flags.bActive=f;
        // orientation
        float a = rad2deg(_O->PRotate.y);
        seHeading->ObjApplyFloat(a);
        _O->PRotate.y = deg2rad(a);
        // type
        if (bTypeChanged){
	        if (ebTypePlayer->Down) 		_O->m_Type=CRPoint::etPlayer;
        	else if (ebTypeEntity->Down) 	_O->m_Type=CRPoint::etEntity;
        }
        // entity
        if (bEntityChanged)	strcpy(_O->m_EntityRefs,ebEntityRefs->Caption.c_str());
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

void __fastcall TfrmPropertiesRPoint::ebEntityRefsClick(TObject *Sender)
{
	LPCSTR N = TfrmChoseItem::SelectEntity((!bMultiSel&&(ebEntityRefs->Caption!=NONE_CAPTION))?ebEntityRefs->Caption.c_str():0);
    if (N){
    	ebEntityRefs->Caption = N;
		bEntityChanged = true;
        OnModified(Sender);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesRPoint::ebTypeClick(TObject *Sender)
{
	bTypeChanged = true;
    SetType((CRPoint::EType)((TExtBtn*)Sender)->Tag);
	OnModified(Sender);

}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesRPoint::SetType(CRPoint::EType type)
{
    if (type==CRPoint::etPlayer)    	ebTypePlayer->Down=true;
    else if (type==CRPoint::etEntity) 	ebTypeEntity->Down=true;
	paEntity->Visible 					= (type==CRPoint::etEntity);
}

