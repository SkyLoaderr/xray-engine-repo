#include "stdafx.h"
#pragma hdrstop

#include "xrMemory_align.h"

#ifndef __BORLANDC__

MEMPOOL		mem_pools			[mem_pools_count];

// MSVC
u8*		acc_header				(void* P)	{	u8*		_P		= (u8*)P;	return	_P-1;	}
u32		get_header				(void* P)	{	return	(u32)*acc_header(P);				}
u32		get_pool				(size_t size)
{
	u32		pid					= u32(size/mem_pools_ebase);
	if (pid>=mem_pools_count)	return mem_generic;
	else						return pid;
}

void*	xrMemory::mem_alloc		(size_t size)
{
	stat_calls++;

	// if (size>14310800 && size<14310860)	__asm int 3;

	//
	void* _ptr					= 0;
	if (!mem_initialized || debug_mode)		
	{
		// generic
		void*	_real			= xr_aligned_offset_malloc	(size,16,0x1);
		_ptr					= (void*)(((u8*)_real)+1);
		*acc_header(_ptr)		= mem_generic;
		return	_ptr;
	}

	//	accelerated
	u32	pool					=	get_pool	(size);
	if (mem_generic==pool)		
	{
		// generic
		stat_counter			+=	size;
		void*	_real			=	xr_aligned_offset_malloc	(size,16,0x1);
		_ptr					=	(void*)(((u8*)_real)+1);
		*acc_header(_ptr)		=	mem_generic;
	} else {
		// pooled
		stat_counter			+=	mem_pools[pool].get_element	();
		void*	_real			=	mem_pools[pool].create();
		_ptr					=	(void*)(((u8*)_real)+1);
		*acc_header(_ptr)		=	(u8)pool;
	}
	return	_ptr;
}

void	xrMemory::mem_free		(void* P)
{
	stat_calls++;
	u32	pool					= get_header(P);
	void* _real					= (void*)(((u8*)P)-1);
	if (mem_generic==pool)		
	{
		// generic
		stat_counter			-=	xr_aligned_msize	(_real);
		xr_aligned_free			(_real);
	} else {
		// pooled
		R_ASSERT2				(pool<mem_pools_count,"Memory corruption");
		stat_counter			-=	mem_pools[pool].get_element	();
		mem_pools[pool].destroy	(_real);
	}
}

void*	xrMemory::mem_realloc	(void* P, size_t size)
{
	if (0==P)					return mem_alloc(size);

	stat_calls++;
	u32		p_current			= get_header(P);
	void*	_real				= (void*)(((u8*)P)-1);
	void*	_ptr				= NULL;
	if (mem_generic==p_current)
	{
		stat_counter			-=	xr_aligned_msize			(_real);
		void*	_real2			=	xr_aligned_offset_realloc	(_real,size,16,0x1);
		stat_counter			+=	xr_aligned_msize			(_real2);
		_ptr					= (void*)(((u8*)_real2)+1);
		*acc_header(_ptr)		= mem_generic;
	} else {
		R_ASSERT2				(p_current<mem_pools_count,"Memory corruption");
		u32		s_current		= mem_pools[p_current].get_element();
		u32		s_dest			= (u32)size;
		void*	p_old			= P;
		void*	p_new			= mem_alloc(size);
		mem_copy				(p_new,p_old,_min(s_current,s_dest));
		mem_free				(p_old);
		_ptr					= p_new;
	}
	return	_ptr;
}

#endif
