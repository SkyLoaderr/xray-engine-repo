//----------------------------------------------------
// file: ELight.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "ELight.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "Frustum.h"
#include "D3DUtils.h"
#include "BottomBar.h"

#define LIGHT_VERSION   				0x0010
//----------------------------------------------------
#define LIGHT_CHUNK_VERSION				0xB411
#define LIGHT_CHUNK_BUILD_OPTIONS		0xB412
#define LIGHT_CHUNK_BRIGHTNESS			0xB425
#define LIGHT_CHUNK_FLARES		        0xB430
#define LIGHT_CHUNK_D3D_PARAMS         	0xB435
#define LIGHT_CHUNK_USE_IN_D3D			0xB436
#define LIGHT_CHUNK_HPB_ROTATION		0xB437
#define LIGHT_CHUNK_PROCEDURAL_DATA		0xB455
//----------------------------------------------------

#define VIS_RADIUS 		0.25f
#define DIR_SELRANGE 	250
#define DIR_RANGE 		1.5
#define SEL_COLOR 		0x00FFFFFF
#define NORM_COLOR 		0x00FFFF00
#define NORM_DYN_COLOR 	0x0000FF00
#define LOCK_COLOR 		0x00FF0000

CLight::CLight( char *name ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

CLight::CLight():SceneObject(){
	Construct();
}

void CLight::Construct(){
	m_ClassID 		= OBJCLASS_LIGHT;

    m_Flares 		= true;
	m_CastShadows 	= false;
    m_UseInD3D		= true;

    ZeroMemory		(&m_D3D,sizeof(m_D3D));

    m_D3D.type 		= D3DLIGHT_POINT;
	m_D3D.diffuse.set(1.f,1.f,1.f,0);
	m_D3D.attenuation0 = 1.f;
	m_D3D.range 	= 8.f;
    m_D3D.p_speed 	= 0.5f;

    m_Brightness 	= 1;
    m_ShadowedScale	= 0.05f;

	m_D3DIndex 		= -1;
    m_Enabled 		= TRUE;

    Fvector temp; 	temp.set(0.f,0.f,0.f);
    SetRotate		(temp);

    m_D3D.flags 	= XRLIGHT_LMAPS;

    InitDefaultFlaresText();
}

CLight::~CLight(){
}

void CLight::UpdateTransform(){
	m_D3D.direction.direct(vRotate.y,vRotate.x);
}

void CLight::CopyFrom(CLight* src){
    m_Enabled 		= src->m_Enabled;
	m_D3D			= src->m_D3D;
    m_Data			= src->m_Data;

	m_CastShadows	= src->m_CastShadows;
	m_Flares		= src->m_Flares;
    m_UseInD3D		= src->m_UseInD3D;

    m_Brightness	= src->m_Brightness;
    m_ShadowedScale	= src->m_ShadowedScale;

	m_FlaresText	= src->m_FlaresText;
}

void CLight::AffectD3D(BOOL flag){
	m_UseInD3D = flag;
    UI->UpdateScene();
}

void CLight::InitDefaultFlaresText(){
	char deffilename[MAX_PATH];
	int handle,s;
    m_FlaresText="";
	strcpy(deffilename,"default.flarestext");
	FS.m_Config.Update( deffilename );
    char buf[4096];
    if (FS.Exist(deffilename,true)){
	    CFileStream F(deffilename);
		F.RstringZ(buf); m_FlaresText = buf;
	}
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

void CLight::Render( Fmatrix& parent, ERenderPriority flag ){
    if (flag==rpNormal){
    	DWORD clr;
        clr = Locked()?LOCK_COLOR:(Selected()?SEL_COLOR:(m_D3D.flags&XRLIGHT_MODELS?NORM_DYN_COLOR:NORM_COLOR));
    	switch (m_D3D.type){
        case D3DLIGHT_POINT:
            if (Selected()) DU::DrawLineSphere( m_D3D.position, m_D3D.range, clr, true );
            DU::DrawPointLight(m_D3D.position,VIS_RADIUS, clr);
        break;
        case D3DLIGHT_DIRECTIONAL:
            if (Selected()) DU::DrawDirectionalLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, DIR_SELRANGE, clr );
            else			DU::DrawDirectionalLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, DIR_RANGE, clr );
        break;
        default: THROW;
        }
    }
}

void CLight::Enable( BOOL flag )
{
    if (m_UseInD3D){
		UI->Device.LightEnable(m_D3DIndex,flag);
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
    UI->Device.SetLight(m_D3DIndex,L);
}

void CLight::UnSet(){
	VERIFY( m_D3DIndex>=0 );
	UI->Device.LightEnable(m_D3DIndex,FALSE);
}

bool CLight::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
//    return (frustum.testSphere(m_Position,m_Range))?true:false;
    return (frustum.testSphere(m_D3D.position,VIS_RADIUS))?true:false;
}

bool CLight::RayPick(float& distance, Fvector& start, Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf){
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

void CLight::Move( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	m_D3D.position.add(amount);
    for (ALItemIt it=m_Data.begin(); it!=m_Data.end(); it++) it->position.add(amount);
    Update();
    UI->UpdateScene();
}

void CLight::Rotate(Fvector& center, Fvector& axis, float angle){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }

	Fmatrix m;
	m.rotation			(axis, -angle);

	m_D3D.position.sub	(center);
    m.transform_tiny	(m_D3D.position);
	m_D3D.position.add	(center);

    vRotate.direct		(vRotate,axis,angle);

    UpdateTransform();
    UI->UpdateScene();
}
//----------------------------------------------------
void CLight::LocalRotate(Fvector& axis, float angle){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    vRotate.direct(vRotate,axis,angle);
    UpdateTransform();
    UI->UpdateScene();
}
//----------------------------------------------------

