//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffect.h"
#include "render.h"
#include "psystem.h"

using namespace PAPI;
using namespace PS;

//------------------------------------------------------------------------------
// class CParticleEffectDef
//------------------------------------------------------------------------------
CPEDef::CPEDef()
{
    strcpy				(m_Name,"unknown");
    m_ShaderName		= 0;
    m_TextureName		= 0;
    m_Frame.InitDefault	();
    m_ActionCount		= 0;
    m_ActionList		= 0;
    m_MaxParticles		= 0;
	m_CachedShader		= 0;
	m_TimeLimit			= 0;
}
CPEDef::~CPEDef()
{
    xr_free				(m_ShaderName);
    xr_free				(m_TextureName);
    xr_free				(m_ActionList);
}
void CPEDef::SetName(LPCSTR name)
{
    strcpy				(m_Name,name);
}
void CPEDef::pSprite(string64& sh_name, string64& tex_name)
{
    xr_free(m_ShaderName);	m_ShaderName		= xr_strdup(sh_name);
    xr_free(m_TextureName);	m_TextureName	= xr_strdup(tex_name);
	m_Flags.set	(dfSprite,TRUE);
}
void CPEDef::pFrame(BOOL random_frame, u32 frame_count, u32 tex_width, u32 tex_height, u32 frame_width, u32 frame_height)
{
	m_Flags.set			(dfFramed,TRUE);
	m_Flags.set			(dfRandomFrame,random_frame);
	m_Frame.Set			(frame_count, (float)tex_width, (float)tex_height, (float)frame_width, (float)frame_height);
}
void CPEDef::pAnimate(float speed, BOOL random_playback)
{
	m_Frame.m_fSpeed	= speed;
	m_Flags.set			(dfAnimated,TRUE);
	m_Flags.set			(dfRandomPlayback,random_playback);
}
void CPEDef::pTimeLimit(float time_limit)
{
	m_Flags.set			(dfTimeLimit,TRUE);
	m_TimeLimit			= iFloor(time_limit*1000.f);
}
void CPEDef::pFrameInitExecute(ParticleEffect *effect)
{
	for(int i = 0; i < effect->p_count; i++){
		Particle &m = effect->list[i];
        if (m.flags.is(Particle::BIRTH)){
            if (m_Flags.is(dfRandomFrame))
                m.frame	= (float)Random.randI(m_Frame.m_iFrameCount);
            if (m_Flags.is(dfAnimated)&&m_Flags.is(dfRandomPlayback)&&Random.randI(2))
                m.flags.set(Particle::ANIMATE_CCW,TRUE);
        }
    }
}
void CPEDef::pAnimateExecute(ParticleEffect *effect, float dt)
{
	float speedFac = m_Frame.m_fSpeed * dt;
	for(int i = 0; i < effect->p_count; i++){
		Particle &m = effect->list[i];
		m.frame						+= ((m.flags.is(Particle::ANIMATE_CCW))?-1.f:1.f)*speedFac;
		if (m.frame>m_Frame.m_iFrameCount)	m.frame-=m_Frame.m_iFrameCount;
		if (m.frame<0.f)					m.frame+=m_Frame.m_iFrameCount;
	}
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPEDef::Load(IReader& F)
{
	R_ASSERT		(F.find_chunk(PED_CHUNK_VERSION));
	u16 version		= F.r_u16();

    if (version!=PED_VERSION)
    	return FALSE;

	R_ASSERT		(F.find_chunk(PED_CHUNK_NAME));
	F.r_stringZ		(m_Name);

	R_ASSERT		(F.find_chunk(PED_CHUNK_EFFECTDATA));
    m_MaxParticles	= F.r_u32();

    R_ASSERT		(F.find_chunk(PED_CHUNK_ACTIONLIST));
    m_ActionCount	= F.r_u32();
    m_ActionList	= xr_alloc<PAPI::PAHeader>(m_ActionCount);
    F.r				(m_ActionList,m_ActionCount*sizeof(PAPI::PAHeader));

	F.r_chunk		(PED_CHUNK_FLAGS,&m_Flags);

    string256		buf;
    if (m_Flags.is(dfSprite)){
        R_ASSERT	(F.find_chunk(PED_CHUNK_SPRITE));
        F.r_stringZ	(buf); m_ShaderName = xr_strdup(buf);
        F.r_stringZ	(buf); m_TextureName= xr_strdup(buf);
    }

    if (m_Flags.is(dfFramed)){
        R_ASSERT	(F.find_chunk(PED_CHUNK_FRAME));
        F.r			(&m_Frame,sizeof(SFrame));
    }

    if (m_Flags.is(dfTimeLimit)){
        R_ASSERT	(F.find_chunk(PED_CHUNK_TIMELIMIT));
        m_TimeLimit	= F.r_u32();
    }
    
#ifdef _PARTICLE_EDITOR
	F.find_chunk	(PED_CHUNK_SOURCETEXT);
    F.r_stringZ		(m_SourceText);
#endif

    return TRUE;
}

void CPEDef::Save(IWriter& F)
{
	F.open_chunk	(PED_CHUNK_VERSION);
	F.w_u16			(PED_VERSION);
    F.close_chunk	();

	F.open_chunk	(PED_CHUNK_NAME);
    F.w_stringZ		(m_Name);
    F.close_chunk	();

	F.open_chunk	(PED_CHUNK_EFFECTDATA);
    F.w_u32			(m_MaxParticles);
    F.close_chunk	();

	F.open_chunk	(PED_CHUNK_ACTIONLIST);
    F.w_u32			(m_ActionCount);
    F.w				(m_ActionList,m_ActionCount*sizeof(PAPI::PAHeader));
    F.close_chunk	();

	F.w_chunk		(PED_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

    if (m_Flags.is(dfSprite)){
        F.open_chunk	(PED_CHUNK_SPRITE);
        F.w_stringZ		(m_ShaderName);
        F.w_stringZ		(m_TextureName);
        F.close_chunk	();
    }

    if (m_Flags.is(dfFramed)){
        F.open_chunk	(PED_CHUNK_FRAME);
        F.w				(&m_Frame,sizeof(SFrame));
        F.close_chunk	();
    }

    if (m_Flags.is(dfTimeLimit)){
        F.open_chunk	(PED_CHUNK_TIMELIMIT);
        F.w_u32			(m_TimeLimit);
        F.close_chunk	();
    }
#ifdef _PARTICLE_EDITOR
	F.open_chunk	(PED_CHUNK_SOURCETEXT);
    F.w_stringZ		(m_SourceText.c_str());
	F.close_chunk	();
#endif
}
//------------------------------------------------------------------------------
// class CParticleEffect
//------------------------------------------------------------------------------
CParticleEffect::CParticleEffect():IRender_Visual()
{
	m_HandleEffect 			= pGenParticleEffects(1, 1);
    m_HandleActionList		= pGenActionLists();
    m_Flags.zero			();
    m_Def					= 0;
    m_ElapsedLimit			= 0;
	m_MemDT					= 0;
	m_InitialPosition.set	(0,0,0);
}
CParticleEffect::~CParticleEffect()
{
	OnDeviceDestroy			();
	pDeleteParticleEffects	(m_HandleEffect);
	pDeleteActionLists		(m_HandleActionList);
}

void CParticleEffect::OnDeviceCreate()
{
	hGeom				= Device.Shader.CreateGeom	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
}

void CParticleEffect::OnDeviceDestroy()
{
	Device.Shader.DeleteGeom(hGeom);
}

void CParticleEffect::Play()
{
	m_Flags.set			(flPlaying,TRUE);
   	pStartPlaying		(m_HandleActionList);
}
void CParticleEffect::Stop(bool bFinishPlaying)
{
	if (bFinishPlaying){
		m_Flags.set		(flDefferedStop,TRUE);
    	pStopPlaying	(m_HandleActionList);
    }else{
    	m_Flags.set		(flPlaying,FALSE);
		ResetParticles	();
    }
}
void CParticleEffect::RefreshShader()
{
	OnDeviceDestroy();
    OnDeviceCreate();
}

void CParticleEffect::ResetParticles()
{
	if (m_Def){
        pSetMaxParticlesG	(m_HandleEffect,0);
        pSetMaxParticlesG	(m_HandleEffect,m_Def->m_MaxParticles);
    }
}
void CParticleEffect::UpdateParent(const Fmatrix& m, const Fvector& velocity)
{
	m_InitialPosition		= m.c;
	pSetActionListParenting	(m_HandleActionList,m,velocity);
}

static const u32	uDT_STEP = 33;
static const float	fDT_STEP = float(uDT_STEP)/1000.f;
void CParticleEffect::OnFrame(u32 frame_dt)
{
	vis.box.set			(m_InitialPosition,m_InitialPosition);
	vis.box.grow		(EPS_L);
	vis.box.getsphere	(vis.sphere.P,vis.sphere.R);

	if (m_Def&&m_Flags.is(flPlaying)){
		m_MemDT			+= frame_dt;
		for (;m_MemDT>=uDT_STEP; m_MemDT-=uDT_STEP){
    		if (m_Def->m_Flags.is(CPEDef::dfTimeLimit)){ 
        		m_ElapsedLimit 	-= uDT_STEP;
				if (m_ElapsedLimit<0){
            		m_ElapsedLimit 	= m_Def->m_TimeLimit;
                    Stop		(true);
					return;
				}
			}
			pTimeStep			(fDT_STEP);
			pCurrentEffect		(m_HandleEffect);

			// execute action list
			pCallActionList		(m_HandleActionList);
			ParticleEffect *pg 	= _GetEffectPtr(m_HandleEffect);
			// our actions
			if (m_Def->m_Flags.is(CPEDef::dfFramed))    		  		m_Def->pFrameInitExecute(pg);
			if (m_Def->m_Flags.is(CPEDef::dfFramed|CPEDef::dfAnimated))	m_Def->pAnimateExecute	(pg,fDT_STEP);

			//-move action
			if (pg->p_count)	
			{
				vis.box.invalidate	();
				float p_size = 0.f;
				for(int i = 0; i < pg->p_count; i++){
					Particle &m 	= pg->list[i]; 
					if (m.flags.is(Particle::DYING)){}
					if (m.flags.is(Particle::BIRTH))m.flags.set(Particle::BIRTH,FALSE);
					vis.box.modify((Fvector&)m.pos);
					if (m.size.x>p_size) p_size = m.size.x;
				}
				vis.box.grow		(p_size);
				vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
			}
			vis.box.grow		(p_size);
			vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
            
            if (m_Flags.is(flDefferedStop)&&(0==pg->p_count)){
				m_Flags.set		(flPlaying|flDefferedStop,FALSE);
                break;
            }
		}
	    Log("Radius:",vis.sphere.R);
	}
}

BOOL CParticleEffect::Compile(CPEDef* def)
{
	m_Def 						= def;
    if (m_Def){
        // set current effect for action
        pCurrentEffect			(m_HandleEffect);
        // refresh shader
        RefreshShader			();
        // reset particles
        ResetParticles			();
        // load action list
        // get pointer to specified action list.
        PAPI::PAHeader* pa		= _GetListPtr(m_HandleActionList);
        if(pa == NULL)	
			return FALSE; // ERROR

        // start append actions
        pNewActionList			(m_HandleActionList);
        for (int k=0; k<m_Def->m_ActionCount; k++)
            pAddActionToList	(m_Def->m_ActionList+k);
        // end append action
        pEndActionList();
        // time limit
		if (m_Def->m_Flags.is(CPEDef::dfTimeLimit))
			m_ElapsedLimit 		= m_Def->m_TimeLimit;
    }
	if (def)	hShader			= def->m_CachedShader();
	return TRUE;
}

//------------------------------------------------------------------------------
// Render
//------------------------------------------------------------------------------
void CParticleEffect::Copy(IRender_Visual* pFrom)
{
	Debug.fatal("Can't duplicate particle system - NOT IMPLEMENTED");
}

//----------------------------------------------------
IC void FillSprite	(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float radius, u32 clr, float angle, float scale, float w_2, float h_2)
{
	FVF::TL			PT;

	PT.transform	(pos, M);
	float sz		= scale * radius / PT.p.w;
	// 'Cause D3D clipping have to clip Four points
	// We can help him :)

	if (sz<1.5f)	return;
	if (PT.p.x< -1)	return;
	if (PT.p.x>  1)	return;
	if (PT.p.y< -1)	return;
	if (PT.p.y>  1)	return;
	if (PT.p.z<  0) return;

	// Convert to screen coords
	Fvector2	c;
	c.x				= (PT.p.x+1)*w_2;
	c.y				= (PT.p.y+1)*h_2;

	// Rotation
	float	_sin1,_cos1,_sin2,_cos2,da;
	da = angle;		 _sin1=_sin(da); _cos1=_cos(da);
	da += PI_DIV_2;  _sin2=_sin(da); _cos2=_cos(da);

	pv->set	(c.x+sz*_sin1,	c.y+sz*_cos1,	PT.p.z, PT.p.w, clr, lt.x,rb.y);	pv++;
	pv->set	(c.x-sz*_sin2,	c.y-sz*_cos2,	PT.p.z, PT.p.w, clr, lt.x,lt.y);	pv++;
	pv->set	(c.x+sz*_sin2,	c.y+sz*_cos2,	PT.p.z, PT.p.w, clr, rb.x,rb.y);	pv++;
	pv->set	(c.x-sz*_sin1,	c.y-sz*_cos1,	PT.p.z, PT.p.w, clr, rb.x,lt.y);	pv++;
}

IC void FillSprite	(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float radius, u32 clr, const Fvector& D, float scale, float w_2, float h_2)
{
	Fvector			P1,P2;
	P1.mad			(pos,D,-radius);
	P2.mad			(pos,D,radius);

	FVF::TL			s1,s2;
	s1.transform	(P1,M);
	s2.transform	(P2,M);

	if ((s1.p.w<=0)||(s2.p.w<=0)) return;

	float l1		= scale*radius/s1.p.w*1.4142f;
	float l2		= scale*radius/s2.p.w*1.4142f;

	Fvector2		dir,R;
	dir.set			(s2.p.x-s1.p.x,s2.p.y-s1.p.y);
	dir.norm		();
	R.cross			(dir);

	pv->set			((s1.p.x+l1*R.x+1)*w_2,	(s1.p.y+l1*R.y+1)*h_2,	s2.p.z, s2.p.w, clr, lt.x,rb.y);	pv++;
	pv->set			((s2.p.x+l2*R.x+1)*w_2,	(s2.p.y+l2*R.y+1)*h_2,	s2.p.z, s2.p.w, clr, lt.x,lt.y);	pv++;
	pv->set			((s1.p.x-l1*R.x+1)*w_2,	(s1.p.y-l1*R.y+1)*h_2,	s2.p.z, s2.p.w, clr, rb.x,rb.y);	pv++;
	pv->set			((s2.p.x-l2*R.x+1)*w_2,	(s2.p.y-l2*R.y+1)*h_2,	s2.p.z, s2.p.w, clr, rb.x,lt.y);	pv++;
}

u32 CParticleEffect::RenderTO	(FVF::TL* dest)
{
	// Get a pointer to the particles in gp memory
	PAPI::ParticleEffect *pe = PAPI::_GetEffectPtr(m_HandleEffect);

	if(pe == NULL)		return 0; // ERROR
	if(pe->p_count < 1)	return 0;

	// build transform matrix
	Fmatrix mSpriteTransform	= Device.mFullTransform;

	float	w_2			= float(::Render->getTarget()->get_width()) / 2;
	float	h_2			= float(::Render->getTarget()->get_height()) / 2;
	float	fov_scale	= float(::Render->getTarget()->get_width()) / (Device.fFOV/90.f);

	FVF::TL* pv_start	= dest;
	FVF::TL* pv			= pv_start;

	for(int i = 0; i < pe->p_count; i++){
		PAPI::Particle &m = pe->list[i];

		Fcolor c; c.set(m.color.x,m.color.y,m.color.z,m.alpha);
		u32 C 			= c.get();
		Fvector2 lt,rb;
		lt.set			(0.f,0.f);
		rb.set			(1.f,1.f);
		if (m_Def->m_Flags.is(CPEDef::dfFramed)){
			m_Def->m_Frame.CalculateTC(iFloor(m.frame),lt,rb);
			FillSprite(pv,mSpriteTransform,(Fvector&)m.pos,lt,rb,m.size.x*.5f,C,m.rot.x,fov_scale,w_2,h_2);
		}else
			FillSprite(pv,mSpriteTransform,(Fvector&)m.pos,lt,rb,m.size.x*.5f,C,m.rot.x,fov_scale,w_2,h_2);
	}
	return pv-pv_start;
}

void CParticleEffect::Render(float LOD)
{
	u32			vOffset;
	ParticleEffect *pe 		= _GetEffectPtr(m_HandleEffect);
    if (pe&&pe->p_count){
        FVF::TL*	pv			= (FVF::TL*)RCache.Vertex.Lock(pe->p_count*4,hGeom->vb_stride,vOffset);
        u32			dwCount		= RenderTO(pv);
        RCache.Vertex.Unlock(dwCount,hGeom->vb_stride);
        if (dwCount)    {
            RCache.set_Geometry		(hGeom);
            RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,dwCount,0,dwCount/2);
        }
    }
}

