#include "stdafx.h"
#pragma hdrstop

#include "xr_print.h"
#include "xr_LuaAllocator.h"

size_t	__cdecl xr_memusage	()
{
	mallinfo	_i	= dlmallinfo	();
	return		_i.uordblks;
	return	0;	// don't working at this moment
}
