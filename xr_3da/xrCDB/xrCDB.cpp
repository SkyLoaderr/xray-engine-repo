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

void COLLIDER::r_add	(int id)
{
	if (rd_count>=rd_size)	
	{
		if (rd_size)	rd_size	*=	2;
		else			rd_size	=	32;
		rd_ptr			= (int*) realloc(rd_ptr,rd_size*sizeof(int));
	}
	rd_ptr[rd_count++]	= id;
}
void COLLIDER::r_free	()
{
	if (rd_ptr)		{ free(rd_ptr); rd_ptr = 0; }
	rd_count		= 0;
	rd_size			= 0;
}
