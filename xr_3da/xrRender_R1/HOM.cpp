// HOM.cpp: implementation of the CHOM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HOM.h"
#include "occRasterizer.h"
 
float	psOSSR		= .001f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHOM::CHOM()
{
	bEnabled		= FALSE;
	m_pModel		= 0;
	m_pTris			= 0;
}

CHOM::~CHOM()
{
}

#pragma pack(push,4)
struct HOM_poly			
{
	Fvector	v1,v2,v3;
	u32		flags;
};
#pragma pack(pop)

IC float	Area		(Fvector& v0, Fvector& v1, Fvector& v2)
{
	float	e1 = v0.distance_to(v1);
	float	e2 = v0.distance_to(v2);
	float	e3 = v1.distance_to(v2);
	
	float	p  = (e1+e2+e3)/2.f;
	return	_sqrt( p*(p-e1)*(p-e2)*(p-e3) );
}

void CHOM::Load			()
{
	// Find and open file
	string256		fName;
	FS.update_path	(fName,"$level$","level.hom");
	if (!FS.exist(fName))
	{
		Msg		("! WARNING: Occlusion map '%s' not found.",fName);
		return;
	}
	Msg	("* Loading HOM: %s",fName);
	
	destructor<IReader> fs	(FS.r_open(fName));
	destructor<IReader>	S	(fs().open_chunk(1));

	// Load tris and merge them
	CDB::Collector		CL;
	while (!S().eof())
	{
		HOM_poly				P;
		S().r					(&P,sizeof(P));
		CL.add_face_packed_D	(P.v1,P.v2,P.v3,P.flags,0.01f);
	}
	
	// Determine adjacency
	xr_vector<u32>		adjacency;
	CL.calc_adjacency	(adjacency);

	// Create RASTER-triangles
	m_pTris				= (occTri*) xr_malloc(CL.getTS()*sizeof(occTri));
	for (u32 it=0; it<CL.getTS(); it++)
	{
		CDB::TRI&	clT = CL.getT()[it];
		occTri&		rT	= m_pTris[it];
		Fvector&	v0	= CL.getV()[clT.verts[0]];
		Fvector&	v1	= CL.getV()[clT.verts[1]];
		Fvector&	v2	= CL.getV()[clT.verts[2]];
		rT.adjacent[0]	= (0xffffffff==adjacency[3*it+0])?((occTri*) 0xffffffff):(m_pTris+adjacency[3*it+0]);
		rT.adjacent[1]	= (0xffffffff==adjacency[3*it+1])?((occTri*) 0xffffffff):(m_pTris+adjacency[3*it+1]);
		rT.adjacent[2]	= (0xffffffff==adjacency[3*it+2])?((occTri*) 0xffffffff):(m_pTris+adjacency[3*it+2]);
		rT.flags		= clT.dummy;
		rT.area			= Area	(v0,v1,v2);
		rT.plane.build	(v0,v1,v2);
		rT.skip			= 0;
	}

	// Create AABB-tree
	m_pModel			= xr_new<CDB::MODEL> ();
	m_pModel->build		(CL.getV(),int(CL.getVS()),CL.getT(),int(CL.getTS()));
	bEnabled			= TRUE;
}

void CHOM::Unload		()
{
	xr_delete	(m_pModel);
	xr_free		(m_pTris);
	bEnabled	= FALSE;
}

