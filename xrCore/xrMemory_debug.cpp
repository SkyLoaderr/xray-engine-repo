#include "StdAfx.h"
#pragma hdrstop

void	xrMemory::dbg_register		(void* _p)
{
	VERIFY					(debug_mode);
	debug_cs.Enter			();
	debug_mode				= FALSE;

	debug_info.push_back	(_p);

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
		for (int it=debug_info.size()-1; it>=0; it--)
			if (debug_info[it]==_p)		{ _found=it; break; }
	}

	if (u32(-1)==_found)	{ 
		__asm int 3;	
		Debug.fatal("Memory allocation error"); 
	} else	{ 
		debug_info[_found]	= NULL; 
		debug_info_update++;
	}

	if (debug_info_update>1024)
	{
		debug_info_update	= 0;
		debug_info.erase	(std::remove(debug_info.begin(),debug_info.end(),(void*)0),debug_info.end());
	}

	debug_mode				= TRUE;
	debug_cs.Leave			();
}

