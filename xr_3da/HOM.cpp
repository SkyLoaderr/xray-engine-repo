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
	for (; it!=end; it++)
	{
		occTri& T	= m_pTris	[it->id];

		// Test for good occluder - should be improved :)
		if (!(T.flags || (T.plane.classify(COP)>0)))	continue;

		// Access to triangle vertices
		CDB::TRI& t	= m_pModel->get_tris() [it->id];

		// XForm
		XF.transform	(T.raster[0],*t.verts[0]);
		XF.transform	(T.raster[1],*t.verts[1]);
		XF.transform	(T.raster[2],*t.verts[2]);
		
		// Rasterize
		Raster.rasterize(&T);
	}

	// Propagade
	Raster.propagade	();

	Device.Statistic.RenderCALC_HOM.End		();
}
