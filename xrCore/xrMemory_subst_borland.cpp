#include "stdafx.h"
#pragma hdrstop

#ifdef __BORLANDC__
// Borland doesn't support marked aligned allocs ???
void*	xrMemory::mem_alloc		(size_t size)
{
	if (0==size) size=1;
	void* data = malloc	(size); 	R_ASSERT2(data,"Out of memory.");
	return data;
}
void	xrMemory::mem_free		(void* P)
{
	free			(P);
}
void*	xrMemory::mem_realloc	(void* P, size_t size)
{	
	void* data	= realloc(P,size); 	R_ASSERT2(!size||(size&&data),"Out of memory.");
	return data;
}
#endif
