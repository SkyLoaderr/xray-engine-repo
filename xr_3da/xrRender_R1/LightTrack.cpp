// LightTrack.cpp: implementation of the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightTrack.h"
#include "..\xr_object.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLightTrack::CLightTrack()
{
	ambient				= 0;
	dwFrame				= u32(-1);
	Shadowed_dwFrame	= u32(-1);
	Shadowed_Slot		= -1;
}

void CLightTrack::add	(light* source)
{
	// Search
	for (xr_vector<Item>::iterator I=track.begin(); I!=track.end(); I++)	
		if (source == I->source)	{
			I->frame_touched		= Device.dwFrame;
			return;
		}

	// Register _new_
	track.push_back		(Item());
	Item&	L			= track.back();
	L.frame_touched		= Device.dwFrame;
	L.source			= source;
	L.cache.verts[0].set(0,0,0);
	L.cache.verts[1].set(0,0,0);
	L.cache.verts[2].set(0,0,0);
	L.test				= 0.f;
	L.energy			= 0.f;
}

IC bool	pred_energy		(const CLightTrack::Light& L1, const CLightTrack::Light& L2)
{
	return L1.energy>L2.energy;
}

void	CLightTrack::ltrack	(IRenderable* O)
{
	Fvector					pos; 
	float					fRadius;

	// Prepare
	if						(0==O)																	return;
	if						((!O->renderable_ShadowGenerate()) && (!O->renderable_ShadowReceive()))	return;

	VERIFY					(dynamic_cast<CLightTrack*>(O->renderable.ROS));
	if						(dwFrame==Device.dwFrame)	return;
	dwFrame					= Device.dwFrame;
	O->renderable.xform.transform_tiny	(pos,O->renderable.visual->vis.sphere.P);
	fRadius					= O->renderable.visual->vis.sphere.R;

	// *******DEBUG
	/*
	CObject*	O1			= dynamic_cast<CObject*>(g_pGameLevel->CurrentEntity());
	CObject*	O2			= dynamic_cast<CObject*>(O);
	BOOL		bActor		= (O1==O2);
	if			(bActor)	Msg	("* [%X]LTRACK-sizes(%s): %d / %d",Device.dwFrame,O2->cName(),lights.size(),track.size());
	*/
	// *******DEBUG

	// Process ambient lighting
	float	dt				= Device.fTimeDelta;
	float	l_f				= dt*lt_smooth;
	float	l_i				= 1.f-l_f;
	ambient					= l_i*ambient + l_f*O->renderable_Ambient();
	clamp					(ambient,0.f,255.f);
	
	// Select nearest lights
	Fvector					bb_size	=	{fRadius,fRadius,fRadius};
	g_SpatialSpace->q_box				(RImplementation.lstSpatial,0,STYPE_LIGHTSOURCE,pos,bb_size);
	add									(RImplementation.L_DB->sun_base);

	// Process selected lights
	for (u32 o_it=0; o_it<RImplementation.lstSpatial.size(); o_it++)
	{
		ISpatial*	spatial		= RImplementation.lstSpatial[o_it];
		light*		source		= dynamic_cast<light*>	(spatial);
		if (0==source)			continue;
		float	R				= fRadius+source->range;
		if (pos.distance_to(source->position) < R)	add		(source);
	}
	
	// Trace visibility
	lights.clear	();
	float traceR								= fRadius*.5f;
	for (s32 id=0; id<s32(track.size()); id++)
	{
		// remove untouched lights
		xr_vector<CLightTrack::Item>::iterator I	= track.begin()+id;
		if (I->frame_touched!=Device.dwFrame)		{
			track.erase	(I)	;
			id			--	;
			continue		;
		}

		// Trace visibility
		float		amount	= 0;
		light*		xrL		= I->source;
		Fvector&	LP		= xrL->position;

		// Random point inside range
		Fvector				P,D;
		P.random_dir		();
		P.mad				(pos,P,traceR);
		
		// Direction/range	
		if (xrL->flags.type==IRender_Light::DIRECT)
		{
			// direct
			D.invert			(xrL->direction);
			if (g_pGameLevel->ObjectSpace.RayTest(P,D,500.f,Collide::rqtStatic,&I->cache))	amount -=	lt_dec;
			else																			amount +=	lt_inc;
		} else {
			// point/spot
			float	f			= D.sub(P,LP).magnitude();
			D.div				(f);
			if (g_pGameLevel->ObjectSpace.RayTest(LP,D,f,Collide::rqtStatic,&I->cache))		amount -=	lt_dec;
			else																			amount +=	lt_inc;
		}
	
		// 
		I->test			+= amount * dt;
		clamp			(I->test,-.5f,1.f);
		I->energy		= .9f*I->energy + .1f*I->test;

		float	E		= I->energy * xrL->color.intensity();
		if (E > EPS)
		{
			// Select light
			lights.push_back		(CLightTrack::Light());
			CLightTrack::Light&	L	= lights.back();
			L.source				= xrL;
			L.color.mul_rgb			(xrL->color,I->energy/2);
			L.energy				= I->energy/2;
		}
	}
	
	// Sort lights by importance
	std::sort(lights.begin(),lights.end(), pred_energy);
}
