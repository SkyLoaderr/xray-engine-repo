#include "stdafx.h"
#pragma hdrstop

#ifdef __BORLANDC__
// Borland doesn't support marked aligned allocs ???
void*	xrMemory::mem_alloc		(size_t size)
{
	return	malloc	(size);
}
void	xrMemory::mem_free		(void* P)
{
	free			(P);
}
void*	xrMemory::mem_realloc	(void* P, size_t size)
{	
	return realloc 	(P,size); 
}
#endif
