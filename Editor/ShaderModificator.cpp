#include "stdafx.h"
#pragma hdrstop

#include "ShaderModificator.h"
#include "Shader.h"
#include "ShaderFunction.h"
#include "xr_tokens.h"
#include "WaveForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmShaderMatrix *TfrmShaderMatrix::form=0;
//---------------------------------------------------------------------------
xr_token							mode_token					[ ]={
	{ "Programmable",			  	CMatrix::modeProgrammable  	},
	{ "TCM",					  	CMatrix::modeTCM		  	},
	{ "Spherical Reflection",	  	CMatrix::modeS_refl	   		},
	{ "Cube Reflection",			CMatrix::modeC_refl	  		},
	{ 0,							0							}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TfrmShaderMatrix::TfrmShaderMatrix(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"shader_ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
	FillMenuFromToken(pmMode, mode_token, stModeClick);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TfrmShaderMatrix::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------


void __fastcall TfrmShaderMatrix::ebOkClick(TObject *Sender)
{
	UpdateMatrixData();
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderMatrix::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------


//----------------------------------------------------
int __fastcall TfrmShaderMatrix::Run(CMatrix* mat)
{
	VERIFY(mat);
	form = new TfrmShaderMatrix(0);
	form->m_CurMatrix 	= mat;
    form->m_SaveMatrix	= new CMatrix(*mat);
    form->GetMatrixData	();
    form->UpdateMatrixData();
    int res 	= form->ShowModal();
    if (res!=mrOk)CopyMemory(form->m_CurMatrix,form->m_SaveMatrix,sizeof(CMatrix));
    _DELETE(form->m_SaveMatrix);
    return res;
}

void TfrmShaderMatrix::GetMatrixData(){
	bLoadMode = true;
	stMode->Caption 	= GetTokenNameFromVal_EQ(m_CurMatrix->dwMode, AnsiString(stMode->Caption), mode_token);
    cbScale->Checked	= m_CurMatrix->tcm & CMatrix::tcmScale;
    cbRotate->Checked	= m_CurMatrix->tcm & CMatrix::tcmRotate;
    cbScroll->Checked	= m_CurMatrix->tcm & CMatrix::tcmScroll;
	bLoadMode = false;
}

void TfrmShaderMatrix::UpdateMatrixData(){
	if (bLoadMode) return;
	m_CurMatrix->dwMode 	= GetTokenValFromName(AnsiString(stMode->Caption), mode_token);
    m_CurMatrix->tcm = 0;
    if (cbScale->Checked) 	m_CurMatrix->tcm 	|= CMatrix::tcmScale;
    if (cbRotate->Checked) 	m_CurMatrix->tcm 	|= CMatrix::tcmRotate;
    if (cbScroll->Checked) 	m_CurMatrix->tcm 	|= CMatrix::tcmScroll;
	// visible TCM params
    paTCM->Visible		= m_CurMatrix->dwMode==CMatrix::modeTCM;
}
void __fastcall TfrmShaderMatrix::seArgExit(TObject *Sender)
{
	UpdateMatrixData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderMatrix::CheckClick(TObject *Sender)
{
	ebScaleU->Enabled 		= cbScale->Checked;
	ebScaleV->Enabled 		= cbScale->Checked;

	ebScrollU->Enabled 		= cbScroll->Checked;
	ebScrollV->Enabled 		= cbScroll->Checked;

    ebRotate->Enabled 		= cbRotate->Checked;
	UpdateMatrixData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderMatrix::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action 	= caFree;
//    form 	= 0;
}
//---------------------------------------------------------------------------

static TStaticText* temp_text=0;
void __fastcall TfrmShaderMatrix::stModeMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    POINT pt;
    GetCursorPos(&pt);
    temp_text = (TStaticText*)Sender;
	pmMode->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderMatrix::stModeClick(TObject *Sender)
{
	if (temp_text) temp_text->Caption=((TMenuItem*)Sender)->Caption;
    temp_text = 0;
    UpdateMatrixData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderMatrix::ebScaleUClick(TObject *Sender)
{
	TfrmShaderFunction::Run(&m_CurMatrix->scaleU);
}
//---------------------------------------------------------------------------


void __fastcall TfrmShaderMatrix::ebScaleVClick(TObject *Sender)
{
	TfrmShaderFunction::Run(&m_CurMatrix->scaleV);
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderMatrix::ebRotateClick(TObject *Sender)
{
	TfrmShaderFunction::Run(&m_CurMatrix->rotate);
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderMatrix::ebScrollUClick(TObject *Sender)
{
	TfrmShaderFunction::Run(&m_CurMatrix->scrollU);
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderMatrix::ebScrollVClick(TObject *Sender)
{
	TfrmShaderFunction::Run(&m_CurMatrix->scrollV);
}
//---------------------------------------------------------------------------

