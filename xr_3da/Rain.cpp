#include "stdafx.h"
#pragma once

#include "Rain.h"
#include "igame_persistent.h"
#include "environment.h"
#include "render.h"

#ifdef _EDITOR
    #include "ui_toolscustom.h"
#else
	#include "igame_level.h"
	#include "xr_area.h"
	#include "xr_object.h"
#endif

const int	max_desired_items	= 2500;
const float	source_radius		= 10.f;
const float	source_height		= 40.f;
const float	drop_length			= 3.f;
const float drop_width			= 0.04f;
const float drop_angle			= 3.01f;
const float drop_max_angle		= PI_DIV_8*0.5f;
const float drop_max_wind_vel	= 20.0f;
const float drop_speed_min		= 40.f;
const float drop_speed_max		= 80.f;

const int	max_particles		= 1000;
const int	particles_cache		= 400;
const float particles_time		= .3f;
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffect_Rain::CEffect_Rain()
{
	state							= stIdle;
	
	Sound->create					(snd_Ambient,TRUE,"ambient\\rain");

	destructor<IReader>	F			(FS.r_open("$game_meshes$","dm\\rain.dm"));
	DM_Drop							= ::Render->model_CreateDM		(&F());

	//
	SH_Rain.create					("effects\\rain","fx\\rain");
	hGeom_Rain.create				(FVF::F_LIT, RCache.Vertex.Buffer(), RCache.QuadIB);
	hGeom_Drops.create				(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RCache.Vertex.Buffer(), RCache.Index.Buffer());
	p_create						();
}

CEffect_Rain::~CEffect_Rain()
{
	Sound->destroy					(snd_Ambient);

	// Cleanup
	p_destroy						();
	::Render->model_Delete			(DM_Drop);
}

// Born
void	CEffect_Rain::Born		(Item& dest, float radius, float height)
{
	Fvector		axis;	
    axis.set			(0,-1,0);
	float	factor		= drop_max_angle*drop_max_angle*(g_pGamePersistent->Environment.CurrentEnv.wind_velocity/drop_max_wind_vel);
    clamp				(factor,0.f,1.f);
    factor				+= -PI_DIV_2;
    axis.setHP			(g_pGamePersistent->Environment.CurrentEnv.wind_direction,factor);
    
	Fvector&	view	= Device.vCameraPosition;
	float		angle	= ::Random.randF	(0,PI_MUL_2);
	float		dist	= ::Random.randF	(0,radius);
	float		x		= dist*_cos			(angle);
	float		z		= dist*_sin			(angle);
	dest.P.set			(x+view.x,height+view.y,z+view.z);
	dest.D.random_dir	(axis,deg2rad(drop_angle));
	dest.fSpeed			= ::Random.randF	(drop_speed_min,drop_speed_max);

	height				*= 2.f;
	UpdateItem			(dest,height,RayPick(dest.P,dest.D,height));
}

BOOL CEffect_Rain::RayPick(const Fvector& s, const Fvector& d, float& range)
{
	BOOL bRes 			= TRUE;
#ifdef _EDITOR
    Tools->RayPick		(s,d,range);
#else
	Collide::rq_result	RQ;
	CObject* E 			= g_pGameLevel->CurrentViewEntity();
	if (E)				E->setEnabled		(FALSE);
	bRes 				= g_pGameLevel->ObjectSpace.RayPick(s,d,range,Collide::rqtBoth,RQ);	
	if (E)				E->setEnabled		(TRUE);
    if (bRes) range 	= RQ.range;
#endif
    return bRes;
}

void CEffect_Rain::UpdateItem(Item& dest, float height, BOOL bHit)
{
    if (bHit){
		dest.fTime_Life	= height/dest.fSpeed;
		dest.fTime_Hit	= height/dest.fSpeed;
		dest.Phit.mad	(dest.P,dest.D,height);
	}else{
		dest.fTime_Life	= (height*2)/dest.fSpeed;
		dest.fTime_Hit	= (height*3)/dest.fSpeed;
		dest.Phit.set	(dest.P);
	}
}

