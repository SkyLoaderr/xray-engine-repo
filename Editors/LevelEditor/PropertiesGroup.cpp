#include "stdafx.h"
#pragma hdrstop

#include "PropertiesGroup.h"
#include "GroupObject.h"
#include "Scene.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma resource "*.dfm"

TfrmPropertiesGroup* TfrmPropertiesGroup::form = 0;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesGroup::TfrmPropertiesGroup(TComponent* Owner)
    : TForm(Owner)
{
	bLoadMode = false;
}
//---------------------------------------------------------------------------


void TfrmPropertiesGroup::GetObjectsInfo(){
	bLoadMode = true;

	VERIFY(!m_Objects->empty());
	ObjectIt _F = m_Objects->begin();
    VERIFY( (*_F)->ClassID==OBJCLASS_GROUP );

    CGroupObject *_S = (CGroupObject*)(*_F);
	edName->Text= _S->Name;

    if (m_Objects->size()>1){
    	edName->Enabled 	= false;
        edName->Text		= "<Multiple selection>";
    }else{
    	edName->Enabled 	= true;
    }
	bLoadMode = false;
}

bool TfrmPropertiesGroup::ApplyObjectsInfo(){
    VERIFY( !m_Objects->empty() );
	ObjectIt _F = m_Objects->begin();
    bool bMultiSel = (m_Objects->size()>1);
	for(;_F!=m_Objects->end();_F++){
    	CGroupObject *_O = (CGroupObject*)(*_F);
        if (!bMultiSel){
        	if (Scene.FindObjectByName(edName->Text.c_str(),_O)){
            	ELog.DlgMsg(mtError,"Name already exist in scene: '%s'",edName->Text.c_str());
            	return false;
            }
        	_O->Name = edName->Text.c_str();
        }
	}
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesGroup::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesGroup::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGroup::OnModified(TObject *Sender)
{
	if (bLoadMode) return;
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGroup::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGroup::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesGroup::Run(ObjectList* pObjects, bool& bChange)
{
	VERIFY(!TfrmPropertiesGroup::form);
	form = new TfrmPropertiesGroup(0);
    form->m_Objects = pObjects;
    VERIFY(form->m_Objects);
	form->GetObjectsInfo();
    int res = form->ShowModal();
    bChange = (res==mrOk);
    return res;
}

void __fastcall TfrmPropertiesGroup::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------

