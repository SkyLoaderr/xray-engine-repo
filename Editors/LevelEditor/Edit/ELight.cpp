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
#include "PropertiesListHelper.h"

#define LIGHT_VERSION   				0x0011
//----------------------------------------------------
#define LIGHT_CHUNK_VERSION				0xB411
#define LIGHT_CHUNK_FLAG				0xB413
#define LIGHT_CHUNK_BRIGHTNESS			0xB425
#define LIGHT_CHUNK_D3D_PARAMS         	0xB435
#define LIGHT_CHUNK_USE_IN_D3D			0xB436
#define LIGHT_CHUNK_ROTATE				0xB437
#define LIGHT_CHUNK_ANIMREF				0xB438
#define LIGHT_CHUNK_SPOT_TEXTURE		0xB439
#define LIGHT_CHUNK_FUZZY_DATA			0xB440
//----------------------------------------------------
//----------------------------------------------------
#define FLARE_CHUNK_FLAG				0x1002
#define FLARE_CHUNK_SOURCE				0x1010
#define FLARE_CHUNK_GRADIENT			0x1011
#define FLARE_CHUNK_FLARES2				0x1013
#define FLARE_CHUNK_GRADIENT2			0x1014
//----------------------------------------------------

#define VIS_RADIUS 		0.25f
#define DIR_SELRANGE 	250
#define DIR_RANGE 		1.5
#define SEL_COLOR 		0x00FFFFFF
#define NORM_COLOR 		0x00FFFF00
#define NORM_DYN_COLOR 	0x0000FF00
#define LOCK_COLOR 		0x00FF0000

CLight::CLight(LPVOID data, LPCSTR name):CCustomObject(data,name){
	Construct(data);
}

void CLight::Construct(LPVOID data){
	ClassID 		= OBJCLASS_LIGHT;

    m_UseInD3D		= TRUE;

    ZeroMemory		(&m_D3D,sizeof(m_D3D));

    m_D3D.type 		= D3DLIGHT_POINT;
	m_D3D.diffuse.set(1.f,1.f,1.f,0);
	m_D3D.attenuation0 = 1.f;
	m_D3D.range 	= 8.f;
    m_D3D.phi		= PI_DIV_8;

    m_Brightness 	= 1;

	m_D3DIndex 		= -1;
    m_Enabled 		= TRUE;

    m_pAnimRef		= 0;

    m_Flags.set		(flAffectStatic);
}

CLight::~CLight(){
}

void CLight::OnUpdateTransform(){
    FScale.set(1.f,1.f,1.f);
	inherited::OnUpdateTransform();
	m_D3D.direction.setHP(PRotation.y,PRotation.x);
	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Update(m_D3D.direction, m_D3D.diffuse);
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

bool CLight::GetBox( Fbox& box ){
	if( m_D3D.type==D3DLIGHT_DIRECTIONAL){
		box.set( m_D3D.position, m_D3D.position );
        box.grow(VIS_RADIUS);
    	return true;
    }
	box.set( m_D3D.position, m_D3D.position );
	box.min.sub(m_D3D.range);
	box.max.add(m_D3D.range);
	return true;
}

void CLight::Render(int priority, bool strictB2F)
{
	inherited::Render(priority,strictB2F);
    if ((1==priority)&&(false==strictB2F)){
    	u32 clr = Locked()?LOCK_COLOR:(Selected()?SEL_COLOR:(m_Flags.is(flAffectDynamic)?NORM_DYN_COLOR:NORM_COLOR));
    	switch (m_D3D.type){
        case D3DLIGHT_POINT:
            if (Selected()) DU.DrawLineSphere( m_D3D.position, m_D3D.range, clr, true );
            DU.DrawPointLight(m_D3D.position,VIS_RADIUS, clr);
            if (m_Flags.is(CLight::flPointFuzzy)){
			    for (FvectorIt it=m_FuzzyData.m_Positions.begin(); it!=m_FuzzyData.m_Positions.end(); it++){
                	Fvector tmp; _Transform().transform_tiny(tmp,*it);
		            DU.DrawPointLight(tmp,VIS_RADIUS/2, clr);
	            }
			}
        break;
        case D3DLIGHT_DIRECTIONAL:
            if (Selected()) DU.DrawDirectionalLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, DIR_SELRANGE, clr );
            else			DU.DrawDirectionalLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, DIR_RANGE, clr );
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
    }else if ((1==priority)&&(true==strictB2F)){
    	switch (m_D3D.type){
        case D3DLIGHT_POINT:
            if (m_Flags.is(CLight::flPointFuzzy)){
		    	u32 clr = Locked()?LOCK_COLOR:(Selected()?SEL_COLOR:(m_Flags.is(flAffectDynamic)?NORM_DYN_COLOR:NORM_COLOR));
                clr 	= subst_alpha(clr,0x40);
            	const Fvector zero={0.f,0.f,0.f};
                switch (m_FuzzyData.m_ShapeType){
                case CLight::SFuzzyData::fstSphere: 
                	DU.DrawSphere	(_Transform(),zero,m_FuzzyData.m_SphereRadius,clr);
                break;
                case CLight::SFuzzyData::fstBox:
                	DU.DrawAABB		(_Transform(),zero,m_FuzzyData.m_BoxDimension,clr);
                break;
                }
			}
        break;
        case D3DLIGHT_DIRECTIONAL:        break;
        case D3DLIGHT_SPOT:               break;
        default: THROW;
        }
	}else if ((3==priority)&&(true==strictB2F)){
		if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Render();
    }
}

