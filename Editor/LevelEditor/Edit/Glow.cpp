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
#include "bottombar.h"

#define GLOW_VERSION	   				0x0012
//----------------------------------------------------
#define GLOW_CHUNK_VERSION				0xC411
#define GLOW_CHUNK_PARAMS				0xC413
#define GLOW_CHUNK_SHADER				0xC414
#define GLOW_CHUNK_TEXTURE				0xC415
//----------------------------------------------------

#define VIS_RADIUS 		0.25f

CGlow::CGlow( char *name ):CCustomObject(){
	Construct();
    Name		= name;
}

CGlow::CGlow():CCustomObject(){
	Construct();
}

void CGlow::Construct(){
	ClassID		= OBJCLASS_GLOW;
    m_GShader   = 0;
    m_Range     = 0.5f;
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
	box.set( PPosition, PPosition );
	box.min.sub(m_Range);
	box.max.add(m_Range);
	return true;
}

void CGlow::Render(int priority, bool strictB2F){
    if ((1==priority)&&(true==strictB2F)){
    	// определяем параметры для RayPick
    	Fvector D;
        SRayPickInfo pinf;
        D.sub(Device.m_Camera.GetPosition(),PPosition);
        pinf.inf.range = D.magnitude();
        if (pinf.inf.range) D.div(pinf.inf.range);
        // тестируем находится ли во фрустуме glow
		Device.SetTransform(D3DTS_WORLD,Fidentity);
        if (Device.m_Frustum.testSphere(PPosition,m_Range)){
        	// рендерим Glow
        	if ((fraBottomBar->miGlowTestVisibility->Checked&&!Scene.RayPick(PPosition,D,OBJCLASS_SCENEOBJECT,&pinf,true,0))||
            	!fraBottomBar->miGlowTestVisibility->Checked){
                if (m_GShader){	Device.SetShader(m_GShader);
                }else{			Device.SetShader(Device.m_WireShader);}
                m_RenderSprite.Render(PPosition,m_Range);
			}else{
                // рендерим bounding sphere
        		Device.SetShader(Device.m_WireShader);
                DU::DrawRomboid(PPosition, VIS_RADIUS, 0x00FF8507);
			}
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
    return (frustum.testSphere(PPosition,m_Range))?true:false;
}

bool CGlow::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
	Fvector ray2;
	ray2.sub( PPosition, start );

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

bool CGlow::Load(CStream& F){
	DWORD version = 0;
    char sh_name[64]="";
    char tex_name[128]="";

    R_ASSERT(F.ReadChunk(GLOW_CHUNK_VERSION,&version));
    if((version!=0x0011)&&(version!=GLOW_VERSION)){
        ELog.DlgMsg( mtError, "CGlow: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    if (F.FindChunk(GLOW_CHUNK_SHADER)) F.RstringZ (sh_name);

    R_ASSERT(F.FindChunk(GLOW_CHUNK_TEXTURE));
	F.RstringZ	(tex_name); m_TexName = tex_name;

    R_ASSERT(F.FindChunk(GLOW_CHUNK_PARAMS));
	m_Range  		= F.Rfloat();
	if (version==0x0011){
		F.Rvector	(FPosition);
        UpdateTransform();
    }


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

void CGlow::OnDeviceCreate(){
	// создать заново shaders
	if (!m_TexName.IsEmpty()&&!m_ShaderName.IsEmpty()) m_GShader = Device.Shader.Create(m_ShaderName.c_str(),m_TexName.c_str());
}

void CGlow::OnDeviceDestroy(){
	// удалить shaders
	if (m_GShader) Device.Shader.Delete(m_GShader);
}