void CHOM::Render_DB	(CFrustum& base)
{
	// Query DB
	xrc.frustum_options		(0);
	xrc.frustum_query		(m_pModel,base);
	if (0==xrc.r_count())	return;

	// Prepare
	CDB::RESULT*	it			= xrc.r_begin	();
	CDB::RESULT*	end			= xrc.r_end		();
	Fvector			COP			= Device.vCameraPosition;
	float			view_dim	= occ_dim_0;
	Fmatrix			m_viewport		= {
		view_dim/2.f,			0.0f,					0.0f,		0.0f,
		0.0f,					-view_dim/2.f,			0.0f,		0.0f,
		0.0f,					0.0f,					1.0f,		0.0f,
		view_dim/2.f + 0 + 0,	view_dim/2.f + 0 + 0,	0.0f,		1.0f
	};
	Fmatrix			m_viewport_01	= {
		1.f/2.f,			0.0f,				0.0f,		0.0f,
		0.0f,				-1.f/2.f,			0.0f,		0.0f,
		0.0f,				0.0f,				1.0f,		0.0f,
		1.f/2.f + 0 + 0,	1.f/2.f + 0 + 0,	0.0f,		1.0f
	};
	m_xform.mul					(m_viewport,	Device.mFullTransform);
	m_xform_01.mul				(m_viewport_01,	Device.mFullTransform);

	// Build frustum with near plane only
	CFrustum					clip;
	clip.CreateFromMatrix		(Device.mFullTransform,FRUSTUM_P_NEAR);
	sPoly						src,dst;

	// Perfrom selection, sorting, culling
	for (; it!=end; it++)
	{
		// Control skipping
		occTri& T		= m_pTris	[it->id];
		if (T.skip)		{ T.skip--; continue; }
		u32	next		= ::Random.randI(3,10);

		// Test for good occluder - should be improved :)
		if (!(T.flags || (T.plane.classify(COP)>0)))	
		{ T.skip=next; continue; }

		// Access to triangle vertices
		CDB::TRI& t		= m_pModel->get_tris()	[it->id];
		Fvector*  v		= m_pModel->get_verts();
		src.clear		();	dst.clear	();
		src.push_back	(v[t.verts[0]]);
		src.push_back	(v[t.verts[1]]);
		src.push_back	(v[t.verts[2]]);
		sPoly* P =		clip.ClipPoly	(src,dst);
		if (0==P)		{ T.skip=next; continue; }

		// XForm and Rasterize
		u32		pixels	= 0;
		int		limit	= int(P->size())-1;
		for (int v=1; v<limit; v++)	{
			m_xform.transform(T.raster[0],(*P)[0]);
			m_xform.transform(T.raster[1],(*P)[v+0]);
			m_xform.transform(T.raster[2],(*P)[v+1]);
			pixels	+=		Raster.rasterize(&T);
		}
		if (0==pixels)
		{ T.skip=next; continue; }
	}
}

void CHOM::Render		(CFrustum& base)
{
	if (!bEnabled)		return;
	
	Device.Statistic.RenderCALC_HOM.Begin	();
	Raster.clear		();
	Render_DB			(base);
	Raster.propagade	();
	Device.Statistic.RenderCALC_HOM.End		();
}

IC	BOOL	xform_b0	(Fvector2& min, Fvector2& max, float& minz, Fmatrix& X, float _x, float _y, float _z)
{
	float z		= _x*X._13 + _y*X._23 + _z*X._33 + X._43;			if (z<EPS) return TRUE;
	float iw	= 1.f/(_x*X._14 + _y*X._24 + _z*X._34 + X._44);		
	min.x=max.x	= (_x*X._11 + _y*X._21 + _z*X._31 + X._41)*iw;
	min.y=max.y	= (_x*X._12 + _y*X._22 + _z*X._32 + X._42)*iw;	
	minz		= 0.f+z*iw;
	return FALSE;
}
IC	BOOL	xform_b1	(Fvector2& min, Fvector2& max, float& minz, Fmatrix& X, float _x, float _y, float _z)
{
	float t;
	float z		= _x*X._13 + _y*X._23 + _z*X._33 + X._43;		if (z<EPS)	return TRUE;
	float iw	= 1.f/(_x*X._14 + _y*X._24 + _z*X._34 + X._44);
	t 			= (_x*X._11 + _y*X._21 + _z*X._31 + X._41)*iw;	if (t<min.x) min.x=t; else if (t>max.x) max.x=t;
	t			= (_x*X._12 + _y*X._22 + _z*X._32 + X._42)*iw;	if (t<min.y) min.y=t; else if (t>max.y) max.y=t;
	t			= 0.f+z*iw;										if (t<minz)	 minz =t;
	return FALSE;
}
IC	BOOL	_visible	(Fbox& B, Fmatrix& m_xform_01)
{
	// Find min/max points of xformed-box
	Fvector2	min,max;
	float		z;
	if (xform_b0(min,max,z,m_xform_01,B.min.x, B.min.y, B.min.z)) return TRUE;
	if (xform_b1(min,max,z,m_xform_01,B.min.x, B.min.y, B.max.z)) return TRUE;
	if (xform_b1(min,max,z,m_xform_01,B.max.x, B.min.y, B.max.z)) return TRUE;
	if (xform_b1(min,max,z,m_xform_01,B.max.x, B.min.y, B.min.z)) return TRUE;
	if (xform_b1(min,max,z,m_xform_01,B.min.x, B.max.y, B.min.z)) return TRUE;
	if (xform_b1(min,max,z,m_xform_01,B.min.x, B.max.y, B.max.z)) return TRUE;
	if (xform_b1(min,max,z,m_xform_01,B.max.x, B.max.y, B.max.z)) return TRUE;
	if (xform_b1(min,max,z,m_xform_01,B.max.x, B.max.y, B.min.z)) return TRUE;
	return Raster.test	(min.x,min.y,max.x,max.y,z);
}

