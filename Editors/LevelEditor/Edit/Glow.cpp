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
#include "PropertiesListHelper.h"

#define GLOW_VERSION	   				0x0012
//----------------------------------------------------
#define GLOW_CHUNK_VERSION				0xC411
#define GLOW_CHUNK_PARAMS				0xC413
#define GLOW_CHUNK_SHADER				0xC414
#define GLOW_CHUNK_TEXTURE				0xC415
#define GLOW_CHUNK_FLAGS				0xC416
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
	// создать заново shaders
	if (!m_TexName.IsEmpty()&&!m_ShaderName.IsEmpty()) m_GShader = Device.Shader.Create(m_ShaderName.c_str(),m_TexName.c_str());
	m_bDefLoad = true;
}

void CGlow::OnDeviceDestroy()
{
	m_bDefLoad = false;
	// удалить shaders
	Device.Shader.Delete(m_GShader);
}

void __fastcall	CGlow::ShaderChange(PropValue* value)
{
	OnDeviceDestroy();
}

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
    	// определяем параметры для RayPick
    	Fvector D;
        SRayPickInfo pinf;
        D.sub(Device.m_Camera.GetPosition(),PPosition);
        pinf.inf.range = D.magnitude();
        if (pinf.inf.range) D.div(pinf.inf.range);
        // тестируем находится ли во фрустуме glow
		RCache.set_xform_world(Fidentity);
        if (::Render->ViewBase.testSphere_dirty(PPosition,m_fRadius)){
        	// рендерим Glow
        	if ((fraBottomBar->miGlowTestVisibility->Checked&&!Scene.RayPick(PPosition,D,OBJCLASS_SCENEOBJECT,&pinf,true,0))||
            	!fraBottomBar->miGlowTestVisibility->Checked){
                if (m_GShader){	Device.SetShader(m_GShader);
                }else{			Device.SetShader(Device.m_WireShader);}
                m_RenderSprite.Render(PPosition,m_fRadius,m_Flags.is(gfFixedSize));
			}else{
                // рендерим bounding sphere
        		Device.SetShader(Device.m_WireShader);
                DU::DrawRomboid(PPosition, VIS_RADIUS, 0x00FF8507);
			}
            if( Selected() ){
            	Fbox bb; GetBox(bb);
                u32 clr = Locked()?0xFFFF0000:0xFFFFFFFF;
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

bool CGlow::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
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

bool CGlow::Load(IReader& F){
	u32 version = 0;
    string256 buf;

    R_ASSERT(F.r_chunk(GLOW_CHUNK_VERSION,&version));
    if((version!=0x0011)&&(version!=GLOW_VERSION)){
        ELog.DlgMsg( mtError, "CGlow: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    if (F.find_chunk(GLOW_CHUNK_SHADER)){
    	F.r_stringZ (buf); m_ShaderName = buf;
    }

    R_ASSERT(F.find_chunk(GLOW_CHUNK_TEXTURE));
	F.r_stringZ	(buf); m_TexName = buf;

    R_ASSERT(F.find_chunk(GLOW_CHUNK_PARAMS));
	m_fRadius  		= F.r_float();
	if (version==0x0011){
		F.r_fvector3	(FPosition);
        UpdateTransform();
    }

    if (F.find_chunk(GLOW_CHUNK_FLAGS))
    	m_Flags.set	(F.r_u16());

    return true;
}

void CGlow::Save(IWriter& F){
	CCustomObject::Save(F);

	F.open_chunk	(GLOW_CHUNK_VERSION);
	F.w_u16			(GLOW_VERSION);
	F.close_chunk	();

	F.open_chunk	(GLOW_CHUNK_PARAMS);
	F.w_float   		(m_fRadius);
	F.close_chunk	();

    if (!m_ShaderName.IsEmpty()){
		F.open_chunk(GLOW_CHUNK_SHADER);
	    F.w_stringZ 	(m_ShaderName.c_str());
		F.close_chunk();
    }

	F.open_chunk	(GLOW_CHUNK_TEXTURE);
	F.w_stringZ		(m_TexName.c_str());
	F.close_chunk	();

	F.open_chunk	(GLOW_CHUNK_FLAGS);
	F.w_u16			(m_Flags.get());
	F.close_chunk	();
}
//----------------------------------------------------

void CGlow::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref, items);
    PropValue* V=0;
    V=PHelper.CreateATexture(items,PHelper.PrepareKey(pref,"Texture"), 		&m_TexName);	V->OnChangeEvent = ShaderChange;
    V=PHelper.CreateAEShader(items,PHelper.PrepareKey(pref,"Shader"),		&m_ShaderName);	V->OnChangeEvent = ShaderChange;
    PHelper.CreateFloat		(items,PHelper.PrepareKey(pref,"Radius"),		&m_fRadius,		0.01f,10000.f);
    PHelper.CreateFlag8		(items,PHelper.PrepareKey(pref,"Fixed size"),	&m_Flags, 		gfFixedSize);
}
//----------------------------------------------------

bool CGlow::GetSummaryInfo(SSceneSummary* inf)
{
	if (!m_TexName.IsEmpty()) inf->textures.push_back(ChangeFileExt(m_TexName,"").LowerCase());
	inf->glow_cnt++;
	return true;
}

