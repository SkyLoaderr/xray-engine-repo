#include "StdAfx.h"

#include <malloc.h>

#include "xrMemory.h"
#include "xrMemory_pso.h"
#include "xrMemory_align.h"

xrMemory	Memory;

// Processor specific implementations
extern		pso_MemCopy		xrMemCopy_MMX;
extern		pso_MemCopy		xrMemCopy_x86;
extern		pso_MemFill		xrMemFill_x86;
extern		pso_MemFill32	xrMemFill32_MMX;
extern		pso_MemFill32	xrMemFill32_x86;

void	xrMemory::_initialize	()
{
	DWORD	features		= CPU::ID.feature & CPU::ID.os_support;
	if (features & _CPU_FEATURE_MMX)
	{
		mem_copy	= xrMemCopy_MMX;
		mem_fill	= xrMemFill_x86;
		mem_fill32	= xrMemFill32_MMX;
	} else {
		mem_copy	= xrMemCopy_x86;
		mem_fill	= xrMemFill_x86;
		mem_fill32	= xrMemFill32_x86;
	}
}

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
#else  
// MSVC
void*	xrMemory::mem_alloc		(size_t size)
{
	stat_calls++;
	return	xr_aligned_malloc	(size,16);
}
void	xrMemory::mem_free		(void* P)
{
	stat_calls++;
	xr_aligned_free				(P);
}
void*	xrMemory::mem_realloc	(void* P, size_t size)
{	
	stat_calls++;
	return xr_aligned_realloc	(P,size,16); 
}
#endif

void	xrMemory::mem_compact	()
{
	RegFlushKey			( HKEY_CLASSES_ROOT );
	RegFlushKey			( HKEY_CURRENT_USER );
	_heapmin			();
	HeapCompact			(GetProcessHeap(),0);
}

u32		xrMemory::mem_usage		(u32* pBlocksUsed, u32* pBlocksFree)
{
	_HEAPINFO		hinfo;
	int				heapstatus;
	hinfo._pentry	= NULL;
	size_t	total	= 0;
	u32	blocks_free	= 0;
	u32	blocks_used	= 0;
	while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
	{ 
		if (hinfo._useflag == _USEDENTRY)	{
			total		+= hinfo._size;
			blocks_used	+= 1;
		} else {
			blocks_free	+= 1;
		}
	}
	if (pBlocksFree)	*pBlocksFree= (u32)blocks_free;
	if (pBlocksUsed)	*pBlocksUsed= (u32)blocks_used;

	switch( heapstatus )
	{
	case _HEAPEMPTY:
		break;
	case _HEAPEND:
		break;
	case _HEAPBADPTR:
		Debug.fatal		( "bad pointer to heap"		);
		break;
	case _HEAPBADBEGIN:
		Debug.fatal		( "bad start of heap"		);
		break;
	case _HEAPBADNODE:
		Debug.fatal		( "bad node in heap"		);
		break;
	}
	return (u32) total;
}

// xr_strdup
char* __stdcall xr_strdup	(const char* string)
{	
	VERIFY	(string);
	u32		len			= u32(strlen(string))+1;
	char *	memory		= (char *) Memory.mem_alloc( len );
	Memory.mem_copy		(memory,string,len);
	return	memory;
}

