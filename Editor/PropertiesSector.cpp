#include "stdafx.h"
#pragma hdrstop

#include "PropertiesSector.h"
#include "SceneClassList.h"
#include "Sector.h"
#include "ui_main.h"
#include "Scene.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmPropertiesSector *frmPropertiesSector=0;
//---------------------------------------------------------------------------
int frmPropertiesSectorRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesSector = new TfrmPropertiesSector(0);
    int res = frmPropertiesSector->Run(pObjects,bChange);
    _DELETE(frmPropertiesSector);
    return res;
}
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesSector::TfrmPropertiesSector(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void TfrmPropertiesSector::GetObjectsInfo(){
	VERIFY(!m_Objects->empty());

	ObjectIt _F = m_Objects->begin();	VERIFY( (*_F)->ClassID()==OBJCLASS_SECTOR );
	CSector *_O = (CSector*)(*_F);

	mcSectorColor->ObjFirstInit( _O->sector_color.get_windows() );
    edName->Text = _O->m_Name;

	_F++;
	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_SECTOR );
    	_O = (CSector *)(*_F);
		mcSectorColor->ObjNextInit( _O->sector_color.get_windows() );
	    edName->Text = "< Multiple selection >";
        edName->Enabled = false;
	}

    if (m_Objects->size()==1) gbSector->Caption = AnsiString("Sector: '")+AnsiString((*m_Objects->begin())->GetName())+AnsiString("'");
}

bool TfrmPropertiesSector::ApplyObjectsInfo(){
    VERIFY( !m_Objects->empty() );
    CSector *_O = 0;
    ObjectIt _F = m_Objects->begin();
    for(;_F!=m_Objects->end();_F++){
        VERIFY( (*_F)->ClassID()==OBJCLASS_SECTOR );
        _O = (CSector*)(*_F);
        int c;
        if (mcSectorColor->ObjApply(c)){  _O->sector_color.set_windows(c); }
    }
    if (m_Objects->size()==1){
	    CSector *_O = (CSector*)(m_Objects->front());
        if (!edName->Text.Length()){
            ELog.DlgMsg(mtError,"Enter Object Name!");
            return false;
        }else{
            if (Scene->FindObjectByName(edName->Text.c_str(),_O)){
		    	ELog.DlgMsg(mtError,"Object Name already found in scene! Enter new name.");
                return false;
            }
        }
    }
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSector::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesSector::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
    edName->Enabled		= true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSector::OnModified(TObject *Sender)
{
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSector::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSector::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSector::mcSectorColorMouseDown(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
	DWORD color = ((TMultiObjColor*)Sender)->Brush->Color;
	if (SelectColorWin(&color,false)){
    	((TMultiObjColor*)Sender)->_Set(color);
	    OnModified(0);
    }
}
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesSector::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
    VERIFY(m_Objects);
	GetObjectsInfo();
    bChange = (ShowModal()==mrOk);
    return mrOk;
}



