//----------------------------------------------------
// file: ELight.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "ELight.h"
#include "BottomBar.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "LightAnimLibrary.h"
#include "scene.h"
#include "escenelighttools.h"
#include "PropertiesListHelper.h"

static const u32 LIGHT_VERSION   			= 0x0011;
//----------------------------------------------------
enum{
    LIGHT_CHUNK_VERSION			= 0xB411ul,
    LIGHT_CHUNK_FLAG			= 0xB413ul,
    LIGHT_CHUNK_BRIGHTNESS		= 0xB425ul,
    LIGHT_CHUNK_D3D_PARAMS     	= 0xB435ul,
    LIGHT_CHUNK_USE_IN_D3D		= 0xB436ul,
    LIGHT_CHUNK_ROTATE			= 0xB437ul,
    LIGHT_CHUNK_ANIMREF			= 0xB438ul,
    LIGHT_CHUNK_SPOT_TEXTURE	= 0xB439ul,
    LIGHT_CHUNK_FUZZY_DATA		= 0xB440ul,
    LIGHT_CHUNK_LCONTROL		= 0xB441ul,
};
//----------------------------------------------------

#define VIS_RADIUS 		0.25f
#define SEL_COLOR 		0x00FFFFFF
#define NORM_COLOR 		0x00FFFF00
#define NORM_DYN_COLOR 	0x0000FF00
#define LOCK_COLOR 		0x00FF0000

CLight::CLight(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct(data);
}

void CLight::Construct(LPVOID data)
{
	ClassID 		= OBJCLASS_LIGHT;

    m_UseInD3D		= TRUE;

    ZeroMemory		(&m_D3D,sizeof(m_D3D));

    m_FuzzyData		= 0;
    
    m_D3D.type 		= D3DLIGHT_POINT;
	m_D3D.diffuse.set(1.f,1.f,1.f,0);
	m_D3D.attenuation0 = 1.f;
	m_D3D.range 	= 8.f;
    m_D3D.phi		= PI_DIV_8;

    m_Brightness 	= 1;
    m_SunQuality	= 1;

    m_pAnimRef		= 0;
    m_LControl		= 0;

    m_Flags.set		(flAffectStatic);
}

CLight::~CLight(){
}

void CLight::OnUpdateTransform()
{
    FScale.set(1.f,1.f,1.f);
	inherited::OnUpdateTransform();
}

void CLight::CopyFrom(CLight* src)
{
	THROW2("CLight:: Go to AlexMX");
}

void CLight::AffectD3D(BOOL flag){
	m_UseInD3D = flag;
    UI.UpdateScene();
}
//----------------------------------------------------

bool CLight::GetBox( Fbox& box )
{
	box.set( m_D3D.position, m_D3D.position );
	box.min.sub(m_D3D.range);
	box.max.add(m_D3D.range);
	return true;
}