void	CEffect_Rain::Render	()
{
#ifndef _EDITOR
	if (!g_pGameLevel)			return;
#endif
	// Parse states
	float	factor				= g_pGamePersistent->Environment.CurrentEnv.rain_density;

	switch (state)
	{
	case stIdle:		
		if (factor<EPS_L)		return;
		state					= stWorking;
		snd_Ambient.play		(0,TRUE);
		snd_Ambient.set_range	(15.f,20.f);
		break;
	case stWorking:
		if (factor<EPS_L)
		{
			state					= stIdle;
			snd_Ambient.stop		();
			return;
		}
		break;
	}
	u32 desired_items			= iFloor	(0.5f*(1.f+factor)*float(max_desired_items));

	// ambient sound
	Fvector						sndP;
	sndP.mad					(Device.vCameraPosition,Device.vCameraDirection,.1f);
	snd_Ambient.set_position	(sndP);
	snd_Ambient.set_volume		(factor);

	// visual
	float		factor_visual	= factor/2.f+.5f;
	Fvector3	f_rain_color	= g_pGamePersistent->Environment.CurrentEnv.rain_color;
	u32			u_rain_color	= color_rgba_f(f_rain_color.x,f_rain_color.y,f_rain_color.z,factor_visual);

	// born _new_ if needed
	float	b_radius			= source_radius;
	float	b_radius_wrap		= b_radius+.5f;
	float	b_height			= source_height;
	if (items.size()<desired_items)	{
		// items.reserve		(desired_items);
		while (items.size()<desired_items)	{
			Item				one;
			Born				(one,b_radius,b_height);
			items.push_back		(one);
		}
	}

	// build source plane
    Fplane src_plane;
    Fvector norm	={0.f,-1.f,0.f};
    Fvector pos; 	pos.set(Device.vCameraPosition.x,Device.vCameraPosition.y+b_height,Device.vCameraPosition.z);
    src_plane.build(pos,norm);
	
	// perform update
	u32			vOffset;
	FVF::LIT	*verts		= (FVF::LIT	*) RCache.Vertex.Lock(desired_items*4,hGeom_Rain->vb_stride,vOffset);
	FVF::LIT	*start		= verts;
	float		dt			= Device.fTimeDelta;
	const Fvector&	vCenter	= Device.vCameraPosition;
	for (u32 I=0; I<items.size(); I++)
	{
		// physics and time control
		Item&	one		=	items[I];

		one.fTime_Hit	-=  dt;	if (one.fTime_Hit<0)	Hit (one.Phit);
		one.fTime_Life	-=	dt; if (one.fTime_Life<0)	Born(one,b_radius,b_height);

		one.P.mad		(one.D,one.fSpeed*dt);
		
		// cylindrical wrap
		Fvector	wdir;	wdir.set(one.P.x-vCenter.x,0,one.P.z-vCenter.z);
		float	wlen	=	wdir.magnitude();
		if (wlen>b_radius_wrap)	{
//			Device.Statistic.TEST0.Begin();
			if ((vCenter.y-one.P.y)>b_height){
				// need born
				one.invalidate();
			}else{
				// perform wrapping
                Fvector 	inv_dir, src_p;
				wdir.div	(wlen);
				one.P.mad	(one.P, wdir, -(wlen+b_radius));
                inv_dir.invert(one.D);
                src_plane.intersectRayPoint(one.P,inv_dir,src_p);
                float dist	= one.P.distance_to(src_p);
				float height= 2.f*b_height;
				if (RayPick(src_p,one.D,height)){	
					if (height<=dist)	one.invalidate();					// need born
					else				UpdateItem(one,height-dist,TRUE);	// fly to point
				}else{
					UpdateItem			(one,height,FALSE);					// fly ...
				}
            }
//			Device.Statistic.TEST0.End();
		}

		// Build line
		Fvector&	pos_head	= one.P;
		Fvector		pos_trail;	pos_trail.mad	(pos_head,one.D,-drop_length*factor_visual);
		
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
		P.mad(pos_trail,lineTop,-w);	verts->set(P,u_rain_color,0,1);	verts++;
		P.mad(pos_trail,lineTop,w);		verts->set(P,u_rain_color,0,0);	verts++;
		P.mad(pos_head, lineTop,-w);	verts->set(P,u_rain_color,1,1);	verts++;
		P.mad(pos_head, lineTop,w);		verts->set(P,u_rain_color,1,0);	verts++;
	}
	u32 vCount					= (u32)(verts-start);
	RCache.Vertex.Unlock		(vCount,hGeom_Rain->vb_stride);
	
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
		RCache.set_Shader		(DM_Drop->shader);
		
		Fmatrix					mXform,mScale;
		int						pcount  = 0;
		u32						v_offset,i_offset;
		u32						vCount_Lock		= particles_cache*DM_Drop->number_vertices;
		u32						iCount_Lock		= particles_cache*DM_Drop->number_indices;
		IRender_DetailModel::fvfVertexOut* v_ptr= (IRender_DetailModel::fvfVertexOut*) RCache.Vertex.Lock	(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
		u16*					i_ptr			= _IS.Lock													(iCount_Lock, i_offset);
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
				DM_Drop->transfer	(mXform,v_ptr,u_rain_color,i_ptr,pcount*DM_Drop->number_vertices);
				v_ptr			+=	DM_Drop->number_vertices;
				i_ptr			+=	DM_Drop->number_indices;
				pcount			++;

				if (pcount >= particles_cache) {
					// flush
					u32	dwNumPrimitives		= iCount_Lock/3;
					RCache.Vertex.Unlock	(vCount_Lock,hGeom_Drops->vb_stride);
					_IS.Unlock				(iCount_Lock);
					RCache.set_Geometry		(hGeom_Drops);
					RCache.Render			(D3DPT_TRIANGLELIST,v_offset, 0,vCount_Lock,i_offset,dwNumPrimitives);
					
					v_ptr					= (IRender_DetailModel::fvfVertexOut*)			RCache.Vertex.Lock	(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
					i_ptr					= _IS.Lock										(iCount_Lock, i_offset);
					
					pcount	= 0;
				}
			}
			
			P = next;
		}

		// Flush if needed
		vCount_Lock						= pcount*DM_Drop->number_vertices;
		iCount_Lock						= pcount*DM_Drop->number_indices;
		u32	dwNumPrimitives				= iCount_Lock/3;
		RCache.Vertex.Unlock			(vCount_Lock,hGeom_Drops->vb_stride);
		_IS.Unlock						(iCount_Lock);
		if (pcount)	{
			RCache.set_Geometry		(hGeom_Drops);
			RCache.Render			(D3DPT_TRIANGLELIST,v_offset,0,vCount_Lock,i_offset,dwNumPrimitives);
		}
	}
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
	P->mXForm.transform_tiny	(P->bounds.P,DM_Drop->bv_sphere.P);
	P->bounds.R					= DM_Drop->bv_sphere.R;
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

// _delete_ node from _list_
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

// determine size of _list_
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
