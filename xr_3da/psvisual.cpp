//----------------------------------------------------
// file: PSVisual.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "fmesh.h"
#include "PSVisual.h"
#include "ParticleSystem.h"
#include "PSLibrary.h"

static int dwPCNT=0;

CPSVisual::CPSVisual():FBasicVisual(){
	m_Stream	= Device.Streams.Create(FVF::F_TL,MAX_PARTICLES*4);
	
	m_Definition= 0;
	m_Emitter	= 0;
    m_Particles.clear();

	dwPCNT++;
}
//----------------------------------------------------

CPSVisual::~CPSVisual(){
	dwPCNT--;
	m_Particles.clear();
	m_Stream	= 0;
}
//----------------------------------------------------

void CPSVisual::Copy(FBasicVisual* pFrom){
}
//----------------------------------------------------

//////////////////////////////////////////////////
// - доделать рождение/умирание без удаления и добавления в массив
// 
void CPSVisual::Update(DWORD dt)
{
	float fTime = Device.fTimeGlobal;
	float dT = float(dt)/1000.f;
	
	// update visual params
	bv_Position.set	(m_Emitter->m_Position);
	bv_BBox.set		(m_Emitter->m_Position,m_Emitter->m_Position);
	bv_BBox.grow	(2.f);
	// update visual radius
	bv_Radius = bv_BBox.getradius();

	// update all particles that we own
    for (int i=0; i<int(m_Particles.size()); i++)
	{
    	SParticle& P = m_Particles[i];
 		if (fTime>P.m_Time.end){
        	m_Particles.erase(m_Particles.begin()+i);
            i--;
        }
    }

	// calculate how many particles we should create from ParticlesPerSec and time elapsed taking the
	int iParticlesCreated = m_Emitter->CalculateBirth(m_Particles.size(),fTime,dT);

    if (iParticlesCreated)
	{
        float TM	 	= fTime-dT;
        float dT_delta 	= dT/iParticlesCreated;
        // see if actually have any to create
        for (i=0; i<iParticlesCreated; i++, TM+=dT_delta)
		{
            m_Particles.push_back(SParticle());
            SParticle& P = m_Particles.back();

            m_Emitter->GenerateParticle(P,m_Definition,TM);
        }
    }
}

//----------------------------------------------------
IC void FillSprite(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float radius, DWORD clr, float angle)
{
	FVF::TL			PT;

	PT.transform	(pos, M);
	float sz		= Device.dwWidth * radius/PT.p.w;
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
	da = angle;		_sincos	(da,_sin1,_cos1);
	da += PI_DIV_2;	_sincos	(da,_sin2,_cos2);
	
	pv->set	(c.x+sz*_sin1,	c.y+sz*_cos1,	PT.p.z, PT.p.w, clr, lt.x,rb.y);	pv++;
	pv->set	(c.x-sz*_sin2,	c.y-sz*_cos2,	PT.p.z, PT.p.w, clr, lt.x,lt.y);	pv++;
	pv->set	(c.x+sz*_sin2,	c.y+sz*_cos2,	PT.p.z, PT.p.w, clr, rb.x,rb.y);	pv++;
	pv->set	(c.x-sz*_sin1,	c.y-sz*_cos1,	PT.p.z, PT.p.w, clr, rb.x,lt.y);	pv++;
}

IC void FillSprite(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float radius, DWORD clr, const Fvector& D)
{
	Fvector			P1,P2;
    P1.direct		(pos,D,-radius);
    P2.direct		(pos,D,radius);

	FVF::TL			s1,s2;
	s1.transform	(P1,M);
	s2.transform	(P2,M);

    if ((s1.p.w<=0)||(s2.p.w<=0)) return;

	float l1 = radius/s1.p.w*1.4142f;
    float l2 = radius/s2.p.w*1.4142f;

	Fvector2		dir,R;
	dir.set			(s2.p.x-s1.p.x,s2.p.y-s1.p.y);
	dir.norm		();
	R.cross			(dir);

	pv->set			(Device._x2real(s1.p.x+l1*R.x),	Device._y2real(s1.p.y+l1*R.y),	s2.p.z, s2.p.w, clr, lt.x,rb.y);	pv++;
	pv->set			(Device._x2real(s2.p.x+l2*R.x),	Device._y2real(s2.p.y+l2*R.y),	s2.p.z, s2.p.w, clr, lt.x,lt.y);	pv++;
	pv->set			(Device._x2real(s1.p.x-l1*R.x),	Device._y2real(s1.p.y-l1*R.y),	s2.p.z, s2.p.w, clr, rb.x,rb.y);	pv++;
	pv->set			(Device._x2real(s2.p.x-l2*R.x),	Device._y2real(s2.p.y-l2*R.y),	s2.p.z, s2.p.w, clr, rb.x,lt.y);	pv++;
}

