#include "stdafx.h"
#include "xrPool.h"
#include "r_constants.h"

static	poolSS<R_constant,64>	g_constant_allocator;

BOOL	R_constant_table::parse	(D3DXSHADER_CONSTANTTABLE* desc, u16 destination)
{
	u32	dwCount					= desc->Constants;
	D3DXSHADER_CONSTANTINFO* it	= (D3DXSHADER_CONSTANTINFO*) (LPBYTE(desc)+desc->ConstantInfo);
	while (dwCount)
	{
		// Name
		LPCSTR	name		=	LPCSTR(LPBYTE(it)+it->Name);

		// Type
		u16		type		=	RC_float;
		if	(D3DXRS_BOOL == it->RegisterSet)	type	= RC_bool;
		if	(D3DXRS_INT4 == it->RegisterSet)	type	= RC_int;

		// 

		R_constant*	C		=	g_constant_allocator.create();
	}
	return TRUE;
}
