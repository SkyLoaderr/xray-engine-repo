//----------------------------------------------------
// file: ELight.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ELight.h"
#include "escenelighttools.h"
#include "LightAnimLibrary.h"
#include "PropertiesListHelper.h"
#include "ui_main.h"

//----------------------------------------------------
void __fastcall	CLight::OnAutoClick(PropValue* value, bool& bModif, bool& bSafe)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
	switch(B->btn_num){
    case 0:{
        float P = 0.1f;
		m_Attenuation0 = 1.f;
        m_Attenuation1 = (m_Brightness-P-P*m_Range*m_Range*m_Attenuation2)/(P*m_Range);
        clamp(m_Attenuation1,0.f,1.f);
    }break;
	case 1:{
        float P = 0.1f;
        m_Attenuation0 = 1.f;
        m_Attenuation2 = (m_Brightness-P-P*m_Range*m_Attenuation1)/(P*m_Range*m_Range);
        clamp(m_Attenuation2,0.f,1.f);
    }
    }
    bModif = true;
	OnPointDataChange(value);
}

void __fastcall	CLight::OnFuzzyGenerateClick(PropValue* value, bool& bModif, bool& bSafe)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
	switch(B->btn_num){
    case 0:{
    	OnFuzzyDataChange(value);
    }break;
    }
    bModif = true;
}

void __fastcall	CLight::OnFuzzyFlagChange(PropValue* value)
{
	if (m_Flags.is(ELight::flPointFuzzy)){ 
    	m_FuzzyData		= xr_new<SFuzzyData>();
	    OnFuzzyTypeChange	(value);
    }else{
    	xr_delete		(m_FuzzyData);
        OnTypeChange 	(value);
    }
}

void __fastcall	CLight::OnFuzzyTypeChange(PropValue* value)
{
	OnTypeChange		(value);
    OnFuzzyDataChange   (value);
}

void __fastcall	CLight::OnFuzzyDataChange(PropValue* value)
{
    VERIFY(m_FuzzyData);
	m_FuzzyData->m_Positions.resize	(m_FuzzyData->m_PointCount);
    for (FvectorIt it=m_FuzzyData->m_Positions.begin(); it!=m_FuzzyData->m_Positions.end(); it++)
    	m_FuzzyData->Generate(*it);
}

#define X_GRID 14
#define Y_GRID 6

void __fastcall CLight::OnAttenuationDraw(PropValue* sender, TCanvas* canvas, const TRect& rect)
{
//	canvas
    int w = rect.Width();
    int h = rect.Height();
    int x0= rect.left;
    int y0= rect.top;

    canvas->Brush->Color = clBlack;
    canvas->FillRect(rect);
    canvas->Pen->Color = TColor(0x00006600);
    canvas->MoveTo(x0,y0);
    for (int i=0; i<X_GRID+1; i++){
        canvas->LineTo(x0+i*w/X_GRID,y0+h);
        canvas->MoveTo(x0+(i+1)*w/X_GRID,y0+0);
    }
    canvas->MoveTo(x0+0,y0+0);
    for (int j=0; j<Y_GRID+1; j++){
        canvas->LineTo(x0+w,y0+j*h/Y_GRID);
        canvas->MoveTo(x0+0,y0+(j+1)*h/Y_GRID);
    }
    canvas->Pen->Color = clYellow;
    canvas->MoveTo(x0+0,y0+h/2);
    canvas->LineTo(x0+w,y0+h/2);

    float d_cost = m_Range/w;
    AnsiString temp;
//    float v = m_D3D.range;
//    temp.sprintf("Range = %.2f",v); lbRange->Caption = temp;
    canvas->Pen->Color = clLime;
    if (!(fis_zero(m_Attenuation0)&&fis_zero(m_Attenuation1)&&fis_zero(m_Attenuation2))){
        for (int d=1; d<w; d++){
            float R = d*d_cost;
            float b = m_Brightness/(m_Attenuation0+m_Attenuation1*R+m_Attenuation2*R*R);
            float bb = h-((h/(/*br_max*/3.f*2))*b + h/2);
            int y = iFloor(y0+bb); clamp(y,int(rect.Top),int(rect.Bottom));
            if (1==d)	canvas->MoveTo(x0+d,y);
            else		canvas->LineTo(x0+d,y);
        }
    }
}

