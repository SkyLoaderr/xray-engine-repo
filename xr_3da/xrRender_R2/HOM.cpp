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
		Msg		("! Occlusion map '%s' not found.",fName);
		return;
	}
	Msg	("* Loading HOM: %s",fName);
	
	destructor<IReader> FS	(FS.r_open(fName));
	destructor<IReader>	S	(FS().open_chunk(1));

	// Load tris and merge them
	CDB::Collector		CL;
	while (!S().eof())
	{
		HOM_poly			P;
		S().r				(&P,sizeof(P));
		CL.add_face_packed	(P.v1,P.v2,P.v3,CDB::edge_open,CDB::edge_open,CDB::edge_open,0,0,P.flags,0.01f);
	}
	
	// Determine adjacency
	CL.calc_adjacency	();

	// Create RASTER-triangles
	m_pTris				= (occTri*) xr_malloc(CL.getTS()*sizeof(occTri));
	for (u32 it=0; it<CL.getTS(); it++)
	{
		CDB::TRI&	clT = CL.getT()[it];
		occTri&		rT	= m_pTris[it];
		Fvector&	v0	= CL.getV()[clT.IDverts()[0]];
		Fvector&	v1	= CL.getV()[clT.IDverts()[1]];
		Fvector&	v2	= CL.getV()[clT.IDverts()[2]];
		rT.adjacent[0]	= (CDB::edge_open==clT.IDadj()[0])?((occTri*) 0xffffffff):(m_pTris+clT.IDadj()[0]);
		rT.adjacent[1]	= (CDB::edge_open==clT.IDadj()[1])?((occTri*) 0xffffffff):(m_pTris+clT.IDadj()[1]);
		rT.adjacent[2]	= (CDB::edge_open==clT.IDadj()[2])?((occTri*) 0xffffffff):(m_pTris+clT.IDadj()[2]);
		rT.flags		= clT.dummy;
		rT.area			= Area(v0,v1,v2);
		rT.plane.build	(v0,v1,v2);
		rT.skip			= 0;
	}

	// Create AABB-tree
	m_pModel			= xr_new<CDB::MODEL> ();
	m_pModel->build		(CL.getV(),CL.getVS(),CL.getT(),CL.getTS());
	bEnabled			= TRUE;
	m_ZB.clear			();

	/*
	h_Geom					= Device.Shader.CreateGeom	(FVF::F_L, RCache.Vertex.Buffer(), NULL	);
	h_Shader				= Device.Shader.Create		("zfill"	);
	
	// Debug
	HW.pDevice->CreateTexture(occ_dim_0,occ_dim_0,1,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,&dbg_surf,NULL);
	R_ASSERT				(dbg_surf);
	LPCSTR		RTname		= "$user$hom";
	dbg_geom				= Device.Shader.CreateGeom		(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
	dbg_texture				= Device.Shader._CreateTexture	(RTname);
	dbg_shader				= Device.Shader.Create			("effects\\screen_set",		RTname);
	dbg_texture->surface_set(dbg_surf);
	*/
}

void CHOM::Unload		()
{
	/*
	Device.Shader.Delete		(h_Shader);
	Device.Shader.DeleteGeom	(h_Geom);
	_RELEASE	(dbg_surf);
	*/
	xr_delete	(m_pModel);
	xr_free		(m_pTris);
	bEnabled	= FALSE;
}

IC	void	xform		(Fmatrix& X, Fvector& D, Fvector& S, float dim_2)
{
	float x	= S.x*X._11 + S.y*X._21 + S.z*X._31 + X._41;
	float y	= S.x*X._12 + S.y*X._22 + S.z*X._32 + X._42;
	float z	= S.x*X._13 + S.y*X._23 + S.z*X._33 + X._43;
	float w	= S.x*X._14 + S.y*X._24 + S.z*X._34 + X._44;
	
	D.x = (1.f+x/w)*dim_2;
	D.y = (1.f-y/w)*dim_2;
	D.z	= z/w;
}

