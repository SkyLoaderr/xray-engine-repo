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

// Allocators / Deallocators
template <class T>
IC T*	cl_alloc	(u32 count, HANDLE H)
{
	return (T*) HeapAlloc	(H,0,count*sizeof(T));
}
template <class T>
IC void cl_free		(T* P, HANDLE H)
{
	HeapFree				(H,0,P);
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
	heapHandle	= GetProcessHeap();
	heapPrivate	= FALSE;
	tree		= 0;
	tris		= 0;
	tris_count	= 0;
	verts		= 0;
	verts_count	= 0;
}
MODEL::~MODEL()
{
	delete		tree;	tree = 0;
	if (tris)	{ cl_free(tris,heapHandle);		tris=0;		tris_count=0;	}
	if (verts)	{ cl_free(verts,heapHandle);	verts=0;	verts_count=0;	}

	if (heapPrivate)
	{
		HeapDestroy	(heapHandle);
	}
}

u32	MODEL::build(Fvector* V, int Vcnt, TRI* T, int Tcnt, BOOL bPrivateHeap)
{
	if (bPrivateHeap)
	{
		DWORD V		= verts_count*sizeof(Fvector);
		DWORD T		= tris_count *sizeof(TRI);
		DWORD T3	= tris_count*3*sizeof(DWORD);

		heapPrivate	= TRUE;
		heapHandle	= HeapCreate(0,V+T+T3,0);
	}

	// verts
	verts_count	= Vcnt;
	verts		= cl_alloc<Fvector>	(verts_count,heapHandle);
	if (0==verts)	return err_memory_0;
	CopyMemory	(verts,V,verts_count*sizeof(Fvector));
	
	// tris
	tris_count	= Tcnt;
	tris		= cl_alloc<TRI>		(tris_count,heapHandle);
	if (0==tris)	{
		cl_free		(verts,heapHandle);
		return		err_memory_1;
	}
	CopyMemory	(tris,T,tris_count*sizeof(TRI));
	
	// Allocate temporary "OPCODE" tris + convert tris to 'pointer' form
	DWORD*		temp_tris	= cl_alloc<DWORD>	(tris_count*3,heapHandle);
	if (0==temp_tris)	{
		cl_free		(verts,heapHandle);
		cl_free		(tris,heapHandle);
		return		err_memory_2;
	}
	DWORD*		temp_ptr	= temp_tris;
	for (int i=0; i<tris_count; i++)
	{
		*temp_ptr++	= tris[i].IDverts()[0];
		*temp_ptr++	= tris[i].IDverts()[1];
		*temp_ptr++	= tris[i].IDverts()[2];
		tris[i].convert_I2P(verts,tris);
	}
	
	// Build a non quantized no-leaf tree
	OPCODECREATE	OPCC;
	OPCC.NbTris		= tris_count;
	OPCC.NbVerts	= verts_count;
	OPCC.Tris		= (unsigned*)temp_tris;
	OPCC.Verts		= (Point*)verts;
	OPCC.Rules		= SPLIT_COMPLETE | SPLIT_SPLATTERPOINTS | SPLIT_GEOMCENTER;
	OPCC.NoLeaf		= true;
	OPCC.Quantized	= false;
	tree			= new OPCODE_Model;
	if (!tree->Build(OPCC)) 
	{
		cl_free		(verts,heapHandle);
		cl_free		(tris,heapHandle);
		cl_free		(temp_tris,heapHandle);
		return		err_build;
	};


	// Free temporary tris
	cl_free			(temp_tris,heapHandle);
	return err_ok;
}

u32 MODEL::memory()
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

RESULT& COLLIDER::r_add	()
{
	if (rd_count>=rd_size)	
	{
		if (rd_size)	rd_size	*=	2;
		else			rd_size	=	32;
		rd_ptr			= (RESULT*) realloc(rd_ptr,rd_size*sizeof(RESULT));
	}
	return rd_ptr[rd_count++];
}

void COLLIDER::r_free	()
{
	if (rd_ptr)		{ free(rd_ptr); rd_ptr = 0; }
	rd_count		= 0;
	rd_size			= 0;
}

// C-style
extern "C" {
	void*		__cdecl		cdb_model_create	()
	{
		return new MODEL;
	}
	void		__cdecl		cdb_model_destroy	(void* P)
	{
		delete (MODEL*)P;
	}
	u32			__cdecl		cdb_model_build		(CDB::MODEL *m_def, Fvector* V, int Vcnt, CDB::TRI* T, int Tcnt)
	{
		return m_def->build(V,Vcnt,T,Tcnt);
	}
	void*		__cdecl		cdb_collider_create	()
	{
		return new COLLIDER;
	}
	void		__cdecl		cdb_collider_destroy(void* P)
	{
		delete (COLLIDER*)P;
	}
	void		__cdecl		cdb_query_ray		(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range)
	{
		((CDB::COLLIDER*)C)->ray_query(m_def,r_start,r_dir,r_range);
	}
	void		__cdecl		cdb_query_box		(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim)
	{
		((CDB::COLLIDER*)C)->box_query(m_def,b_center,b_dim);
	}
	void		__cdecl		cdb_query_frustum	(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const CFrustum& F)
	{
		((CDB::COLLIDER*)C)->frustum_query(m_def,F);
	}
};
