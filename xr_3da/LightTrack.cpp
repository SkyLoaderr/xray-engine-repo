// LightTrack.cpp: implementation of the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightTrack.h"
#include "FLightsController.h"

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

void	CLightDB_Static::Track(Fvector &pos, float fRadius, CLightTrack& dest)
{
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
	vector<CLightTrack::Item>& track		= dest.track;
	vector<CLightTrack::Item>::iterator I	= track.begin(), E=track.end();
	float R									= fRadius*.8f;
	float dt								= Device.fTimeDelta;
	for (; I!=E; I++)
	{
		// Random point inside range
		Fvector			P;
		P.random_dir	();
		P.mad			(pos,P,R);
		
		// Direction/range	
		Fvector& LP		= Lights[I->id].position;
		Fvector	D;	
		D.sub			(P,LP);
		float	f		= D.magnitude();
		D.div			(f);
		if (pCreator->ObjectSpace.RayTest(LP,D,f,false,&I->Cache))	I->test -=	lt_dec*dt;
		else														I->test +=	lt_inc*dt;

		clamp(I->test,-1.f,1.f);
	}
}
