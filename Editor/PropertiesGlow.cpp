#include "stdafx.h"
#pragma hdrstop

#include "PropertiesGlow.h"
#include "sceneclasslist.h"
#include "ui_main.h"
#include "glow.h"
#include "xrshader.h"
#include "shader.h"
#include "texture.h"
#include "ChoseForm.h"
#include "xr_trims.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "CloseBtn"
#pragma link "Placemnt"
#pragma link "multi_edit"
#pragma resource "*.dfm"
TfrmPropertiesGlow *frmPropertiesGlow=0;
//---------------------------------------------------------------------------
void frmPropertiesGlowRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesGlow = new TfrmPropertiesGlow(0);
    frmPropertiesGlow->Run(pObjects,bChange);
    _DELETE(frmPropertiesGlow);
}
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesGlow::TfrmPropertiesGlow(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::Run(ObjectList* pObjects, bool& bChange)
{
	ebApply->Enabled 	= false;
	ebOk->Enabled 		= false;
    m_Objects = pObjects;
	GetObjectsInfo();
    bChange = (ShowModal()==mrOk);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------

void __fastcall TfrmPropertiesGlow::ebOkClick(TObject *Sender)
{
    if (ApplyObjectsInfo()){
	    Close();
    	ModalResult = mrOk;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void TfrmPropertiesGlow::GetObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	CGlow *_G = 0;
	ObjectIt _F = m_Objects->begin();
	VERIFY( (*_F)->ClassID()==OBJCLASS_GLOW );
	_G = (CGlow *)(*_F);

    m_Glow = _G;

	seRange->ObjFirstInit( _G->m_Range );
	_F++;
    bool bDifTex=false, bDifShader=false;
    AnsiString sh 	= _G->m_ShaderName;
    AnsiString tex1,tex2;
    tex1 = ListToSequence(_G->m_TexNames);
	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_GLOW );
		_G = (CGlow *)(*_F);
		seRange->ObjNextInit( _G->m_Range );
		if (!bDifShader&&(_G->m_ShaderName!=sh)) bDifShader = true;
		tex2 = ListToSequence(_G->m_TexNames);
        if (!bDifTex&&!_G->m_TexNames.empty()&&(tex2!=tex1))bDifTex = true;
        m_Glow = 0;
	}

    if (m_Glow){
    	lbShader->Font->Color = clNavy;
    	lbTexture->Font->Color = clNavy;
		lbTexture->Caption = m_Glow->m_TexNames.empty()?AnsiString("..."):tex1;
		lbShader->Caption  = m_Glow->m_ShaderName.IsEmpty()?AnsiString("..."):m_Glow->m_ShaderName;
    }else{
		_F = m_Objects->begin();
		_G = (CGlow *)(*_F);
		lbTexture->Caption = _G->m_TexNames.empty()?AnsiString("..."):tex1;
		lbShader->Caption  = _G->m_ShaderName.IsEmpty()?AnsiString("..."):_G->m_ShaderName;
        if (bDifTex)	lbTexture->Color = clMaroon;
        else			lbTexture->Font->Color = clNavy;
        if (bDifShader)	lbShader->Font->Color = clMaroon;
        else        	lbShader->Font->Color = clNavy;
    }
}

bool TfrmPropertiesGlow::ApplyObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	CGlow *_G = 0;
	ObjectIt _F = m_Objects->begin();
    bool bValidTex 		= (lbTexture->Caption!="...");
    bool bValidShader 	= (lbShader->Caption!="...");
	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_GLOW );
		_G = (CGlow *)(*_F);
		seRange->ObjApplyFloat( _G->m_Range );
        // apply shader
        if (bValidTex)		SequenceToList(_G->m_TexNames,lbTexture->Caption.c_str());
        if (bValidShader) 	_G->m_ShaderName = lbShader->Caption;
        if (bValidShader&&bValidTex) _G->Compile();
	}
	pbImagePaint(0); paImage->Repaint();
    UI->UpdateScene();
    return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::ebApplyClick(TObject *Sender)
{
    ApplyObjectsInfo();
    GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::pbImagePaint(TObject *Sender)
{
    bool bValidTex 		= (lbTexture->Caption!="...");
    if (bValidTex){
        RECT r; r.left = 1; r.top = 1;
        float w, h;
        ETextureCore* tx = UI->Device.Shader.FindTexture(lbTexture->Caption.c_str());
        if (tx){
	        w = tx->width();
    	    h = tx->height();
	        if (w>h){   r.right = pbImage->Width; r.bottom = h/w*pbImage->Height;
    	    }else{      r.right = h/w*pbImage->Width; r.bottom = pbImage->Height;}
    	    tx->StretchThumbnail(paImage->Handle, &r);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::OnModified(TObject *Sender)
{
	ebApply->Enabled 	= true;
	ebOk->Enabled 		= true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::seRangeKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN)	OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::seRangeLWChange(TObject *Sender,
      int Val)
{
	OnModified(Sender);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::ebSelectShaderClick(TObject *Sender)
{
    bool bValidShader 	= (lbShader->Caption!="...");
	LPCSTR S = TfrmChoseItem::SelectShader(true,0,bValidShader?lbShader->Caption.c_str():0);
    if (S){lbShader->Caption=S; OnModified(Sender);}
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::ebSelectTextureClick(TObject *Sender)
{
    bool bValidTex 		= (lbTexture->Caption!="...");
	LPCSTR S = TfrmChoseItem::SelectTexture(true,bValidTex?lbTexture->Caption.c_str():0);
    if (S){lbTexture->Caption=S; OnModified(Sender);}
}
//---------------------------------------------------------------------------

