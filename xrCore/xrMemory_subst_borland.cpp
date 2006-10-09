#include "stdafx.h"
#pragma hdrstop

#ifdef __BORLANDC__
MEMPOOL		mem_pools			[mem_pools_count];
// Borland doesn't support marked aligned allocs ???
void*	xrMemory::mem_alloc		(size_t size, LPCSTR)
{
	if (0==size) size			= 1;
	void* data 					= malloc	(size); 	
    if(!data)
#ifndef __BORLANDC__
		Debug.fatal		(DEBUG_INFO,"Out of memory. Memory request: %d K",size/1024);
#else // __BORLANDC__
		Debug.fatal		("Out of memory. Memory request: %d K",size/1024);
#endif // __BORLANDC__
	return data;
}
void	xrMemory::mem_free		(void* P)
{
	free						(P);
}
void*	xrMemory::mem_realloc	(void* P, size_t size, LPCSTR)
{	
	void* data					= realloc(P,size); 	
    if(size&&(0==data))
#ifndef __BORLANDC__
		Debug.fatal(DEBUG_INFO,"Out of memory. Memory request: %d K",size/1024);
#else // __BORLANDC__
		Debug.fatal("Out of memory. Memory request: %d K",size/1024);
#endif // __BORLANDC__
	return data;
}
#endif
