#include "stdafx.h"

void	__stdcall	xrMemFill_x86	(void* dest,  u32 value, u32 count)
{
	FillMemory(dest,count,int(value));
}