void CLight::Render(int priority, bool strictB2F)
{
	inherited::Render(priority,strictB2F);
    if ((1==priority)&&(false==strictB2F)){
        Device.SetShader		(Device.m_WireShader);
        RCache.set_xform_world	(Fidentity);
    	u32 clr = Locked()?LOCK_COLOR:(Selected()?SEL_COLOR:(m_Flags.is(flAffectDynamic)?NORM_DYN_COLOR:NORM_COLOR));
    	switch (m_D3D.type){
        case D3DLIGHT_POINT:
            if (Selected()) DU.DrawLineSphere( m_D3D.position, m_D3D.range, clr, true );
            DU.DrawPointLight(m_D3D.position,VIS_RADIUS, clr);
            if (m_Flags.is(CLight::flPointFuzzy)){
            	VERIFY(m_FuzzyData);
			    for (FvectorIt it=m_FuzzyData->m_Positions.begin(); it!=m_FuzzyData->m_Positions.end(); it++){
                	Fvector tmp; _Transform().transform_tiny(tmp,*it);
		            DU.DrawPointLight(tmp,VIS_RADIUS/6, clr);
	            }
			}
        break;
        case D3DLIGHT_SPOT:{
//        	Fmatrix m;
//            m.translate(m_D3D.position);
//		 	RCache.set_xform_world(m);
//        	DU.DrawIdentCone(clr);
        	if (Selected())	DU.DrawSpotLight( m_D3D.position, m_D3D.direction, m_D3D.range, m_D3D.phi, clr );
            else			DU.DrawSpotLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, m_D3D.phi, clr );
        }break;
        default: THROW;
        }
    	ESceneLightTools* lt = dynamic_cast<ESceneLightTools*>(ParentTools); VERIFY(lt);
        if (lt->m_Flags.is(ESceneLightTools::flShowControlName)){ 
            Fvector D;	D.sub(Device.vCameraPosition,PPosition);
            float dist 	= D.normalize_magn();
        	if (!Scene.RayPickObject(dist,PPosition,D,OBJCLASS_SCENEOBJECT,0,0))
	        	DU.DrawText (PPosition,AnsiString().sprintf(" %s",GetLControlName()).c_str(),0xffffffff,0xff000000);
        }
    }else if ((1==priority)&&(true==strictB2F)){
        Device.SetShader		(Device.m_SelectionShader);
        RCache.set_xform_world	(Fidentity);
    	switch (m_D3D.type){
        case D3DLIGHT_POINT:
            if (m_Flags.is(CLight::flPointFuzzy)){
		    	u32 clr = Locked()?LOCK_COLOR:(Selected()?SEL_COLOR:(m_Flags.is(flAffectDynamic)?NORM_DYN_COLOR:NORM_COLOR));
                clr 	= subst_alpha(clr,0x40);
            	const Fvector zero={0.f,0.f,0.f};
                VERIFY(m_FuzzyData);
                switch (m_FuzzyData->m_ShapeType){
                case CLight::SFuzzyData::fstSphere: 
                	DU.DrawSphere	(_Transform(),zero,m_FuzzyData->m_SphereRadius,clr);
                break;
                case CLight::SFuzzyData::fstBox:
                	DU.DrawAABB		(_Transform(),zero,m_FuzzyData->m_BoxDimension,clr);
                break;
                }
			}
        break;
        case D3DLIGHT_SPOT:               break;
        default: THROW;
        }
	}
}

bool CLight::FrustumPick(const CFrustum& frustum)
{
//    return (frustum.testSphere(m_Position,m_Range))?true:false;
    return (frustum.testSphere_dirty(m_D3D.position,VIS_RADIUS))?true:false;
}

bool CLight::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	Fvector ray2;
	ray2.sub( m_D3D.position, start );

    float d = ray2.dotproduct(direction);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (VIS_RADIUS*VIS_RADIUS)) && (d>VIS_RADIUS) ){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
        }
    }
	return false;
}
//----------------------------------------------------

void CLight::OnFrame	(){
	inherited::OnFrame	();
}
//----------------------------------------------------

void CLight::Update()
{
	UpdateTransform();
}
//----------------------------------------------------

void CLight::OnShowHint(AStringVec& dest){
    CCustomObject::OnShowHint(dest);
    AnsiString temp;
    temp.sprintf("Type:  ");
    switch(m_D3D.type){
    case D3DLIGHT_DIRECTIONAL:  THROW; break;
    case D3DLIGHT_POINT:        temp+="point"; break;
    case D3DLIGHT_SPOT:			temp+="spot"; break;
    default: temp+="undef";
    }
    dest.push_back(temp);
    temp = "Flags: ";
    if (m_Flags.is(flAffectStatic))  	temp+="Stat ";
    if (m_Flags.is(flAffectDynamic)) 	temp+="Dyn ";
    if (m_Flags.is(flProcedural))		temp+="Proc ";
    dest.push_back(temp);
    temp.sprintf("Pos:   %3.2f, %3.2f, %3.2f",m_D3D.position.x,m_D3D.position.y,m_D3D.position.z);
    dest.push_back(temp);
}

