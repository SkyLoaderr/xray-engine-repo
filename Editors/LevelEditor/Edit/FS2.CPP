#include "stdafx.h"
#pragma hdrstop

#include "FS2.h"

// memory
CMemoryStream::~CMemoryStream() 
{	
	_FREE(data);	
}

void CMemoryStream::write(const void* ptr, DWORD count)
{
	if (position+count > mem_size) {
		// reallocate
		if (mem_size==0)	mem_size=128;
		while (mem_size <= (position+count)) mem_size*=2;
		data = (BYTE*) realloc(data,mem_size);
	}
	CopyMemory(data+position,ptr,count);
	position+=count;
	if (position>file_size) file_size=position;
}
