// LightTrack.cpp: implementation of the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightTrack.h"
#include "FLightsController.h"
#include "xr_creator.h"
#include "xr_object.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CLightTrack::add	(int id)
{
	// Search
	for (vector<Item>::iterator I=track.begin(); I!=track.end(); I++)	if (id == I->id)	return;
		
	// Register new
	track.push_back		(Item());
	Item&	L			= track.back();
	L.id				= id;
	L.Cache.verts[0].set(0,0,0);
	L.Cache.verts[1].set(0,0,0);
	L.Cache.verts[2].set(0,0,0);
	L.test				= 0.f;
	L.energy			= 0.f;
}
void CLightTrack::remove(int id)
{
	// Search
	for (vector<Item>::iterator I=track.begin(); I!=track.end(); I++)
	{
		if (id == I->id)
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

void	CLightDB_Static::Track	(CObject* O)
{
	Fvector				pos; 
	float				fRadius;

	// Prepare
	if					(0==O)							return;
	CLightTrack& dest	= *O->Lights();
	if					(dest.dwFrame==Device.dwFrame)	return;
	dest.dwFrame		= Device.dwFrame;
	O->clCenter			(pos);
	fRadius				= O->Radius();
	
	// Process ambient lighting
	float	dt			= Device.fTimeDelta;
	float	l_f			= dt*lt_smooth;
	float	l_i			= 1.f-l_f;
	dest.ambient		= l_i*dest.ambient + l_f*O->Ambient();
	clamp				(dest.ambient,0.f,255.f);
	
	// Process selected lights
	for (vecI_it it=Selected.begin(); it!=Selected.end(); it++)
	{
		int		num	= *it;
		xrLIGHT &L	= Lights[num];
		if (L.type == D3DLIGHT_DIRECTIONAL)				dest.add	(num);
		else {
			float	R	= fRadius+L.range;
			if (pos.distance_to_sqr(L.position) < R*R)	dest.add	(num);
			else										dest.remove	(num);
		}
	}
	
	// Trace visibility
	dest.lights.clear	();
	vector<CLightTrack::Item>& track		= dest.track;
	vector<CLightTrack::Item>::iterator I	= track.begin(), E=track.end();
	float R									= fRadius*.5f;
	for (; I!=E; I++)
	{
		float	amount	= 0;
		xrLIGHT& xrL	= Lights[I->id];
		Fvector& LP		= xrL.position;

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
			if (pCreator->ObjectSpace.RayTest(LP,D,f,false,&I->Cache))	amount -=	lt_dec;
			else														amount +=	lt_inc;
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
