// LightPPA.cpp: implementation of the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightPPA.h"

const u32	MAX_POLYGONS			=	1024*8;
const float MAX_DISTANCE			=	50.f;
const float	SSM_near_plane			=	.1f;
const float	SSM_tex_size 			=	32.f;

//////////////////////////////////////////////////////////////////////////
// binders for lighting
//////////////////////////////////////////////////////////////////////////
void cl_light_PR::setup		(R_constant* C)					{
	Fvector&	P	= RImplementation.r1_dlight_light->position;
	float		R	= RImplementation.r1_dlight_light->range;
	RCache.set_c	(C,P.x,P.y,P.z,1.f/R);
}
void cl_light_C::setup		(R_constant* C)					{
	Fcolor&		_C	= RImplementation.r1_dlight_light->color;
	RCache.set_c	(C,_C.r,_C.g,_C.b,0.f);
}
void cl_light_XFORM::setup	(R_constant* C)					{
	RCache.set_c	(C,RImplementation.r1_dlight_tcgen);
}

//////////////////////////////////////////////////////////////////////////
IC void mk_vertex					(CLightR_Vertex& D, Fvector& P, Fvector& N, Fvector& C, float r2)
{
	D.P.set	(P);
	D.N.set	(P);
	D.u0	= (P.x-C.x)/r2+.5f; 
	D.v0	= (P.z-C.z)/r2+.5f;
	D.u1	= (P.y-C.y)/r2+.5f;
	D.v1	=.5f;
}

void CLightR_Manager::render_point	()
{
	// Worl/View/Projection
	float _43					 = Device.mProject._43;
	Device.mProject._43			-= 0.001f; 
	RCache.set_xform_world		(Fidentity);
	RCache.set_xform_project	(Device.mProject);

	RCache.set_Shader			(hShader);
	for (xr_vector<light*>::iterator it=selected_point.begin(); it!=selected_point.end(); it++)
	{
		light&	PPL		= *(*it);

		// Culling
		if (PPL.range<0.05f)														continue;
		if (PPL.color.magnitude_sqr_rgb()<EPS)										continue;
		float	alpha		= Device.vCameraPosition.distance_to(PPL.position)/MAX_DISTANCE;
		if (alpha>=1)																continue;
		if (!RImplementation.ViewBase.testSphere_dirty (PPL.position,PPL.range))	continue;

		// Calculations and rendering
		Device.Statistic.RenderDUMP_Lights.Begin();
		Fcolor				factor;
		factor.mul_rgba		(PPL.color,(1-alpha));
		CHK_DX				(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,factor.get()));
		{
			// Build bbox
			float				size_f	= PPL.range+EPS_L;
			Fvector				size;	
			size.set			(size_f,size_f,size_f);

			// Query collision DB (Select polygons)
			xrc.box_options		(0);
			xrc.box_query		(g_pGameLevel->ObjectSpace.GetStaticModel(),PPL.position,size);
			u32	triCount		= xrc.r_count	();
			if	(0==triCount)	return;
			CDB::TRI* tris		= g_pGameLevel->ObjectSpace.GetStaticTris();

			// Lock
			RCache.set_Geometry		(hGeom);
			u32 triLock				= _min(256u,triCount);
			u32	vOffset;
			CLightR_Vertex* VB		= (CLightR_Vertex*)RCache.Vertex.Lock(triLock*3,hGeom->vb_stride,vOffset);

			// Cull and triangulate polygons
			Fvector	cam		= Device.vCameraPosition;
			float	r2		= PPL.range*2;
			u32		actual	= 0;
			for (u32 t=0; t<triCount; t++)
			{
				CDB::TRI&	T	= tris	[xrc.r_begin()[t].id];

				Fvector	V1		= *T.verts[0];
				Fvector V2		= *T.verts[1];
				Fvector V3		= *T.verts[2];
				Fplane  Poly;	Poly.build(V1,V2,V3);

				// Test for poly facing away from light or camera
				if (Poly.classify(PPL.position)<0)	continue;
				if (Poly.classify(cam)<0)			continue;

				// Triangulation and UV0/UV1
				mk_vertex(*VB,V1,Poly.n,PPL.position,r2);	VB++;
				mk_vertex(*VB,V2,Poly.n,PPL.position,r2);	VB++;
				mk_vertex(*VB,V3,Poly.n,PPL.position,r2);	VB++;
				actual++;

				if (actual>=triLock)
				{
					RCache.Vertex.Unlock		(actual*3,hGeom->vb_stride);
					if (actual) RCache.Render	(D3DPT_TRIANGLELIST,vOffset,actual);
					actual						= 0;
					triLock						= _min(256u,triCount-t);
					VB							= (CLightR_Vertex*)RCache.Vertex.Lock(triLock*3,hGeom->vb_stride,vOffset);
				}
			}

			// Unlock and render
			RCache.Vertex.Unlock		(actual*3,hGeom->vb_stride);
			if (actual) RCache.Render	(D3DPT_TRIANGLELIST,vOffset,actual);
		}
		Device.Statistic.RenderDUMP_Lights.End	();
	}

	// Projection
	Device.mProject._43 = _43;
	RCache.set_xform_project	(Device.mProject);
}

