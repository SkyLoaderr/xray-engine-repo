// LocalAllocator.h: interface for the CLocalAllocator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALALLOCATOR_H__BB213680_25A3_11D4_B4E3_00C02610C34E__INCLUDED_)
#define AFX_LOCALALLOCATOR_H__BB213680_25A3_11D4_B4E3_00C02610C34E__INCLUDED_

#pragma once

#define LA_MAX_BLOCK_NUMBER	32

class CLocalAllocator
{
	char*	pMemory;
	BOOL	bFree[LA_MAX_BLOCK_NUMBER];
	u32	dwCount;
	u32	dwSize;
public:
	CLocalAllocator		(u32 max_blocks, u32 max_block_size);
	~CLocalAllocator	();

	void*	Alloc		(u32 size	);
	void	Free		(void *ptr	);
};

#endif // !defined(AFX_LOCALALLOCATOR_H__BB213680_25A3_11D4_B4E3_00C02610C34E__INCLUDED_)
