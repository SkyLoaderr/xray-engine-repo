//----------------------------------------------------
// file: Light.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Glow.h"
#include "Frustum.h"
#include "Scene.h"
#include "xr_trims.h"
#include "bottombar.h"
#include "d3dutils.h"
#include "render.h"
#include "PropertiesListTypes.h"

#define GLOW_VERSION	   				0x0012
//----------------------------------------------------
#define GLOW_CHUNK_VERSION				0xC411
#define GLOW_CHUNK_PARAMS				0xC413
#define GLOW_CHUNK_SHADER				0xC414
#define GLOW_CHUNK_TEXTURE				0xC415
//----------------------------------------------------

#define VIS_RADIUS 		0.25f

CGlow::CGlow(LPVOID data, LPCSTR name):CCustomObject(data,name){
	Construct(data);
}

void CGlow::Construct(LPVOID data){
	ClassID		= OBJCLASS_GLOW;
    m_GShader   = 0;
    m_fRadius	= 0.5f;
    m_bDefLoad	= false;
    m_Flags.zero();
}

CGlow::~CGlow()
{
	OnDeviceDestroy();
}

void CGlow::OnDeviceCreate()
{
	if (m_bDefLoad) return;
	// ������� ������ shaders
	if (!m_TexName.IsEmpty()&&!m_ShaderName.IsEmpty()) m_GShader = Device.Shader.Create(m_ShaderName.c_str(),m_TexName.c_str());
	m_bDefLoad = true;
}

void CGlow::OnDeviceDestroy()
{
	m_bDefLoad = false;
	// ������� shaders
	Device.Shader.Delete(m_GShader);
}

void __fastcall	CGlow::ShaderChange(PropValue* value)
{
	OnDeviceDestroy();
}

void CGlow::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref, items);
    PHelper.CreateATexture	(items,PHelper.PrepareKey(pref,"Texture"), 		&m_TexName, 	0,0,0,ShaderChange);
    PHelper.CreateAEShader	(items,PHelper.PrepareKey(pref,"Shader"),		&m_ShaderName, 	0,0,0,ShaderChange);
    PHelper.CreateFloat		(items,PHelper.PrepareKey(pref,"Radius"),		&m_fRadius,		0.01f,10000.f);
    PHelper.CreateFlag		(items,PHelper.PrepareKey(pref,"Fixed size"),	&m_Flags, 		gfFixedSize);
}
//----------------------------------------------------
bool CGlow::GetBox( Fbox& box )
{
	box.set( PPosition, PPosition );
	box.min.sub(m_fRadius);
	box.max.add(m_fRadius);
	return true;
}

void CGlow::Render(int priority, bool strictB2F){
    if ((1==priority)&&(true==strictB2F)){
    	if (!m_bDefLoad) OnDeviceCreate();
    	// ���������� ��������� ��� RayPick
    	Fvector D;
        SRayPickInfo pinf;
        D.sub(Device.m_Camera.GetPosition(),PPosition);
        pinf.inf.range = D.magnitude();
        if (pinf.inf.range) D.div(pinf.inf.range);
        // ��������� ��������� �� �� �������� glow
		Device.SetTransform(D3DTS_WORLD,Fidentity);
        if (::Render->ViewBase.testSphere_dirty(PPosition,m_fRadius)){
        	// �������� Glow
        	if ((fraBottomBar->miGlowTestVisibility->Checked&&!Scene.RayPick(PPosition,D,OBJCLASS_SCENEOBJECT,&pinf,true,0))||
            	!fraBottomBar->miGlowTestVisibility->Checked){
                if (m_GShader){	Device.SetShader(m_GShader);
                }else{			Device.SetShader(Device.m_WireShader);}
                m_RenderSprite.Render(PPosition,m_fRadius,m_Flags.is(gfFixedSize));
			}else{
                // �������� bounding sphere
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

bool CGlow::FrustumPick(const CFrustum& frustum)
{
    return (frustum.testSphere_dirty(PPosition,m_fRadius))?true:false;
}

bool CGlow::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf)
{
	Fvector ray2;
	ray2.sub( PPosition, start );

    float d = ray2.dotproduct(direction);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (m_fRadius*m_fRadius)) && (d>m_fRadius) ){
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
    string256 buf;

    R_ASSERT(F.ReadChunk(GLOW_CHUNK_VERSION,&version));
    if((version!=0x0011)&&(version!=GLOW_VERSION)){
        ELog.DlgMsg( mtError, "CGlow: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    if (F.FindChunk(GLOW_CHUNK_SHADER)){ 
    	F.RstringZ (buf); m_ShaderName = buf;
    }

    R_ASSERT(F.FindChunk(GLOW_CHUNK_TEXTURE));
	F.RstringZ	(buf); m_TexName = buf;

    R_ASSERT(F.FindChunk(GLOW_CHUNK_PARAMS));
	m_fRadius  		= F.Rfloat();
	if (version==0x0011){
		F.Rvector	(FPosition);
        UpdateTransform();
    }

    return true;
}

void CGlow::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(GLOW_CHUNK_VERSION);
	F.Wword			(GLOW_VERSION);
	F.close_chunk	();

	F.open_chunk	(GLOW_CHUNK_PARAMS);
	F.Wfloat   		(m_fRadius);
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


