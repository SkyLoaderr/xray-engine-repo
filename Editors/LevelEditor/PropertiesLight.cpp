
#include "stdafx.h"
#pragma hdrstop

#include "PropertiesLight.h"
#include "ELight.h"
#include "Scene.h"                              
#include "PropertiesList.h"
#include "ColorPicker.h"
#include "ui_main.h"
#include "xr_tokens.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
TfrmPropertiesLight *frmPropertiesLight=0;

//---------------------------------------------------------------------------
void frmPropertiesLightRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesLight = new TfrmPropertiesLight(0);
    frmPropertiesLight->Run(pObjects,bChange);
    _DELETE(frmPropertiesLight);
}
//---------------------------------------------------------------------------

__fastcall TfrmPropertiesLight::TfrmPropertiesLight(TComponent* Owner)
    : TForm(Owner)
{
	m_SpotProps	= TProperties::CreateForm(paSpotProps,alClient,OnModified);
	m_PointProps= TProperties::CreateForm(paPointProps,alClient,OnModified);
    m_SunProps 	= TProperties::CreateForm(tsSun,alClient,OnModified);
    m_Props		= TProperties::CreateForm(paProps,alClient,OnModified);
	DEFINE_INI(fsStorage);
}
//----------------------------------------------------
void __fastcall TfrmPropertiesLight::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    switch (ModalResult){
    case mrOk: 		ApplyObjectsInfo();		break;
    case mrCancel: 	CancelObjectsInfo();	break;
    default: THROW2("Invalid case");
    }
    TProperties::DestroyForm(m_SpotProps);
    TProperties::DestroyForm(m_PointProps);
	TProperties::DestroyForm(m_SunProps);
	TProperties::DestroyForm(m_Props);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesLight::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
    pcType->ActivePage = tsPoint;
    pcType->Show();
	GetObjectsInfo();
    bChange = (ShowModal()==mrOk);
}

//---------------------------------------------------------------------------
#define X_GRID 14
#define Y_GRID 6

void __fastcall TfrmPropertiesLight::DrawGraph()
{
	if (!flBrightness) return;
	DrawGraph(flPointRange->GetValue(), flBrightness->GetValue(), flBrightness->lim_mx, flPointA0->GetValue(), flPointA1->GetValue(), flPointA2->GetValue());
}

void __fastcall TfrmPropertiesLight::DrawGraph(float range, float br, float br_max, float a0, float a1, float a2)
{
    int w = LG->Width;
    int h = LG->Height;

    TRect r; r.Left=0; r.Top=0;  r.Right=w; r.Bottom=h;
    LG->Canvas->Brush->Color = clBlack;
    LG->Canvas->FillRect(r);
    LG->Canvas->Pen->Color = TColor(0x00006600);
    LG->Canvas->MoveTo(0,0);
    for (int i=0; i<X_GRID+1; i++){
        LG->Canvas->LineTo(i*w/X_GRID,h);
        LG->Canvas->MoveTo((i+1)*w/X_GRID,0);
    }
    LG->Canvas->MoveTo(0,0);
    for (int j=0; j<Y_GRID+1; j++){
        LG->Canvas->LineTo(w,j*h/Y_GRID);
        LG->Canvas->MoveTo(0,(j+1)*h/Y_GRID);
    }
    LG->Canvas->Pen->Color = clYellow;
    LG->Canvas->MoveTo(0,h/2);
    LG->Canvas->LineTo(w,h/2);

    float d_cost = range/w;
    AnsiString temp;
    float v = range;
    temp.sprintf("Range = %.2f",v); lbRange->Caption = temp;
    LG->Canvas->MoveTo(-1,h/2);
    LG->Canvas->Pen->Color = clLime;
    if (!(fis_zero(a0)&&fis_zero(a1)&&fis_zero(a2))){
        for (int d=1; d<w; d++){
            float R = d*d_cost;
            float b = br/(a0+a1*R+a2*R*R);
            float bb = h-((h/(br_max*2))*b + h/2);
            LG->Canvas->LineTo(d-2,bb);
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesLight::mcColorMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	DWORD color = ((TMultiObjColor*)Sender)->Brush->Color;
	if (SelectColorWin(&color)) ((TMultiObjColor*)Sender)->_Set(color);
}
//---------------------------------------------------------------------------

void TfrmPropertiesLight::GetObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	CLight *_L = 0;
	ObjectIt _F = m_Objects->begin();
    LPCSTR pref						= GetClassNameByClassID((*_F)->ClassID);

	VERIFY( (*_F)->ClassID==OBJCLASS_LIGHT );
	_L = (CLight *)(*_F);

    switch(_L->m_D3D.type){
    case D3DLIGHT_POINT:   			pcType->ActivePage = tsPoint;   break;
    case D3DLIGHT_DIRECTIONAL: 		pcType->ActivePage = tsSun; 	break;
    case D3DLIGHT_SPOT:   			pcType->ActivePage = tsSpot;	break;
    default: THROW;
    }

    PropValueVec values;
    PropValueVec sun_values;
    PropValueVec point_values;
    PropValueVec spot_values;
	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID==OBJCLASS_LIGHT );
		_L 							= (CLight *)(*_F);
        _L->FillProp				(GetClassNameByClassID(_L->ClassID),values);
        _L->FillSunProp				(GetClassNameByClassID(_L->ClassID),sun_values);
        _L->FillPointProp			(GetClassNameByClassID(_L->ClassID),point_values);
        _L->FillSpotProp			(GetClassNameByClassID(_L->ClassID),spot_values);
	}
	flBrightness 					= (FloatValue*)PHelper.FindProp(values,pref,"Brightness"); R_ASSERT(flBrightness);
    flBrightness->OnChange			= OnBrightnessChange;
    flPointRange					= (FloatValue*)PHelper.FindProp(point_values,pref,"Range");						R_ASSERT(flPointRange);
    flPointA0						= (FloatValue*)PHelper.FindProp(point_values,pref,"Attenuation\\Constant"); 	R_ASSERT(flPointA0);
    flPointA1						= (FloatValue*)PHelper.FindProp(point_values,pref,"Attenuation\\Linear"); 	 	R_ASSERT(flPointA1);
    flPointA2						= (FloatValue*)PHelper.FindProp(point_values,pref,"Attenuation\\Quadratic"); 	R_ASSERT(flPointA2);
    m_Props->AssignValues			(values,true);
    m_SunProps->AssignValues		(sun_values,true);
    m_PointProps->AssignValues		(point_values,true);
    m_SpotProps->AssignValues		(spot_values,true);
}