void __fastcall	CLight::OnPointDataChange(PropValue* value)
{
	UI->Command(COMMAND_UPDATE_PROPERTIES);
}

bool __fastcall	CLight::OnPointDataTestEqual(PropValue* a, PropValue* b)
{
	CLight* A = (CLight*)(a->tag); VERIFY(A);
	CLight* B = (CLight*)(b->tag); VERIFY(B);
	return (fsimilar(A->m_Range,B->m_Range)&&
    		fsimilar(A->m_Attenuation0,B->m_Attenuation0)&&
    		fsimilar(A->m_Attenuation1,B->m_Attenuation1)&&
    		fsimilar(A->m_Attenuation2,B->m_Attenuation2)&&
    		fsimilar(A->m_Brightness,B->m_Brightness));
}

void CLight::FillAttProp(LPCSTR pref, PropItemVec& items)
{
	PropValue* V;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref, "Range"),					&m_Range,		0.1f,1000.f);
    V->OnChangeEvent 		= OnPointDataChange;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref, "Attenuation\\Constant"),	&m_Attenuation0,0.f,1.f,0.0001f,6);
    V->OnChangeEvent 		= OnPointDataChange;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref, "Attenuation\\Linear"),	&m_Attenuation1,0.f,1.f,0.0001f,6);
    V->OnChangeEvent 		= OnPointDataChange;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref, "Attenuation\\Quadratic"),	&m_Attenuation2,0.f,1.f,0.0001f,6);
    V->OnChangeEvent 		= OnPointDataChange;
	ButtonValue* B=0;
    B=PHelper.CreateButton	(items,	FHelper.PrepareKey(pref, "Attenuation\\Auto"),"Linear,Quadratic",0);
    B->OnBtnClickEvent		= OnAutoClick;
	CanvasValue* C=0;
    C=PHelper.CreateCanvas	(items,	FHelper.PrepareKey(pref, "Attenuation\\Graphic"),	"", 64);
    C->tag					= this;
    C->OnDrawCanvasEvent 	= OnAttenuationDraw;
    C->OnTestEqual			= OnPointDataTestEqual;
}

xr_token fuzzy_shape_types[]={
	{ "Sphere",			CLight::SFuzzyData::fstSphere	},
	{ "Box",			CLight::SFuzzyData::fstBox		},
	{ 0,				0				}
};
void CLight::FillPointProp(LPCSTR pref, PropItemVec& items)
{
	// flags
    PHelper.CreateFlag<Flags32>(items,	FHelper.PrepareKey(pref,"Usage\\LightMap"),	&m_Flags,	ELight::flAffectStatic);
    PHelper.CreateFlag<Flags32>(items,	FHelper.PrepareKey(pref,"Usage\\Dynamic"),	&m_Flags,	ELight::flAffectDynamic);
    PHelper.CreateFlag<Flags32>(items,	FHelper.PrepareKey(pref,"Usage\\Animated"),	&m_Flags,	ELight::flProcedural);
//    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Flags\\Breakable"),&m_Flags,	CLight::flBreaking);

	FillAttProp			(pref,items);
    PropValue* 			P=0;
    ButtonValue* 		B=0;
    P=PHelper.CreateFlag<Flags32>(items,	FHelper.PrepareKey(pref, "Fuzzy"),				&m_Flags,	ELight::flPointFuzzy);
    P->OnChangeEvent		= OnFuzzyFlagChange;
	if (m_Flags.is(ELight::flPointFuzzy)){
        VERIFY				(m_FuzzyData);
        P=PHelper.CreateS16		(items,	FHelper.PrepareKey(pref, "Fuzzy\\Count"),			&m_FuzzyData->m_PointCount,0,100);
        P->OnChangeEvent		= OnFuzzyDataChange;
	    B=PHelper.CreateButton	(items,	FHelper.PrepareKey(pref, "Fuzzy\\Generate"),"Random",0);
    	B->OnBtnClickEvent		= OnFuzzyGenerateClick;
        P=PHelper.CreateToken<u8>(items,	FHelper.PrepareKey(pref, "Fuzzy\\Shape"),		(u8*)&m_FuzzyData->m_ShapeType,	fuzzy_shape_types);
        P->OnChangeEvent		= OnFuzzyTypeChange;
        switch (m_FuzzyData->m_ShapeType){
        case CLight::SFuzzyData::fstSphere: 
            P=PHelper.CreateFloat(items,	FHelper.PrepareKey(pref, "Fuzzy\\Radius"),		&m_FuzzyData->m_SphereRadius,0.01f,100.f,0.01f,2);
            P->OnChangeEvent	= OnFuzzyDataChange;
        break;
        case CLight::SFuzzyData::fstBox: 
            P=PHelper.CreateVector(items,	FHelper.PrepareKey(pref, "Fuzzy\\Half Dimension"),&m_FuzzyData->m_BoxDimension,0.01f,100.f,0.01f,2);
            P->OnChangeEvent	= OnFuzzyDataChange;
        break;
        }
    }
}
//----------------------------------------------------

