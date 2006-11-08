#include "stdafx.h"
#pragma hdrstop

#include	"xrsharedmem.h"

#include	<malloc.h>

xrMemory	Memory;
BOOL		mem_initialized	= FALSE;
bool		shared_str_initialized	= false;

// Processor specific implementations
extern		pso_MemCopy		xrMemCopy_MMX;
extern		pso_MemCopy		xrMemCopy_x86;
extern		pso_MemFill		xrMemFill_x86;
extern		pso_MemFill32	xrMemFill32_MMX;
extern		pso_MemFill32	xrMemFill32_x86;

#ifdef DEBUG
XRCORE_API void dump_phase		()
{
	if (!Memory.debug_mode)
		return;

	static int					phase_counter = 0;

	string256					temp;
	sprintf						(temp,"x:\\$phase$%d.dump",++phase_counter);
	Memory.mem_statistic		(temp);
	}
#endif // DEBUG

xrMemory::xrMemory()
#ifdef PROFILE_CRITICAL_SECTIONS
	:debug_cs(MUTEX_PROFILE_ID(xrMemory))
#endif // PROFILE_CRITICAL_SECTIONS
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

//	dump_phase					();
	g_pStringContainer			= xr_new<str_container>		();
	shared_str_initialized		= true;
//	dump_phase					();
	g_pSharedMemoryContainer	= xr_new<smem_container>	();
//	dump_phase					();
}

extern void dbg_dump_leaks();
extern void dbg_dump_str_leaks();
void	xrMemory::_destroy()
{
#ifndef M_BORLAND
#ifdef DEBUG
	if (debug_mode)				dbg_dump_str_leaks	();
#endif
#endif
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
		FATAL			("bad pointer to heap");
		break;
	case _HEAPBADBEGIN:
		FATAL			("bad start of heap");
		break;
	case _HEAPBADNODE:
		FATAL			("bad node in heap");
		break;
	}
	return (u32) total;
}

#ifndef DEBUG
void	xrMemory::mem_statistic	(LPCSTR fn)
{
}
#else
ICF	u8*		acc_header			(void* P)	{	u8*		_P		= (u8*)P;	return	_P-1;	}
ICF	u32		get_header			(void* P)	{	return	(u32)*acc_header(P);				}
void	xrMemory::mem_statistic	(LPCSTR fn)
{
	if (!debug_mode)	return	;
	mem_compact				()	;

	debug_cs.Enter			()	;
	debug_mode				= FALSE;

	FILE*		Fa			= fopen		(fn,"w");
	fprintf					(Fa,"$BEGIN CHUNK #0\n");
	fprintf					(Fa,"POOL: %d %dKb\n",mem_pools_count,mem_pools_ebase);

	fprintf					(Fa,"$BEGIN CHUNK #1\n");
	for (u32 k=0; k<mem_pools_count; ++k)
		fprintf				(Fa,"%2d: %d %db\n",k,mem_pools[k].get_block_count(),(k+1)*16);
	
	fprintf					(Fa,"$BEGIN CHUNK #2\n");
	for (u32 it=0; it<debug_info.size(); it++)
	{
		if (0==debug_info[it]._p)	continue	;

		u32 p_current		= get_header(debug_info[it]._p);
		int pool_id			= (mem_generic==p_current)?-1:p_current;

		fprintf				(Fa,"0x%08X[%2d]: %8d %s\n",*(u32*)(&debug_info[it]._p),pool_id,debug_info[it]._size,debug_info[it]._name);
	}

	{
		for (u32 k=0; k<mem_pools_count; ++k) {
			MEMPOOL			&pool = mem_pools[k];
			u8				*list = pool.list;
			while (list) {
				pool.cs.Enter	();
				u32				temp = *(u32*)(&list);
				if (!temp)
					break;
				fprintf			(Fa,"0x%08X[%2d]: %8d mempool\n",temp,k,pool.s_element);
				list			= (u8*)*pool.access(list);
				pool.cs.Leave	();
			}
		}
	}

	/*
	fprintf					(Fa,"$BEGIN CHUNK #3\n");
	for (u32 it=0; it<debug_info.size(); it++)
	{
		if (0==debug_info[it]._p)	continue	;
		try{
			if (0==strcmp(debug_info[it]._name,"storage: sstring"))
				fprintf		(Fa,"0x%08X: %8d %s %s\n",*(u32*)(&debug_info[it]._p),debug_info[it]._size,debug_info[it]._name,((str_value*)(*(u32*)(&debug_info[it]._p)))->value);
		}catch(...){
		}
	}
	*/

	fclose		(Fa)		;

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
	CopyMemory		(memory,string,len);
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
