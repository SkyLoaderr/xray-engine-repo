#include "stdafx.h"
#pragma hdrstop

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

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
		if (0==data)		data = (BYTE*)	xr_malloc	(mem_size);
		else				data = (BYTE*)	xr_realloc	(data,mem_size);
	}
#ifdef ENGINE_BUILD	
	PSGP.memCopy	(data+position,ptr,count);
#else
	CopyMemory		(data+position,ptr,count);
#endif
	position		+=count;
	if (position>file_size) file_size=position;
}

void	CFS_Memory::SaveTo	(const char* fn, const char* sign)
{
	if (sign) FileCompress(fn,sign,pointer(),size());
	else {
		int H = open(fn,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,S_IREAD|S_IWRITE);
		R_ASSERT(H>0);
		_write(H,pointer(),size());
		_close(H);
	}
}


DWORD	CFS_Base::align		()
{
	DWORD bytes = correction(tell());
	DWORD copy  = bytes;
	while (bytes) { Wbyte(0); bytes--; }
	return copy;
}
void	CFS_Base::open_chunk	(DWORD type)
{
	Wdword(type);
	chunk_pos.push(tell());
	Wdword(0);	// the place for 'size'
	if (type&CFS_AlignMark)	align_correction = align();
	else					align_correction = 0;
}
void	CFS_Base::close_chunk	()
{
	VERIFY(!chunk_pos.empty());

	int pos			= tell();
	seek			(chunk_pos.top());
	Wdword			(pos-chunk_pos.top()-4-align_correction);
	seek			(pos);
	chunk_pos.pop	();
}
DWORD	CFS_Base::chunk_size	()					// returns size of currently opened chunk, 0 otherwise
{
	if (chunk_pos.empty())	return 0;
	return tell() - chunk_pos.top()-4-align_correction;
}
void	CFS_Base::write_compressed(void* ptr, DWORD count)
{
	BYTE*		dest	= 0;
	unsigned	dest_sz	= 0;
	_compressLZ(&dest,&dest_sz,ptr,count);
	if (dest && dest_sz)
		write(dest,dest_sz);
	xr_free		(dest);
}
void	CFS_Base::write_chunk(DWORD type, void* data, DWORD size)
{
	open_chunk	(type);
	if (type & CFS_CompressMark)	write_compressed(data,size);
	else							write			(data,size);
	close_chunk	();
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

#ifdef	ENGINE_BUILD
void	CStream::Read	(void *p,int cnt)
{
	VERIFY		(Pos+cnt<=Size);
	PSGP.memCopy(p,Pointer(),cnt);
	Advance		(cnt);
};
#else
void	CStream::Read	(void *p,int cnt)
{
	VERIFY(Pos+cnt<=Size);
	CopyMemory(p,Pointer(),cnt);
	Advance(cnt);
};
#endif

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

	hFile	= open(fn,O_RDONLY|O_BINARY|O_SEQUENTIAL);
	R_ASSERT2(hFile>0,fn);
	size	= filelength(hFile);

	buf		= xr_malloc	(size);
	_read	(hFile,buf,size);
	_close	(hFile);
	if (pdwSize) *pdwSize = size;
	return buf;
}

typedef char MARK[9];
IC void mk_mark(MARK& M, const char* S)
{	strncpy(M,S,8); }

ENGINE_API void		FileCompress	(const char *fn, const char* sign, void* data, DWORD size)
{
	MARK M; mk_mark(M,sign);

	int H	= open(fn,O_BINARY|O_CREAT|O_WRONLY|O_TRUNC,S_IREAD|S_IWRITE);
	R_ASSERT2(H>0,fn);
	_write	(H,&M,8);
	_writeLZ(H,data,size);
	_close	(H);
}

ENGINE_API void *	FileDecompress	(const char *fn, const char* sign, DWORD* size)
{
	MARK M,F; mk_mark(M,sign);

	int	H = open	(fn,O_BINARY|O_RDONLY);
	R_ASSERT2(H>0,fn);
	_read	(H,&F,8);
	if (strncmp(M,F,8)!=0)		{
		F[8]=0;		Msg("FATAL: signatures doesn't match, file(%s) / requested(%s)",F,sign);
	}
    R_ASSERT(strncmp(M,F,8)==0);

	void* ptr = 0; DWORD SZ;
	SZ = _readLZ (H, ptr, filelength(H)-8);
	_close	(H);
	if (size) *size = SZ;
	return ptr;
}
