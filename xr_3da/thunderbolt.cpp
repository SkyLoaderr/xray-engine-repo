#include "stdafx.h"
#pragma once

#include "Thunderbolt.h"
#include "igame_persistent.h"

#ifdef _EDITOR
	#ifdef _LEVEL_EDITOR
		#include "ui_tools.h"
    #endif
#else
#endif

CThunderboltDesc::CThunderboltDesc(CInifile* pIni, LPCSTR sect)
{
	name				= sect;
    m_Gradient.shader 	= pIni->r_string ( sect,"gradient_shader" );
    m_Gradient.texture	= pIni->r_string ( sect,"gradient_texture" );
    m_Gradient.fRadius	= pIni->r_float	 ( sect,"gradient_radius"  );
    m_Gradient.fOpacity = pIni->r_float	 ( sect,"gradient_opacity" );
	m_Gradient.hShader.create	(*m_Gradient.shader,*m_Gradient.texture);

    IReader* F			= 0;
    LPCSTR m_name;
	m_name				= pSettings->r_string(sect,"lightning_model");
	F					= FS.r_open("$game_meshes$",m_name); R_ASSERT2(F,"Empty 'lightning_model'.");
    l_model				= Render->model_CreateDM(F);
    FS.r_close			(F);
    m_name				= pSettings->r_string(sect,"glow_model");
	F					= m_name?FS.r_open("$game_meshes$",m_name):0;
    g_model				= F?::Render->model_CreateDM(F):0;
    FS.r_close			(F);
}

CThunderboltDesc::~CThunderboltDesc()
{
    ::Render->model_Delete		(l_model);
    ::Render->model_Delete		(g_model);
    m_Gradient.hShader.destroy	();
}

CEffect_Thunderbolt::CEffect_Thunderbolt()
{
	current		= 0;
	life_time	= 0.f;
    state		= stIdle;
    next_lightning_time = 0.f;
    bEnabled	= FALSE;

    // geom
	hGeom_model.create	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RCache.Vertex.Buffer(), RCache.Index.Buffer());
	hGeom_gradient.create(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.QuadIB);

    // descriptions
    int tb_count	= pSettings->line_count("thunderbolts");
    for (int tb_idx=0; tb_idx<tb_count; tb_idx++){
        LPCSTR tb, sect_tb;
        if (pSettings->r_line("thunderbolts",tb_idx,&tb,&sect_tb))
        	palette.push_back(xr_new<CThunderboltDesc>(pSettings,sect_tb));
    }
}

CEffect_Thunderbolt::~CEffect_Thunderbolt()
{
	for (DescIt d_it=palette.begin(); d_it!=palette.end(); d_it++)
    	xr_delete(*d_it);
	hGeom_model.destroy			();
	hGeom_gradient.destroy		();
}

BOOL CEffect_Thunderbolt::RayPick(const Fvector& s, const Fvector& d, float& range)
{
	BOOL bRes 	= TRUE;
#ifdef _EDITOR
    bRes 				= Tools.RayPick	(s,d,range,0,0);
#else
	Collide::ray_query	RQ;
	CObject* E 			= g_pGameLevel->CurrentViewEntity();
	if (E)				E->setEnabled		(FALSE);
	bRes 				= g_pGameLevel->ObjectSpace.RayPick(s,d,range,RQ);	
	if (E)				E->setEnabled		(TRUE);
    if (bRes) range 	= RQ.range;
#endif
    return bRes;
}

#define FAR_DIST g_pGamePersistent->Environment.CurrentEnv.far_plane
#define SUN_DIR  g_pGamePersistent->Environment.CurrentEnv.sun_dir
void CEffect_Thunderbolt::Bolt(float lt)
{
	state 		= stWorking;
	life_time	= lt+Random.randF(-lt*0.5f,lt*0.5f);
    current_time= 0.f;
    current		= palette[Random.randI(palette.size())]; VERIFY(current);

    Fmatrix XF,S;
    Fvector dir,pos;
    float sun_h, sun_p; SUN_DIR.getHP(sun_h,sun_p);
    float alt	= Random.randF(deg2rad(15.f),deg2rad(25.f));
    float lng	= 0;//-Random.randF(sun_h-PI_MUL_8,sun_h+PI_MUL_8); 
    float dist	= Random.randF(FAR_DIST*0.7f,FAR_DIST*0.9f);
    dir.setHP	(lng,alt);
    pos.mad		(Device.vCameraPosition,dir,dist);
    Fvector dev;
    dev.x		= Random.randF(-deg2rad(30.f),deg2rad(30.f));
    dev.y		= Random.randF(0,PI_MUL_2);
    dev.z		= Random.randF(-deg2rad(30.f),deg2rad(30.f));
    XF.setXYZi	(dev);

    light_dir.set(0.f,-1.f,0.f);
    XF.transform_dir(light_dir);
    float scale	= 1000.f;
    RayPick		(pos,light_dir,scale);

    S.scale		(scale,scale,scale);
    XF.translate_over	(pos);
    current_xform.mul_43(XF,S);
}