bool TfrmPropertiesLight::ApplyObjectsInfo(){
	VERIFY( !m_Objects->empty() );
    bool bResult = true;

	CLight *_L = 0;
	for(ObjectIt _F = m_Objects->begin();_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID==OBJCLASS_LIGHT );
		_L = (CLight *)(*_F);
        if  (pcType->ActivePage==tsSun)			_L->m_D3D.type = D3DLIGHT_DIRECTIONAL;
        else if (pcType->ActivePage==tsPoint)	_L->m_D3D.type = D3DLIGHT_POINT;
        else if (pcType->ActivePage==tsSpot)	_L->m_D3D.type = D3DLIGHT_SPOT;
        _L->Update();
	}

    UI.UpdateScene();
    return bResult;
}
//---------------------------------------------------------------------------

void TfrmPropertiesLight::CancelObjectsInfo()
{
	m_Props->ResetValues();

    UI.UpdateScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::btApplyClick(TObject *Sender)
{
    if (ApplyObjectsInfo())
	    GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::btOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::btCancelClick(TObject *Sender)
{
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::FormPaint(TObject *Sender)
{
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::OnModified()
{
    DrawGraph();
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::OnBrightnessChange(PropValue* sender)
{
	DrawGraph(flPointRange->GetValue(), ((FloatValue*)sender)->GetValue(), flBrightness->lim_mx, flPointA0->GetValue(), flPointA1->GetValue(), flPointA2->GetValue());
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (m_SunProps->IsFocused()||m_Props->IsFocused()||m_PointProps->IsFocused()||m_SpotProps->IsFocused()) return;
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::ebALautoClick(TObject *Sender){
	if (!flBrightness) return;
    float P = seAutoBMax->Value/100.f;
    flPointA0->ApplyValue(1.f);
    flPointA1->ApplyValue((flBrightness->GetValue()-P-P*flPointRange->GetValue()*flPointRange->GetValue()*flPointA2->GetValue())/(P*flPointRange->GetValue()));
    m_PointProps->RefreshForm();
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::ebQLautoClick(TObject *Sender){
	if (!flBrightness) return;
    float P = seAutoBMax->Value/100.f;
    flPointA0->ApplyValue(1.f);
    flPointA2->ApplyValue((flBrightness->GetValue()-P-P*flPointRange->GetValue()*flPointA1->GetValue())/(P*flPointRange->GetValue()*flPointRange->GetValue()));
    m_PointProps->RefreshForm();
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::fsStorageRestorePlacement(
      TObject *Sender)
{
	m_SpotProps->RestoreColumnWidth	(fsStorage);
	m_PointProps->RestoreColumnWidth(fsStorage);
	m_SunProps->RestoreColumnWidth	(fsStorage);
	m_Props->RestoreColumnWidth		(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::fsStorageSavePlacement(
      TObject *Sender)
{
	m_SpotProps->SaveColumnWidth	(fsStorage);
	m_PointProps->SaveColumnWidth	(fsStorage);
	m_SunProps->SaveColumnWidth		(fsStorage);
	m_Props->SaveColumnWidth		(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::FormShow(TObject *Sender)
{
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

