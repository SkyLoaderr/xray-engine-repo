#include "stdafx.h"
#pragma hdrstop

#ifdef __BORLANDC__
// Borland doesn't support marked aligned allocs ???
void*	xrMemory::mem_alloc		(size_t size, LPCSTR)
{
	if (0==size) size			= 1;
	void* data 					= malloc	(size); 	
    if(!data) Debug.fatal		("Out of memory. Memory request: %d K",size/1024);
	return data;
}
void	xrMemory::mem_free		(void* P)
{
	free						(P);
}
void*	xrMemory::mem_realloc	(void* P, size_t size, LPCSTR)
{	
	void* data					= realloc(P,size); 	
    if(size&&(0==data)) Debug.fatal("Out of memory. Memory request: %d K",size/1024);
	return data;
}
#endif
