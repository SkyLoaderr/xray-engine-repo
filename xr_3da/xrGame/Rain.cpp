// Rain.cpp: implementation of the CEffect_Rain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Rain.h"
#include "..\2dsound.h"
#include "..\fstaticrender.h"

const float snd_fade		= 0.1f;
const int	desired_items	= 1000;
const float	drop_length		= 0.5f;
const float drop_width		= 0.1f;
const float drop_angle		= 1.f;
const float drop_speed_min	= 10.f;
const float drop_speed_max	= 20.f;

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
}

void	CEffect_Rain::OnDeviceDestroy	()
{
	REQ_DESTROY			();
	Device.Shader.Delete(SH);
}

void	CEffect_Rain::OnDeviceCreate	()
{
	REQ_CREATE			();
	SH					= Device.Shader.Create	("effects\\rain","effects\\rain");
	VS					= Device.Streams.Create	(FVF::F_LIT,desired_items*4);
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
	float		x		= radius*cosf		(angle);
	float		z		= radius*sinf		(angle);
	dest.P.set			(x+view.x,height+view.y,z+view.z);
	dest.D.random_dir	(axis,deg2rad(drop_angle));
	dest.fSpeed			= ::Random.randF	(drop_speed_min,drop_speed_max);
	dest.fLifetime		= (height*2)/dest.fSpeed;
}

// delete node from list
void CEffect_Rain::p_remove	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	Particle*	prev		= P->prev;
	Particle*	next		= P->next;
	if (prev) prev->next	= next;
	if (next) next->prev	= prev;
	if ((0==prev) && (0==next))	LST = 0;
}
void CEffect_Rain::p_insert	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	P->prev					= 0;
	P->next					= LST;
	if (LST)	LST->prev	= P;
	LST						= P;
}

CEffect_Rain::Particle*	CEffect_Rain::p_allocate	()
{
	Particle*	P			= particle_idle;
	if (0==P)				return NULL;
	p_remove	(P,particle_idle);
	p_insert	(P,particle_active);
	return		P;
}

void	CEffect_Rain::p_free(Particle* P)
{
	p_remove	(P,particle_active);
	p_insert	(P,particle_idle);
}

void	CEffect_Rain::Hit		(Fvector& pos)
{
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
	float	b_height		= 50.f;
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
		
		float	height	=	HM.Query(one.P.x,one.P.z);
		if (height>one.P.y)	{
			one.P.y		= height;
			Hit			(one.P);
			Born		(one,b_radius,b_height);
		}
		Fvector&	pos_head	= one.P;
		Fvector		pos_trail;	pos_trail.mad	(pos_head,one.P,-drop_length);
		
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
		P.mad(pos_trail,lineTop,-w);	verts->set(P,0,0,1);	verts++;
		P.mad(pos_trail,lineTop,w);		verts->set(P,0,0,0);	verts++;
		P.mad(pos_head, lineTop,-w);	verts->set(P,0,1,1);	verts++;
		P.mad(pos_head, lineTop,w);		verts->set(P,0,1,0);	verts++;
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
}
