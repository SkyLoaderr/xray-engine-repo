// HOM.cpp: implementation of the CHOM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HOM.h"
#include "occRasterizer.h"
#include "cl_collector.h"

int a[2][2][2] = { 0,0,0,0,0,0,0,0 };

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

IC float	Area (Fvector& v0, Fvector& v1, Fvector& v2)
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
		CL.add_face_packed	(P.v1,P.v2,P.v3,CDB::edge_open,CDB::edge_open,CDB::edge_open,0,0,P.flags,EPS_L);
	}
	
	// Determine adjacency
	CL.calc_adjacency	();

	// Create RASTER-triangles
	m_pTris				= (occTri*) malloc(CL.getTS()*sizeof(occTri));
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
	}

	// Create AABB-tree
	m_pModel			= new CDB::MODEL();
	m_pModel->build		(CL.getV(),CL.getVS(),CL.getT(),CL.getTS());
	
	// Debug
	HW.pDevice->CreateTexture(occ_dim_0,occ_dim_0,1,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,&m_pDBG);
	R_ASSERT			(m_pDBG);
	LPCSTR		RTname	= "$user$hom";
	pStream		= Device.Streams.Create			(FVF::F_TL,4);
	pTexture	= Device.Shader._CreateTexture	(RTname);
	pShader		= Device.Shader.Create			("effects\\screen_set",		RTname);
	pTexture->surface_set	(m_pDBG);
}

void CHOM::Unload		()
{
	_RELEASE	(m_pDBG);
	_DELETE		(m_pModel);
	_FREE		(m_pTris);
}

IC	void	xform		(Fmatrix& X, Fvector& D, Fvector& S, float dim_2)
{
	float w	= S.x*X._14 + S.y*X._24 + S.z*X._34 + X._44;
	D.x	= ((S.x*X._11 + S.y*X._21 + S.z*X._31 + X._41)/w+1.f)*dim_2;
	D.y	= (-(S.x*X._12 + S.y*X._22 + S.z*X._32 + X._42)/w+1.f)*dim_2;
	D.z	= (S.x*X._13 + S.y*X._23 + S.z*X._33 + X._43)/w;
}

void CHOM::Render		(CFrustum& base)
{
	if (0==m_pModel)	return;

	Device.Statistic.RenderCALC_HOM.Begin	();
	// Clear buffers
	Raster.clear		();

	// Query DB
	XRC.frustum_options	(0);
	XRC.frustum_query	(m_pModel,base);

	// Perfrom selection, sorting, culling
	CDB::RESULT*	it	= XRC.r_begin();
	CDB::RESULT*	end	= XRC.r_end();
	Fvector			COP = Device.vCameraPosition;
	Fmatrix			XF	= Device.mFullTransform;
	float			dim = occ_dim_0/2;
	for (; it!=end; it++)
	{
		occTri& T	= m_pTris	[it->id];
		
		// Test for good occluder - should be improved :)
		if (!(T.flags || (T.plane.classify(COP)>0)))	continue;
		
		// Access to triangle vertices
		CDB::TRI& t	= m_pModel->get_tris() [it->id];
		
		// XForm
		xform		(XF,T.raster[0],*t.verts[0],dim);
		xform		(XF,T.raster[1],*t.verts[1],dim);
		xform		(XF,T.raster[2],*t.verts[2],dim);
		
		// Rasterize
		Raster.rasterize(&T);
	}
	
	// Propagade
	Raster.propagade	();
	
	Device.Statistic.RenderCALC_HOM.End		();
}
void CHOM::Debug		()
{
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
	DWORD _w = 128, _h = 128;
	FVF::TL* pv = (FVF::TL*) pStream->Lock(4,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
	pStream->Unlock			(4);
	
	// Actual rendering
	Device.Shader.set_Shader(pShader);
	Device.Primitive.Draw	(pStream,4,2,Offset,Device.Streams_QuadIB);
}

BOOL CHOM::Visible		(Fbox& B)
{
	// Find min/max points of xformed-box
	Fmatrix&	XF		= Device.mFullTransform;
	Fbox		rect;
	Fvector		test,src;
	float		D		= 1.f;
	B.getpoint(0,src);	xform(XF,test,src,D);	rect.set	(test,test);
	B.getpoint(1,src);	xform(XF,test,src,D);	rect.modify	(test);
	B.getpoint(2,src);	xform(XF,test,src,D);	rect.modify	(test);
	B.getpoint(3,src);	xform(XF,test,src,D);	rect.modify	(test);
	B.getpoint(4,src);	xform(XF,test,src,D);	rect.modify	(test);
	B.getpoint(5,src);	xform(XF,test,src,D);	rect.modify	(test);
	B.getpoint(6,src);	xform(XF,test,src,D);	rect.modify	(test);
	B.getpoint(7,src);	xform(XF,test,src,D);	rect.modify	(test);
	
	return Raster.test	(rect.min.x,rect.min.y,rect.max.x,rect.max.y,rect.min.z);
}
