// LocalAllocator.cpp: implementation of the CLocalAllocator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LocalAllocator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalAllocator::CLocalAllocator(DWORD max_blocks, DWORD max_block_size)
{
	// Aligned on 8b boundary
	dwSize	= ((max_block_size>>3)+1)<<3;
	dwCount	= max_blocks;

	VERIFY(dwCount>0);
	VERIFY(dwCount<=LA_MAX_BLOCK_NUMBER);

	// Preallocate memory
	pMemory = (char *)malloc(dwCount*dwSize);

	// Mark all blocks as FREE
	for (DWORD i=0; i<dwCount; i++)	bFree[i]=TRUE;
}

CLocalAllocator::~CLocalAllocator()
{
#ifdef DEBUG
	for (DWORD i=0; i<dwCount; i++) {
		VERIFY(bFree[i]);
	}
#endif
	free(pMemory);
}

void*	CLocalAllocator::Alloc(DWORD _size) {
	VERIFY(_size <= dwSize);
	for (DWORD i=0; i<dwCount; i++) {
		if (bFree[i]) {
			bFree[i]=FALSE;
			return (void *)( pMemory + i*dwSize );
		}
	}
	// free block not not found
	THROW;
}

void	CLocalAllocator::Free(void *ptr) {
	DWORD idx = (((char *)ptr)-pMemory)/dwSize;
	VERIFY(bFree[idx]);
	bFree[idx]=TRUE;
}
