#include "stdafx.h"
#pragma hdrstop

#include "PropertiesSceneObject.h"
#include "SceneClassList.h"
#include "SceneObject.h"
#include "Scene.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma resource "*.dfm"

TfrmPropertiesSceneObject* TfrmPropertiesSceneObject::form = 0;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesSceneObject::TfrmPropertiesSceneObject(TComponent* Owner)
    : TForm(Owner)
{
	bLoadMode = false;
    m_NewReference="";
}
//---------------------------------------------------------------------------


void TfrmPropertiesSceneObject::GetObjectsInfo(){
	bLoadMode = true;

	VERIFY(!m_Objects->empty());
	ObjectIt _F = m_Objects->begin();
    VERIFY( (*_F)->ClassID==OBJCLASS_SCENEOBJECT );

    CSceneObject *_S = (CSceneObject *)(*_F);
	edName->Text= _S->Name;
    ebReference->Caption=_S->GetReference()->GetName();

    if (m_Objects->size()>1){
    	edName->Enabled 	= false;
        edName->Text		= "<Multiple selection>";
        ebReference->Caption= "<Multiple selection>";
    }else{
    	edName->Enabled 	= true;
    }
	bLoadMode = false;
}

bool TfrmPropertiesSceneObject::ApplyObjectsInfo(){
    VERIFY( !m_Objects->empty() );
	ObjectIt _F = m_Objects->begin();
    bool bMultiSel = (m_Objects->size()>1);
	for(;_F!=m_Objects->end();_F++){
    	CSceneObject *_O = (CSceneObject*)(*_F);
        if (!bMultiSel){
        	if (Scene.FindObjectByName(edName->Text.c_str(),_O)){
            	ELog.DlgMsg(mtError,"Name already exist in scene: '%s'",edName->Text.c_str());
            	return false;
            }
        	_O->Name = edName->Text.c_str();
        }
        // change reference
        if (!m_NewReference.IsEmpty())
        	_O->SetReference(m_NewReference.c_str());
	}
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSceneObject::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesSceneObject::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::OnModified(TObject *Sender)
{
	if (bLoadMode) return;
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSceneObject::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesSceneObject::Run(ObjectList* pObjects, bool& bChange)
{
	VERIFY(!TfrmPropertiesSceneObject::form);
	form = new TfrmPropertiesSceneObject(0);
    form->m_Objects = pObjects;
    VERIFY(form->m_Objects);
	form->GetObjectsInfo();
    int res = form->ShowModal();
    bChange = (res==mrOk);
    return res;
}

void __fastcall TfrmPropertiesSceneObject::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesSceneObject::ebReferenceClick(
      TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Change object reference?")==mrYes){
		LPCSTR N = TfrmChoseItem::SelectObject(false,0,0);
        if (N){
        	m_NewReference=N;
            ebReference->Caption=N;
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------

