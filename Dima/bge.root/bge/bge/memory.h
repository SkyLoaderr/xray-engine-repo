////////////////////////////////////////////////////////////////////////////
//	Module 		: memory.h
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Hardware namespace
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Memory {
	void	init		();
	void	mem_copy	(void *dest, const void *src, size_t size);
	void	mem_fill	(void *dest, char val,  size_t size);
};