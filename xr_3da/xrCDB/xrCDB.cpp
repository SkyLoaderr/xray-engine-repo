// xrCDB.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "xrCDB.h"

using namespace CDB;
using namespace Opcode;

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}

// Triangle utilities
void	TRI::convert_I2P	(Fvector* pBaseV, TRI* pBaseTri)	
{
	DWORD*	pVertsID= (DWORD*)	verts;	// as indexed form
	verts[0] = pBaseV+pVertsID[0];
	verts[1] = pBaseV+pVertsID[1];
	verts[2] = pBaseV+pVertsID[2];
	DWORD*	pAdjID	= (DWORD*)	adj;	// as indexed form (take care about open-edges)
	adj	 [0] = (0xffffffff==pAdjID[0])?0:pBaseTri+pAdjID[0];
	adj	 [1] = (0xffffffff==pAdjID[1])?0:pBaseTri+pAdjID[1];
	adj	 [2] = (0xffffffff==pAdjID[2])?0:pBaseTri+pAdjID[2];
}
void	TRI::convert_P2I	(Fvector* pBaseV, TRI* pBaseTri)	
{
	DWORD*	pVertsID= (DWORD*)	verts;	// as indexed form
	pVertsID[0] = verts[0]-pBaseV;
	pVertsID[1] = verts[1]-pBaseV;
	pVertsID[2] = verts[2]-pBaseV;
	DWORD*	pAdjID	= (DWORD*)	adj;	// as indexed form (take care about open-edges)
	pAdjID	[0]	= (adj[0])?adj[0]-pBaseTri:0xffffffff;
	pAdjID	[1]	= (adj[1])?adj[1]-pBaseTri:0xffffffff;
	pAdjID	[2]	= (adj[2])?adj[2]-pBaseTri:0xffffffff;
}

// Model building
MODEL::MODEL()
{
	tree		= 0;
	tris		= 0;
	tris_count	= 0;
	verts		= 0;
	verts_count	= 0;
}
MODEL::~MODEL()
{
	delete		tree;	tree = 0;
	if (tris)	{ cl_free(tris);	tris=0;		tris_count=0;	}
	if (verts)	{ cl_free(verts);	verts=0;	verts_count=0;	}
}

void	MODEL::build(Fvector* V, int Vcnt, TRI* T, int Tcnt)
{
	// verts
	verts_count	= Vcnt;
	verts		= cl_alloc<Fvector>	(verts_count);
	CopyMemory	(verts,V,verts_count*sizeof(Fvector));
	
	// tris
	tris_count	= Tcnt;
	tris		= cl_alloc<TRI>		(tris_count);
	CopyMemory	(tris,T,tris_count*sizeof(TRI));
	
	// convert tris to 'pointer' form
	for (int i=0; i<tris_count; i++)
		tris[i].convert_I2P(verts,tris);
	
	// Build a non quantized no-leaf tree
	OPCODECREATE	OPCC;
	OPCC.NbTris		= tris_count;
	OPCC.NbVerts	= verts_count;
	OPCC.Tris		= tris;
	OPCC.Verts		= (Point*)verts;
	OPCC.Rules		= SPLIT_COMPLETE | SPLIT_SPLATTERPOINTS;
	OPCC.NoLeaf		= true;
	OPCC.Quantized	= false;
	tree			= new OPCODE_Model;
	tree->Build		(OPCC);
}
DWORD MODEL::memory()
{
	DWORD V = verts_count*sizeof(Fvector);
	DWORD T = tris_count *sizeof(TRI);
	return tree->GetUsedBytes()+V+T+sizeof(*this)+sizeof(*tree);
}


// This is the constructor of a class that has been exported.
// see xrCDB.h for the class definition
COLLIDER::COLLIDER()
{ 
	ray_mode		= 0;
	box_mode		= 0;
	frustum_mode	= 0;
	rd_ptr			= 0;
	rd_count		= 0;
	rd_size			= 0;
}

COLLIDER::~COLLIDER()
{
	r_free			();
}

void COLLIDER::r_add	(int id, float range, float u, float v)
{
	if (rd_count>=rd_size)	
	{
		if (rd_size)	rd_size	*=	2;
		else			rd_size	=	32;
		rd_ptr			= (RESULT*) realloc(rd_ptr,rd_size*sizeof(RESULT));
	}
	rd_ptr[rd_count].id		= id;
	rd_ptr[rd_count].range	= range;
	rd_ptr[rd_count].u		= u;
	rd_ptr[rd_count].v		= v;
	rd_count++;
}

void COLLIDER::r_free	()
{
	if (rd_ptr)		{ free(rd_ptr); rd_ptr = 0; }
	rd_count		= 0;
	rd_size			= 0;
}