bool CLight::Load(IReader& F)
{
	u32 version = 0;

    string1024 buf;
    R_ASSERT(F.r_chunk(LIGHT_CHUNK_VERSION,&version));
    if((version!=0x0010)&&(version!=LIGHT_VERSION)){
        ELog.DlgMsg( mtError, "CLight: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);
    R_ASSERT(F.r_chunk(LIGHT_CHUNK_BRIGHTNESS,&m_Brightness));
    R_ASSERT(F.find_chunk(LIGHT_CHUNK_D3D_PARAMS));
    F.r(&m_D3D,sizeof(m_D3D));
    R_ASSERT(F.r_chunk(LIGHT_CHUNK_USE_IN_D3D,&m_UseInD3D));
    if(version==0x0010){
    	if (F.find_chunk(LIGHT_CHUNK_ROTATE)){
        	F.r_fvector3(FRotation);
        }else{
            FRotation.set(0,0,0);
        }
    }

    if (F.find_chunk(LIGHT_CHUNK_FLAG)) 	F.r(&m_Flags.flags,sizeof(m_Flags));
    if (F.find_chunk(LIGHT_CHUNK_LCONTROL))	F.r(&m_LControl,sizeof(m_LControl));

	if (D3DLIGHT_DIRECTIONAL==m_D3D.type){
    	ESceneLightTools* lt = dynamic_cast<ESceneLightTools*>(ParentTools); VERIFY(lt);
        lt->m_SunShadowDir.set(FRotation.x,FRotation.y);
        ELog.DlgMsg( mtError, "CLight: Can't load sun.");
    	return false;
    } 

    if (F.find_chunk(LIGHT_CHUNK_ANIMREF)){
    	F.r_stringZ(buf);
        m_pAnimRef	= LALib.FindItem(buf);
        if (!m_pAnimRef) ELog.Msg(mtError, "Can't find light animation: %s",buf);
    }

    if (F.find_chunk(LIGHT_CHUNK_SPOT_TEXTURE)){
    	F.r_stringZ(buf);	m_SpotAttTex = buf;
    }

    if (F.find_chunk(LIGHT_CHUNK_FUZZY_DATA)){
        m_FuzzyData	= xr_new<SFuzzyData>();
        m_FuzzyData->Load(F);
		m_Flags.set(CLight::flPointFuzzy,TRUE);
    }else{
		m_Flags.set(CLight::flPointFuzzy,FALSE);
    }
    
	UpdateTransform	();

    return true;
}
//----------------------------------------------------

void CLight::Save(IWriter& F){
	CCustomObject::Save(F);

	F.open_chunk	(LIGHT_CHUNK_VERSION);
	F.w_u16			(LIGHT_VERSION);
	F.close_chunk	();

	F.w_chunk		(LIGHT_CHUNK_BRIGHTNESS,&m_Brightness,sizeof(m_Brightness));
	F.w_chunk		(LIGHT_CHUNK_D3D_PARAMS,&m_D3D,sizeof(m_D3D));
    F.w_chunk		(LIGHT_CHUNK_USE_IN_D3D,&m_UseInD3D,sizeof(m_UseInD3D));
    F.w_chunk		(LIGHT_CHUNK_FLAG,&m_Flags,sizeof(m_Flags));
    F.w_chunk		(LIGHT_CHUNK_LCONTROL,&m_LControl,sizeof(m_LControl));

    if (m_pAnimRef){
		F.open_chunk(LIGHT_CHUNK_ANIMREF);
		F.w_stringZ	(m_pAnimRef->cName);
		F.close_chunk();
    }

    if (!m_SpotAttTex.IsEmpty()){
	    F.open_chunk(LIGHT_CHUNK_SPOT_TEXTURE);
    	F.w_stringZ	(m_SpotAttTex.c_str());
	    F.close_chunk();
    }

	if (m_Flags.is(CLight::flPointFuzzy)){
        VERIFY(m_FuzzyData);
        F.open_chunk(LIGHT_CHUNK_FUZZY_DATA);
        m_FuzzyData->Save(F);
        F.close_chunk();
    }
}
//----------------------------------------------------

xr_token			token_light_type[ ]	=	{
    { "Point",		D3DLIGHT_POINT			},
    { "Spot",		D3DLIGHT_SPOT			},
    { 0,			0						}
};

void CLight::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref,items);

    PropValue* V=0;

    V=PHelper.CreateToken	(items,	FHelper.PrepareKey(pref,"Type"),			&m_D3D.type,token_light_type,4);
    V->OnChangeEvent		= OnTypeChange;
    V=PHelper.CreateFColor	(items,	FHelper.PrepareKey(pref,"Color"),			&m_D3D.diffuse);
	V->OnChangeEvent		= OnNeedUpdate;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref,"Brightness"),		&m_Brightness,-3.f,3.f,0.1f,2);
    V->OnChangeEvent 		= OnPointDataChange;
    PHelper.CreateBOOL		(items,	FHelper.PrepareKey(pref,"Use In D3D"),		&m_UseInD3D);

    ESceneLightTools* lt	= dynamic_cast<ESceneLightTools*>(ParentTools); VERIFY(lt);
	PHelper.CreateAToken	(items,	FHelper.PrepareKey(pref,"Light Control"),	&m_LControl, &lt->lcontrols,	4);

    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Usage\\LightMap"),	&m_Flags,	CLight::flAffectStatic);
    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Usage\\Dynamic"),	&m_Flags,	CLight::flAffectDynamic);
    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Usage\\Animated"),	&m_Flags,	CLight::flProcedural);
    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Flags\\Breakable"),&m_Flags,	CLight::flBreaking);

    switch(m_D3D.type){
    case D3DLIGHT_POINT: 		FillPointProp	(pref, items);	break;
    case D3DLIGHT_SPOT: 		FillSpotProp 	(pref, items);	break;
    default: THROW;
    }
}
//----------------------------------------------------

