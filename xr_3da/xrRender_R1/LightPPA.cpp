// LightPPA.cpp: implementation of the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightPPA.h"

const u32	MAX_POLYGONS=1024*8;
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

void CLightPPA::set_active(bool a)
{
	if (a)
	{
		if (bActive)		return;
		bActive				= true;
		RImplementation.L_Dynamic.Activate		(this);	
	}
	else
	{
		if (!bActive)		return;
		bActive				= false;
		RImplementation.L_Dynamic.Deactivate	(this);
	}
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
	u32	triCount		= XRC.r_count	();
	if (0==triCount)	return;
	CDB::TRI* tris		= pCreator->ObjectSpace.GetStaticTris();

	// Lock
	RCache.set_Geometry		(hGeom);
	u32 triLock				= _min(256u,triCount);
	u32	vOffset;
	CLightPPA_Vertex* VB	= (CLightPPA_Vertex*)RCache.Vertex.Lock(triLock*3,hGeom->vb_stride,vOffset);

	// Cull and triangulate polygons
	Fvector	cam		= Device.vCameraPosition;
	float	r2		= sphere.R*2;
	u32		actual	= 0;
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

		if (actual>=triLock)
		{
			RCache.Vertex.Unlock		(actual*3,hGeom->vb_stride);
			if (actual) RCache.Render	(D3DPT_TRIANGLELIST,vOffset,actual);
			actual						= 0;
			triLock						= _min(256u,triCount-t);
			VB							= (CLightPPA_Vertex*)RCache.Vertex.Lock(triLock*3,hGeom->vb_stride,vOffset);
		}
	}

	// Unlock and render
	RCache.Vertex.Unlock		(actual*3,hGeom->vb_stride);
	if (actual) RCache.Render	(D3DPT_TRIANGLELIST,vOffset,actual);
}

void CLightPPA_Manager::Initialize	()
{
	hShader	= Device.Shader.Create		("effects\\light","effects\\light,effects\\light");
	hGeom	= Device.Shader.CreateGeom	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, RCache.Vertex.Buffer(), NULL);
}

void CLightPPA_Manager::Destroy		()
{
	active.clear				();
	inactive.clear				();
	Device.Shader.DeleteGeom	(hGeom);
	Device.Shader.Delete		(hShader);
}

void CLightPPA_Manager::Render()
{
	const float	clip	= 4.f / 255.f;

	// Projection
	float _43			 = Device.mProject._43;
	Device.mProject._43 -= 0.001f; 
	RCache.set_xform_project	(Device.mProject);

	RCache.set_Shader	(hShader);
	for (xr_set<CLightPPA*>::iterator it=active.begin(); it!=active.end(); it++)
	{
		CLightPPA&	PPL = *(*it);

		// Culling
		if (PPL.sphere.R<0.05f)													continue;
		if (PPL.color.magnitude_sqr_rgb()<EPS)									continue;
		float	alpha		= Device.vCameraPosition.distance_to(PPL.sphere.P)/MAX_DISTANCE;
		if (alpha>=1)															continue;
		if (!RImplementation.ViewBase.testSphere_dirty (PPL.sphere.P,PPL.sphere.R))	continue;

		// Calculations and rendering
		Device.Statistic.RenderDUMP_Lights.Begin();
		Fcolor				factor;
		factor.mul_rgba		(PPL.color,(1-alpha));
		CHK_DX				(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,factor.get()));
		PPL.Render			(hGeom);
		Device.Statistic.RenderDUMP_Lights.End	();
	}

	// Projection
	Device.mProject._43 = _43;
	RCache.set_xform_project	(Device.mProject);
}

CLightPPA*		CLightPPA_Manager::Create			()
{
	CLightPPA*	L	= xr_new<CLightPPA>	();
	L->bActive		= false;
	inactive.insert	(L);
	return			L;
}
void			CLightPPA_Manager::Destroy			(CLightPPA* L)
{
	xr_set<CLightPPA*>::iterator	it;

	//
	it = inactive.find	(L);
	if (it!=inactive.end())	
	{
		inactive.erase	(it);
		xr_delete		(L);
		return;
	}

	// 
	it = active.find	(L);
	if (it!=active.end())	
	{
		active.erase(it);
		xr_delete	(L);
		return;
	}

	// ???
	xr_delete	(L);
	Msg			("! xrRENDER: unregistered light destroyed");
}

void	CLightPPA_Manager::Activate		(CLightPPA* L)
{
	xr_set<CLightPPA*>::iterator	it		= inactive.find	(L);
	R_ASSERT								(it!=inactive.end());
	inactive.erase							(it);

	active.insert				(L);
}
void	CLightPPA_Manager::Deactivate	(CLightPPA* L)
{
	xr_set<CLightPPA*>::iterator	it		= active.find	(L);
	R_ASSERT								(it!=active.end());
	active.erase				(it);

	inactive.insert				(L);
}