void CLight::Enable( BOOL flag )
{
    if (m_UseInD3D){
		Device.LightEnable(m_D3DIndex,flag);
    	m_Enabled = flag;
    }
}

void CLight::Set( int d3dindex ){
	VERIFY( d3dindex>=0 );
    m_D3DIndex = d3dindex;
    Flight L=m_D3D;
    L.diffuse.mul_rgb(m_Brightness);
    L.ambient.mul_rgb(m_Brightness);
    L.specular.mul_rgb(m_Brightness);
    Device.SetLight(m_D3DIndex,L);
}

void CLight::UnSet(){
	VERIFY( m_D3DIndex>=0 );
	Device.LightEnable(m_D3DIndex,FALSE);
}

bool CLight::FrustumPick(const CFrustum& frustum){
//    return (frustum.testSphere(m_Position,m_Range))?true:false;
    return (frustum.testSphere_dirty(m_D3D.position,VIS_RADIUS))?true:false;
}

bool CLight::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf){
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

void CLight::Update(){
	UpdateTransform();
	if (D3DLIGHT_DIRECTIONAL==m_D3D.type){
    	m_LensFlare.Update(m_D3D.direction, m_D3D.diffuse);
	    m_LensFlare.DeleteShaders();
    	m_LensFlare.CreateShaders();
    }
}
//----------------------------------------------------

void CLight::OnShowHint(AStringVec& dest){
    CCustomObject::OnShowHint(dest);
    AnsiString temp;
    temp.sprintf("Type:  ");
    switch(m_D3D.type){
    case D3DLIGHT_DIRECTIONAL:  temp+="direct"; break;
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

bool CLight::Load(IReader& F){
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
	    	if (D3DLIGHT_DIRECTIONAL==m_D3D.type){
		    	// generate from direction
        	    Fvector& dir= m_D3D.direction;
                dir.getHP(FRotation.y,FRotation.x);
            }else{
	        	FRotation.set(0,0,0);
            }
        }
    }

    if (F.find_chunk(LIGHT_CHUNK_FLAG)) F.r(&m_Flags.flags,sizeof(m_Flags));

	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Load(F);

    if (F.find_chunk(LIGHT_CHUNK_ANIMREF)){
    	F.r_stringZ(buf);
        m_pAnimRef	= LALib.FindItem(buf);
        if (!m_pAnimRef) ELog.Msg(mtError, "Can't find light animation: %s",buf);
    }

    if (F.find_chunk(LIGHT_CHUNK_SPOT_TEXTURE)){
    	F.r_stringZ(buf);	m_SpotAttTex = buf;
    }

    if (F.find_chunk(LIGHT_CHUNK_FUZZY_DATA))
        m_FuzzyData.Load(F);
    
	UpdateTransform	();

    return true;
}
//----------------------------------------------------

void CLight::Save(IWriter& F){
	CCustomObject::Save(F);

	F.open_chunk	(LIGHT_CHUNK_VERSION);
	F.w_u16			(LIGHT_VERSION);
	F.close_chunk	();

	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Save(F);

	F.w_chunk		(LIGHT_CHUNK_BRIGHTNESS,&m_Brightness,sizeof(m_Brightness));
	F.w_chunk		(LIGHT_CHUNK_D3D_PARAMS,&m_D3D,sizeof(m_D3D));
    F.w_chunk		(LIGHT_CHUNK_USE_IN_D3D,&m_UseInD3D,sizeof(m_UseInD3D));
    F.w_chunk		(LIGHT_CHUNK_FLAG,&m_Flags,sizeof(m_Flags));

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
        F.open_chunk(LIGHT_CHUNK_FUZZY_DATA);
        m_FuzzyData.Save(F);
        F.close_chunk();
    }
}
//----------------------------------------------------

