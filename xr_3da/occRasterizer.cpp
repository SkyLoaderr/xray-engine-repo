// occRasterizer.cpp: implementation of the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "occRasterizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

occRasterizer::occRasterizer	()
{

}

occRasterizer::~occRasterizer	()
{

}

void fillDW_8x					(LPDWORD ptr, DWORD size, DWORD value)
{
	LPDWORD end = ptr+size;
	for (; ptr!=end; )
	{
		*ptr++	= value;
		*ptr++	= value;
		*ptr++	= value;
		*ptr++	= value;
		*ptr++	= value;
		*ptr++	= value;
		*ptr++	= value;
		*ptr++	= value;
	}
}

void occRasterizer::clear		()
{
	DWORD size	= occ_dim0*occ_dim0;
	fillDW_8x	(bufFrame,	size,0);
	fillDW_8x	(bufDepth0,	size,)
}