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
}

void CHOM::Unload		()
{
	_DELETE		(m_pModel);
	_FREE		(m_pTris);
}

IC	void	xform		(Fmatrix& X, Fvector& D, Fvector& S, float dim_2)
{
	float w	= S.x*X._14 + S.y*X._24 + S.z*X._34 + X._44;
	D.x	= ((S.x*X._11 + S.y*X._21 + S.z*X._31 + X._41)/w+1.f)*dim_2;
	D.y	= ((S.x*X._12 + S.y*X._22 + S.z*X._32 + X._42)/w+1.f)*dim_2;
	D.z	= (S.x*X._13 + S.y*X._23 + S.z*X._33 + X._43)/w;
}

void CHOM::Render		(CFrustum& base)
{
	if (0==m_pModel)	return;

	Device.Statistic.RenderCALC_HOM.Begin	();
	// Clear buffers
	Raster.clear		();

	// Query DB
	XRC.frustum_options	();
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
		xform		(T.raster[0],*t.verts[0]);
		xform		(T.raster[1],*t.verts[1]);
		xform		(T.raster[2],*t.verts[2]);
		
		// Rasterize
		Raster.rasterize(&T);
	}

	// Propagade
	Raster.propagade	();

	Device.Statistic.RenderCALC_HOM.End		();
}

BOOL CHOM::Visible		(Fbox& B)
{
	// Find min/max points of xformed-box
	Fmatrix&	XF		= Device.mFullTransform;
	Fbox		rect;
	Fvector		test,src;
	B.getpoint(0,src);	XF.transform(test,src); rect.set	(test,test);
	B.getpoint(1,src);	XF.transform(test,src); rect.modify	(test);
	B.getpoint(2,src);	XF.transform(test,src); rect.modify	(test);
	B.getpoint(3,src);	XF.transform(test,src); rect.modify	(test);
	B.getpoint(4,src);	XF.transform(test,src); rect.modify	(test);
	B.getpoint(5,src);	XF.transform(test,src); rect.modify	(test);
	B.getpoint(6,src);	XF.transform(test,src); rect.modify	(test);
	B.getpoint(7,src);	XF.transform(test,src); rect.modify	(test);
	
	return Raster.test	(rect.min.x+1,rect.min.y+1,rect.max.x+1,rect.max.y+1,rect.min.z);
}
