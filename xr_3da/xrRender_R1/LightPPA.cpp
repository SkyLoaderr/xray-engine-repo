// LightPPA.cpp: implementation of the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightPPA.h"
#include "xr_creator.h"
#include "render.h"

const u32 MAX_POLYGONS=1024*8;
const float MAX_DISTANCE=50.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightPPA::CLightPPA()
{

}

CLightPPA::~CLightPPA()
{

}

IC void mk_vertex		(CLightPPA_Vertex& D, Fvector& P, Fvector& N, Fvector& C, float r2)
{
	D.P.set	(P);
	D.N.set	(P);
	D.u0	= (P.x-C.x)/r2+.5f; 
	D.v0	= (P.z-C.z)/r2+.5f;
	D.u1	= (P.y-C.y)/r2+.5f;
	D.v1	=.5f;
}

void CLightPPA::Render	(SGeometry* hGeom)
{
	VERIFY	(pCreator);

	// Build bbox
	Fvector size;
	size.set(sphere.R+EPS_L,sphere.R+EPS_L,sphere.R+EPS_L);

	// Query collision DB (Select polygons)
	XRC.box_options		(0);
	XRC.box_query		(pCreator->ObjectSpace.GetStaticModel(),sphere.P,size);
	u32	triCount	= XRC.r_count	();
	if (0==triCount)	return;
	CDB::TRI* tris		= pCreator->ObjectSpace.GetStaticTris();

	// Lock
	u32	vOffset;
	CLightPPA_Vertex* VB = (CLightPPA_Vertex*)RCache.Vertex.Lock(triCount*3,hGeom->vb_stride,vOffset);

	// Cull and triangulate polygons
	Fvector	cam		= Device.vCameraPosition;
	float	r2		= sphere.R*2;
	u32	actual	= 0;
	for (u32 t=0; t<triCount; t++)
	{
		CDB::TRI&	T	= tris	[XRC.r_begin()[t].id];

		Fvector	V1		= *T.verts[0];
		Fvector V2		= *T.verts[1];
		Fvector V3		= *T.verts[2];
		Fplane  Poly;	Poly.build(V1,V2,V3);

		// Test for poly facing away from light or camera
		if (Poly.classify(sphere.P)<0)	continue;
		if (Poly.classify(cam)<0)		continue;

		// Triangulation and UV0/UV1
		mk_vertex(*VB,V1,Poly.n,sphere.P,r2);	VB++;
		mk_vertex(*VB,V2,Poly.n,sphere.P,r2);	VB++;
		mk_vertex(*VB,V3,Poly.n,sphere.P,r2);	VB++;
		actual++;
	}

	// Unlock and render
	RCache.Vertex.Unlock		(actual*3,hGeom->vb_stride);
	if (actual) {
		RCache.set_Geometry		(hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,actual);
	}
}

void CLightPPA_Manager::Initialize	()
{
	hShader	= Device.Shader.Create		("effects\\light","effects\\light,effects\\light");
	hGeom	= Device.Shader.CreateGeom	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, RCache.Vertex.Buffer(), NULL);
}

void CLightPPA_Manager::Destroy		()
{
	Device.Shader.DeleteGeom	(hGeom);
	Device.Shader.Delete		(hShader);
}

void CLightPPA_Manager::Render()
{
	// Projection
	float _43			 = Device.mProject._43;
	Device.mProject._43 -= 0.001f; 
	RCache.set_xform_project	(Device.mProject);

	RCache.set_Shader	(hShader);
	for (u32 L=0; L<container.size(); L++)
	{
		CLightPPA&	PPL = *container[L];
		float	alpha	= Device.vCameraPosition.distance_to(PPL.sphere.P)/MAX_DISTANCE;

		// Culling
		if (alpha>=1)		continue;
		if (!::Render->ViewBase.testSphere_dirty (PPL.sphere.P,PPL.sphere.R))	continue;

		// Calculations and rendering
		Device.Statistic.RenderDUMP_Lights.Begin();
		Fcolor				factor;
		factor.mul_rgba		(PPL.color,(1-alpha));
		CHK_DX				(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,factor.get()));
		PPL.Render			(hGeom);
		Device.Statistic.RenderDUMP_Lights.End	();
	}

	// Clean up
	container.clear		();

	// Projection
	Device.mProject._43 = _43;
	RCache.set_xform_project	(Device.mProject);
}
