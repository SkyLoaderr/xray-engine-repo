//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "render.h"
#include "psystem.h"

using namespace PAPI;
using namespace PS;

#pragma comment(lib,"x:\\ParticleDLL.lib")

void CParticleGroup::Copy(IVisual* pFrom)
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
	da = angle;		 _sincos	(da,_sin1,_cos1);
	da += PI_DIV_2;  _sincos	(da,_sin2,_cos2);

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

u32 CParticleGroup::RenderTO	(FVF::TL* dest)
{
	// Get a pointer to the particles in gp memory
	PAPI::ParticleGroup *pg = PAPI::_GetGroupPtr(m_HandleGroup);

	if(pg == NULL)		return 0; // ERROR
	if(pg->p_count < 1)	return 0;

	// build transform matrix
	Fmatrix mSpriteTransform	= Device.mFullTransform;

	float	w_2					= float(::Render->getTarget()->get_width()) / 2;
	float	h_2					= float(::Render->getTarget()->get_height()) / 2;
	float	fov_scale			= float(::Render->getTarget()->get_width()) / (Device.fFOV/90.f);

	float			p_size	= 0;
	FVF::TL*		pv_start= dest;
	FVF::TL*		pv		= pv_start;

	for(int i = 0; i < pg->p_count; i++){
		PAPI::Particle &m = pg->list[i];

		Fcolor c;
		c.set(m.color.x,m.color.y,m.color.z,m.alpha);
		u32 C = c.get();
		Fvector2 lt,rb;
		lt.set		(0.f,0.f);
		rb.set		(1.f,1.f);
		if (m_Def->m_Flags.is(CPGDef::flFramed)){
			//        	||m_Flags.test(flAnimated)){
			m_Def->m_Frame.CalculateTC(iFloor(m.frame),lt,rb);
			FillSprite(pv,mSpriteTransform,(Fvector&)m.pos,lt,rb,m.size.x*.5f,C,m.rot.x,fov_scale,w_2,h_2);
		}else
			FillSprite(pv,mSpriteTransform,(Fvector&)m.pos,lt,rb,m.size.x*.5f,C,m.rot.x,fov_scale,w_2,h_2);
	}
	return pv-pv_start;
}

void CParticleGroup::Render		(float LOD)
{
	u32			vOffset;
	ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);
	FVF::TL*	pv			= (FVF::TL*)RCache.Vertex.Lock(pg->p_count*4,hGeom->vb_stride,vOffset);
	u32			dwCount		= RenderTO(pv);
	RCache.Vertex.Unlock(dwCount,hGeom->vb_stride);
	if (dwCount)    {
		RCache.set_Geometry		(hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,dwCount,0,dwCount/2);
	}
}