void CLightR_Manager::render_spot	()
{
	// for each light
	for (xr_vector<light*>::iterator it=selected_point.begin(); it!=selected_point.end(); it++)
	{
		light*	L					= *it;

		//		1. Calculate light frustum
		Fvector						L_dir,L_up,L_right,L_pos;
		L_dir.set					(L->direction);			L_dir.normalize		();
		L_up.set					(0,1,0);				if (_abs(L_up.dotproduct(L_dir))>.99f)	L_up.set(0,0,1);
		L_right.crossproduct		(L_up,L_dir);			L_right.normalize	();
		L_up.crossproduct			(L_dir,L_right);		L_up.normalize		();
		L_pos.set					(L->position);
		L_view.build_camera_dir		(L_pos,L_dir,L_up);
		L_project.build_projection	(L->cone,1.f,SSM_near_plane,L->range+EPS_S);
		L_combine.mul				(L_project,L_view);

		//		2. Calculate matrix for TC-gen
		float			fTexelOffs			= (.5f / SSM_tex_size);
		u32				uRange				= 1; 
		float			fRange				= float(uRange);
		float			fBias				= 0.f;
		Fmatrix			m_TexelAdjust		= 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
		};
		Fmatrix		L_texgen;		L_texgen.mul	(m_TexelAdjust,L_combine);

		//		2. Set global light-params to be used by shading
		RImplementation.r1_dlight_light	= L;
		RImplementation.r1_dlight_tcgen	= L_texgen;

		//		3. Calculate visibility for light + build soring tree
		RImplementation.r_pmask						(true,false);
		RImplementation.r_dsgraph_render_subspace	(
			L->spatial.sector,
			L_combine,
			L_pos,
			TRUE
			);

		//		4. Dump sorting tree
		RImplementation.r_dsgraph_render_graph		(0);
	}
	//		??? grass ???
}

void CLightR_Manager::render		()
{
	if (selected_spot.size())		{ render_spot	();	selected_spot.clear();	}
	if (selected_point.size())		{ render_point	();	selected_point.clear(); }
}

void CLightR_Manager::add			(light* L)
{
	if (IRender_Light::POINT==L->flags.type)
	{
		// PPA
		selected_point.push_back	(L);
	} else {
		// spot/flash
		selected_spot.push_back		(L);
	}
}

CLightR_Manager::CLightR_Manager	()
{
	hShader.create					("effects\\light","effects\\light,effects\\light");
	hGeom.create					(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, RCache.Vertex.Buffer(), NULL);
}

CLightR_Manager::~CLightR_Manager	()
{
	hGeom.destroy					();
	hShader.destroy					();
}
