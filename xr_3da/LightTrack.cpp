// LightTrack.cpp: implementation of the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightTrack.h"
#include "FLightsController.h"
#include "..\xr_object.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CLightTrack::add	(R1_light* source)
{
	// Search
	for (xr_vector<Item>::iterator I=track.begin(); I!=track.end(); I++)	if (source == I->source)	return;

	// Register _new_
	track.push_back		(Item());
	Item&	L			= track.back();
	L.source			= source;
	L.cache.verts[0].set(0,0,0);
	L.cache.verts[1].set(0,0,0);
	L.cache.verts[2].set(0,0,0);
	L.test				= 0.f;
	L.energy			= 0.f;
}

void CLightTrack::remove(R1_light* source)
{
	// Search
	for (xr_vector<Item>::iterator I=track.begin(); I!=track.end(); I++)
	{
		if (source == I->source)
		{
			track.erase(I);
			return;
		}
	}
}

IC bool	pred_energy		(const CLightTrack::Light& L1, const CLightTrack::Light& L2)
{
	return L1.energy>L2.energy;
}

void	CLightDB_Static::Track	(IRenderable* O)
{
	Fvector				pos; 
	float				fRadius;

	// Prepare
	if					(0==O)							return;
	CLightTrack* pROS	= dynamic_cast<CLightTrack*>	(O->renderable.ROS);
	R_ASSERT			(pROS);
	CLightTrack& dest	= *pROS;
	if					(dest.dwFrame==Device.dwFrame)	return;
	dest.dwFrame		= Device.dwFrame;
	O->renderable.xform.transform_tiny	(pos,O->renderable.visual->vis.sphere.P);
	fRadius				= O->renderable.visual->vis.sphere.R;
	
	// Process ambient lighting
	float	dt			= Device.fTimeDelta;
	float	l_f			= dt*lt_smooth;
	float	l_i			= 1.f-l_f;
	dest.ambient		= l_i*dest.ambient + l_f*O->renderable_Ambient();
	clamp				(dest.ambient,0.f,255.f);
	
	// Select nearest lights
	Fvector					bb_size	=	{fRadius,fRadius,fRadius};
	g_SpatialSpace.q_box				(0,STYPE_LIGHTSOURCE,pos,bb_size);
	g_SpatialSpace.q_result.push_back	(dynamic_cast<ISpatial*>(LDirect));

	// Process selected lights
	for (u32 o_it=0; o_it<g_SpatialSpace.q_result.size(); o_it++)
	{
		ISpatial*	spatial		= g_SpatialSpace.q_result[o_it];
		R1_light*	source		= dynamic_cast<R1_light*>(spatial);
		if (0==source)			continue;
		Flight &L				= source->data;
		if (L.type == D3DLIGHT_DIRECTIONAL)				dest.add	(source);
		else {
			float	R	= fRadius+L.range;
			if (pos.distance_to_sqr(L.position) < R*R)	dest.add	(source);
			else										dest.remove	(source);
		}
	}
	
	// Trace visibility
	dest.lights.clear	();
	xr_vector<CLightTrack::Item>& track		= dest.track;
	xr_vector<CLightTrack::Item>::iterator I = track.begin(), E=track.end();
	float R									= fRadius*.5f;
	for (; I!=E; I++)
	{
		float	amount	= 0;
		Flight&		xrL	= I->source->data;
		Fvector&	LP	= xrL.position;

		for (int it=0; it<1; it++)
		{
			// Random point inside range
			Fvector			P;
			P.random_dir	();
			P.mad			(pos,P,R);
			
			// Direction/range	
			Fvector	D;	
			D.sub			(P,LP);
			float	f		= D.magnitude();
			D.div			(f);
			if (g_pGameLevel->ObjectSpace.RayTest(LP,D,f,false,&I->cache))	amount -=	lt_dec;
			else															amount +=	lt_inc;
		}
		
		I->test			+= amount * dt;
		clamp			(I->test,-.5f,1.f);
		I->energy		= .9f*I->energy + .1f*I->test;

		float	E		= I->energy * xrL.diffuse.magnitude_rgb();
		if (E > EPS)	
		{
			// Select light
			dest.lights.push_back	(CLightTrack::Light());
			CLightTrack::Light&	L	= dest.lights.back();
			L.L						= xrL;
			L.L.diffuse.mul_rgb		(I->energy/2);
			L.energy				= E;
		}
	}
	
	// Sort lights by importance
	std::sort(dest.lights.begin(),dest.lights.end(), pred_energy);
}
