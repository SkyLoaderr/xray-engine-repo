#include "stdafx.h"
#include "xrPool.h"
#include "r_constants.h"

static	poolSS<R_constant,64>	g_constant_allocator;

BOOL	R_constant_table::parse	(D3DXSHADER_CONSTANTTABLE* desc, u16 destination)
{
}
