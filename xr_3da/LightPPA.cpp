// LightPPA.cpp: implementation of the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightPPA.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightPPA::CLightPPA()
{

}

CLightPPA::~CLightPPA()
{

}

void CLightPPA::Render(CList<PPA_Vertex>&	vlist)
{
	VERIFY	(pCreator);
	Fbox	BB;
	BB.set	(sphere.P);
	BB.grow	(sphere.R+EPS_L);

	// Query collision DB (Select polygons)
	XRC.BBoxMode		(0);
	XRC.BBoxCollide		(precalc_identity,pCreator->ObjectSpace.GetStaticModel(),precalc_identity,BB);
	DWORD	triCount	= XRC.GetBBoxContactCount();
	if (0==triCount)	continue;
	RAPID::tri* tris	= pCreator->ObjectSpace.GetStaticTris();

	// Create clipper
	CFrustum	clipper;
//	clipper.
Fvector down_vec	={0.f,-1.f,0.f};
Fvector left_vec	={-1.f,0.f,0.f};
Fvector right_vec	={1.f,0.f,0.f};
Fvector fwd_vec		={0.f,0.f,1.f};
Fvector back_vec	={0.f,0.f,-1.f};

		bbox.grow		(EPS_L);
    	Fplane			frustum_planes[4];
		frustum_planes[0].build(bbox.min,left_vec);
		frustum_planes[1].build(bbox.min,back_vec);
		frustum_planes[2].build(bbox.max,right_vec);
		frustum_planes[3].build(bbox.max,fwd_vec);

        CFrustum frustum;
        frustum.CreateFromPlanes(frustum_planes,4);

	// Clip and triangulate polygons
	Fvector	cam = Device.vCameraPosition;
	for (DWORD t=0; t<triCount; t++)
	{
		RAPID::tri&	T	= tris[XRC.BBoxContact[t].id];

		Fvector D1,D2,D3;
		float	a1,a2,a3;
		float	b1,b2,b3;

		Fvector	V1		= *T.verts[0];
		Fvector V2		= *T.verts[1];
		Fvector V3		= *T.verts[2];
		Fvector N;		N.mknormal(V1,V2,V3);

		// Test for poly facing away from light
		D1.sub(V1,sphere.P); D1.normalize_safe	();
		D2.sub(V2,sphere.P); D2.normalize_safe	();
		D3.sub(V3,sphere.P); D3.normalize_safe	();
		if ((N.dotproduct(D1)>=0)&&(N.dotproduct(D2)>=0)&&(N.dotproduct(D3)>=0))	continue;	

		// Test for poly facing away from camera
		D1.sub(V1,cam);		D1.normalize_safe	();
		D2.sub(V2,cam);		D2.normalize_safe	();
		D3.sub(V3,cam);		D3.normalize_safe	();
		if ((N.dotproduct(D1)>=0)&&(N.dotproduct(D2)>=0)&&(N.dotproduct(D3)>=0))	continue;	

		// BOX clipping
		sPoly			src,dst;
		src.clear		();
		src.push_back	(V1);
		src.push_back	(V2);
		src.push_back	(V3);
		dst.clear		();
		sPoly* P		= clipper.ClipPoly	(src, dst);
		if (0==P)		continue;

		// Triangulation
		PPA_Vertex		vert1,vert2,vert3;
		vert1.P.set		((*P)[0]);	vert1.N.set(N);
		vert2.P.set		((*P)[0]);	vert2.N.set(N);
		vert3.N.set		(N);
		for (DWORD i=2; i<P->size(); i++)
		{
			vert3.P.set	((*P)[i]);
			vlist.push_back	(vert1);
			vlist.push_back	(vert2);
			vlist.push_back	(vert3);
			vert2.P.set		(vert3.P);
		}
	}
}
