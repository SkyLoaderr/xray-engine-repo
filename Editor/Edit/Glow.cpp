//----------------------------------------------------
// file: Light.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Glow.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "D3DUtils.h"
#include "Frustum.h"
#include "Scene.h"
#include "xr_trims.h"

#define GLOW_VERSION	   				0x0011
//----------------------------------------------------
#define GLOW_CHUNK_VERSION				0xC411
#define GLOW_CHUNK_PARAMS				0xC413
#define GLOW_CHUNK_SHADER				0xC414
#define GLOW_CHUNK_TEXTURE				0xC415
//----------------------------------------------------

CGlow::CGlow( char *name ):CCustomObject(){
	Construct();
	strcpy( m_Name, name );
}

CGlow::CGlow():CCustomObject(){
	Construct();
}

void CGlow::Construct(){
	m_ClassID   = OBJCLASS_GLOW;
    m_GShader   = 0;
    m_Range     = 0.5f;
	m_Position.set(0,0,0);
}

CGlow::~CGlow(){
    if (m_GShader) Device.Shader.Delete(m_GShader);
}

void CGlow::Compile(){
	if (m_GShader) Device.Shader.Delete(m_GShader);
	if (!m_TexName.IsEmpty()&&!m_ShaderName.IsEmpty()) m_GShader = Device.Shader.Create(m_ShaderName.c_str(),m_TexName.c_str());
}
//----------------------------------------------------
bool CGlow::GetBox( Fbox& box ){
	box.set( m_Position, m_Position );
	box.min.sub(m_Range);
	box.max.add(m_Range);
	return true;
}

void CGlow::Render(int priority, bool strictB2F){
    if ((1==priority)&&(true==strictB2F)){
    	// определяем параметры для RayPick
    	Fvector D;
        SRayPickInfo pinf;
        D.sub(Device.m_Camera.GetPosition(),m_Position);
        pinf.rp_inf.range = D.magnitude();
        if (pinf.rp_inf.range) D.div(pinf.rp_inf.range);
        // тестируем находится ли во фрустуме glow
		Device.SetTransform(D3DTS_WORLD,precalc_identity);
        if (Device.m_Frustum.testSphere(m_Position,m_Range)){
        	// рендерим Glow
//        	if (!Scene.RayPick(m_Position,D,OBJCLASS_EDITOBJECT,&pinf)){
                if (m_GShader){	Device.SetShader(m_GShader);
                }else{			Device.SetShader(Device.m_WireShader);}
                m_RenderSprite.Render(m_Position,m_Range);
//			}else{
                // рендерим bounding sphere
//				Fcolor clr; clr.set(0,0.85f,0,0);
//        		Device.Shader.Set(Device.m_WireShader);
//          	DU::DrawLineSphere( m_Position, m_Range, clr, false );
//			}
            if( Selected() ){
            	Fbox bb; GetBox(bb);
                DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                Device.SetShader(Device.m_WireShader);
                DU::DrawSelectionBox(bb,&clr);
            }
        }
    }
}

bool CGlow::FrustumPick(const CFrustum& frustum){
    return (frustum.testSphere(m_Position,m_Range))?true:false;
}

bool CGlow::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
	Fvector ray2;
	ray2.sub( m_Position, start );

    float d = ray2.dotproduct(direction);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (m_Range*m_Range)) && (d>m_Range) ){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
        }
    }
	return false;
}

void CGlow::Move( Fvector& amount ){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI.UpdateScene();
	m_Position.add( amount );
}
//----------------------------------------------------

void CGlow::Rotate( Fvector& center, Fvector& axis, float angle ){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	Fmatrix m;
	m.rotation			(axis, -angle);
	m_Position.sub		(center);
	m.transform_tiny	(m_Position);
	m_Position.add		(center);
    UI.UpdateScene		();
}

bool CGlow::Load(CStream& F){
	DWORD version = 0;
    char sh_name[64]="";
    char tex_name[128]="";

    R_ASSERT(F.ReadChunk(GLOW_CHUNK_VERSION,&version));
    if( version!=GLOW_VERSION ){
        ELog.DlgMsg( mtError, "CGlow: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    if (F.FindChunk(GLOW_CHUNK_SHADER)) F.RstringZ (sh_name);

    R_ASSERT(F.FindChunk(GLOW_CHUNK_TEXTURE));
	F.RstringZ	(tex_name); m_TexName = tex_name;

    R_ASSERT(F.FindChunk(GLOW_CHUNK_PARAMS));
	m_Range  		= F.Rfloat();
	F.Rvector		(m_Position);

    m_ShaderName	= sh_name;
    Compile			();
    return true;
}

void CGlow::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(GLOW_CHUNK_VERSION);
	F.Wword			(GLOW_VERSION);
	F.close_chunk	();

	F.open_chunk	(GLOW_CHUNK_PARAMS);
	F.Wfloat   		(m_Range);
	F.Wvector 		(m_Position);
	F.close_chunk	();

    if (!m_ShaderName.IsEmpty()){
		F.open_chunk(GLOW_CHUNK_SHADER);
	    F.WstringZ 	(m_ShaderName.c_str());
		F.close_chunk();
    }

	F.open_chunk	(GLOW_CHUNK_TEXTURE);
	F.WstringZ		(m_TexName.c_str());
	F.close_chunk	();
}
//----------------------------------------------------

