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
	vlist.clear();

	// Build bbox
	Fbox	BB;
	BB.set	(sphere.P);
	BB.grow	(sphere.R+EPS_L);

	// Query collision DB (Select polygons)
	XRC.BBoxMode		(0);
	XRC.BBoxCollide		(precalc_identity,pCreator->ObjectSpace.GetStaticModel(),precalc_identity,BB);
	DWORD	triCount	= XRC.GetBBoxContactCount();
	if (0==triCount)	continue;
	RAPID::tri* tris	= pCreator->ObjectSpace.GetStaticTris();

	// Calculate XForms
	Fmatrix	XForm, invXForm;
	float	R			= sphere.R;
	XForm.scale			(R,R,R);
	XForm.translate_over(sphere.P);
	invXForm.invert		(XForm);

	// Create clipper
	CFrustum			clipper;
	Fplane				P;
	Fvector down_vec;	down_vec.set	(0.f,-1.f,0.f);
	Fvector left_vec;	left_vec.set	(-1.f,0.f,0.f);
	Fvector right_vec;	right_vec.set	(1.f,0.f,0.f);
	Fvector fwd_vec;	fwd_vec.set		(0.f,0.f,1.f);
	Fvector back_vec;	back_vec.set	(0.f,0.f,-1.f);

	P.build	(BB.min,left_vec);			clipper._add(P);
	P.build	(BB.min,back_vec);			clipper._add(P);
	P.build	(BB.max,right_vec);			clipper._add(P);
	P.build	(BB.max,fwd_vec);			clipper._add(P);

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
		Fvector			uv;
		vert1.P.set		((*P)[0]);	vert1.N.set(N);	invXForm.transform_tiny(uv,vert1.P); vert1.u0=uv.x; vert1.v0=uv.y; vert1.u1=uv.z; vert1.v1=.5f;
		vert2.P.set		((*P)[0]);	vert2.N.set(N);	invXForm.transform_tiny(uv,vert2.P); vert2.u0=uv.x; vert2.v0=uv.y; vert2.u1=uv.z; vert2.v1=.5f;
		vert3.N.set		(N);
		for (DWORD i=2; i<P->size(); i++)
		{
			vert3.P.set		((*P)[i]);
			invXForm.transform_tiny(uv,vert1.P); 
			vert3.u0=uv.x; vert3.v0=uv.y; 
			vert3.u1=uv.z; vert3.v1=.5f;			
			vlist.push_back	(vert1);
			vlist.push_back	(vert2);
			vlist.push_back	(vert3);
			vert2.P.set		(vert3.P);
		}
	}
}
