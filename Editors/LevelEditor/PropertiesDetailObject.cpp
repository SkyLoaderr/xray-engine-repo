#include "stdafx.h"
#pragma hdrstop

#include "PropertiesDetailObject.h"
#include "Sector.h"
#include "DetailFormat.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_check"
#pragma resource "*.dfm"
TfrmPropertiesDO *frmPropertiesDO=0;
//---------------------------------------------------------------------------
int frmPropertiesSectorRun(DDVec& SrcData, bool& bChange){
	frmPropertiesDO = new TfrmPropertiesDO(0);
    int res = frmPropertiesDO->Run(SrcData,bChange);
    _DELETE(frmPropertiesDO);
    return res;
}
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesDO::TfrmPropertiesDO(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void TfrmPropertiesDO::GetObjectsInfo(){
	VERIFY(!m_Data->empty());

    DDIt _I = m_Data->begin();

	seScaleMinY->ObjFirstInit	( (*_I)->m_fMinScale );
	seScaleMaxY->ObjFirstInit	( (*_I)->m_fMaxScale );
	seDensityFactor->ObjFirstInit( (*_I)->m_fDensityFactor );
    cbNoWaving->ObjFirstInit    ( TCheckBoxState(!!((*_I)->m_dwFlags&DO_NO_WAVING)) );

	_I++;
	for(;_I!=m_Data->end();_I++){
		seScaleMinY->ObjNextInit( (*_I)->m_fMinScale );
		seScaleMaxY->ObjNextInit( (*_I)->m_fMaxScale );
		seDensityFactor->ObjNextInit( (*_I)->m_fDensityFactor );
	    cbNoWaving->ObjNextInit( TCheckBoxState(!!((*_I)->m_dwFlags&DO_NO_WAVING)) );
	}
}

bool TfrmPropertiesDO::ApplyObjectsInfo(){
    VERIFY( !m_Data->empty() );

    DDIt _I = m_Data->begin();
	for(;_I!=m_Data->end();_I++){
		seScaleMinY->ObjApplyFloat( (*_I)->m_fMinScale );
		seScaleMaxY->ObjApplyFloat( (*_I)->m_fMaxScale );
		seDensityFactor->ObjApplyFloat( (*_I)->m_fDensityFactor );
		seDensityFactor->ObjApplyFloat( (*_I)->m_fDensityFactor );
        if (cbNoWaving->Checked)  (*_I)->m_dwFlags|=DO_NO_WAVING;
        else					  (*_I)->m_dwFlags&=~DO_NO_WAVING;
	}
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesDO::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesDO::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesDO::OnModified(TObject *Sender)
{
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesDO::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesDO::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int __fastcall TfrmPropertiesDO::Run(DDVec& SrcData, bool& bChange)
{
    m_Data = &SrcData;
    VERIFY(m_Data);
	GetObjectsInfo();
    bChange = (ShowModal()==mrOk);
    return mrOk;
}



