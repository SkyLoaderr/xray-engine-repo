#ifndef xrMemoryH
#define xrMemoryH
#pragma once

#include "xrMemory_pso.h"
#include "xrMemory_POOL.h"

class XRCORE_API	xrMemory
{
public:
	xrMemory			();
	void				_initialize		(BOOL _debug_mode=FALSE);
	void				_destroy		();

	xrCriticalSection	debug_cs;
	BOOL				debug_mode;
	std::vector<void*>	debug_info;

	u32					stat_calls;
	s32					stat_counter;
public:
	void				dbg_register	(void* _p);
	void				dbg_unregister	(void* _p);

	u32					mem_usage		(u32* pBlocksUsed=NULL, u32* pBlocksFree=NULL);
	void				mem_compact		();
	void				mem_statistic	();
	void				mem_counter_set	(u32 _val)	{ stat_counter = _val;	}
	u32					mem_counter_get	()			{ return stat_counter;	}

	void*				mem_alloc		(size_t	size				);
	void				mem_free		(void*	p					);
	void*				mem_realloc		(void*	p, size_t size		);

	pso_MemCopy*		mem_copy;
	pso_MemFill*		mem_fill;
	pso_MemFill32*		mem_fill32;
};

extern XRCORE_API	xrMemory	Memory;

#undef	ZeroMemory
#undef	CopyMemory
#undef	FillMemory
#define ZeroMemory(a,b)		Memory.mem_fill(a,0,b)
#define CopyMemory(a,b,c)	Memory.mem_copy(a,b,c)
#define FillMemory(a,b,c)	Memory.mem_fill(a,c,b)

// delete
#ifdef __BORLANDC__
	#include "xrMemory_subst_borland.h"
#else
	#include "xrMemory_subst_msvc.h"
#endif

// generic "C"-like allocations/deallocations
template <class T>
IC T*		xr_alloc	(u32 count)				{	return  (T*)Memory.mem_alloc(count*sizeof(T));	}

template <class T>
IC void		xr_free		(T* &P)					{	if (P) { Memory.mem_free((void*)P); P=NULL;	};	}

IC void*	xr_malloc	(size_t size)			{	return	Memory.mem_alloc(size);					}

IC void*	xr_realloc	(void* P, size_t size)	{	return Memory.mem_realloc(P,size);				}

XRCORE_API char* __stdcall	xr_strdup	(const char* string);

// Global new/delete override
#if !(defined(__BORLANDC__) || defined(NO_XRNEW))
	IC void*	__cdecl operator new		(size_t size)		{	return xr_malloc(size?size:1);			}
	IC void		__cdecl operator delete		(void *p)			{	xr_free(p);								}
	IC void*	__cdecl operator new[]		(size_t size)		{	return xr_malloc(size?size:1);			}
	IC void		__cdecl	operator delete[]	(void* p)			{	xr_free(p);								}
#endif

// POOL-ing
const		u32			mem_pools_count			=	65;
const		u32			mem_pools_ebase			=	16;
const		u32			mem_generic				=	mem_pools_count+1;
extern		MEMPOOL		mem_pools				[mem_pools_count];
extern		BOOL		mem_initialized;

#endif
