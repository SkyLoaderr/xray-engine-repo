// Rain.cpp: implementation of the CEffect_Rain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Rain.h"

const float snd_fade		= 0.2f;
const int	desired_items	= 2500;
const float	drop_length		= 1.5f;
const float drop_width		= 0.02f;
const float drop_angle		= 3.01f;
const float drop_speed_min	= 40.f;
const float drop_speed_max	= 80.f;

const int	max_particles	= 1000;
const int	particles_cache	= 400;
const float particles_time	= .3f;
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffect_Rain::CEffect_Rain()
{
	state				= stIdle;
	control_start		= Engine.Event.Handler_Attach	("level.weather.rain.start",this);
	control_stop		= Engine.Event.Handler_Attach	("level.weather.rain.stop",this);
	
	Sound->create		(snd_Ambient,TRUE,"amb_rain");
	snd_Ambient_volume	= 0;

	Device.seqDevCreate.Add	(this);
	Device.seqDevDestroy.Add(this);

	OnDeviceCreate		();
}

CEffect_Rain::~CEffect_Rain()
{
	Engine.Event.Handler_Detach		(control_stop,this);
	Engine.Event.Handler_Detach		(control_start,this);

	Sound->destroy					(snd_Ambient);

	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);

	// Cleanup
	OnDeviceDestroy		();
	Device.Shader.Delete(SH_Rain);
	p_destroy			();
	DM_Drop.Unload		();
}

void	CEffect_Rain::OnDeviceCreate	()
{
	REQ_CREATE			();

	string256			name;
	strconcat			(name,Path.Meshes,"dm\\rain.dm");
	IReader*	 fs		= FS.r_open(name);
	DM_Drop.Load		(fs);
	FS.r_close		(fs);

	SH_Rain				= Device.Shader.Create		("effects\\rain","fx\\rain");
	hGeom_Rain			= Device.Shader.CreateGeom	(FVF::F_LIT, RCache.Vertex.Buffer(), RCache.QuadIB);
	hGeom_Drops			= Device.Shader.CreateGeom	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RCache.Vertex.Buffer(), RCache.Index.Buffer());
	p_create			();
}

void	CEffect_Rain::OnDeviceDestroy	()
{
	REQ_DESTROY				();
	Device.Shader.DeleteGeom	(hGeom_Drops);
	Device.Shader.DeleteGeom	(hGeom_Rain);
	Device.Shader.Delete	(SH_Rain);
	p_destroy				();
	DM_Drop.Unload			();
}

void	CEffect_Rain::OnEvent	(EVENT E, u32 P1, u32 P2)
{
	if ((E==control_start) && (state!=stWorking))	{
		state				= stStarting;
		Sound->play			(snd_Ambient,0,TRUE);
		snd_Ambient.feedback->set_volume	(snd_Ambient_volume);
	} else if ((E==control_stop) && (state!=stIdle))	{
		state				= stStopping;
	}
}

// Born
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

	RayTest		(dest,height);
}

// Raytrace
void	CEffect_Rain::RayTest	(Item& dest, float height)
{
	Collide::ray_query	RQ;
	CObject* E = Level().CurrentViewEntity();
	E->setEnabled(FALSE);
	if (Level().ObjectSpace.RayPick(dest.P,dest.D,height*2,RQ))	
	{
		dest.fTime_Life	= RQ.range/dest.fSpeed;
		dest.fTime_Hit	= RQ.range/dest.fSpeed;
		dest.Phit.mad	(dest.P,dest.D,RQ.range);
	} else {
		dest.fTime_Life	= (height*2)/dest.fSpeed;
		dest.fTime_Hit	= (height*3)/dest.fSpeed;
		dest.Phit.set	(dest.P);
	}
	E->setEnabled(TRUE);
}