void CPSVisual::Render(float LOD)
{
	float fTime			= Device.fTimeGlobal;
	
	// build transform matrix
	Fmatrix mSpriteTransform = Device.mFullTransform;
	
    int 	mb_samples 	= 1;
    float 	mb_step 	= 0;
	
	Fvector2 lt,rb;
    lt.set	(0.f,0.f);
	rb.set	(1.f,1.f);

	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv_start= (FVF::TL*)m_Stream->Lock(m_Particles.size()*4,vOffset);
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
            float T 	= fTime-P->m_Time.start-(sample*mb_step);
            if (T<0) continue;
            float mb_v	= 1-float(sample)/float(mb_samples);
            float k 	= T/(P->m_Time.end-P->m_Time.start);
			if ((m_Emitter->m_dwFlag&PS_EM_PLAY_ONCE) && (k>1)) continue;
            float k_inv = 1-k;
			
            Fvector Pos;
            
            PS::SimulatePosition(Pos,P,T,k);		// this moves the particle using the last known velocity and the time that has passed
            
            PS::SimulateColor	(C,P,k,k_inv,mb_v);	// adjust current Color from calculated Deltas and time elapsed.
            PS::SimulateSize	(sz,P,k,k_inv);		// adjust current Size & Angle
			
            Fvector D;
			if (m_Definition->m_dwFlag&PS_ALIGNTOPATH){
				Fvector p;
                float PT = T-0.1f;
				float kk = PT/(P->m_Time.end-P->m_Time.start);
                PS::SimulatePosition(p,P,PT,kk);
				D.sub(Pos,p);
                D.normalize_safe();
			}else{
				PS::SimulateAngle(angle,P,T,k,k_inv);
            }
			
			// Animation
			if (m_Definition->m_dwFlag&PS_FRAME_ENABLED){
				int frame;
				if (m_Definition->m_dwFlag&PS_FRAME_ANIMATE)PS::SimulateAnimation(frame,m_Definition,P,T);
				else										frame = P->m_iAnimStartFrame;
                m_Definition->m_Animation.CalculateTC(frame,lt,rb);
            }
			if (m_Definition->m_dwFlag&PS_ALIGNTOPATH) 	FillSprite(pv,mSpriteTransform,Pos,lt,rb,sz*.5f,C,D);
			else                                     	FillSprite(pv,mSpriteTransform,Pos,lt,rb,sz*.5f,C,angle);
			
			// modify visual bbox
			bv_BBox.modify(Pos);
        }
    }
	
	// unlock VB and Render it as triangle list
	DWORD dwNumVerts = pv-pv_start;
	m_Stream->Unlock(dwNumVerts);
	if (dwNumVerts)
	{
		Device.Shader.Set		(hShader);
		Device.Primitive.Draw	(m_Stream,dwNumVerts,dwNumVerts/2,vOffset,Device.Streams_QuadIB);
	}
}

//----------------------------------------------------
void CPSVisual::Compile(LPCSTR name, PS::SEmitter* E){
	PS::SDef* source= Device.PSLib.FindPS(name);
	R_ASSERT		(source);
	R_ASSERT		(source->m_ShaderName[0]&&source->m_TextureName[0]);
    hShader			= Device.Shader.Create(source->m_ShaderName,source->m_TextureName);
	m_Definition	= source;
	// set default emitter data
	VERIFY			(E);
    m_Emitter		= E;
	m_Emitter->Compile(&source->m_DefaultEmitter);
}
//----------------------------------------------------