void CLight::RTL_Update	(float dT){
	if (!Locked()&&Visible()&&fraBottomBar->miDrawAnimateLight->Checked){
		if (m_D3D.flags&XRLIGHT_PROCEDURAL)
    		if (m_Data.size()>=2) m_D3D.interpolate(dT,m_TempPlayData.begin());
    }
}
//----------------------------------------------------

void CLight::Update(){
	m_TempPlayData.clear();
    for (ALItemIt it=m_Data.begin(); it!=m_Data.end(); it++){
        Flight l=*it;
        l.diffuse.mul_rgb(it->m_Brightness);
		m_TempPlayData.push_back(l);
    }
    m_D3D.p_key_start = 0;
    m_D3D.p_key_count = m_Data.size();
}
//----------------------------------------------------

void CLight::OnShowHint(AStringVec& dest){
    SceneObject::OnShowHint(dest);
    AnsiString temp;
    temp.sprintf("Type:  ");
    switch(m_D3D.type){
    case D3DLIGHT_DIRECTIONAL:  temp+="direct"; break;
    case D3DLIGHT_POINT:        temp+="point"; break;
    default: temp+="undef";
    }
    dest.push_back(temp);
    temp = "Flags: ";
    if (m_D3D.flags&XRLIGHT_LMAPS)      temp+="Lmap ";
    if (m_D3D.flags&XRLIGHT_MODELS)     temp+="Dyn ";
    if (m_D3D.flags&XRLIGHT_PROCEDURAL) temp+="Proc ";
    dest.push_back(temp);
    temp.sprintf("Pos:   %3.2f, %3.2f, %3.2f",m_D3D.position.x,m_D3D.position.y,m_D3D.position.z);
    dest.push_back(temp);
}

bool CLight::Load(CStream& F){
	DWORD version = 0;

    char buf[1024];
    R_ASSERT(F.ReadChunk(LIGHT_CHUNK_VERSION,&version));
    if( version!=LIGHT_VERSION ){
        Log->DlgMsg( mtError, "CLight: Unsupported version.");
        return false;
    }

	SceneObject::Load(F);

    R_ASSERT(F.FindChunk(LIGHT_CHUNK_BUILD_OPTIONS));
	m_CastShadows 	= F.Rword();
    m_ShadowedScale	= F.Rfloat();

    R_ASSERT(F.FindChunk(LIGHT_CHUNK_FLARES));
    m_Flares 		= F.Rword();
    F.RstringZ		(buf); m_FlaresText = buf;

    R_ASSERT(F.ReadChunk(LIGHT_CHUNK_BRIGHTNESS,&m_Brightness));
    R_ASSERT(F.ReadChunk(LIGHT_CHUNK_D3D_PARAMS,&m_D3D));
    F.ReadChunk		(LIGHT_CHUNK_USE_IN_D3D, &m_UseInD3D);

	R_ASSERT(F.FindChunk(LIGHT_CHUNK_PROCEDURAL_DATA));
    DWORD cnt 		= F.Rword();
	m_Data.resize	(cnt);
    F.Read(m_Data.begin(), sizeof(SAnimLightItem) *cnt);

    if (F.FindChunk(LIGHT_CHUNK_HPB_ROTATION)){
    	F.Rvector	(vRotate);
    }else{
    	// generate from direction
        Fvector& dir= m_D3D.direction;
        // parse heading
        Fvector DYaw; DYaw.set(dir.x,0.f,dir.z); DYaw.normalize_safe();
        if (DYaw.x>=0)	vRotate.x = acosf(DYaw.z);
        else			vRotate.x = 2*PI-acosf(DYaw.z);
        // parse pitch
        dir.normalize_safe	();
        vRotate.y		= -asinf(dir.y);
    }

    Update();
	UpdateTransform();
    return true;
}

void CLight::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(LIGHT_CHUNK_VERSION);
	F.Wword			(LIGHT_VERSION);
	F.close_chunk	();

	F.open_chunk	(LIGHT_CHUNK_BUILD_OPTIONS);
	F.Wword			(m_CastShadows);
	F.Wfloat		(m_ShadowedScale);
	F.close_chunk	();

	F.open_chunk	(LIGHT_CHUNK_FLARES);
	F.Wword			(m_Flares);
	F.WstringZ		(m_FlaresText.c_str());
	F.close_chunk	();

	F.write_chunk	(LIGHT_CHUNK_BRIGHTNESS,&m_Brightness,sizeof(m_Brightness));
	F.write_chunk	(LIGHT_CHUNK_D3D_PARAMS,&m_D3D,sizeof(m_D3D));
    F.write_chunk	(LIGHT_CHUNK_USE_IN_D3D,&m_UseInD3D,sizeof(m_UseInD3D));

	F.open_chunk	(LIGHT_CHUNK_PROCEDURAL_DATA);
    F.Wdword		(m_Data.size());
	F.write			(m_Data.begin(), sizeof(SAnimLightItem)*m_Data.size());
	F.close_chunk	();

	F.write_chunk	(LIGHT_CHUNK_HPB_ROTATION,&vRotate,sizeof(vRotate));
}
//----------------------------------------------------

