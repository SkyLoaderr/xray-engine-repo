#include "stdafx.h"
#pragma hdrstop

#include	"xrMemory_align.h"

#ifndef		__BORLANDC__

#ifndef		DEBUG
#define		debug_mode 0
#endif

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

	if		(debug_mode)		debug_cs.Enter	();
	u32		_footer				=	debug_mode?4:0;
	void*	_ptr				=	0;

	//
	if (!mem_initialized || debug_mode)		
	{
		// generic
		void*	_real			=	xr_aligned_offset_malloc	(size + _footer, 16, 0x1);
		_ptr					=	(void*)(((u8*)_real)+1);
		*acc_header(_ptr)		=	mem_generic;
	} else {
		//	accelerated
		u32	pool					=	get_pool	(size+_footer);
		if (mem_generic==pool)		
		{
			// generic
			void*	_real			=	xr_aligned_offset_malloc	(size + _footer,16,0x1);
			_ptr					=	(void*)(((u8*)_real)+1);
			*acc_header(_ptr)		=	mem_generic;
		} else {
			// pooled
			void*	_real			=	mem_pools[pool].create();
			_ptr					=	(void*)(((u8*)_real)+1);
			*acc_header(_ptr)		=	(u8)pool;
		}
	}

	if		(debug_mode)		dbg_register	(_ptr,size);
	if		(debug_mode)		debug_cs.Leave	();
	return	_ptr;
}

void	xrMemory::mem_free		(void* P)
{
	stat_calls++;
	if		(debug_mode)		{
		debug_cs.Enter	();
		dbg_unregister	(P);
	}
	u32	pool					= get_header	(P);
	void* _real					= (void*)(((u8*)P)-1);
	if (mem_generic==pool)		
	{
		// generic
		xr_aligned_free			(_real);
	} else {
		// pooled
		VERIFY2					(pool<mem_pools_count,"Memory corruption");
		mem_pools[pool].destroy	(_real);
	}
	if		(debug_mode)		debug_cs.Leave	();
}

void*	xrMemory::mem_realloc	(void* P, size_t size)
{
	stat_calls++;
	if (0==P)					return mem_alloc(size);

	if		(debug_mode)		debug_cs.Enter	();
	u32		p_current			= get_header(P);
	void*	_real				= (void*)(((u8*)P)-1);
	void*	_ptr				= NULL;
	if (mem_generic==p_current)
	{
		u32		_footer			=	debug_mode?4:0;
		if		(debug_mode)	dbg_unregister	(P);
		void*	_real2			=	xr_aligned_offset_realloc	(_real,size+_footer,16,0x1);
		_ptr					= (void*)(((u8*)_real2)+1);
		*acc_header(_ptr)		= mem_generic;
		if		(debug_mode)	dbg_register	(_ptr,size);
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
	if		(debug_mode)		debug_cs.Leave	();
	return	_ptr;
}

#endif