// initialize particles pool
void CEffect_Rain::p_create		()
{
	// pool
	particle_pool.resize	(max_particles);
	for (u32 it=0; it<particle_pool.size(); it++)
	{
		Particle&	P	= particle_pool[it];
		P.prev			= it?(&particle_pool[it-1]):0;
		P.next			= (it<(particle_pool.size()-1))?(&particle_pool[it+1]):0;
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
	particle_pool.clear	();
}

// _delete_ node from list
void CEffect_Rain::p_remove	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	Particle*	prev		= P->prev;	P->prev = NULL;
	Particle*	next		= P->next;	P->next	= NULL;
	if (prev) prev->next	= next;
	if (next) next->prev	= prev;
	if (LST==P)	LST			= next;
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

// xr_free node
void	CEffect_Rain::p_free(Particle* P)
{
	p_remove	(P,particle_active);
	p_insert	(P,particle_idle);
}

// startup _new_ particle system
void	CEffect_Rain::Hit		(Fvector& pos)
{
	if (0!=::Random.randI(2))	return;
	Particle*	P	= p_allocate();
	if (0==P)	return;

	P->time						= particles_time;
	P->mXForm.rotateY			(::Random.randF(PI_MUL_2));
	P->mXForm.translate_over	(pos);
	P->mXForm.transform_tiny	(P->bounds.P,DM_Drop.bv_sphere.P);
	P->bounds.R					= DM_Drop.bv_sphere.R;
}

void	CEffect_Rain::Render	()
{
	// Parse states
	BOOL	bBornNewItems	= FALSE;
	Fvector sndP;
	switch (state)
	{
	case stIdle:		return;
	case stStarting:	
		snd_Ambient_volume	+= snd_fade*Device.fTimeDelta;
		snd_Ambient.feedback->set_volume		(snd_Ambient_volume);
		sndP.mad							(Device.vCameraPosition,Device.vCameraDirection,.1f);
		snd_Ambient.feedback->set_position	(sndP);
		if (snd_Ambient_volume > 1)	state=stWorking;
		bBornNewItems	= TRUE;
		break;
	case stWorking:		
		bBornNewItems	= TRUE;
		break;
	case stStopping:
		snd_Ambient_volume	-= snd_fade*Device.fTimeDelta;
		snd_Ambient.feedback->set_volume	(snd_Ambient_volume);
		if (snd_Ambient_volume < 0)	{
			snd_Ambient.feedback->stop	();
			state=stIdle;
		}
		bBornNewItems	= FALSE;
		break;
	}
	
	// Born _new_ if needed
	float	b_radius		= 10.f;
	float	b_radius_wrap	= b_radius+.5f;
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
	u32		vOffset;
	FVF::LIT	*verts		= (FVF::LIT	*) RCache.Vertex.Lock(desired_items*4,hGeom_Rain->vb_stride,vOffset);
	FVF::LIT	*start		= verts;
	float		dt			= Device.fTimeDelta;
	Fvector		vCenter		= Device.vCameraPosition;
	for (u32 I=0; I<items.size(); I++)
	{
		// Physics and time control
		Item&	one		=	items[I];

		one.fTime_Hit	-=  dt;	if (one.fTime_Hit<0)	Hit (one.Phit);
		one.fTime_Life	-=	dt; if (one.fTime_Life<0)	Born(one,b_radius,b_height);

		one.P.mad		(one.D,one.fSpeed*dt);
		
		// Cylindrical wrap
		Fvector	wdir;	wdir.set(one.P.x-vCenter.x,0,one.P.z-vCenter.z);
		float	wlen	=	wdir.magnitude();
		if (wlen>b_radius_wrap)	{
			// Perform wrapping
			wdir.div	(wlen);
			one.P.mad	(one.P, wdir, -(wlen+b_radius));
			RayTest		(one, b_height);
		}

		// Build line
		Fvector&	pos_head	= one.P;
		Fvector		pos_trail;	pos_trail.mad	(pos_head,one.D,-drop_length);
		
		// Culling
		Fvector sC,lineD;	float sR; 
		sC.sub			(pos_head,pos_trail);
		lineD.normalize	(sC);
		sC.mul			(.5f);
		sR				= sC.magnitude();
		sC.add			(pos_trail);
		if (!::Render->ViewBase.testSphere_dirty(sC,sR))	continue;
		
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
	u32 vCount					= verts-start;
	RCache.Vertex.Unlock	(vCount,hGeom_Rain->vb_stride);
	
	// Render if needed
	if (vCount)	{
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);

		RCache.set_xform_world		(Fidentity);
		RCache.set_Shader			(SH_Rain);
		RCache.set_Geometry			(hGeom_Rain);
		RCache.Render				(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);

		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
	}
	
	// Particles
	Particle*	P		= particle_active;
	if (0==P)			return;
	
	{
		float	dt				= Device.fTimeDelta;
		_IndexStream& _IS		= RCache.Index;
		RCache.set_Shader		(DM_Drop.shader);
		
		Fmatrix					mXform,mScale;
		int						pcount  = 0;
		u32						v_offset,i_offset;
		u32						vCount_Lock		= particles_cache*DM_Drop.number_vertices;
		u32						iCount_Lock		= particles_cache*DM_Drop.number_indices;
		CDetail::fvfVertexOut*	v_ptr			= (CDetail::fvfVertexOut*) RCache.Vertex.Lock	(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
		WORD*					i_ptr			= _IS.Lock										(iCount_Lock, i_offset);
		while (P)	{
			Particle*	next	=	P->next;
			
			// Update
			P->time				-=	dt;
			if (P->time<0)	{
				p_free			(P);
				P				=	next;
				continue;
			}

			// Render
			if (::Render->ViewBase.testSphere_dirty(P->bounds.P, P->bounds.R))
			{
				// Build matrix
				float scale			=	P->time / particles_time;
				mScale.scale		(scale,scale,scale);
				mXform.mul_43		(P->mXForm,mScale);
				
				// XForm verts
				DM_Drop.Transfer	(mXform,v_ptr,0xffffffff,i_ptr,pcount*DM_Drop.number_vertices);
				v_ptr			+=	DM_Drop.number_vertices;
				i_ptr			+=	DM_Drop.number_indices;
				pcount			++;

				if (pcount >= particles_cache) {
					// flush
					u32	dwNumPrimitives		= iCount_Lock/3;
					RCache.Vertex.Unlock	(vCount_Lock,hGeom_Drops->vb_stride);
					_IS.Unlock				(iCount_Lock);
					RCache.set_Geometry		(hGeom_Drops);
					RCache.Render			(D3DPT_TRIANGLELIST,v_offset, 0,vCount_Lock,i_offset,dwNumPrimitives);
					
					v_ptr					= (CDetail::fvfVertexOut*) RCache.Vertex.Lock	(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
					i_ptr					= _IS.Lock										(iCount_Lock, i_offset);
					
					pcount	= 0;
				}
			}
			
			P = next;
		}

		// Flush if needed
		vCount_Lock						= pcount*DM_Drop.number_vertices;
		iCount_Lock						= pcount*DM_Drop.number_indices;
		u32	dwNumPrimitives				= iCount_Lock/3;
		RCache.Vertex.Unlock			(vCount_Lock,hGeom_Drops->vb_stride);
		_IS.Unlock						(iCount_Lock);
		if (pcount)	{
			RCache.set_Geometry		(hGeom_Drops);
			RCache.Render			(D3DPT_TRIANGLELIST,v_offset,0,vCount_Lock,i_offset,dwNumPrimitives);
		}
	}
}
