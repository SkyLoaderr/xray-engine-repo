// HOM.cpp: implementation of the CHOM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "frustum.h"
#include "HOM.h"
#include "occRasterizer.h"
#include "cl_collector.h"
#include "xr_creator.h"

float	psOSSR		= .001f;
BOOL	bHOM_ModeS	= FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHOM::CHOM()
{
	m_pModel	= 0;
	m_pTris		= 0;
}

CHOM::~CHOM()
{
}

#pragma pack(push,4)
struct HOM_poly			
{
	Fvector	v1,v2,v3;
	DWORD	flags;
};
#pragma pack(pop)

IC float	Area		(Fvector& v0, Fvector& v1, Fvector& v2)
{
	float	e1 = v0.distance_to(v1);
	float	e2 = v0.distance_to(v2);
	float	e3 = v1.distance_to(v2);
	
	float	p  = (e1+e2+e3)/2.f;
	return	sqrtf( p*(p-e1)*(p-e2)*(p-e3) );
}

void CHOM::Load			()
{
	// Find and open file
	string256	fName;
	if (!Engine.FS.Exist(fName, Path.Current, "level.hom"))
	{
		Msg		("! Occlusion map '%s' not found.",fName);
		return;
	}
	
	destructor<CStream> FS	(Engine.FS.Open(fName));
	destructor<CStream>	S	(FS().OpenChunk(1));

	// Load tris and merge them
	CDB::Collector		CL;
	while (!S().Eof())
	{
		HOM_poly			P;
		S().Read			(&P,sizeof(P));
		CL.add_face_packed	(P.v1,P.v2,P.v3,CDB::edge_open,CDB::edge_open,CDB::edge_open,0,0,P.flags,0.01f);
	}
	
	// Determine adjacency
	CL.calc_adjacency	();

	// Create RASTER-triangles
	m_pTris				= (occTri*) xr_malloc(CL.getTS()*sizeof(occTri));
	for (DWORD it=0; it<CL.getTS(); it++)
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
	m_pModel			= new CDB::MODEL();
	m_pModel->build		(CL.getV(),CL.getVS(),CL.getT(),CL.getTS());
	
	// Debug
/*
	HW.pDevice->CreateTexture(occ_dim_0,occ_dim_0,1,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,&m_pDBG);
	R_ASSERT			(m_pDBG);
	LPCSTR		RTname	= "$user$hom";
	pStream		= Device.Streams.Create			(FVF::F_TL,4);
	pTexture	= Device.Shader._CreateTexture	(RTname);
	pShader		= Device.Shader.Create			("effects\\screen_set",		RTname);
	pTexture->surface_set	(m_pDBG);
*/
}

