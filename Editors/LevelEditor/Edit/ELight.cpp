//----------------------------------------------------
// file: ELight.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"                                                      
#include "ELight.h"
#include "Frustum.h"
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
	THROW2("Go to AlexMX");
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

void CLight::Render(int priority, bool strictB2F){
    if ((1==priority)&&(false==strictB2F)){
    	DWORD clr;
        clr = Locked()?LOCK_COLOR:(Selected()?SEL_COLOR:(m_Flags.is(flAffectDynamic)?NORM_DYN_COLOR:NORM_COLOR));
    	switch (m_D3D.type){
        case D3DLIGHT_POINT:
            if (Selected()) DU::DrawLineSphere( m_D3D.position, m_D3D.range, clr, true );
            DU::DrawPointLight(m_D3D.position,VIS_RADIUS, clr);
        break;
        case D3DLIGHT_DIRECTIONAL:
            if (Selected()) DU::DrawDirectionalLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, DIR_SELRANGE, clr );
            else			DU::DrawDirectionalLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, DIR_RANGE, clr );
        break;
        case D3DLIGHT_SPOT:
        	if (Selected())	DU::DrawSpotLight( m_D3D.position, m_D3D.direction, m_D3D.range, m_D3D.phi, clr );
            else			DU::DrawSpotLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, m_D3D.phi, clr );
        break;
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

bool CLight::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
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

