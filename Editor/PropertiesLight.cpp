//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "PropertiesLight.h"
#include "Light.h"
#include "SceneClassList.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "multiobj"
#pragma resource "*.dfm"
TfrmPropertiesLight *frmPropertiesLight;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesLight::TfrmPropertiesLight(TComponent* Owner)
    : TForm(Owner)
{
}
//----------------------------------------------------
void __fastcall TfrmPropertiesLight::Run(ObjectList* pObjects)
{
    m_Objects = pObjects;
    pcType->ActivePage = tsPoint;
    if (m_Objects->size()>1){
        pcType->Hide();
    }else{
        pcType->Show();
    }

    // выключить на время загрузки AutoApply если он включен
    bool chk = cbAutoApply->Checked;
    cbAutoApply->Checked = false;
	GetObjectsInfo();
    // включить AutoApply если он был включен
    cbAutoApply->Checked = chk;

    frmPropertiesLight->ShowModal();
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesLight::DrawGraph()
{
    int w = LG->Width;
    int h = LG->Height;

    TRect r; r.Left=0; r.Top=0;  r.Right=w; r.Bottom=h;
    LG->Canvas->Brush->Color = clGray;
    LG->Canvas->FillRect(r);
    LG->Canvas->Pen->Color = clRed;
    LG->Canvas->MoveTo(0,h/2);
    LG->Canvas->LineTo(w,h/2);

    float d_cost = sePointRange->Value/w;
    float maxA   = 1/(seA0->Value+seA1->Value*1*d_cost+seA2->Value*seA2->Value*1*d_cost);
    AnsiString temp;
    temp.sprintf("Max A = %.3f",maxA); lbMaxA->Caption = temp;
    temp.sprintf("Range = %.3f",sePointRange->Value); lbRange->Caption = temp;
    LG->Canvas->MoveTo(-1,h/2);
    LG->Canvas->Pen->Color = clBlue;
    for (int d=1; d<w; d++){
        float b = seBrightness->Value/(seA0->Value+seA1->Value*d*d_cost+seA2->Value*seA2->Value*d*d_cost);
        float bb = h-(h/4)*b/maxA-h/2;
        LG->Canvas->LineTo(d-2,bb);
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesLight::mcColorMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (cdColor->Execute()){
        ((TMultiObjColor*)Sender)->_Set(cdColor->Color);
        if (cbAutoApply->Checked) btApplyClick(0);
    }
}
//---------------------------------------------------------------------------

void TfrmPropertiesLight::GetObjectsInfo(){
	_ASSERTE( !m_Objects->empty() );

	Light *_L = 0;
	ObjectIt _F = m_Objects->begin();

	_ASSERTE( (*_F)->ClassID()==OBJCLASS_LIGHT );
	_L = (Light *)(*_F);

	mcAmbient->ObjFirsInit( _L->m_Ambient.get() );
	mcDiffuse->ObjFirsInit( _L->m_Diffuse.get() );
	mcSpecular->ObjFirsInit( _L->m_Specular.get() );
    cbCastShadows->ObjFirsInit( TCheckBoxState(_L->m_CastShadows) );
    switch(_L->m_Type){
    case ltSun:   pcType->ActivePage = tsSun;   break;
    case ltPoint: pcType->ActivePage = tsPoint; break;
    }
    sePointRange->Value      = _L->m_Range;
    seDirectionalDirX->Value = _L->m_Direction.x;
    seDirectionalDirY->Value = _L->m_Direction.y;
    seDirectionalDirZ->Value = _L->m_Direction.z;

    seSunSize->Value    = _L->fSunSize;
    sePower->Value      = _L->fPower;
    cbFlares->Checked   = _L->bFlares;
    cbGradient->Checked = _L->bGradient;

    seA0->Value = _L->m_Attenuation0;
    seA1->Value = _L->m_Attenuation1;
    seA2->Value = _L->m_Attenuation2;

    seBrightness->Value = _L->m_Brightness;

	_F++;

	for(;_F!=m_Objects->end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_LIGHT );
		_L = (Light *)(*_F);

		mcAmbient->ObjNextInit( _L->m_Ambient.get() );
		mcDiffuse->ObjNextInit( _L->m_Diffuse.get() );
		mcSpecular->ObjNextInit( _L->m_Specular.get() );
        cbCastShadows->ObjNextInit( TCheckBoxState(_L->m_CastShadows) );
	}
}

void TfrmPropertiesLight::ApplyObjectsInfo(){
	_ASSERTE( !m_Objects->empty() );

	Light *_L = 0;
	ObjectIt _F = m_Objects->begin();

	_L = (Light *)(*_F);
    if  (pcType->ActivePage==tsSun){
        _L->m_Type = ltSun;
        _L->m_Direction.x = seDirectionalDirX->Value;
        _L->m_Direction.y = seDirectionalDirY->Value;
        _L->m_Direction.z = seDirectionalDirZ->Value;
        _L->fSunSize = seSunSize->Value;
        _L->fPower   = sePower->Value;
        _L->bFlares  = cbFlares->Checked;
        _L->bGradient= cbGradient->Checked;
    }else if (pcType->ActivePage==tsPoint){
        _L->m_Range = sePointRange->Value;
        _L->m_Type = ltPoint;
        _L->m_Attenuation0 = seA0->Value;
        _L->m_Attenuation1 = seA1->Value;
        _L->m_Attenuation2 = seA2->Value;
        _L->m_Brightness   = seBrightness->Value;
    }

	for(;_F!=m_Objects->end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_LIGHT );
		_L = (Light *)(*_F);
        int c;
        if (mcAmbient->ObjApply(c))  _L->m_Ambient.set(c);
        if (mcDiffuse->ObjApply(c))  _L->m_Diffuse.set(c);
	    if (mcSpecular->ObjApply(c)) _L->m_Specular.set(c);
        cbCastShadows->ObjApply( _L->m_CastShadows );
        _L->FillD3DParams();
	}
    UI.UpdateScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesLight::btApplyClick(TObject *Sender)
{
    ApplyObjectsInfo();
    GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::btOkClick(TObject *Sender)
{
    ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
/*
[sun]
source_radius = 0.15
light_color = 255, 255, 255
power = 0.6
gradient = on
flares = on
textures = flare0.tga, flare1.tga, flare2.tga, flare3.tga
direction=-0.5774,-0.5774,-0.5774
*/

void __fastcall TfrmPropertiesLight::seA0Change(TObject *Sender)
{
    if (seA0->Text.Length()){
        tbA0->Position = seA0->Value*100;
        DrawGraph();
        if (cbAutoApply->Checked) btApplyClick(0);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::seA1Change(TObject *Sender)
{
    if (seA1->Text.Length()){
        tbA1->Position = seA1->Value*100;
        DrawGraph();
        if (cbAutoApply->Checked) btApplyClick(0);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::seA2Change(TObject *Sender)
{
    if (seA2->Text.Length()){
        tbA2->Position = seA2->Value*100;
        DrawGraph();
        if (cbAutoApply->Checked) btApplyClick(0);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::tbA0Change(TObject *Sender)
{
    seA0->Value = float(tbA0->Position)/100.f;
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::tbA1Change(TObject *Sender)
{
    seA1->Value = float(tbA1->Position)/100.f;
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::tbA2Change(TObject *Sender)
{
    seA2->Value = float(tbA2->Position)/100.f;
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::FormPaint(TObject *Sender)
{
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::seBrightnessChange(TObject *Sender)
{
    if (seBrightness->Text.Length()){
        tbBrightness->Position = float(seBrightness->Value)*100.f;
        DrawGraph();
        if (cbAutoApply->Checked) btApplyClick(0);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::tbBrightnessChange(TObject *Sender)
{
    seBrightness->Value = float(tbBrightness->Position)/100.f;
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::sePointRangeChange(TObject *Sender)
{
    if (sePointRange->Text.Length()){
        DrawGraph();
        if (cbAutoApply->Checked) btApplyClick(0);
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesLight::cbAutoApplyClick(TObject *Sender)
{
    if (cbAutoApply->Checked) btApplyClick(0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::pcTypeChange(TObject *Sender)
{
    if (cbAutoApply->Checked) btApplyClick(0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::seDirectionalDirChange(
      TObject *Sender)
{
    if (cbAutoApply->Checked) btApplyClick(0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) btCancel->Click();
}
//---------------------------------------------------------------------------

