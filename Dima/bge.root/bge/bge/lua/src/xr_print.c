#include "stdafx.h"
#include "xr_print.h"
#include "xr_LuaAllocator.h"

size_t	lua_memusage	()
{
	mallinfo	_i	= dlmallinfo	();
	return		_i.uordblks;
}
