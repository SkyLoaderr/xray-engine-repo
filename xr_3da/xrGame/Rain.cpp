// Rain.cpp: implementation of the CEffect_Rain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Rain.h"
#include "..\2dsound.h"
#include "..\fstaticrender.h"
#include "..\psvisual.h"

const float snd_fade		= 0.2f;
const int	desired_items	= 2000;
const float	drop_length		= 1.5f;
const float drop_width		= 0.025f;
const float drop_angle		= 3.f;
const float drop_speed_min	= 40.f;
const float drop_speed_max	= 80.f;
const int	max_particles	= 300;
const int	particles_update= 50;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffect_Rain::CEffect_Rain()
{
	state				= stIdle;
	control_start		= Engine.Event.Handler_Attach	("level.weather.rain.start",this);
	control_stop		= Engine.Event.Handler_Attach	("level.weather.rain.stop",this);
	
	snd_Ambient			= pSounds->Create2D	("amb_rain");
	snd_Ambient_control	= 0;
	snd_Ambient_volume	= 0;

	Device.seqDevCreate.Add	(this);
	Device.seqDevDestroy.Add(this);

	OnDeviceCreate		();
}

CEffect_Rain::~CEffect_Rain()
{
	Engine.Event.Handler_Detach		(control_stop,this);
	Engine.Event.Handler_Detach		(control_start,this);

	pSounds->Delete2D				(snd_Ambient);

	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);

	OnDeviceDestroy		();
	Device.Shader.Delete(SH);
	p_destroy			();
}

void	CEffect_Rain::OnDeviceDestroy	()
{
	REQ_DESTROY			();
	Device.Shader.Delete(SH);
	p_destroy			();
}

void	CEffect_Rain::OnDeviceCreate	()
{
	REQ_CREATE			();
	SH					= Device.Shader.Create	("effects\\rain","effects\\rain");
	VS					= Device.Streams.Create	(FVF::F_LIT,desired_items*4);
	p_create			();
}

void	CEffect_Rain::OnEvent	(EVENT E, DWORD P1, DWORD P2)
{
	if ((E==control_start) && (state!=stWorking))	{
		state				= stStarting;
		snd_Ambient_control	= pSounds->Play2D(snd_Ambient,TRUE);
		snd_Ambient_control->SetVolume	(snd_Ambient_volume);
	} else if ((E==control_stop) && (state!=stIdle))	{
		state				= stStopping;
	}
}

void	CEffect_Rain::Born		(Item& dest, float radius, float height)
{
	Fvector		axis;	axis.set			(0,-1,0);
	Fvector&	view	= Device.vCameraPosition;
	float		angle	= ::Random.randF	(0,PI_MUL_2);
	float		dist	= ::Random.randF	(0,radius);
	float		x		= dist*cosf			(angle);
	float		z		= dist*sinf			(angle);
	dest.P.set			(x+view.x,height+view.y,z+view.z);
	dest.D.random_dir	(axis,deg2rad(drop_angle));
	dest.fSpeed			= ::Random.randF	(drop_speed_min,drop_speed_max);
	dest.fLifetime		= (height*3)/dest.fSpeed;

	// Raytrace
	Collide::ray_query	RQ;
	if (Level().ObjectSpace.RayPick(dest.P,dest.D,height*2,RQ))	dest.fHeight = dest.P.y-RQ.range;
	else dest.fHeight	= dest.P.y - 2*height;
}

// initialize particles pool
void CEffect_Rain::p_create		()
{
	// pool
	particle_pool.resize	(max_particles);
	for (DWORD it=0; it<particle_pool.size(); it++)
	{
		Particle&	P	= particle_pool[it];
		P.prev			= it?(&particle_pool[it-1]):0;
		P.next			= (it<(particle_pool.size()-1))?(&particle_pool[it+1]):0;
		P.visual		= (CPSVisual*) ::Render.Models.CreatePS("effects\\rain",&P.emitter);
	}
	
	// active and idle lists
	particle_active	= 0;
	particle_idle	= &particle_pool.front();
}

// destroy particles pool
void CEffect_Rain::p_destroy	()
{
	// active and idle lists
	particle_active	= 0;
	particle_idle	= 0;
	
	// pool
	for (DWORD it=0; it<particle_pool.size(); it++)
	{
		Particle&	P	= particle_pool[it];
		FBasicVisual* V = (FBasicVisual*)P.visual;
		::Render.Models.Delete	(V);
	}
	particle_pool.clear	();
}

// delete node from list
void CEffect_Rain::p_remove	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	Particle*	prev		= P->prev;	P->prev = NULL;
	Particle*	next		= P->next;	P->next	= NULL;
	if (prev) prev->next	= next;
	if (next) next->prev	= prev;
	if (LST==P)	LST			= next;
//	if ((0==prev) && (0==next))	LST = 0;
}

// insert node at the top of the head
void CEffect_Rain::p_insert	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	P->prev					= 0;
	P->next					= LST;
	if (LST)	LST->prev	= P;
	LST						= P;
}

// determine size of list
int CEffect_Rain::p_size	(Particle* P)
{
	if (0==P)	return 0;
	int cnt = 0;
	while (P)	{
		P	=	P->next;
		cnt +=	1;
	}
	return cnt;
}

