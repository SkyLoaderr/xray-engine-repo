#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffect.h"

using namespace PAPI;
using namespace PS;

//------------------------------------------------------------------------------
// class CParticleEffect
//------------------------------------------------------------------------------
CParticleEffect::CParticleEffect()
{
	m_HandleEffect 			= ParticleManager()->CreateEffect(1);		VERIFY(m_HandleEffect>=0);
	m_HandleActionList		= ParticleManager()->CreateActionList();	VERIFY(m_HandleActionList>=0);
	m_RT_Flags.zero			();
	m_Def					= 0;
	m_fElapsedLimit			= 0.f;
	m_MemDT					= 0;
	m_InitialPosition.set	(0,0,0);
	m_DestroyCallback		= 0;
	m_CollisionCallback		= 0;
	m_XFORM.identity		();
}
CParticleEffect::~CParticleEffect()
{
	OnDeviceDestroy			();
	ParticleManager()->DestroyEffect		(m_HandleEffect);
	ParticleManager()->DestroyActionList	(m_HandleActionList);
}

void CParticleEffect::Play()
{
	m_RT_Flags.set		(flRT_DefferedStop,FALSE);
	m_RT_Flags.set		(flRT_Playing,TRUE);
    ParticleManager()->PlayEffect(m_HandleEffect,m_HandleActionList);
}
void CParticleEffect::Stop(BOOL bDefferedStop)
{
    ParticleManager()->StopEffect(m_HandleEffect,m_HandleActionList,bDefferedStop);
	if (bDefferedStop){
		m_RT_Flags.set	(flRT_DefferedStop,TRUE);
	}else{
		m_RT_Flags.set	(flRT_Playing,FALSE);
	}
}
void CParticleEffect::RefreshShader()
{
	OnDeviceDestroy();
	OnDeviceCreate();
}

void CParticleEffect::UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM)
{
	m_RT_Flags.set			(flRT_XFORM, bXFORM);
	if (bXFORM)				m_XFORM.set	(m);
	else{
		m_InitialPosition	= m.c;
        ParticleManager()->Transform(m_HandleActionList,m,velocity);
	}
}

static const u32	uDT_STEP = 33;
static const float	fDT_STEP = float(uDT_STEP)/1000.f;

void CParticleEffect::OnFrame(u32 frame_dt)
{
	if (m_Def && m_RT_Flags.is(flRT_Playing)){
		m_MemDT			+= frame_dt;
		for (;m_MemDT>=uDT_STEP; m_MemDT-=uDT_STEP){
			if (m_Def->m_Flags.is(CPEDef::dfTimeLimit)){ 
				if (!m_RT_Flags.is(flRT_DefferedStop)){
					m_fElapsedLimit -= fDT_STEP;
					if (m_fElapsedLimit<0.f){
						m_fElapsedLimit = m_Def->m_fTimeLimit;
						Stop		(true);
					}
				}
			}
            ParticleManager()->Update(m_HandleEffect,m_HandleActionList,fDT_STEP);

            PAPI::Particle* particles;
            u32 p_cnt;
            ParticleManager()->GetParticles(m_HandleEffect,particles,p_cnt);
            
			// our actions
			if (m_Def->m_Flags.is(CPEDef::dfFramed|CPEDef::dfAnimated))	m_Def->ExecuteAnimate	(particles,p_cnt,fDT_STEP);
			if (m_Def->m_Flags.is(CPEDef::dfCollision)) 				m_Def->ExecuteCollision	(particles,p_cnt,fDT_STEP,this,m_CollisionCallback);

			//-move action
			if (p_cnt)	
			{
				vis.box.invalidate	();
				float p_size = 0.f;
				for(u32 i = 0; i < p_cnt; i++){
					Particle &m 	= particles[i]; 
					vis.box.modify((Fvector&)m.pos);
					if (m.size.x>p_size) p_size = m.size.x;
					if (m.size.y>p_size) p_size = m.size.y;
					if (m.size.z>p_size) p_size = m.size.z;
				}
				vis.box.grow		(p_size);
				vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
			}
			if (m_RT_Flags.is(flRT_DefferedStop)&&(0==p_cnt)){
				m_RT_Flags.set		(flRT_Playing|flRT_DefferedStop,FALSE);
				break;
			}
		}
	} else {
		vis.box.set			(m_InitialPosition,m_InitialPosition);
		vis.box.grow		(EPS_L);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}
}

