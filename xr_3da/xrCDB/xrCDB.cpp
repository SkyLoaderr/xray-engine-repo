// xrCDB.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "xrCDB.h"

using namespace CDB;

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