BOOL CHOM::visible		(Fbox3& B)
{
	if (!bEnabled)		return TRUE;
	return _visible		(B,m_xform_01);
}

BOOL CHOM::visible		(Fbox2& B, float depth)
{
	if (!bEnabled)		return TRUE;
	return Raster.test	(B.min.x,B.min.y,B.max.x,B.max.y,depth);
}

BOOL CHOM::visible		(vis_data& vis)
{
	if (Device.dwFrame<vis.hom_frame)	return TRUE;				// not at this time :)
	if (!bEnabled)						return TRUE;				// return - everything visible
	
	// Now, the test time comes
	// 0. The object was hidden, and we must prove that each frame	- test		| frame-old, tested-new, hom_res = false;
	// 1. The object was visible, but we must to re-check it		- test		| frame-new, tested-???, hom_res = true;
	// 2. New object slides into view								- delay test| frame-old, tested-old, hom_res = ???;
	u32 frame_current	= Device.dwFrame;
	u32	frame_prev		= frame_current-1;

	Device.Statistic.RenderCALC_HOM.Begin	();
	BOOL result			= _visible			(vis.box,m_xform_01);
	u32  delay			= 1;
	if (vis.frame<frame_prev)
	{
		// either [0] or [2]
		if (vis.hom_tested<frame_prev)
		{
			// [2]
			// assumming, that it will be rendered this frames, situation will be [1]
			if (result)
			{
				// visible	- delay next test
				delay			= ::Random.randI	(2,5);
			} else {
				// hidden	- shedule to next frame
			}
		} else {
			// [0]
			// (a) - visible, but last time it was hidden	- shedule to next frame
			// (b) - invisible								- shedule to next frame
		}
	} else {
		// [1]
		if (result)
		{
			// visible	- delay next test
			delay			= ::Random.randI	(5,10);
		} else {
			// hidden	- shedule to next frame
		}
	}
	vis.hom_frame	= frame_current + delay;
	vis.hom_tested	= frame_current;

	Device.Statistic.RenderCALC_HOM.End	();
	return result;
}

BOOL CHOM::visible		(sPoly& P)
{
	if (!bEnabled)		return TRUE;

	// Find min/max points of xformed-box
	Fvector2	min,max;
	float		z;
	
	if (xform_b0(min,max,z,m_xform_01,P.front().x,P.front().y,P.front().z)) return TRUE;
	for (u32 it=1; it<P.size(); it++)
		if (xform_b1(min,max,z,m_xform_01,P[it].x,P[it].y,P[it].z)) return TRUE;
	return Raster.test	(min.x,min.y,max.x,max.y,z);
}

void CHOM::Disable		()
{
	bEnabled			= FALSE;
}

void CHOM::Enable		()
{
	bEnabled			= m_pModel?TRUE:FALSE;
}
