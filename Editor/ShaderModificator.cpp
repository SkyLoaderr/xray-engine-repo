#include "stdafx.h"
#pragma hdrstop

#include "ShaderModificator.h"
#include "Shader.h"
#include "ShaderFunction.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "ElTree"
#pragma link "RxMenus"
#pragma link "multi_edit"
#pragma resource "*.dfm"
TfrmShaderModificator *frmShaderModificator=0;
//---------------------------------------------------------------------------
int frmShaderModificatorRun(SH_StageDef* stage){
	frmShaderModificator = new TfrmShaderModificator(0);
    int res = frmShaderModificator->Run(stage);
    _DELETE(frmShaderModificator);
    return res;
}
//---------------------------------------------------------------------------
__fastcall TfrmShaderModificator::TfrmShaderModificator(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TfrmShaderModificator::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------


void __fastcall TfrmShaderModificator::ebOkClick(TObject *Sender)
{
	UpdateModificatorData();
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderModificator::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------


//----------------------------------------------------
int __fastcall TfrmShaderModificator::Run(SH_StageDef* stage)
{
	VERIFY(stage);
	m_CurStage 	= stage;
    m_SaveStage	= new SH_StageDef(*stage);
    GetModificatorData	();
    UpdateModificatorData();
    int res 	= ShowModal();
    if (res!=mrOk)CopyMemory(m_CurStage,m_SaveStage,sizeof(SH_StageDef));
    _DELETE(m_SaveStage);
    return res;
}

void TfrmShaderModificator::GetModificatorData(){
	bLoadMode = true;
    cbScale->Checked	= m_CurStage->tcm & SH_StageDef::tcmScale;
    cbRotate->Checked	= m_CurStage->tcm & SH_StageDef::tcmRotate;
    cbScroll->Checked	= m_CurStage->tcm & SH_StageDef::tcmScroll;
    cbStretch->Checked	= m_CurStage->tcm & SH_StageDef::tcmStretch;

    seScaleU->Value		= m_CurStage->tcm_scaleU;
    seScaleV->Value		= m_CurStage->tcm_scaleV;
    seScrollU->Value	= m_CurStage->tcm_scrollU;
    seScrollV->Value	= m_CurStage->tcm_scrollV;
    seRotFactor->Value	= m_CurStage->tcm_rotate;
	bLoadMode = false;
}

void TfrmShaderModificator::UpdateModificatorData(){
	if (bLoadMode) return;
    m_CurStage->tcm = 0;
    if (cbScale->Checked) m_CurStage->tcm 	|= SH_StageDef::tcmScale;
    if (cbRotate->Checked) m_CurStage->tcm 	|= SH_StageDef::tcmRotate;
    if (cbScroll->Checked) m_CurStage->tcm 	|= SH_StageDef::tcmScroll;
    if (cbStretch->Checked) m_CurStage->tcm	|= SH_StageDef::tcmStretch;

    m_CurStage->tcm_scaleU	= seScaleU->Value;
    m_CurStage->tcm_scaleV	= seScaleV->Value;
    m_CurStage->tcm_scrollU	= seScrollU->Value;
    m_CurStage->tcm_scrollV	= seScrollV->Value;
    m_CurStage->tcm_rotate	= seRotFactor->Value;
}
void __fastcall TfrmShaderModificator::seArgExit(TObject *Sender)
{
	UpdateModificatorData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderModificator::CheckClick(TObject *Sender)
{
	seScaleU->Enabled 		= cbScale->Checked;
	seScaleV->Enabled 		= cbScale->Checked;

	seScrollU->Enabled 		= cbScroll->Checked;
	seScrollV->Enabled 		= cbScroll->Checked;

    seRotFactor->Enabled 	= cbRotate->Checked;

    ebStretchFunc->Enabled 	= cbStretch->Checked;
	UpdateModificatorData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderModificator::ebStretchFuncClick(TObject *Sender)
{
	frmShaderFunctionRun(&m_CurStage->tcm_stretch);
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderModificator::seScaleUKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN)	UpdateModificatorData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderModificator::seScaleULWChange(TObject *Sender,
      int Val)
{
	UpdateModificatorData();
}
//---------------------------------------------------------------------------