bool CLight::Load(CStream& F){
	DWORD version = 0;

    string1024 buf;
    R_ASSERT(F.ReadChunk(LIGHT_CHUNK_VERSION,&version));
    if((version!=0x0010)&&(version!=LIGHT_VERSION)){
        ELog.DlgMsg( mtError, "CLight: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);
    R_ASSERT(F.ReadChunk(LIGHT_CHUNK_BRIGHTNESS,&m_Brightness));
    R_ASSERT(F.FindChunk(LIGHT_CHUNK_D3D_PARAMS));
    F.Read(&m_D3D,sizeof(m_D3D));
    R_ASSERT(F.ReadChunk(LIGHT_CHUNK_USE_IN_D3D,&m_UseInD3D));
    if(version==0x0010){
    	if (F.FindChunk(LIGHT_CHUNK_ROTATE)){
        	F.Rvector(FRotation);
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

    if (F.FindChunk(LIGHT_CHUNK_FLAG)) F.Read(&m_Flags.flags,sizeof(m_Flags));

	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Load(F);

    if (F.FindChunk(LIGHT_CHUNK_ANIMREF)){
    	F.RstringZ(buf);
        m_pAnimRef	= LALib.FindItem(buf);
        if (!m_pAnimRef) ELog.Msg(mtError, "Can't find light animation: %s",buf);
    }

    if (F.FindChunk(LIGHT_CHUNK_SPOT_TEXTURE)){
    	F.RstringZ(buf);	m_SpotAttTex = buf;
    }    
    
	UpdateTransform	();

    return true;
}
//----------------------------------------------------

void CLight::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(LIGHT_CHUNK_VERSION);
	F.Wword			(LIGHT_VERSION);
	F.close_chunk	();

	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Save(F);

	F.write_chunk	(LIGHT_CHUNK_BRIGHTNESS,&m_Brightness,sizeof(m_Brightness));
	F.write_chunk	(LIGHT_CHUNK_D3D_PARAMS,&m_D3D,sizeof(m_D3D));
    F.write_chunk	(LIGHT_CHUNK_USE_IN_D3D,&m_UseInD3D,sizeof(m_UseInD3D));
    F.write_chunk	(LIGHT_CHUNK_FLAG,&m_Flags,sizeof(m_Flags));
    
    if (m_pAnimRef){
		F.open_chunk(LIGHT_CHUNK_ANIMREF);
		F.WstringZ	(m_pAnimRef->cName);
		F.close_chunk();
    }

    if (!m_SpotAttTex.IsEmpty()){
	    F.open_chunk(LIGHT_CHUNK_SPOT_TEXTURE);
    	F.WstringZ	(m_SpotAttTex.c_str());
	    F.close_chunk();
    }
}
//----------------------------------------------------


void CLight::FillProp(LPCSTR pref, PropItemVec& items)
{
//	inherited::FillProp(pref,items);
    PropValue* V=0;
    PHelper.CreateFColor(items,	PHelper.PrepareKey(pref,"Color"),			&m_D3D.diffuse);
    V=PHelper.CreateFloat	(items,	PHelper.PrepareKey(pref,"Brightness"),		&m_Brightness,-3.f,3.f,0.1f,2);
    V->Owner()->m_Flags.set(PropItem::flDrawBtnBorder,TRUE);
    PHelper.CreateBOOL	(items,	PHelper.PrepareKey(pref,"Use In D3D"),		&m_UseInD3D);
    PHelper.CreateFlag32(items,	PHelper.PrepareKey(pref,"Usage\\LightMap"),	&m_Flags,	CLight::flAffectStatic);
    PHelper.CreateFlag32(items,	PHelper.PrepareKey(pref,"Usage\\Dynamic"),	&m_Flags,	CLight::flAffectDynamic);
    PHelper.CreateFlag32(items,	PHelper.PrepareKey(pref,"Usage\\Animated"),	&m_Flags,	CLight::flProcedural);
    PHelper.CreateFlag32(items,	PHelper.PrepareKey(pref,"Flags\\Breakable"),&m_Flags,	CLight::flBreaking);
}
//----------------------------------------------------

void CLight::FillSunProp(LPCSTR pref, PropItemVec& items)
{
	CEditFlare& F 			= m_LensFlare;
    PHelper.CreateFlag32	(items, PHelper.PrepareKey(pref,"Source\\Enabled"),		&F.m_Flags,				CEditFlare::flSource);
    PHelper.CreateFloat		(items, PHelper.PrepareKey(pref,"Source\\Radius"),		&F.m_Source.fRadius,	0.f,10.f);
    PHelper.CreateTexture	(items, PHelper.PrepareKey(pref,"Source\\Texture"),		F.m_Source.texture,		sizeof(F.m_Source.texture));

    PHelper.CreateFlag32	(items, PHelper.PrepareKey(pref,"Gradient\\Enabled"),	&F.m_Flags,				CEditFlare::flGradient);
    PHelper.CreateFloat		(items, PHelper.PrepareKey(pref,"Gradient\\Radius"),	&F.m_Gradient.fRadius,	0.f,100.f);
    PHelper.CreateFloat		(items, PHelper.PrepareKey(pref,"Gradient\\Opacity"),	&F.m_Gradient.fOpacity,	0.f,1.f);
    PHelper.CreateTexture	(items, PHelper.PrepareKey(pref,"Gradient\\Texture"),	F.m_Gradient.texture,	sizeof(F.m_Gradient.texture));

    PHelper.CreateFlag32	(items, PHelper.PrepareKey(pref,"Flares\\Enabled"),		&F.m_Flags,				CEditFlare::flFlare);
	for (CEditFlare::FlareIt it=F.m_Flares.begin(); it!=F.m_Flares.end(); it++){
		AnsiString nm; nm.sprintf("Flares\\Flare %d",it-F.m_Flares.begin());
		PHelper.CreateFloat		(items, PHelper.PrepareKey(pref,AnsiString(nm+"\\Radius").c_str()), 	&it->fRadius,  	0.f,10.f);
        PHelper.CreateFloat		(items, PHelper.PrepareKey(pref,AnsiString(nm+"\\Opacity").c_str()),	&it->fOpacity,	0.f,1.f);
        PHelper.CreateFloat		(items, PHelper.PrepareKey(pref,AnsiString(nm+"\\Position").c_str()),	&it->fPosition,	-10.f,10.f);
        PHelper.CreateTexture	(items, PHelper.PrepareKey(pref,AnsiString(nm+"\\Texture").c_str()),	it->texture,	sizeof(it->texture));
	}
}
//----------------------------------------------------

void CLight::FillPointProp(LPCSTR pref, PropItemVec& items)
{
    PHelper.CreateFloat	(items,	PHelper.PrepareKey(pref, "Range"),					&m_D3D.range,		0.1f,1000.f);
    PHelper.CreateFloat	(items,	PHelper.PrepareKey(pref, "Attenuation\\Constant"),	&m_D3D.attenuation0,0.f,1.f,0.0001f,6);
    PHelper.CreateFloat	(items,	PHelper.PrepareKey(pref, "Attenuation\\Linear"),	&m_D3D.attenuation1,0.f,1.f,0.0001f,6);
    PHelper.CreateFloat	(items,	PHelper.PrepareKey(pref, "Attenuation\\Quadratic"),	&m_D3D.attenuation2,0.f,1.f,0.0001f,6);
}
//----------------------------------------------------

void CLight::FillSpotProp(LPCSTR pref, PropItemVec& items)
{
	PropValue* V=0;
	PHelper.CreateFloat		(items,	PHelper.PrepareKey(pref, "Range"),					&m_D3D.range,	0.1f,1000.f);
	V=PHelper.CreateFloat	(items,	PHelper.PrepareKey(pref, "Cone Angle"),				&m_D3D.phi,		0.1f,120.f,0.01f,2);
    V->OnAfterEditEvent		= PHelper.floatRDOnAfterEdit;
    V->OnBeforeEditEvent	= PHelper.floatRDOnBeforeEdit;
    V->Owner()->OnDrawEvent	= PHelper.floatRDOnDraw;
	PHelper.CreateATexture	(items,	PHelper.PrepareKey(pref, "Attenuation\\Texture"),	&m_SpotAttTex);
}
//----------------------------------------------------

void CLight::OnDeviceCreate()
{
	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.OnDeviceCreate();
}
//----------------------------------------------------

void CLight::OnDeviceDestroy()
{
//	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) 
    m_LensFlare.OnDeviceDestroy();
}
//----------------------------------------------------

//----------------------------------------------------
// Edit Flare
//----------------------------------------------------
CEditFlare::CEditFlare()
{
    m_Flags.set(flFlare|flSource|flGradient,TRUE);
	// flares
    m_Flares.resize		(6);
    FlareIt it=m_Flares.begin();
	it->fRadius=0.08f; it->fOpacity=0.06f; it->fPosition=1.3f; strcpy(it->texture,"fx\\fx_flare1"); it++;
	it->fRadius=0.12f; it->fOpacity=0.04f; it->fPosition=1.0f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.04f; it->fOpacity=0.10f; it->fPosition=0.5f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.08f; it->fOpacity=0.08f; it->fPosition=-0.3f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.12f; it->fOpacity=0.04f; it->fPosition=-0.6f; strcpy(it->texture,"fx\\fx_flare3"); it++;
	it->fRadius=0.30f; it->fOpacity=0.04f; it->fPosition=-1.0f; strcpy(it->texture,"fx\\fx_flare1"); it++;
	// source
    strcpy(m_Source.texture,"fx\\fx_sun");
    m_Source.fRadius 	= 0.15f;
    // gradient
    strcpy(m_Gradient.texture,"fx\\fx_gradient");
    m_Gradient.fOpacity = 0.9f;
    m_Gradient.fRadius 	= 4.f;
}

void CEditFlare::Load(CStream& F){
	if (!F.FindChunk(FLARE_CHUNK_FLAG)) return;

    R_ASSERT(F.FindChunk(FLARE_CHUNK_FLAG));
    F.Read			(&m_Flags.flags,sizeof(m_Flags));

    R_ASSERT(F.FindChunk(FLARE_CHUNK_SOURCE));
    F.RstringZ		(m_Source.texture);
    m_Source.fRadius= F.Rfloat();

    if (F.FindChunk(FLARE_CHUNK_GRADIENT2)){
	    F.RstringZ	(m_Gradient.texture);
	    m_Gradient.fOpacity = F.Rfloat();
	    m_Gradient.fRadius  = F.Rfloat();
    }else{
		R_ASSERT(F.FindChunk(FLARE_CHUNK_GRADIENT));
	    m_Gradient.fOpacity = F.Rfloat();
    }

    // flares
    if (F.FindChunk(FLARE_CHUNK_FLARES2)){
	    DeleteShaders();
	    DWORD deFCnt	= F.Rdword(); VERIFY(deFCnt==6);
	   	F.Read			(m_Flares.begin(),m_Flares.size()*sizeof(SFlare));
    	for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++) it->hShader=0;
    	CreateShaders();
    }
}
//----------------------------------------------------

void CEditFlare::Save(CFS_Base& F)
{
	F.open_chunk	(FLARE_CHUNK_FLAG);
    F.write			(&m_Flags.flags,sizeof(m_Flags));
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_SOURCE);
    F.WstringZ		(m_Source.texture);
    F.Wfloat		(m_Source.fRadius);
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_GRADIENT2);
    F.WstringZ		(m_Gradient.texture);
    F.Wfloat		(m_Gradient.fOpacity);
    F.Wfloat		(m_Gradient.fRadius);
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_FLARES2);
    F.Wdword		(m_Flares.size());
    F.write			(m_Flares.begin(),m_Flares.size()*sizeof(SFlare));
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
    CLensFlare::OnDeviceDestroy();
}

void CEditFlare::CreateShaders()
{
    CLensFlare::OnDeviceCreate();
}
//----------------------------------------------------

