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

void CHOM::Load			(CStream* S)
{
	// Load tris and merge them
	CDB::Collector		CL;
	while (!S->Eof())
	{
		HOM_poly			P;
		S->Read				(&P,sizeof(P));
		CL.add_face_packed	(P.v1,P.v2,P.v3,CDB::edge_open,CDB::edge_open,CDB::edge_open,0,0,0,EPS_L);
	}
	
	// Determine adjacency
}
