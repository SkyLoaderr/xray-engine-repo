
#include "stdafx.h"
#pragma hdrstop

#include "PropertiesLight.h"
#include "ELight.h"
#include "SceneClassList.h"
#include "ui_main.h"
 #include "ColorPicker.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
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
}
//----------------------------------------------------
void __fastcall TfrmPropertiesLight::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
    pcType->ActivePage = tsPoint;
    m_SaveLight = 0;

    pcType->Show();
	ObjectIt _F=m_Objects->begin();
    DWORD t=((CLight*)(*_F))->m_D3D.type;
    _F++;
	for(;_F!=m_Objects->end();_F++) if (t!=((CLight*)(*_F))->m_D3D.type) pcType->Hide();

	GetObjectsInfo();

    bChange = (ShowModal()==mrOk);
	if (m_CurLight) m_CurLight->Lock(false);

    _DELETE(m_SaveLight);
}

//---------------------------------------------------------------------------
#define X_GRID 14
#define Y_GRID 6
void __fastcall TfrmPropertiesLight::DrawGraph()
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

    float d_cost = sePointRange->Value/w;
    AnsiString temp;
    float v = sePointRange->Value;
    temp.sprintf("Range = %.2f",v); lbRange->Caption = temp;
    LG->Canvas->MoveTo(-1,h/2);
    LG->Canvas->Pen->Color = clLime;
    if (!(fis_zero(seA0->Value)&&fis_zero(seA1->Value)&&fis_zero(seA2->Value))){
        for (int d=1; d<w; d++){
            float R = d*d_cost;
            float b = seBrightness->Value/(seA0->Value+seA1->Value*R+seA2->Value*R*R);
            float bb = h-((h/(seBrightness->MaxValue*2))*b + h/2);
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
	tmAnimation->Enabled = false;

	CLight *_L = 0;
	ObjectIt _F = m_Objects->begin();

	VERIFY( (*_F)->ClassID()==OBJCLASS_LIGHT );
	_L = (CLight *)(*_F);

	sePointRange->ObjFirstInit		( _L->m_D3D.range );

    switch(_L->m_D3D.type){
    case D3DLIGHT_POINT:   		pcType->ActivePage = tsPoint;   break;
    case D3DLIGHT_DIRECTIONAL: 	pcType->ActivePage = tsSun; break;
    default: THROW;
    }
    cbFlares->Checked        		= _L->m_Flares;
    mmFlares->Text           		= _L->m_FlaresText;
    mmFlares->Enabled        		= cbFlares->Checked;

	mcDiffuse->ObjFirstInit			( _L->m_D3D.diffuse.get_windows() );
    cbUseInD3D->ObjFirstInit	   	( TCheckBoxState(_L->m_UseInD3D) );
    cbTargetLM->ObjFirstInit	   	( (_L->m_Flags.bAffectStatic)?cbChecked:cbUnchecked );
    cbTargetDynamic->ObjFirstInit	( (_L->m_Flags.bAffectDynamic)?cbChecked:cbUnchecked );
    cbTargetAnimated->ObjFirstInit	( (_L->m_Flags.bProcedural)?cbChecked:cbUnchecked );

    seA0->ObjFirstInit				( _L->m_D3D.attenuation0 );
    seA1->ObjFirstInit				( _L->m_D3D.attenuation1 );
    seA2->ObjFirstInit				( _L->m_D3D.attenuation2 );

    seBrightness->ObjFirstInit		( _L->m_Brightness );

	_F++;
	for(;_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_LIGHT );
		_L = (CLight *)(*_F);

        seA0->ObjNextInit				( _L->m_D3D.attenuation0 );
        seA1->ObjNextInit				( _L->m_D3D.attenuation1 );
        seA2->ObjNextInit				( _L->m_D3D.attenuation2 );

		sePointRange->ObjNextInit		( _L->m_D3D.range );
	    seBrightness->ObjNextInit		( _L->m_Brightness );

		mcDiffuse->ObjNextInit			( _L->m_D3D.diffuse.get_windows() );
        cbUseInD3D->ObjNextInit			( TCheckBoxState(_L->m_UseInD3D) );

	    cbTargetLM->ObjNextInit			( (_L->m_Flags.bAffectStatic)?cbChecked:cbUnchecked );
    	cbTargetDynamic->ObjNextInit	( (_L->m_Flags.bAffectDynamic)?cbChecked:cbUnchecked );
	    cbTargetAnimated->ObjNextInit	( (_L->m_Flags.bProcedural)?cbChecked:cbUnchecked );
	}
}

