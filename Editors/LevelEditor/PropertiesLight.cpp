
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
    m_SunProps 	= TProperties::CreateForm(tsSun,alClient);
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
	TProperties::DestroyForm(m_SunProps);
	TProperties::DestroyForm(m_Props);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesLight::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
    pcType->ActivePage = tsPoint;

    pcType->Show();
	ObjectIt _F=m_Objects->begin();
    DWORD t=((CLight*)(*_F))->m_D3D.type;
    _F++;
	for(;_F!=m_Objects->end();_F++) if (t!=((CLight*)(*_F))->m_D3D.type) pcType->Hide();

	GetObjectsInfo();

    bChange = (ShowModal()==mrOk);
}

//---------------------------------------------------------------------------
#define X_GRID 14
#define Y_GRID 6

void __fastcall TfrmPropertiesLight::DrawGraph()
{
	if (!flBrightness) return;
	DrawGraph(sePointRange->Value, flBrightness->GetValue(), flBrightness->lim_mx, seA0->Value, seA1->Value, seA2->Value);
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

	VERIFY( (*_F)->ClassID==OBJCLASS_LIGHT );
	_L = (CLight *)(*_F);

	sePointRange->ObjFirstInit		( _L->m_D3D.range );
    seA0->ObjFirstInit				( _L->m_D3D.attenuation0 );
    seA1->ObjFirstInit				( _L->m_D3D.attenuation1 );
    seA2->ObjFirstInit				( _L->m_D3D.attenuation2 );

    switch(_L->m_D3D.type){
    case D3DLIGHT_POINT:   			pcType->ActivePage = tsPoint;   break;
    case D3DLIGHT_DIRECTIONAL: 		pcType->ActivePage = tsSun; 	break;
    default: THROW;
    }

//    m_Props->BeginFillMode	();
    PropValueMap values;
    _L->FillProp(values,true);
	flBrightness 					= (FloatValue*)PROP::FindProp(values,"Brightness"); R_ASSERT(flBrightness);
    flBrightness->OnAfterEdit		= OnBrightnessAfterEdit;
    if (m_Objects->size()>1){
		PropValue* V 				= PROP::FindProp(values,"Name"); R_ASSERT(V);
        V->bEnabled					= false;
    }
                                                          
	_F++;
	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID==OBJCLASS_LIGHT );
		_L 							= (CLight *)(*_F);
        _L->FillProp				(values,false);

        seA0->ObjNextInit		 	(_L->m_D3D.attenuation0);
        seA1->ObjNextInit		 	(_L->m_D3D.attenuation1);
        seA2->ObjNextInit		 	(_L->m_D3D.attenuation2);

		sePointRange->ObjNextInit	(_L->m_D3D.range);
	}
    m_Props->AssignValues			(values,true);
//    m_Props->EndFillMode	();
    
    // init flares
    if (m_Objects->size()==1){
		CEditFlare& F = ((CLight*)m_Objects->front())->m_LensFlare;
/*       
//p 
    	m_SunProps->Enabled = true;
		m_SunProps->BeginFillMode();
        TElTreeItem* M=0;
        TElTreeItem* N=0;
		M = m_SunProps->AppendMarkerValue	(0,"Source")->item;
			m_SunProps->AppendFlagValue		(M,"Enabled",	&F.m_dwFlags,CEditFlare::flSource);
			m_SunProps->AppendFloatValue	(M,"Radius", 	&F.m_Source.fRadius,0.f,10.f);
			m_SunProps->AppendTextureValue	(M,"Texture",	F.m_Source.texture,sizeof(F.m_Source.texture));
		M = m_SunProps->AppendMarkerValue	(0,"Gradient")->item;
			m_SunProps->AppendFlagValue		(M,"Enabled",	&F.m_dwFlags,CEditFlare::flGradient);
			m_SunProps->AppendFloatValue	(M,"Radius", 	&F.m_Gradient.fRadius,0.f,100.f);
			m_SunProps->AppendFloatValue	(M,"Opacity",	&F.m_Gradient.fOpacity,0.f,1.f);
			m_SunProps->AppendTextureValue	(M,"Texture",	F.m_Gradient.texture,sizeof(F.m_Gradient.texture));
		M = m_SunProps->AppendMarkerValue	(0,"Flares")->item;
			m_SunProps->AppendFlagValue		(M,"Enabled",	&F.m_dwFlags,CEditFlare::flFlare);
		for (CEditFlare::FlareIt it=F.m_Flares.begin(); it!=F.m_Flares.end(); it++){
            AnsiString nm; nm.sprintf("Flare %d",it-F.m_Flares.begin());
		N = m_SunProps->AppendMarkerValue	(M,nm.c_str())->item;
			m_SunProps->AppendFloatValue	(N,"Radius", 	&it->fRadius,0.f,10.f);
			m_SunProps->AppendFloatValue	(N,"Opacity", 	&it->fOpacity,0.f,1.f);
			m_SunProps->AppendFloatValue	(N,"Position",	&it->fPosition,-10.f,10.f);
			m_SunProps->AppendTextureValue	(N,"Texture",	it->texture,sizeof(it->texture));
		}
		m_SunProps->EndFillMode();
*/
    }else{
    	m_SunProps->Enabled = false;
    }
}

