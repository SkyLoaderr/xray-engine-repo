//----------------------------------------------------
// file: PSVisual.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "fmesh.h"
#include "PSVisual.h"
#include "PSRuntime.h"
#include "PSLibrary.h"

CPSVisual::CPSVisual():CVisual()
{
	m_Stream	= Device.Streams.Create(FVF::F_TL,MAX_PARTICLES*4);
	
	m_Definition= 0;
	m_Emitter	= 0;
    m_Particles.clear();
}
//----------------------------------------------------
CPSVisual::~CPSVisual()
{
	m_Particles.clear();
	m_Stream	= 0;
}
//----------------------------------------------------
void CPSVisual::Copy(CVisual* pFrom)
{
	Device.Fatal("Can't duplicate particle system - NOT IMPLEMENTED");
}
//----------------------------------------------------
//////////////////////////////////////////////////
// - доделать рождение/умирание без удаления и добавления в массив
// 
void CPSVisual::Update(DWORD dt)
{
	float fTime		= Device.fTimeGlobal;
	float dT		= float(dt)/1000.f;
	
	// calculate number of particles to destroy
	int iCount_Destroy	= 0;
	for (int i=0; i<int(m_Particles.size()); i++)
		if (fTime>m_Particles[i].m_Time.end)	iCount_Destroy++;
		
	// calculate how many particles we should create from ParticlesPerSec and time elapsed
	int iCount_Create	= m_Emitter->CalculateBirth(m_Particles.size() - iCount_Destroy, fTime, dT);
		
	// create/destroy/simulate particles that we own
	float TM	 		= fTime-dT;
	float dT_delta 		= dT/(iCount_Create?iCount_Create:1);
	float p_size		= 0;
	Fvector Pos; float size;
	bv_BBox.invalidate	();
	for (i=0; i<int(m_Particles.size()); i++)
	{
		if (fTime>m_Particles[i].m_Time.end) {
			// Need to delete particle
			if (iCount_Create)	{
				// Replace
				m_Emitter->GenerateParticle(m_Particles[i], m_Definition, TM);
				TM				+= dT_delta;
				iCount_Create	-= 1;
			} else {
				// Erase
				m_Particles.erase	(m_Particles.begin()+i);
				i--;
				continue;
			}
		}
		
		// Simulate this particle
		SParticle& P	= m_Particles[i];
		float T 		= fTime-P.m_Time.start;
		float k 		= T/(P.m_Time.end-P.m_Time.start);
		
		PS::SimulatePosition(Pos, &P,T,k);		bv_BBox.modify		(Pos);
		PS::SimulateSize	(size,&P,k,1-k);	if (size>p_size)	p_size = size;
	}
	
	// if we need to create somewhat more particles...
	while (iCount_Create) {
		// Create
		m_Particles.push_back		(SParticle());
		SParticle& P				=	m_Particles.back();
		m_Emitter->GenerateParticle	(P, m_Definition, TM);
		TM							+=	dT_delta;
		iCount_Create				-=	1;

		// Simulate
		float T 					=	fTime-P.m_Time.start;
		float k 					=	T/(P.m_Time.end-P.m_Time.start);
		PS::SimulatePosition		(Pos, &P,T,k);		bv_BBox.modify		(Pos);
		PS::SimulateSize			(size,&P,k,1-k);	if (size>p_size)	p_size = size;
	}
	
	if (m_Particles.empty())	{
		bv_BBox.set			(m_Emitter->m_Position, m_Emitter->m_Position);
		bv_BBox.grow		(0.1f);
		bv_BBox.getsphere	(bv_Position,bv_Radius);
	} else {
		bv_BBox.grow		(p_size);
		bv_BBox.getsphere	(bv_Position,bv_Radius);
	}
}

//----------------------------------------------------
IC void FillSprite(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float radius, DWORD clr, float angle, float scale)
{
	FVF::TL			PT;

	PT.transform	(pos, M);
	float sz		= scale * Device.dwWidth * radius / PT.p.w;
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
	c.x			= Device._x2real(PT.p.x);
	c.y			= Device._y2real(PT.p.y);
	// Rotation
	float	_sin1,_cos1,_sin2,_cos2,da;
	da = angle;		 _sincos	(da,_sin1,_cos1);
	da += PI_DIV_2;  _sincos	(da,_sin2,_cos2);
	
	pv->set	(c.x+sz*_sin1,	c.y+sz*_cos1,	PT.p.z, PT.p.w, clr, lt.x,rb.y);	pv++;
	pv->set	(c.x-sz*_sin2,	c.y-sz*_cos2,	PT.p.z, PT.p.w, clr, lt.x,lt.y);	pv++;
	pv->set	(c.x+sz*_sin2,	c.y+sz*_cos2,	PT.p.z, PT.p.w, clr, rb.x,rb.y);	pv++;
	pv->set	(c.x-sz*_sin1,	c.y-sz*_cos1,	PT.p.z, PT.p.w, clr, rb.x,lt.y);	pv++;
}

IC void FillSprite	(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float radius, DWORD clr, const Fvector& D, float scale)
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

	pv->set			(Device._x2real(s1.p.x+l1*R.x),	Device._y2real(s1.p.y+l1*R.y),	s2.p.z, s2.p.w, clr, lt.x,rb.y);	pv++;
	pv->set			(Device._x2real(s2.p.x+l2*R.x),	Device._y2real(s2.p.y+l2*R.y),	s2.p.z, s2.p.w, clr, lt.x,lt.y);	pv++;
	pv->set			(Device._x2real(s1.p.x-l1*R.x),	Device._y2real(s1.p.y-l1*R.y),	s2.p.z, s2.p.w, clr, rb.x,rb.y);	pv++;
	pv->set			(Device._x2real(s2.p.x-l2*R.x),	Device._y2real(s2.p.y-l2*R.y),	s2.p.z, s2.p.w, clr, rb.x,lt.y);	pv++;
}