// alloc node
CEffect_Rain::Particle*	CEffect_Rain::p_allocate	()
{
	Particle*	P			= particle_idle;
	if (0==P)				return NULL;
	p_remove	(P,particle_idle);
	p_insert	(P,particle_active);
	return		P;
}

// free node
void	CEffect_Rain::p_free(Particle* P)
{
	p_remove	(P,particle_active);
	p_insert	(P,particle_idle);
}

// startup new particle system
void	CEffect_Rain::Hit		(Fvector& pos)
{
	Particle*	P	= p_allocate();
	if (0==P)	return;

	P->dwNextUpdate				= Device.dwTimeGlobal;
	P->emitter.m_Position.set	(pos);
	P->emitter.Play				();
}

void	CEffect_Rain::Render	()
{
	// Parse states
	BOOL	bBornNewItems;
	switch (state)
	{
	case stIdle:		return;
	case stStarting:	
		snd_Ambient_volume	+= snd_fade*Device.fTimeDelta;
		snd_Ambient_control->SetVolume	(snd_Ambient_volume);
		if (snd_Ambient_volume > 1)	state=stWorking;
		bBornNewItems	= TRUE;
		break;
	case stWorking:		
		bBornNewItems	= TRUE;
		break;
	case stStopping:
		snd_Ambient_volume	-= snd_fade*Device.fTimeDelta;
		snd_Ambient_control->SetVolume	(snd_Ambient_volume);
		if (snd_Ambient_volume < 0)	{
			snd_Ambient_control->Stop	();
			state=stIdle;
		}
		bBornNewItems	= FALSE;
		break;
	}
	
	// Born new if needed
	float	b_radius		= 30.f;	// Need to ask from heightmap
	float	b_height		= 40.f;
	if (bBornNewItems && (items.size()<desired_items))	{
		items.reserve	(desired_items);
		while (items.size()<desired_items)	{
			Item			one;
			Born			(one,b_radius,b_height);
			items.push_back	(one);
		}
	}
	
	// Perform update
	DWORD		vOffset;
	CHeightMap&	HM			= Level().HeightMap;
	FVF::LIT	*verts		= (FVF::LIT	*) VS->Lock(desired_items*4,vOffset);
	FVF::LIT	*start		= verts;
	float		dt			= Device.fTimeDelta;
	Fvector		vCenter		= Device.vCameraPosition;
	for (DWORD I=0; I<items.size(); I++)
	{
		// Physics and time control
		Item&	one		=	items[I];
		one.fLifetime	-=	dt;
		if (one.fLifetime<0)	Born(one,b_radius,b_height);
		one.P.mad		(one.D,one.fSpeed*dt);
		
		if (one.fHeight>one.P.y)	{
			one.P.y		= one.fHeight;
			Hit			(one.P);
			Born		(one,b_radius,b_height);
		}
		Fvector&	pos_head	= one.P;
		Fvector		pos_trail;	pos_trail.mad	(pos_head,one.D,-drop_length);
		
		// Culling
		Fvector sC,lineD;	float sR; 
		sC.sub	(pos_head,pos_trail);
		lineD.normalize(sC);
		sC.mul	(.5f);
		sR		= sC.magnitude();
		sC.add	(pos_trail);
		if (!::Render.ViewBase.testSphereDirty(sC,sR))	continue;
		
		// Everything OK - build vertices
		Fvector	P,lineTop,camDir;
		camDir.sub			(sC,vCenter);
		camDir.normalize	();
		lineTop.crossproduct(camDir,lineD);
		float	w = drop_width;
		P.mad(pos_trail,lineTop,-w);	verts->set(P,0xffffffff,0,1);	verts++;
		P.mad(pos_trail,lineTop,w);		verts->set(P,0xffffffff,0,0);	verts++;
		P.mad(pos_head, lineTop,-w);	verts->set(P,0xffffffff,1,1);	verts++;
		P.mad(pos_head, lineTop,w);		verts->set(P,0xffffffff,1,0);	verts++;
	}
	DWORD vCount			= verts-start;
	VS->Unlock				(vCount);

	// Render if needed
	if (vCount)	{
		HW.pDevice->SetTransform	(D3DTS_WORLD, precalc_identity.d3d());
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);
		Device.Shader.set_Shader	(SH);
		Device.Primitive.Draw		(VS,vCount,vCount/2,vOffset,Device.Streams_QuadIB);
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
	}
	
	// Particles
	Particle*	P	= particle_active;
	DWORD	dwTime	= Device.dwTimeGlobal;
	if (P)	Device.Shader.set_Shader	(P->visual->hShader);
	while (P)	{
		Particle*	next	= P->next;

		// Update
		if (dwTime>=P->dwNextUpdate)
		{
			P->visual->Update	(dwTime - P->dwNextUpdate + particles_update);
			P->dwNextUpdate		= dwTime+particles_update;
		}
		
		// Render
		if (::Render.ViewBase.testSphereDirty(P->visual->bv_Position,P->visual->bv_Radius))
			P->visual->Render	(1.f);

		// Stop if needed
		if (P->emitter.m_dwFlag&PS_EM_PLAY_ONCE)
		{
			if ((0==P->visual->ParticleCount()) && !P->emitter.IsPlaying()) 
			{
				P->visual->Stop	();
				p_free			(P);
			}
		}
		
		P = next;
	}
}
