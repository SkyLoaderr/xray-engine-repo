////////////////////////////////////////////////////////////////////////////
//	Module 		: memory.cpp
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Hardware namespace
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "memory.h"

void Memory::init		()
{
}

void Memory::mem_copy	(void *dest, const void *src, size_t size)
{
	memcpy	(dest,src,size);
}

void Memory::mem_fill	(void *dest, char val,  size_t size)
{
	memset	(dest,val,size);
}