LPCSTR CLight::GetLControlName()
{
    ESceneLightTools* lt	= dynamic_cast<ESceneLightTools*>(ParentTools); VERIFY(lt);
    xr_a_token* lc			= lt->FindLightControl(m_LControl);
	return lc?lc->name.c_str():0;
}
//----------------------------------------------------

void __fastcall	CLight::OnNeedUpdate(PropValue* value)
{
	Update();
}
//----------------------------------------------------

void __fastcall	CLight::OnAutoClick(PropValue* value, bool& bModif)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
	switch(B->btn_num){
    case 0:{
        float P = 0.1f;
		m_D3D.attenuation0 = 1.f;
        m_D3D.attenuation1 = (m_Brightness-P-P*m_D3D.range*m_D3D.range*m_D3D.attenuation2)/(P*m_D3D.range);
        clamp(m_D3D.attenuation1,0.f,1.f);
    }break;
	case 1:{
        float P = 0.1f;
        m_D3D.attenuation0 = 1.f;
        m_D3D.attenuation2 = (m_Brightness-P-P*m_D3D.range*m_D3D.attenuation1)/(P*m_D3D.range*m_D3D.range);
        clamp(m_D3D.attenuation2,0.f,1.f);
    }
    }
    bModif = true;
	OnPointDataChange(value);
}

void __fastcall	CLight::OnFuzzyGenerateClick(PropValue* value, bool& bModif)
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
	if (m_Flags.is(flPointFuzzy)){ 
    	m_FuzzyData		= xr_new<SFuzzyData>();
	    OnFuzzyTypeChange	(value);
    }else				xr_delete	(m_FuzzyData);
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

    float d_cost = m_D3D.range/w;
    AnsiString temp;
//    float v = m_D3D.range;
//    temp.sprintf("Range = %.2f",v); lbRange->Caption = temp;
    canvas->Pen->Color = clLime;
    if (!(fis_zero(m_D3D.attenuation0)&&fis_zero(m_D3D.attenuation1)&&fis_zero(m_D3D.attenuation2))){
        for (int d=1; d<w; d++){
            float R = d*d_cost;
            float b = m_Brightness/(m_D3D.attenuation0+m_D3D.attenuation1*R+m_D3D.attenuation2*R*R);
            float bb = h-((h/(/*br_max*/3.f*2))*b + h/2);
            int y = iFloor(y0+bb); clamp(y,int(rect.Top),int(rect.Bottom));
            if (1==d)	canvas->MoveTo(x0+d,y);
            else		canvas->LineTo(x0+d,y);
        }
    }
}

void __fastcall	CLight::OnPointDataChange(PropValue* value)
{
	UI.Command(COMMAND_UPDATE_PROPERTIES);
}

bool __fastcall	CLight::OnPointDataTestEqual(PropValue* a, PropValue* b)
{
	CLight* A = (CLight*)(a->tag); VERIFY(A);
	CLight* B = (CLight*)(b->tag); VERIFY(B);
	return (fsimilar(A->m_D3D.range,B->m_D3D.range)&&
    		fsimilar(A->m_D3D.attenuation0,B->m_D3D.attenuation0)&&
    		fsimilar(A->m_D3D.attenuation1,B->m_D3D.attenuation1)&&
    		fsimilar(A->m_D3D.attenuation2,B->m_D3D.attenuation2)&&
    		fsimilar(A->m_Brightness,B->m_Brightness));
}