void CHOM::Unload		()
{
//	_RELEASE	(m_pDBG);
	_DELETE		(m_pModel);
	_FREE		(m_pTris);
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

IC void MouseRayFromPoint	( Fvector& direction, int x, int y, Fmatrix& m_CamMat )
{
	int halfwidth  = occ_dim/2;
	int halfheight = occ_dim/2;

	Ipoint point2;
	point2.set	(x-halfwidth, halfheight-y);

	float size_y = VIEWPORT_NEAR * tanf( deg2rad(60.f) * 0.5f );
	float size_x = size_y;

	float r_pt	= float(point2.x) * size_x / (float) halfwidth;
	float u_pt	= float(point2.y) * size_y / (float) halfheight;

	direction.mul		( m_CamMat.k, VIEWPORT_NEAR		);
	direction.mad		( direction, m_CamMat.j, u_pt	);
	direction.mad		( direction, m_CamMat.i, r_pt	);
	direction.normalize	();
}

void CHOM::Render_DB	(CFrustum& base)
{
	bHOM_ModeS			= psDeviceFlags&rsOcclusion;
	if (bHOM_ModeS)	
	{
		Fmatrix	mInv;	mInv.invert	(Device.mView);
		float	fFAR	= pCreator->Environment.c_Far;
		XRC.ray_options	(CDB::OPT_CULL|CDB::OPT_ONLYNEAREST);
		for (u32 y=0; y<occ_dim; y++)
		{
			for (u32 x=0; x<occ_dim; x++)
			{
				Fvector				D;
				MouseRayFromPoint	(D,x,y,mInv);
				XRC.ray_query		(m_pModel, Device.vCameraPosition, D, fFAR);
				if (XRC.r_count())	{
					Raster.get_frame()[y*occ_dim+x]	= m_pTris + XRC.r_begin()->id;
					Raster.get_depth()[y*occ_dim+x]	= XRC.r_begin()->range/fFAR;
				}
			}
		}
	} else {
		// Query DB
		XRC.frustum_options	(0);
		XRC.frustum_query	(m_pModel,base);
		if (0==XRC.r_count())	return;

		// Prepare
		CDB::RESULT*	it	= XRC.r_begin();
		CDB::RESULT*	end	= XRC.r_end();
		Fvector			COP = Device.vCameraPosition;
		Fmatrix			XF	= Device.mFullTransform;
		float			dim = occ_dim_0/2;

		// Build frustum with near plane only
		CFrustum	clip;
		clip.CreateFromMatrix(XF,FRUSTUM_P_NEAR);
		sPoly		src,dst;

		// Perfrom selection, sorting, culling
		for (; it!=end; it++)
		{
			// Control skipping
			occTri& T		= m_pTris	[it->id];
			if (T.skip)		{ T.skip--; continue; }
			DWORD	next	= ::Random.randI(7,30);

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
			if (0==P)		
			{ T.skip=next; continue; }

			// XForm and Rasterize
			DWORD	pixels	= 0;
			int		limit	= int(P->size())-1;
			for (int v=1; v<limit; v++)
			{
				xform			(XF,T.raster[0],(*P)[0],	dim);
				xform			(XF,T.raster[1],(*P)[v+0],	dim);
				xform			(XF,T.raster[2],(*P)[v+1],	dim);
				pixels +=		Raster.rasterize(&T);
			}
			if (0==pixels)
			{ T.skip=next; continue; }
		}
	}
}

void CHOM::Render		(CFrustum& base)
{
	if (0==m_pModel)	return;
	
	Device.Statistic.RenderCALC_HOM.Begin	();
	Raster.clear		();
	Render_DB			(base);
	Raster.propagade	();
	Device.Statistic.RenderCALC_HOM.End		();
}

void CHOM::Debug		()
{
	return;

/*
	// Texture
	D3DLOCKED_RECT		R;
	R_CHK				(m_pDBG->LockRect(0,&R,0,0));
	for (int y=0; y<occ_dim_0; y++)
	{
		for (int x=0; x<occ_dim_0; x++)
		{
			int*	pD	= Raster.get_depth_level(0);
			int		D	= pD[y*occ_dim_0+x];
			int		V	= iFloor(Raster.d2float(D)*255.f);
			clamp	(V,0,255);
			DWORD	C	= D3DCOLOR_XRGB(V,V,V);
			LPDWORD(R.pBits)[y*occ_dim_0+x]	= C;
		}
	}
	m_pDBG->UnlockRect	(0);
	
	// UV
	Fvector2		p0,p1;
	p0.set			(.5f/occ_dim_0, .5f/occ_dim_0);
	p1.set			((occ_dim_0+.5f)/occ_dim_0, (occ_dim_0+.5f)/occ_dim_0);
	
	// Fill vertex buffer
	DWORD Offset, C=0xffffffff;
	DWORD _w = occ_dim_0*2, _h = occ_dim_0*2;
	FVF::TL* pv = (FVF::TL*) pStream->Lock(4,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
	pStream->Unlock			(4);
	
	// Actual rendering
	Device.Shader.set_Shader(pShader);
	Device.Primitive.Draw	(pStream,4,2,Offset,Device.Streams_QuadIB);
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
	float z		= _x*X._13 + _y*X._23 + _z*X._33 + X._43;			if (z<EPS && !bHOM_ModeS) return TRUE;
	float w		= 1/(_x*X._14 + _y*X._24 + _z*X._34 + X._44);		
	t 			= 1.f+(_x*X._11 + _y*X._21 + _z*X._31 + X._41)*w;	if (t<min.x) min.x=t; else if (t>max.x) max.x=t;
	t			= 1.f-(_x*X._12 + _y*X._22 + _z*X._32 + X._42)*w;	if (t<min.y) min.y=t; else if (t>max.y) max.y=t;
	t			= 0.f+z*w;											if (t<minz)			minz  =t;
	// if (bHOM_ModeS)		{ Fvector T; T.set(_x,_y,_z);	t = Device.vCameraPosition.distance_to(T); }
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
	if (0==m_pModel)	return TRUE;
	return _visible		(B);
}

BOOL CHOM::visible		(sPoly& P)
{
	if (0==m_pModel)	return TRUE;

	// Find min/max points of xformed-box
	Fmatrix&	XF		= Device.mFullTransform;
	Fvector2	min,max;
	float		z;
	
	if (xform_b0(min,max,z,XF, P.front().x, P.front().y, P.front().z)) return TRUE;
	for (u32 it=1; it<P.size(); it++)
		if (xform_b1(min,max,z,XF, P[it].x, P[it].y, P[it].z)) return TRUE;
	return Raster.test	(min.x,min.y,max.x,max.y,z);
}