void CLight::FillSpotProp(LPCSTR pref, PropItemVec& items)
{
	// flags
    PHelper.CreateFlag<Flags32>(items,	FHelper.PrepareKey(pref,"Usage\\LightMap"),	&m_Flags,	ELight::flAffectStatic);
    PHelper.CreateFlag<Flags32>(items,	FHelper.PrepareKey(pref,"Usage\\Dynamic"),	&m_Flags,	ELight::flAffectDynamic);
    PHelper.CreateFlag<Flags32>(items,	FHelper.PrepareKey(pref,"Usage\\Animated"),	&m_Flags,	ELight::flProcedural);
//    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Flags\\Breakable"),&m_Flags,	CLight::flBreaking);

	FillAttProp				(pref,items);
	PHelper.CreateAngle		(items,	FHelper.PrepareKey(pref, "Spot R1\\Cone Angle"),	&m_Cone,		0.1f,deg2rad(120),0.01f,2);
	PHelper.CreateChoose	(items,	FHelper.PrepareKey(pref, "Spot R1\\Texture"),		&m_FalloffTex, 	smTexture);
}
//----------------------------------------------------

xr_token			token_light_type[ ]	=	{
    { "Point",		ELight::ltPoint			},
    { "Spot",		ELight::ltSpot			},
    { 0,			0	  					}
};

void CLight::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref,items);

    PropValue* V=0;

    V=PHelper.CreateToken<u32>(items,	FHelper.PrepareKey(pref,"Type"),		(u32*)&m_Type,token_light_type);
    V->OnChangeEvent		= OnTypeChange;
    V=PHelper.CreateFColor	(items,	FHelper.PrepareKey(pref,"Color"),			&m_Color);
	V->OnChangeEvent		= OnNeedUpdate;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref,"Brightness"),		&m_Brightness,-3.f,3.f,0.1f,2);
    V->OnChangeEvent 		= OnPointDataChange;

    ESceneLightTools* lt	= dynamic_cast<ESceneLightTools*>(ParentTools); VERIFY(lt);
	PHelper.CreateAToken<u32>(items,FHelper.PrepareKey(pref,"Light Control"),	&m_LControl, &lt->lcontrols);

    switch(m_Type){
    case ELight::ltPoint:	FillPointProp	(pref, items);	break;
    case ELight::ltSpot: 	FillSpotProp 	(pref, items);	break;
    default: THROW;
    }
    PHelper.CreateBOOL		(items,	FHelper.PrepareKey(pref,"Use In D3D"),		&m_UseInD3D);
}

void __fastcall	CLight::OnTypeChange(PropValue* value)
{
	UI->Command		(COMMAND_UPDATE_PROPERTIES);

	Update			();
}
//----------------------------------------------------

void CLight::OnShowHint(AStringVec& dest){
    CCustomObject::OnShowHint(dest);
    AnsiString temp;
    temp.sprintf("Type:  ");
    switch(m_Type){
    case ELight::ltPoint:	        temp+="point"; break;
    case ELight::ltSpot:			temp+="spot"; break;
    default: temp+="undef";
    }
    dest.push_back(temp);
    temp = "Flags: ";
    if (m_Flags.is(ELight::flAffectStatic))  	temp+="Stat ";
    if (m_Flags.is(ELight::flAffectDynamic)) 	temp+="Dyn ";
    if (m_Flags.is(ELight::flProcedural))		temp+="Proc ";
    dest.push_back(temp);
    temp.sprintf("Pos:   %3.2f, %3.2f, %3.2f",PPosition.x,PPosition.y,PPosition.z);
    dest.push_back(temp);
}