void CPSVisual::Render		(float LOD)
{
	DWORD			vOffset;
	FVF::TL*		pv		= (FVF::TL*)m_Stream->Lock(m_Particles.size()*4,vOffset);
	DWORD			dwCount	= RenderTO(pv);
	m_Stream->Unlock(dwCount);
	if (dwCount)
		Device.Primitive.Draw	(m_Stream,dwCount,dwCount/2,vOffset,Device.Streams_QuadIB);
}
 
DWORD CPSVisual::RenderTO	(FVF::TL* dest)
{
	float fTime			= Device.fTimeGlobal;
	
	// build transform matrix
	Fmatrix mSpriteTransform	= Device.mFullTransform;
	float	fov_scale			= 1/(Device.fFOV/90.f);
	
    int 	mb_samples 	= 1;
    float 	mb_step 	= 0;
	
	Fvector2 lt,rb;
    lt.set	(0.f,0.f);
	rb.set	(1.f,1.f);
	
	// actual rendering
	bv_BBox.invalidate	();
	float			p_size	= 0;
	FVF::TL*		pv_start= dest;
	FVF::TL*		pv		= pv_start;
	for (PS::ParticleIt P=m_Particles.begin(); P!=m_Particles.end(); P++)
	{
		DWORD 	C;
		float 	sz;
		float 	angle;
        if (m_Definition->m_dwFlag&PS_MOTIONBLUR)
		{
            float T 	=	fTime-P->m_Time.start;
            float k 	=	T/(P->m_Time.end-P->m_Time.start);
            float k_inv =	1-k;
            mb_samples 	=	iFloor(m_Definition->m_BlurSubdiv.start*k_inv+m_Definition->m_BlurSubdiv.end*k+0.5f);
            mb_step 	=	m_Definition->m_BlurTime.start*k_inv+m_Definition->m_BlurTime.end*k;
            mb_step		/=	mb_samples;
        }
		
        // update
		for (int sample=mb_samples-1; sample>=0; sample--)
		{
            float T 	=	fTime-P->m_Time.start-(sample*mb_step);
            if (T<0)		continue;
            float mb_v	=	1-float(sample)/float(mb_samples);
            float k 	=	T/(P->m_Time.end-P->m_Time.start);
			if ((m_Emitter->m_dwFlag&PS_EM_PLAY_ONCE) && (k>1)) continue;
            float k_inv =	1-k;
			
            Fvector Pos;
            
            PS::SimulatePosition(Pos,P,T,k);		// this moves the particle using the last known velocity and the time that has passed
			bv_BBox.modify		(Pos);
            PS::SimulateColor	(C,P,k,k_inv,mb_v);	// adjust current Color from calculated Deltas and time elapsed.
            PS::SimulateSize	(sz,P,k,k_inv);		// adjust current Size & Angle
			if (sz>p_size)		p_size = sz;
			
            Fvector D;
			if (m_Definition->m_dwFlag&PS_ALIGNTOPATH) {
				Fvector p;
                float PT = T-0.1f;
				float kk = PT/(P->m_Time.end-P->m_Time.start);
                PS::SimulatePosition(p,P,PT,kk);
				D.sub				(Pos,p);
                D.normalize_safe	();
				
				if (m_Definition->m_dwFlag&PS_FRAME_ENABLED){
					int frame;
					if (m_Definition->m_dwFlag&PS_FRAME_ANIMATE)PS::SimulateAnimation(frame,m_Definition,P,T);
					else										frame = P->m_iAnimStartFrame;
					m_Definition->m_Animation.CalculateTC(frame,lt,rb);
				}
				FillSprite(pv,mSpriteTransform,Pos,lt,rb,sz*.5f,C,D,fov_scale);
			}else{
				PS::SimulateAngle	(angle,P,T,k,k_inv);
				
				if (m_Definition->m_dwFlag&PS_FRAME_ENABLED){
					int frame;
					if (m_Definition->m_dwFlag&PS_FRAME_ANIMATE)PS::SimulateAnimation(frame,m_Definition,P,T);
					else										frame = P->m_iAnimStartFrame;
					m_Definition->m_Animation.CalculateTC(frame,lt,rb);
				}
				FillSprite(pv,mSpriteTransform,Pos,lt,rb,sz*.5f,C,angle,fov_scale);
            }
        }
    }
	if (m_Particles.empty())	{
		bv_BBox.set			(m_Emitter->m_Position,m_Emitter->m_Position);
		bv_BBox.grow		(0.1f);
		bv_BBox.getsphere	(bv_Position,bv_Radius);
	} else {
		bv_BBox.grow		(p_size);
		bv_BBox.getsphere	(bv_Position,bv_Radius);
	}
	
	return pv-pv_start;
}

//----------------------------------------------------
void CPSVisual::Compile(PS::SDef_RT* source, PS::SEmitter* E)
{
    hShader				= source->m_CachedShader;
	m_Definition		= source;

	// set default emitter data
	VERIFY				(E);
    m_Emitter			= E;
	m_Emitter->Compile	(&source->m_DefaultEmitter);
}
//----------------------------------------------------
