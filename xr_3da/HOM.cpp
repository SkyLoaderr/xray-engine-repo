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
	destructor<CStream>	S	(FS->OpenChunk(1));

	// Load tris and merge them
	CDB::Collector		CL;
	while (!S().Eof())
	{
		HOM_poly			P;
		S().Read			(&P,sizeof(P));
		CL.add_face_packed	(P.v1,P.v2,P.v3,CDB::edge_open,CDB::edge_open,CDB::edge_open,0,0,0,EPS_L);
	}
	
	// Determine adjacency
	CL.calc_adjacency	();

	// Create RASTER-triangles
	m_pTris				= (occTri*) malloc(CL.getTS()*sizeof(occTri));
	for (DWORD it=0; it<CL.getTS(); it++)
	{
		CDB::TRI&	clT = CL.getT()[it];
		occTri&		rT	= m_pTris[it];
		rT.adjacent[0]	= (CDB::edge_open==clT.IDadj()[0])?0xffffffff:(m_pTris+clT.IDadj()[0]);
		rT.adjacent[1]	= (CDB::edge_open==clT.IDadj()[1])?0xffffffff:(m_pTris+clT.IDadj()[1]);
		rT.adjacent[2]	= (CDB::edge_open==clT.IDadj()[2])?0xffffffff:(m_pTris+clT.IDadj()[2]);
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

	// Clear buffers
	Raster.clear		();

	// Query DB
	XRC.frustum_options	(0);
	XRC.frustum_query	(m_pModel,base);
	CDB::RESULT*	it	= XRC.r_begin();
	CDB::RESULT*	end	= XRC.r_end();
	for (; it!=end; it++)
	{
		
	}
}