BOOL CParticleEffect::Compile(CPEDef* def)
{
	m_Def 						= def;
	if (m_Def){
		// refresh shader
		RefreshShader			();

		// append actions
		IReader F				(m_Def->m_Actions.pointer(),m_Def->m_Actions.size());
        ParticleManager()->LoadActions		(m_HandleActionList,F);
        ParticleManager()->SetMaxParticles	(m_HandleEffect,m_Def->m_MaxParticles);
        ParticleManager()->SetCallback		(m_HandleEffect,OnParticleBirth,OnParticleDead,m_Def);
		// time limit
		if (m_Def->m_Flags.is(CPEDef::dfTimeLimit))
			m_fElapsedLimit 	= m_Def->m_fTimeLimit;
	}
	if (def)	hShader			= def->m_CachedShader;
	return TRUE;
}

u32 CParticleEffect::ParticlesCount()
{
	return ParticleManager()->GetParticlesCount(m_HandleEffect);
}

//------------------------------------------------------------------------------
// Render
//------------------------------------------------------------------------------
void CParticleEffect::Copy(IRender_Visual* pFrom)
{
	Debug.fatal("Can't duplicate particle system - NOT IMPLEMENTED");
}

void CParticleEffect::OnDeviceCreate()
{
	if (m_Def){
		if (m_Def->m_Flags.is(CPEDef::dfSprite)){
			hGeom.create	(FVF::F_LIT, RCache.Vertex.Buffer(), RCache.QuadIB);
			if (m_Def) hShader = m_Def->m_CachedShader;
		}
	}
}

void CParticleEffect::OnDeviceDestroy()
{
	if (m_Def){
		if (m_Def->m_Flags.is(CPEDef::dfSprite)){
			hGeom.destroy		();
			hShader.destroy		();
		}    
	}
}
//----------------------------------------------------
IC void FillSprite	(FVF::LIT*& pv, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float r1, float r2, u32 clr, float angle)
{
	float sa	= _sin(angle);  
	float ca	= _cos(angle);  
	const Fvector& T 	= Device.vCameraTop;
	const Fvector& R 	= Device.vCameraRight;
	Fvector Vr, Vt;
	Vr.x 		= T.x*r1*sa+R.x*r1*ca;
	Vr.y 		= T.y*r1*sa+R.y*r1*ca;
	Vr.z 		= T.z*r1*sa+R.z*r1*ca;
	Vt.x 		= T.x*r2*ca-R.x*r2*sa;
	Vt.y 		= T.y*r2*ca-R.y*r2*sa;
	Vt.z 		= T.z*r2*ca-R.z*r2*sa;

	Fvector 	a,b,c,d;
	a.sub		(Vt,Vr);
	b.add		(Vt,Vr);
	c.invert	(a);
	d.invert	(b);
	pv->set		(d.x+pos.x,d.y+pos.y,d.z+pos.z, clr, lt.x,rb.y);	pv++;
	pv->set		(a.x+pos.x,a.y+pos.y,a.z+pos.z, clr, lt.x,lt.y);	pv++;
	pv->set		(c.x+pos.x,c.y+pos.y,c.z+pos.z, clr, rb.x,rb.y);	pv++;
	pv->set		(b.x+pos.x,b.y+pos.y,b.z+pos.z,	clr, rb.x,lt.y);	pv++;
}