bool TfrmPropertiesLight::ApplyObjectsInfo(){
	VERIFY( !m_Objects->empty() );
    bool bResult = true;

	CLight *_L = 0;
	for(ObjectIt _F = m_Objects->begin();_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID==OBJCLASS_LIGHT );
		_L = (CLight *)(*_F);
        if  (pcType->ActivePage==tsSun){
            _L->m_D3D.type 		= D3DLIGHT_DIRECTIONAL;
        }else if (pcType->ActivePage==tsPoint){
            _L->m_D3D.type 		= D3DLIGHT_POINT;
        }

        seA0->ObjApplyFloat( _L->m_D3D.attenuation0 );
        seA1->ObjApplyFloat( _L->m_D3D.attenuation1 );
        seA2->ObjApplyFloat( _L->m_D3D.attenuation2 );
        sePointRange->ObjApplyFloat( _L->m_D3D.range );

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

void __fastcall TfrmPropertiesLight::OnModified()
{
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::OnBrightnessAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	DrawGraph(sePointRange->Value, *(float*)edit_val, flBrightness->lim_mx, seA0->Value, seA1->Value, seA2->Value);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (m_SunProps->Focused()||m_Props->Focused()) return;

    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::seA0Change(TObject *Sender){
    if (seA0->Text.Length()){ tbA0->Position = seA0->Value*100; DrawGraph(); }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::seA1Change(TObject *Sender){
    if (seA1->Text.Length()){ tbA1->Position = seA1->Value*100; DrawGraph(); }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::seA2Change(TObject *Sender){
    if (seA2->Text.Length()){ tbA2->Position = seA2->Value*100; DrawGraph(); }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::sePointRangeChange(TObject *Sender){
    if (sePointRange->Text.Length()) DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::ebALautoClick(TObject *Sender){
	if (!flBrightness) return;
    float P = seAutoBMax->Value/100.f;
    seA0->Value = 1.f;
    seA1->Value = (flBrightness->GetValue()-P-P*sePointRange->Value*sePointRange->Value*seA2->Value)/(P*sePointRange->Value);
    tbA0->Position = float(seA0->Value)*100.f;
    tbA1->Position = float(seA1->Value)*100.f;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::ebQLautoClick(TObject *Sender){
	if (!flBrightness) return;
    float P = seAutoBMax->Value/100.f;
    seA0->Value = 1.f;
    seA2->Value = (flBrightness->GetValue()-P-P*sePointRange->Value*seA1->Value)/(P*sePointRange->Value*sePointRange->Value);
    tbA0->Position = float(seA0->Value)*100.f;
    tbA2->Position = float(seA2->Value)*100.f;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::fsStorageRestorePlacement(
      TObject *Sender)
{
	m_SunProps->RestoreColumnWidth(fsStorage);
	m_Props->RestoreColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::fsStorageSavePlacement(
      TObject *Sender)
{
	m_SunProps->SaveColumnWidth(fsStorage);
	m_Props->SaveColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

