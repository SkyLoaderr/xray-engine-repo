#include "stdafx.h"
#pragma hdrstop

#include	"xrsharedmem.h"

#include	<malloc.h>

xrMemory	Memory;
BOOL		mem_initialized	= FALSE;

// Processor specific implementations
extern		pso_MemCopy		xrMemCopy_MMX;
extern		pso_MemCopy		xrMemCopy_x86;
extern		pso_MemFill		xrMemFill_x86;
extern		pso_MemFill32	xrMemFill32_MMX;
extern		pso_MemFill32	xrMemFill32_x86;

xrMemory::xrMemory()
{
#ifdef DEBUG
	debug_mode	= FALSE;
#endif
	mem_copy	= xrMemCopy_x86;
	mem_fill	= xrMemFill_x86;
	mem_fill32	= xrMemFill32_x86;
}

XRCORE_API	BOOL	g_bMEMO		= FALSE;

void	xrMemory::_initialize	(BOOL bDebug)
{
#ifdef DEBUG
	debug_mode				= bDebug;
	debug_info_update		= 0;
#endif

	stat_calls				= 0;
	stat_counter			= 0;

	u32	features		= CPU::ID.feature & CPU::ID.os_support;
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

#ifndef M_BORLAND
	// initialize POOLs
	u32	element		= mem_pools_ebase;
	u32 sector		= mem_pools_ebase*1024;
	for (u32 pid=0; pid<mem_pools_count; pid++)
	{
		mem_pools[pid]._initialize(element,sector,0x1);
		element		+=	mem_pools_ebase;
	}
#endif    

#ifdef DEBUG
	if (0==strstr(Core.Params,"-memo"))	mem_initialized				= TRUE;
	else								g_bMEMO						= TRUE;
#else
	mem_initialized				= TRUE;
#endif
	g_pStringContainer			= xr_new<str_container>		();
	g_pSharedMemoryContainer	= xr_new<smem_container>	();
}

extern void dbg_dump_leaks();
void	xrMemory::_destroy()
{
	xr_delete					(g_pSharedMemoryContainer);
	xr_delete					(g_pStringContainer);

#ifndef M_BORLAND
#ifdef DEBUG
	if (debug_mode)				dbg_dump_leaks	();
#endif
#endif

	mem_initialized				= FALSE;
	debug_mode					= FALSE;
}

void	xrMemory::mem_compact	()
{
	RegFlushKey						( HKEY_CLASSES_ROOT );
	RegFlushKey						( HKEY_CURRENT_USER );
	_heapmin						( );
	HeapCompact						(GetProcessHeap(),0);
	if (g_pStringContainer)			g_pStringContainer->clean		();
	if (g_pSharedMemoryContainer)	g_pSharedMemoryContainer->clean	();
	SetProcessWorkingSetSize		(GetCurrentProcess(),size_t(-1),size_t(-1));
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

#ifndef DEBUG
void	xrMemory::mem_statistic	()
{
}
#else
LPCSTR	memstat_file			= "x:\\$memstat$.txt";
void	xrMemory::mem_statistic	()
{
	if (!debug_mode)	return	;
	mem_compact				()	;

	debug_cs.Enter			()	;
	debug_mode				= FALSE;

	FILE*		F			= fopen		(memstat_file,"w");
	for (u32 it=0; it<debug_info.size(); it++)
	{
		if (0==debug_info[it]._p)	continue	;
		fprintf			(F,"%8d %s\n",debug_info[it]._size,debug_info[it]._name);
	}
	fclose		(F)			;

	// leave
	debug_mode				= TRUE;
	debug_cs.Leave			();

	/*
	mem_compact				();
	LPCSTR					fn	= "$memstat$.tmp";
	xr_map<u32,u32>			stats;

	if (g_pStringContainer)			Msg	("memstat: shared_str: economy: %d bytes",g_pStringContainer->stat_economy());
	if (g_pSharedMemoryContainer)	Msg	("memstat: shared_mem: economy: %d bytes",g_pSharedMemoryContainer->stat_economy());

	// Dump memory stats into file to avoid reallocation while traversing
	{
		IWriter*	F		= FS.w_open(fn);
		F->w_u32			(0);
		_HEAPINFO			hinfo;
		int					heapstatus;
		hinfo._pentry		= NULL;
		while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
			if (hinfo._useflag == _USEDENTRY)	F->w_u32	(u32(hinfo._size));
		FS.w_close			(F);
	}

	// Read back and perform sorting
	{
		IReader*	F		= FS.r_open	(fn);
		u32 size			= F->r_u32	();
		while (!F->eof())
		{
			size						= F->r_u32	();
			xr_map<u32,u32>::iterator I	= stats.find(size);
			if (I!=stats.end())			I->second += 1;
			else						stats.insert(mk_pair(size,1));
		}
		FS.r_close			(F);
		FS.file_delete		(fn);
	}

	// Output to log
	{
		xr_map<u32,u32>::iterator I		= stats.begin();
		xr_map<u32,u32>::iterator E		= stats.end();
		for (; I!=E; I++)	Msg			("%8d : %-4d [%d]",I->first,I->second,I->first*I->second);
	}
	*/
}
#endif

// xr_strdup
char*			xr_strdup		(const char* string)
{	
	VERIFY	(string);
	u32		len			= u32(xr_strlen(string))+1	;
	char *	memory		= (char*)	Memory.mem_alloc( len
#ifdef DEBUG
		, "strdup"
#endif
		);
	Memory.mem_copy		(memory,string,len);
	return	memory;
}

XRCORE_API		BOOL			is_stack_ptr		( void* _ptr)
{
	int			local_value		= 0;
	void*		ptr_refsound	= _ptr;
	void*		ptr_local		= &local_value;
	ptrdiff_t	difference		= (ptrdiff_t)_abs(s64(ptrdiff_t(ptr_local) - ptrdiff_t(ptr_refsound)));
	return		(difference < (512*1024));
}