xr_token			token_light_type[ ]	=	{
    { "Sun",		D3DLIGHT_DIRECTIONAL	},
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
    PHelper.CreateFloat		(items,	FHelper.PrepareKey(pref,"Brightness"),		&m_Brightness,-3.f,3.f,0.1f,2);
    PHelper.CreateBOOL		(items,	FHelper.PrepareKey(pref,"Use In D3D"),		&m_UseInD3D);
    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Usage\\LightMap"),	&m_Flags,	CLight::flAffectStatic);
    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Usage\\Dynamic"),	&m_Flags,	CLight::flAffectDynamic);
    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Usage\\Animated"),	&m_Flags,	CLight::flProcedural);
    PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref,"Flags\\Breakable"),&m_Flags,	CLight::flBreaking);

    switch(m_D3D.type){
    case D3DLIGHT_DIRECTIONAL: 	FillSunProp		(pref, items);	break;
    case D3DLIGHT_POINT: 		FillPointProp	(pref, items);	break;
    case D3DLIGHT_SPOT: 		FillSpotProp 	(pref, items);	break;
    }
}
//----------------------------------------------------

void __fastcall	CLight::OnNeedUpdate(PropValue* value)
{
	Update();
}
//----------------------------------------------------

void CLight::FillSunProp(LPCSTR pref, PropItemVec& items)
{
	CEditFlare& F 			= m_LensFlare;
    PropValue* prop			= 0;
    PHelper.CreateFlag32	(items, FHelper.PrepareKey(pref,"Sun\\Source\\Enabled"),		&F.m_Flags,				CEditFlare::flSource);
    PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Sun\\Source\\Radius"),			&F.m_Source.fRadius,	0.f,10.f);
    prop 					= PHelper.CreateTexture	(items, FHelper.PrepareKey(pref,"Sun\\Source\\Texture"),		F.m_Source.texture,		sizeof(F.m_Source.texture));
	prop->OnChangeEvent		= OnNeedUpdate;

    PHelper.CreateFlag32	(items, FHelper.PrepareKey(pref,"Sun\\Gradient\\Enabled"),		&F.m_Flags,				CEditFlare::flGradient);
    PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Sun\\Gradient\\Radius"),		&F.m_Gradient.fRadius,	0.f,100.f);
    PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Sun\\Gradient\\Opacity"),		&F.m_Gradient.fOpacity,	0.f,1.f);
	prop					= PHelper.CreateTexture	(items, FHelper.PrepareKey(pref,"Sun\\Gradient\\Texture"),	F.m_Gradient.texture,	sizeof(F.m_Gradient.texture));
	prop->OnChangeEvent		= OnNeedUpdate;

    PHelper.CreateFlag32	(items, FHelper.PrepareKey(pref,"Sun\\Flares\\Enabled"),		&F.m_Flags,				CEditFlare::flFlare);
	for (CEditFlare::FlareIt it=F.m_Flares.begin(); it!=F.m_Flares.end(); it++){
		AnsiString nm; nm.sprintf("%s\\Sun\\Flares\\Flare %d",pref,it-F.m_Flares.begin());
		PHelper.CreateFloat	(items, FHelper.PrepareKey(nm.c_str(),"Radius"), 	&it->fRadius,  	0.f,10.f);
        PHelper.CreateFloat	(items, FHelper.PrepareKey(nm.c_str(),"Opacity"),	&it->fOpacity,	0.f,1.f);
        PHelper.CreateFloat	(items, FHelper.PrepareKey(nm.c_str(),"Position"),	&it->fPosition,	-10.f,10.f);
        prop				= PHelper.CreateTexture	(items, FHelper.PrepareKey(nm.c_str(),"Texture"),	it->texture,	sizeof(it->texture));
        prop->OnChangeEvent	= OnNeedUpdate;
	}
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

void __fastcall	CLight::OnFuzzyTypeChange(PropValue* value)
{
	OnTypeChange		(value);
    OnFuzzyDataChange   (value);
}

void __fastcall	CLight::OnFuzzyDataChange(PropValue* value)
{
	m_FuzzyData.m_Positions.resize	(m_FuzzyData.m_PointCount);
    for (FvectorIt it=m_FuzzyData.m_Positions.begin(); it!=m_FuzzyData.m_Positions.end(); it++)
    	m_FuzzyData.Generate(*it);
}

xr_token fuzzy_shape_types[]={
	{ "Sphere",			CLight::SFuzzyData::fstSphere	},
	{ "Box",			CLight::SFuzzyData::fstBox		},
	{ 0,				0				}
};
void CLight::FillPointProp(LPCSTR pref, PropItemVec& items)
{
    PHelper.CreateFloat		(items,	FHelper.PrepareKey(pref, "Point\\Range"),					&m_D3D.range,		0.1f,1000.f);
    PHelper.CreateFloat		(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Constant"),	&m_D3D.attenuation0,0.f,1.f,0.0001f,6);
    PHelper.CreateFloat		(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Linear"),		&m_D3D.attenuation1,0.f,1.f,0.0001f,6);
    PHelper.CreateFloat		(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Quadratic"),	&m_D3D.attenuation2,0.f,1.f,0.0001f,6);
	ButtonValue* B=0;
    B=PHelper.CreateButton	(items,	FHelper.PrepareKey(pref, "Point\\Attenuation\\Auto"),"Linear,Quadratic");
    B->OnBtnClickEvent		= OnAutoClick;
    PropValue* P=0;
    P=PHelper.CreateFlag32	(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy"),					&m_Flags,	CLight::flPointFuzzy);
    P->OnChangeEvent		= OnFuzzyTypeChange;           
	if (m_Flags.is(CLight::flPointFuzzy)){
        P=PHelper.CreateS16		(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Count"),			&m_FuzzyData.m_PointCount,0,100);
        P->OnChangeEvent		= OnFuzzyDataChange;
	    B=PHelper.CreateButton	(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Generate"),"Random");
    	B->OnBtnClickEvent		= OnFuzzyGenerateClick;
        P=PHelper.CreateToken	(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Shape"),				&m_FuzzyData.m_ShapeType,	fuzzy_shape_types, 1);
        P->OnChangeEvent		= OnFuzzyTypeChange;
        switch (m_FuzzyData.m_ShapeType){
        case CLight::SFuzzyData::fstSphere: 
            P=PHelper.CreateFloat(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Radius"),		&m_FuzzyData.m_SphereRadius,0.01f,100.f,0.01f,2);
            P->OnChangeEvent	= OnFuzzyDataChange;
        break;
        case CLight::SFuzzyData::fstBox: 
            P=PHelper.CreateVector(items,	FHelper.PrepareKey(pref, "Point\\Fuzzy\\Half Dimension"),&m_FuzzyData.m_BoxDimension,0.01f,100.f,0.01f,2);
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

void CLight::OnDeviceCreate()
{
	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.DDLoad();
}
//----------------------------------------------------

void CLight::OnDeviceDestroy()
{
//	if (D3DLIGHT_DIRECTIONAL==m_D3D.type)
    m_LensFlare.DDUnload();
}
//----------------------------------------------------

bool CLight::GetSummaryInfo(SSceneSummary* inf)
{
    switch (m_D3D.type){
    case D3DLIGHT_DIRECTIONAL:	inf->light_sun_cnt++; 	break;
    case D3DLIGHT_POINT:		inf->light_point_cnt++; break;
    case D3DLIGHT_SPOT:			inf->light_spot_cnt++; 	break;
    }

    if (m_Flags.is(flAffectStatic))		inf->light_static_cnt++;
    if (m_Flags.is(flAffectDynamic))	inf->light_dynamic_cnt++;
    if (m_Flags.is(flProcedural))		inf->light_procedural_cnt++;
    if (m_Flags.is(flBreaking))			inf->light_breakable_cnt++;
	return true;
}

//----------------------------------------------------
// Edit Flare
//----------------------------------------------------
CEditFlare::CEditFlare()
{
    m_Flags.set(flFlare|flSource|flGradient,TRUE);
	// flares
    m_Flares.resize		(6);
    FlareIt it=m_Flares.begin();
	it->fRadius=0.08f; it->fOpacity=0.18f; it->fPosition=1.3f; strcpy(it->texture,"fx\\fx_flare1"); it++;
	it->fRadius=0.12f; it->fOpacity=0.12f; it->fPosition=1.0f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.04f; it->fOpacity=0.30f; it->fPosition=0.5f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.08f; it->fOpacity=0.24f; it->fPosition=-0.3f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.12f; it->fOpacity=0.12f; it->fPosition=-0.6f; strcpy(it->texture,"fx\\fx_flare3"); it++;
	it->fRadius=0.30f; it->fOpacity=0.12f; it->fPosition=-1.0f; strcpy(it->texture,"fx\\fx_flare1"); it++;
	// source
    strcpy(m_Source.texture,"fx\\fx_sun");
    m_Source.fRadius 	= 0.15f;
    // gradient
    strcpy(m_Gradient.texture,"fx\\fx_gradient");
    m_Gradient.fOpacity = 0.9f;
    m_Gradient.fRadius 	= 4.f;
}

void CEditFlare::Load(IReader& F){
	if (!F.find_chunk(FLARE_CHUNK_FLAG)) return;

    R_ASSERT(F.find_chunk(FLARE_CHUNK_FLAG));
    F.r				(&m_Flags.flags,sizeof(m_Flags));

    R_ASSERT(F.find_chunk(FLARE_CHUNK_SOURCE));
    F.r_stringZ		(m_Source.texture);
    m_Source.fRadius= F.r_float();

    if (F.find_chunk(FLARE_CHUNK_GRADIENT2)){
	    F.r_stringZ	(m_Gradient.texture);
	    m_Gradient.fOpacity = F.r_float();
	    m_Gradient.fRadius  = F.r_float();
    }else{
		R_ASSERT(F.find_chunk(FLARE_CHUNK_GRADIENT));
	    m_Gradient.fOpacity = F.r_float();
    }

    // flares
    if (F.find_chunk(FLARE_CHUNK_FLARES2)){
	    DeleteShaders();
	    u32 deFCnt	= F.r_u32(); VERIFY(deFCnt==6);
	   	F.r				(m_Flares.begin(),m_Flares.size()*sizeof(SFlare));
    	for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++) it->hShader._object=0;
    	CreateShaders();
    }
}
//----------------------------------------------------

void CEditFlare::Save(IWriter& F)
{
	F.open_chunk	(FLARE_CHUNK_FLAG);
    F.w				(&m_Flags.flags,sizeof(m_Flags));
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_SOURCE);
    F.w_stringZ		(m_Source.texture);
    F.w_float		(m_Source.fRadius);
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_GRADIENT2);
    F.w_stringZ		(m_Gradient.texture);
    F.w_float		(m_Gradient.fOpacity);
    F.w_float		(m_Gradient.fRadius);
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_FLARES2);
    F.w_u32			(m_Flares.size());
    F.w				(m_Flares.begin(),m_Flares.size()*sizeof(SFlare));
	F.close_chunk	();
}
//----------------------------------------------------

void CEditFlare::Render()
{
	CLensFlare::Render(m_Flags.is(flSource),m_Flags.is(flFlare),m_Flags.is(flGradient));
}
//----------------------------------------------------

void CEditFlare::DeleteShaders()
{
    CLensFlare::DDUnload();
}

void CEditFlare::CreateShaders()
{
    CLensFlare::DDLoad();
}
//----------------------------------------------------