bool TfrmPropertiesLight::ApplyObjectsInfo(){
	VERIFY( !m_Objects->empty() );
    bool bResult = true;
	tmAnimation->Enabled = false;

	CLight *_L = 0;
	for(ObjectIt _F = m_Objects->begin();_F!=m_Objects->end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_LIGHT );
		_L = (CLight *)(*_F);
        if  (pcType->ActivePage==tsSun){
            _L->m_D3D.type 		= D3DLIGHT_DIRECTIONAL;
            _L->m_Flares       	= !!cbFlares->Checked;
            _L->m_FlaresText   	= mmFlares->Text;
        }else if (pcType->ActivePage==tsPoint){
            _L->m_D3D.type 		= D3DLIGHT_POINT;
        }

        seA0->ObjApplyFloat( _L->m_D3D.attenuation0 );
        seA1->ObjApplyFloat( _L->m_D3D.attenuation1 );
        seA2->ObjApplyFloat( _L->m_D3D.attenuation2 );
        seBrightness->ObjApplyFloat( _L->m_Brightness );
        sePointRange->ObjApplyFloat( _L->m_D3D.range );

        int c;
        if (mcDiffuse->ObjApply(c)){  _L->m_D3D.diffuse.set_windows(c); }

        cbUseInD3D->ObjApply( _L->m_UseInD3D );

	    c=(_L->m_Flags.bAffectStatic)?1:0;
	    cbTargetLM->ObjApply		(c); _L->m_Flags.bAffectStatic=c;
	    c=(_L->m_Flags.bAffectDynamic)?1:0;
	    cbTargetDynamic->ObjApply	(c); _L->m_Flags.bAffectDynamic=c;
	    c=(_L->m_Flags.bProcedural)?1:0;
	    cbTargetAnimated->ObjApply(c); 	 _L->m_Flags.bProcedural=c;
	}

    UI->UpdateScene();
    return bResult;
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
    if (ApplyObjectsInfo()){
	    ModalResult = mrOk;
    }else{
	    ModalResult = mrNone;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::btCancelClick(TObject *Sender)
{
	if (m_CurLight&&m_SaveLight)
	    m_CurLight->CopyFrom(m_SaveLight);
    Close();
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


void __fastcall TfrmPropertiesLight::tbBrightnessChange(TObject *Sender)
{
    seBrightness->Value = float(tbBrightness->Position)/100.f;
    DrawGraph();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
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

void __fastcall TfrmPropertiesLight::seBrightnessChange(TObject *Sender){
    if (seBrightness->Text.Length()){
    	tbBrightness->Position = float(seBrightness->Value)*100.f;
        DrawGraph();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::sePointRangeChange(TObject *Sender){
    if (sePointRange->Text.Length()) DrawGraph();
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesLight::ebALautoClick(TObject *Sender){
    float P = seAutoBMax->Value/100.f;
    seA0->Value = 1.f;
    seA1->Value = (seBrightness->Value-P-P*sePointRange->Value*sePointRange->Value*seA2->Value)/(P*sePointRange->Value);
    tbA0->Position = float(seA0->Value)*100.f;
    tbA1->Position = float(seA1->Value)*100.f;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::ebQLautoClick(TObject *Sender){
    float P = seAutoBMax->Value/100.f;
    seA0->Value = 1.f;
    seA2->Value = (seBrightness->Value-P-P*sePointRange->Value*seA1->Value)/(P*sePointRange->Value*sePointRange->Value);
    tbA0->Position = float(seA0->Value)*100.f;
    tbA2->Position = float(seA2->Value)*100.f;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::cbFlaresClick(TObject *Sender){
    mmFlares->Enabled  = cbFlares->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::cbTargetLMClick(TObject *Sender)
{
    if (cbTargetLM->Checked) cbTargetAnimated->Checked = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesLight::cbTargetAnimatedClick(TObject *Sender){
	ebAdjustScene->Visible 		= cbTargetAnimated->Checked;
    if (cbTargetAnimated->Checked) cbTargetLM->Checked = false;
}
//---------------------------------------------------------------------------


