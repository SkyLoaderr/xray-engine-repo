#include "StdAfx.h"
#pragma hdrstop

#ifndef DEBUG
void	xrMemory::dbg_register		(void* _p, size_t _size)	{ }
void	xrMemory::dbg_unregister	(void* _p)					{ }
void	xrMemory::dbg_check			()							{ }

#else

#include <malloc.h>

bool	pred_mdbg	(const xrMemory::mdbg& A)	{
	return (0==A._p && 0==A._size);
}

void	xrMemory::dbg_register		(void* _p, size_t _size)
{
	VERIFY					(debug_mode);
	debug_cs.Enter			();
	debug_mode				= FALSE;

	// register + mark
	mdbg	dbg				=  { _p,_size };
	debug_info.push_back	(dbg);
	u8*			_ptr		= (u8*)	_p;
	u32*		_shred		= (u32*)(_ptr + _size);
	*_shred					= u32	(-1);

	debug_mode				= TRUE;
	debug_cs.Leave			();
}
void	xrMemory::dbg_unregister	(void* _p)
{
	VERIFY					(debug_mode);
	debug_cs.Enter			();
	debug_mode				= FALSE;

	// search entry
	u32	_found				= u32(-1);

	if (!debug_info.empty())
	{
		for (int it=int(debug_info.size()-1); it>=0; it--)
			if (debug_info[it]._p==_p)	{ _found=it; break; }
	}

	// unregister entry
	if (u32(-1)==_found)	{ 
		Debug.fatal			("Memory allocation error"); 
	} else	{
		u8*			_ptr	= (u8*)	debug_info[_found]._p;
		u32*		_shred	= (u32*)(_ptr + debug_info[_found]._size);
		R_ASSERT2			(u32(-1)==*_shred, "Memory overrun error");
		debug_info[_found]._p		= NULL; 
		debug_info[_found]._size	= 0;
		debug_info_update			++;
	}

	// perform cleanup
	if (debug_info_update>1024)
	{
		debug_info_update	=	0;
		debug_info.erase	(std::remove_if(debug_info.begin(),debug_info.end(),pred_mdbg),debug_info.end());
	}

	debug_mode				= TRUE;
	debug_cs.Leave			();
}

extern	u32		get_header		(void*	P);
extern	u32		get_pool		(size_t size);

void	xrMemory::dbg_check		()
{
	if (!debug_mode)		return;

	// Check RO strings
	g_pStringContainer->verify	();

	// Check overrun
	debug_cs.Enter			();
	debug_mode				= FALSE;
	for (int it=0; it<int(debug_info.size()); it++)
	{
		if (0==debug_info[it]._p)	
			continue;

		// check header
		R_ASSERT2			(get_header(debug_info[it]._p)==get_pool(debug_info[it]._size),"CorePanic: Memory block header corrupted");

		// check footer
		u8*			_ptr	= (u8*)	debug_info[it]._p;
		u32*		_shred	= (u32*)(_ptr + debug_info[it]._size);
		R_ASSERT2			(u32(-1)==*_shred, "CorePanic: Memory overrun error");
	}

	// crt-check
	R_ASSERT2(_HEAPOK==_heapchk(),					"CorePanic: CRT heap corruption");
	R_ASSERT2(HeapValidate(GetProcessHeap(),0,0),	"CorePanic: Win32 heap corruption");

	// leave
	debug_mode				= TRUE;
	debug_cs.Leave			();
}
#endif