void CHOM::Render_DB	(CFrustum& base)
{
	// Query DB
	XRC.frustum_options		(0);
	XRC.frustum_query		(m_pModel,base);
	if (0==XRC.r_count())	return;

	// Prepare
	CDB::RESULT*	it		= XRC.r_begin	();
	CDB::RESULT*	end		= XRC.r_end		();
	Fvector			COP		= Device.vCameraPosition;
	Fmatrix			XF		= Device.mFullTransform;
	float			dim		= occ_dim_0/2;

	// Build frustum with near plane only
	CFrustum		clip;
	clip.CreateFromMatrix(XF,FRUSTUM_P_NEAR);
	sPoly			src,dst;

	// Perfrom selection, sorting, culling
	for (; it!=end; it++)
	{
		// Control skipping
		occTri& T		= m_pTris	[it->id];
		m_ZB.push_back	(it->id);

		if (T.skip)		{ T.skip--; continue; }
		u32	next		= ::Random.randI(3,10);

		// Test for good occluder - should be improved :)
		if (!(T.flags || (T.plane.classify(COP)>0)))	
		{ T.skip=next; continue; }

		// Access to triangle vertices
		CDB::TRI& t		= m_pModel->get_tris() [it->id];
		src.clear		();	dst.clear	();
		src.push_back	(*t.verts[0]);
		src.push_back	(*t.verts[1]);
		src.push_back	(*t.verts[2]);
		sPoly* P =		clip.ClipPoly	(src,dst);
		if (0==P)		{ T.skip=next; continue; }

		// XForm and Rasterize
		u32		pixels	= 0;
		int		limit	= int(P->size())-1;
		for (int v=1; v<limit; v++)
		{
			xform			(XF,T.raster[0],(*P)[0],	dim);
			xform			(XF,T.raster[1],(*P)[v+0],	dim);
			xform			(XF,T.raster[2],(*P)[v+1],	dim);
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
	m_ZB.clear			();
	Raster.clear		();
	Render_DB			(base);
	Raster.propagade	();
	Device.Statistic.RenderCALC_HOM.End		();
}

/*
void CHOM::Render_ZB	()
{
	if (m_ZB.empty())		return;

	// Fill VB
	u32								vCount	= m_ZB.size()*3;
	u32								vOffset;
	FVF::L*		V					= (FVF::L*) RCache.Vertex.Lock	(vCount,h_Geom->vb_stride, vOffset);

	xr_vector<u32>::iterator	I		= m_ZB.begin	();
	xr_vector<u32>::iterator	E		= m_ZB.end		();

	u32 C							= D3DCOLOR_RGBA	(0xff,0,0,0xff);
	for (; I!=E; I++)
	{
		CDB::TRI& t					= m_pModel->get_tris() [*I];
		V->set	(*t.verts[0],C);	V++;
		V->set	(*t.verts[1],C);	V++;
		V->set	(*t.verts[2],C);	V++;
	}

	RCache.Vertex.Unlock			(vCount,h_Geom->vb_stride);

	// Render it
	RCache.set_xform_world			(Fidentity);
	RCache.set_Shader				(h_Shader);
	RCache.set_Geometry				(h_Geom);
	//CHK_DX(HW.pDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0));
	RCache.Render					(D3DPT_TRIANGLELIST,vOffset,vCount/3);
	//CHK_DX(HW.pDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0xf));
}
*/
void CHOM::Debug		()
{
	return;
/*
	if (0==dbg_surf)	return;

	// Texture
	D3DLOCKED_RECT		R;
	R_CHK				(dbg_surf->LockRect(0,&R,0,0));
	for (int y=0; y<occ_dim_0; y++)
	{
		for (int x=0; x<occ_dim_0; x++)
		{
			int*	pD	= Raster.get_depth_level(0);
			int		D	= pD[y*occ_dim_0+x];
			float   d	= Raster.d2float(D);
			int		V	= (d<(1.f-EPS))?0:255;
			u32	C	= D3DCOLOR_XRGB(V,V,V);
			LPDWORD(R.pBits)[y*occ_dim_0+x]	= C;
		}
	}
	dbg_surf->UnlockRect	(0);
	
	// UV
	Fvector2		p0,p1;
	p0.set			(.5f/occ_dim_0, .5f/occ_dim_0);
	p1.set			((occ_dim_0+.5f)/occ_dim_0, (occ_dim_0+.5f)/occ_dim_0);
	
	// Fill vertex buffer
	u32 vOffset, C=0xffffffff;
	u32 _w = occ_dim_0*2, _h = occ_dim_0*2;
	FVF::TL* pv		= (FVF::TL*) RCache.Vertex.Lock(4,dbg_geom->vb_stride,vOffset);
	pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock			(4,dbg_geom->vb_stride);
	
	// Actual rendering
	RCache.set_Shader	(dbg_shader);
	RCache.set_Geometry	(dbg_geom);
	RCache.Render		(D3DPT_TRIANGLELIST,vOffset,0,4,0,2);
*/
}

IC	BOOL	xform_b0	(Fvector2& min, Fvector2& max, float& minz, Fmatrix& X, float _x, float _y, float _z)
{
	float z		= _x*X._13 + _y*X._23 + _z*X._33 + X._43;			if (z<EPS) return TRUE;
	float w		= 1/(_x*X._14 + _y*X._24 + _z*X._34 + X._44);		
	min.x=max.x	= 1.f+(_x*X._11 + _y*X._21 + _z*X._31 + X._41)*w;
	min.y=max.y	= 1.f-(_x*X._12 + _y*X._22 + _z*X._32 + X._42)*w;	
	minz		= 0.f+z*w;
	return FALSE;
}
IC	BOOL	xform_b1	(Fvector2& min, Fvector2& max, float& minz, Fmatrix& X, float _x, float _y, float _z)
{
	float t;
	float z		= _x*X._13 + _y*X._23 + _z*X._33 + X._43;			if (z<EPS)	return TRUE;
	float w		= 1/(_x*X._14 + _y*X._24 + _z*X._34 + X._44);		
	t 			= 1.f+(_x*X._11 + _y*X._21 + _z*X._31 + X._41)*w;	if (t<min.x) min.x=t; else if (t>max.x) max.x=t;
	t			= 1.f-(_x*X._12 + _y*X._22 + _z*X._32 + X._42)*w;	if (t<min.y) min.y=t; else if (t>max.y) max.y=t;
	t			= 0.f+z*w;											if (t<minz)			minz  =t;
	return FALSE;
}
IC	BOOL	_visible	(Fbox& B)
{
	// Find min/max points of xformed-box
	Fmatrix&	XF		= Device.mFullTransform;
	Fvector2	min,max;
	float		z;
	if (xform_b0(min,max,z,XF,B.min.x, B.min.y, B.min.z)) return TRUE;
	if (xform_b1(min,max,z,XF,B.min.x, B.min.y, B.max.z)) return TRUE;
	if (xform_b1(min,max,z,XF,B.max.x, B.min.y, B.max.z)) return TRUE;
	if (xform_b1(min,max,z,XF,B.max.x, B.min.y, B.min.z)) return TRUE;
	if (xform_b1(min,max,z,XF,B.min.x, B.max.y, B.min.z)) return TRUE;
	if (xform_b1(min,max,z,XF,B.min.x, B.max.y, B.max.z)) return TRUE;
	if (xform_b1(min,max,z,XF,B.max.x, B.max.y, B.max.z)) return TRUE;
	if (xform_b1(min,max,z,XF,B.max.x, B.max.y, B.min.z)) return TRUE;
	return Raster.test	(min.x,min.y,max.x,max.y,z);
}

BOOL CHOM::visible		(Fbox& B)
{
	if (!bEnabled)		return TRUE;
	return _visible		(B);
}

BOOL CHOM::visible		(vis_data& vis)
{
	if (Device.dwFrame<vis.hom_frame)	return TRUE;	// not at this time :)
	if (!bEnabled)	{
		vis.hom_frame	= u32	(-1);					// delay testing as much as possible
		return TRUE;									// return - everything visible
	}
	
	// Now, the test time comes
	// 0. The object was hidden, and we must prove that each frame	- test		| frame-old, tested-new, hom_res = false;
	// 1. The object was visible, but we must to re-check it		- test		| frame-new, tested-???, hom_res = true;
	// 2. New object slides into view								- delay test| frame-old, tested-old, hom_res = ???;
	u32 frame_current	= Device.dwFrame;
	u32	frame_prev		= frame_current-1;

	Device.Statistic.RenderCALC_HOM.Begin	();
	BOOL result			= _visible			(vis.box);
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
	Fmatrix&	XF		= Device.mFullTransform;
	Fvector2	min,max;
	float		z;
	
	if (xform_b0(min,max,z,XF, P.front().x, P.front().y, P.front().z)) return TRUE;
	for (u32 it=1; it<P.size(); it++)
		if (xform_b1(min,max,z,XF, P[it].x, P[it].y, P[it].z)) return TRUE;
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