IC void FillSprite	(FVF::LIT*& pv, const Fvector& pos, const Fvector& dir, const Fvector2& lt, const Fvector2& rb, float r1, float r2, u32 clr, float angle)
{
	float sa	= _sin(angle);  
	float ca	= _cos(angle);  
	const Fvector& T 	= dir;
	Fvector R; 	R.crossproduct(T,Device.vCameraDirection).normalize_safe();
	Fvector Vr, Vt;
	Vr.x 		= T.x*r1*sa+R.x*r1*ca;
	Vr.y 		= T.y*r1*sa+R.y*r1*ca;
	Vr.z 		= T.z*r1*sa+R.z*r1*ca;
	Vt.x 		= T.x*r2*ca-R.x*r2*sa;
	Vt.y 		= T.y*r2*ca-R.y*r2*sa;
	Vt.z 		= T.z*r2*ca-R.z*r2*sa;

	Fvector 	a,b,c,d;
	a.sub		(Vt,Vr);
	b.add		(Vt,Vr);
	c.invert	(a);
	d.invert	(b);
	pv->set		(d.x+pos.x,d.y+pos.y,d.z+pos.z, clr, lt.x,rb.y);	pv++;
	pv->set		(a.x+pos.x,a.y+pos.y,a.z+pos.z, clr, lt.x,lt.y);	pv++;
	pv->set		(c.x+pos.x,c.y+pos.y,c.z+pos.z, clr, rb.x,rb.y);	pv++;
	pv->set		(b.x+pos.x,b.y+pos.y,b.z+pos.z,	clr, rb.x,lt.y);	pv++;
}
void CParticleEffect::Render(float LOD)
{
	u32			dwOffset,dwCount;
	// Get a pointer to the particles in gp memory
    PAPI::Particle* particles;
    u32 			p_cnt;
    ParticleManager()->GetParticles(m_HandleEffect,particles,p_cnt);
    
	if(p_cnt>0){
		if (m_Def&&m_Def->m_Flags.is(CPEDef::dfSprite)){
			FVF::LIT* pv_start	= (FVF::LIT*)RCache.Vertex.Lock(p_cnt*4*4,hGeom->vb_stride,dwOffset);
			FVF::LIT* pv		= pv_start;

			for(u32 i = 0; i < p_cnt; i++){
				PAPI::Particle &m = particles[i];

				Fvector2 lt,rb;
				lt.set			(0.f,0.f);
				rb.set			(1.f,1.f);
				if (m_Def->m_Flags.is(CPEDef::dfFramed)) m_Def->m_Frame.CalculateTC(iFloor(float(m.frame)/255.f),lt,rb);
				float r_x		= m.size.x*0.5f;
				float r_y		= m.size.y*0.5f;
				if (m_Def->m_Flags.is(CPEDef::dfVelocityScale)){
					float speed	= m.vel.magnitude();
					r_x			+= speed*m_Def->m_VelocityScale.x;
					r_y			+= speed*m_Def->m_VelocityScale.y;
				}
				if (m_Def->m_Flags.is(CPEDef::dfAlignToPath)){
					Fvector 	dir;
					float speed	= m.vel.magnitude();
					if (speed>=EPS_S)	dir.div	(m.vel,speed);
					else				dir.setHP(-m_Def->m_APDefaultRotation.y,-m_Def->m_APDefaultRotation.x);
					if (m_RT_Flags.is(flRT_XFORM)){
						Fvector p,d;
						m_XFORM.transform_tiny	(p,m.pos);
						m_XFORM.transform_dir	(d,dir);
						FillSprite	(pv,p,d,lt,rb,r_x,r_y,m.color,m.rot.x);
					}else{
						FillSprite	(pv,m.pos,dir,lt,rb,r_x,r_y,m.color,m.rot.x);
					}
				}else{
					if (m_RT_Flags.is(flRT_XFORM)){
						Fvector p;
						m_XFORM.transform_tiny	(p,m.pos);
						FillSprite	(pv,p,lt,rb,r_x,r_y,m.color,m.rot.x);
					}else{
						FillSprite	(pv,m.pos,lt,rb,r_x,r_y,m.color,m.rot.x);
					}
				}
			}
			dwCount 			= u32(pv-pv_start);
			RCache.Vertex.Unlock(dwCount,hGeom->vb_stride);
			if (dwCount)    {
				RCache.set_xform_world	(Fidentity);
				RCache.set_Geometry		(hGeom);
				RCache.Render	   		(D3DPT_TRIANGLELIST,dwOffset,0,dwCount,0,dwCount/2);
			}
		}
	}
}
