#include "stdafx.h"
#pragma hdrstop

#include "xr_print.h"
#include "xr_LuaAllocator.h"

size_t	__cdecl		lua_memusage	()
{
	mallinfo	_i	= dlmallinfo	();
	return		_i.uordblks;
}