void CEffect_Thunderbolt::OnFrame(BOOL enabled, float period, float duration)
{
	if (bEnabled!=enabled){
    	bEnabled			= enabled;
	    next_lightning_time = Device.fTimeGlobal+period+Random.randF(-period*0.5f,period*0.5f);
    }else if (bEnabled&&(Device.fTimeGlobal>next_lightning_time)){ 
    	if (state==stIdle)	Bolt(duration);
	    next_lightning_time = Device.fTimeGlobal+period+Random.randF(-period*0.5f,period*0.5f);
    }
}

void CEffect_Thunderbolt::Render()
{                  
	if (state==stWorking){
    	VERIFY	(current);
    	current_time	+= Device.fTimeDelta;
    	if (current_time>life_time) state = stIdle;

        float dv		= 1.5f* (0.5f*current_time/life_time);
        if (dv>0.5f){
//        	dv 			= Random.randF(0.f,0.5f);
			int r_val	= Random.randI(2);
        	dv 			= r_val*0.5f;
        }
        
        // render
//        if (::Render->ViewBase.testSphere_dirty(P->bounds.P, P->bounds.R))

        {
			HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);
            u32					v_offset,i_offset;
            u32					vCount_Lock		= current->l_model->number_vertices;
            u32					iCount_Lock		= current->l_model->number_indices;
            IRender_DetailModel::fvfVertexOut* v_ptr= (IRender_DetailModel::fvfVertexOut*) 	RCache.Vertex.Lock	(vCount_Lock, hGeom_model->vb_stride, v_offset);
            u16*				i_ptr				=										RCache.Index.Lock	(iCount_Lock, i_offset);
            // XForm verts
            current->l_model->transfer(current_xform,v_ptr,0xffffffff,i_ptr,0,0.f,dv);
            // Flush if needed
            RCache.Vertex.Unlock(vCount_Lock,hGeom_model->vb_stride);
            RCache.Index.Unlock	(iCount_Lock);
            RCache.set_xform_world(Fidentity);
			RCache.set_Shader	(current->l_model->shader);
            RCache.set_Geometry	(hGeom_model);
            RCache.Render		(D3DPT_TRIANGLELIST,v_offset,0,vCount_Lock,i_offset,iCount_Lock/3);
			HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
/*
			// gradient
			float 	fDistance	= FAR_DIST*0.75f;
            Fvector				vec, vecSx, vecSy;
            Fvector				vecDx, vecDy;
            vecSx.mul			(vecX, current->m_Gradient.fRadius*fDistance);
            vecSy.mul			(vecY, current->m_Gradient.fRadius*fDistance);

            u32 c				= 0xffffffff;
			u32					VS_Offset;
			FVF::LIT *pv		= (FVF::LIT*) RCache.Vertex.Lock(4,hGeom_gradient.stride(),VS_Offset);
            pv->set				(light_dir.x+vecSx.x-vecSy.x, light_dir.y+vecSx.y-vecSy.y, light_dir.z+vecSx.z-vecSy.z, c, 0, 0); pv++;
            pv->set				(light_dir.x+vecSx.x+vecSy.x, light_dir.y+vecSx.y+vecSy.y, light_dir.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
            pv->set				(light_dir.x-vecSx.x-vecSy.x, light_dir.y-vecSx.y-vecSy.y, light_dir.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
            pv->set				(light_dir.x-vecSx.x+vecSy.x, light_dir.y-vecSx.y+vecSy.y, light_dir.z-vecSx.z+vecSy.z, c, 1, 1); pv++;
            RCache.Vertex.Unlock	(4,hGeom_gradient.stride());

            RCache.set_xform_world	(Fidentity);
            RCache.set_Geometry		(hGeom_gradient);
            RCache.set_Shader		(current->m_Gradient.hShader);
            RCache.Render			(D3DPT_TRIANGLELIST,VS_Offset, 0,4,0,2);
*/
        }
    }
}