xr_token fuzzy_shape_types[]={
	{ "Sphere",			CLight::SFuzzyData::fstSphere	},
	{ "Box",			CLight::SFuzzyData::fstBox		},
	{ 0,				0				}
};
void CLight::FillPointProp(LPCSTR pref, PropItemVec& items)
{
	PropValue* V;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref, "Point\\Range"),					&m_D3D.range,		0.1f,1000.f);
    V->OnChangeEvent 		= OnPointDataChange;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Constant"),	&m_D3D.attenuation0,0.f,1.f,0.0001f,6);
    V->OnChangeEvent 		= OnPointDataChange;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Linear"),		&m_D3D.attenuation1,0.f,1.f,0.0001f,6);
    V->OnChangeEvent 		= OnPointDataChange;
    V=PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Quadratic"),	&m_D3D.attenuation2,0.f,1.f,0.0001f,6);
    V->OnChangeEvent 		= OnPointDataChange;
	ButtonValue* B=0;
    B=PHelper.CreateButton	(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Auto"),"Linear,Quadratic",0);
    B->OnBtnClickEvent		= OnAutoClick;
	CanvasValue* C=0;
    C=PHelper.CreateCanvas	(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Graphic"),	"", 64);
    C->tag					= this;
    C->OnDrawCanvasEvent 	= OnAttenuationDraw;
    C->OnTestEqual			= OnPointDataTestEqual;
    PropValue* P=0;
    P=PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy"),					&m_Flags,	CLight::flPointFuzzy);
    P->OnChangeEvent		= OnFuzzyFlagChange;
	if (m_Flags.is(CLight::flPointFuzzy)){
        VERIFY				(m_FuzzyData);
        P=PHelper.CreateS16		(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Count"),			&m_FuzzyData->m_PointCount,0,100);
        P->OnChangeEvent		= OnFuzzyDataChange;
	    B=PHelper.CreateButton	(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Generate"),"Random",0);
    	B->OnBtnClickEvent		= OnFuzzyGenerateClick;
        P=PHelper.CreateToken	(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Shape"),			&m_FuzzyData->m_ShapeType,	fuzzy_shape_types, 1);
        P->OnChangeEvent		= OnFuzzyTypeChange;
        switch (m_FuzzyData->m_ShapeType){
        case CLight::SFuzzyData::fstSphere: 
            P=PHelper.CreateFloat(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Radius"),		&m_FuzzyData->m_SphereRadius,0.01f,100.f,0.01f,2);
            P->OnChangeEvent	= OnFuzzyDataChange;
        break;
        case CLight::SFuzzyData::fstBox: 
            P=PHelper.CreateVector(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Half Dimension"),&m_FuzzyData->m_BoxDimension,0.01f,100.f,0.01f,2);
            P->OnChangeEvent	= OnFuzzyDataChange;
        break;
        }
    }
}
//----------------------------------------------------

void CLight::FillSpotProp(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateFloat		(items,	FHelper.PrepareKey(pref, "Spot", "Range"),					&m_D3D.range,	0.1f,1000.f);
	PHelper.CreateAngle		(items,	FHelper.PrepareKey(pref, "Spot", "Cone Angle"),				&m_D3D.phi,		0.1f,120.f,0.01f,2);
	PHelper.CreateATexture	(items,	FHelper.PrepareKey(pref, "Spot", "Attenuation\\Texture"),	&m_SpotAttTex);
}
//----------------------------------------------------

void __fastcall	CLight::OnTypeChange(PropValue* value)
{
	UI.Command		(COMMAND_UPDATE_PROPERTIES);

	Update			();
}

//----------------------------------------------------

bool CLight::GetSummaryInfo(SSceneSummary* inf)
{
    switch (m_D3D.type){
    case D3DLIGHT_POINT:		inf->light_point_cnt++; break;
    case D3DLIGHT_SPOT:			inf->light_spot_cnt++; 	break;
    }

    if (m_Flags.is(flAffectStatic))		inf->light_static_cnt++;
    if (m_Flags.is(flAffectDynamic))	inf->light_dynamic_cnt++;
    if (m_Flags.is(flProcedural))		inf->light_procedural_cnt++;
    if (m_Flags.is(flBreaking))			inf->light_breakable_cnt++;
	return true;
}

