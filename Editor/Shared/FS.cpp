// FS.cpp: implementation of the CFS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
// memory
CFS_Memory::~CFS_Memory() 
{	_FREE(data);	}
void CFS_Memory::write	(const void* ptr, DWORD count)
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

//---------------------------------------------------
// base stream
CStream*	CStream::OpenChunk(DWORD ID)
{
	BOOL	bCompressed;
	DWORD	dwSize = FindChunk(ID,&bCompressed);
	if (dwSize!=0) {
		if (bCompressed) {
			BYTE*		dest;
			unsigned	dest_sz;
			_decompressLZ(&dest,&dest_sz,Pointer(),dwSize);
			return new CTempStream(dest,dest_sz);
		} else {
			return new CStream(Pointer(),dwSize);
		}
	} else return 0;
};
void	CStream::Close()
{	delete this; }

//---------------------------------------------------
// temp stream
CTempStream::~CTempStream()
{	_FREE(data);	};
//---------------------------------------------------
// file stream
CFileStream::~CFileStream()
{	_FREE(data);	};
//---------------------------------------------------
// compressed stream
CCompressedStream::~CCompressedStream()
{	_FREE(data);	};


//---------------------------------------------------
ENGINE_API void *FileDownload(const char *fn, DWORD *pdwSize)
{
	int		hFile;
	DWORD	size;
	void*	buf;

#ifdef _EDITOR
	hFile	= open(fn,O_RDONLY|O_BINARY);
	Log->Msg(mtInformation,"* FS: Download %s",fn);
	R_ASSERT(hFile>0);
	size	= filelength(hFile);
#else
	hFile	= _open(fn,O_RDONLY|O_BINARY|O_SEQUENTIAL);
	Log("* FS: Download ",fn);
	R_ASSERT(hFile>0);
	size	= _filelength(hFile);
#endif
	buf		= malloc(size);
	_read	(hFile,buf,size);
	_close	(hFile);
	if (pdwSize) *pdwSize = size;
	return buf;
}

typedef char MARK[8];
IC void mk_mark(MARK& M, const char* S)
{	strncpy(M,S,8); }

ENGINE_API void		FileCompress	(const char *fn, const char* sign, void* data, DWORD size)
{
	MARK M; mk_mark(M,sign);

#ifdef _EDITOR
	int H	= open(fn,O_BINARY|O_CREAT|O_WRONLY|O_TRUNC,S_IREAD|S_IWRITE);
#else
	int H	= _open(fn,O_BINARY|O_CREAT|O_WRONLY|O_TRUNC,S_IREAD|S_IWRITE);
#endif
	_write	(H,&M,8);
	_writeLZ(H,data,size);
	_close	(H);
}

ENGINE_API void *	FileDecompress	(const char *fn, const char* sign, DWORD* size)
{
	MARK M,F; mk_mark(M,sign);

	int	H = _open	(fn,O_BINARY|O_RDONLY);
	_read	(H,&F,8);
    R_ASSERT(strcmp(M,F)==0);
//	R_ASSERT(M==F); 
	void* ptr = 0; DWORD SZ;
	SZ = _readLZ (H, ptr, filelength(H)-8);
	_close	(H);
	if (size) *size = SZ;
	return ptr;
}
