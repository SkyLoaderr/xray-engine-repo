#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default:4995)

#ifdef M_BORLAND
	#define O_SEQUENTIAL 0
#endif
//////////////////////////////////////////////////////////////////////
// Tools
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
void VerifyPath(LPCSTR path)
{
	string1024 tmp;
	for(int i=0;path[i];i++){
		if( path[i]!='\\' || i==0 )
			continue;
		Memory.mem_copy( tmp, path, i );
		tmp[i] = 0;
		CreateDirectory( tmp, 0 );
	}
    return TRUE;
}
void* __stdcall FileDownload(LPCSTR fn, u32* pdwSize)
{
	int		hFile;
	u32	size;
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

void __stdcall FileCompress	(const char *fn, const char* sign, void* data, u32 size)
{
	MARK M; mk_mark(M,sign);

	int H	= open(fn,O_BINARY|O_CREAT|O_WRONLY|O_TRUNC,S_IREAD|S_IWRITE);
	R_ASSERT2(H>0,fn);
	_write	(H,&M,8);
	_writeLZ(H,data,size);
	_close	(H);
}

void* __stdcall FileDecompress	(const char *fn, const char* sign, u32* size)
{
	MARK M,F; mk_mark(M,sign);

	int	H = open	(fn,O_BINARY|O_RDONLY);
	R_ASSERT2(H>0,fn);
	_read	(H,&F,8);
	if (strncmp(M,F,8)!=0)		{
		F[8]=0;		Msg("FATAL: signatures doesn't match, file(%s) / requested(%s)",F,sign);
	}
    R_ASSERT(strncmp(M,F,8)==0);

	void* ptr = 0; u32 SZ;
	SZ = _readLZ (H, ptr, filelength(H)-8);
	_close	(H);
	if (size) *size = SZ;
	return ptr;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
// memory
CMemoryWriter::~CMemoryWriter() 
{	xr_free(data);	}

void CMemoryWriter::w	(const void* ptr, u32 count)
{
	if (position+count > mem_size) {
		// reallocate
		if (mem_size==0)	mem_size=128;
		while (mem_size <= (position+count)) mem_size*=2;
		if (0==data)		data = (BYTE*)	xr_malloc	(mem_size);
		else				data = (BYTE*)	xr_realloc	(data,mem_size);
	}
	Memory.mem_copy	(data+position,ptr,count);
	position		+=count;
	if (position>file_size) file_size=position;
}

static const u32 mb_sz = 0x1000000;
void CMemoryWriter::save_to	(const char* fn, const char* sign)
{
	if (sign) FileCompress(fn,sign,pointer(),size());
	else {
		int H = open(fn,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,S_IREAD|S_IWRITE);
		R_ASSERT(H>0);
        LPBYTE ptr 		= pointer();
        for (int req_size = size(); req_size>mb_sz; req_size-=mb_sz, ptr+=mb_sz){
			int W = _write(H,ptr,mb_sz);
			R_ASSERT2(W==mb_sz,"Can't write mem block to file.");
        }
        int W = _write(H,ptr,req_size);
		R_ASSERT2(W==req_size,"Can't write mem block to file.");
		_close(H);
	}
}


u32	IWriter::align		()
{
	u32 bytes = correction(tell());
	u32 copy  = bytes;
	while (bytes) { w_u8(0); bytes--; }
	return copy;
}
void	IWriter::open_chunk	(u32 type)
{
	w_u32(type);
	chunk_pos.push(tell());
	w_u32(0);	// the place for 'size'
	if (type&CFS_AlignMark)	align_correction = align();
	else					align_correction = 0;
}
void	IWriter::close_chunk	()
{
	VERIFY(!chunk_pos.empty());

	int pos			= tell();
	seek			(chunk_pos.top());
	w_u32			(pos-chunk_pos.top()-4-align_correction);
	seek			(pos);
	chunk_pos.pop	();
}
u32	IWriter::chunk_size	()					// returns size of currently opened chunk, 0 otherwise
{
	if (chunk_pos.empty())	return 0;
	return tell() - chunk_pos.top()-4-align_correction;
}
void	IWriter::w_compressed(void* ptr, u32 count)
{
	BYTE*		dest	= 0;
	unsigned	dest_sz	= 0;
	_compressLZ(&dest,&dest_sz,ptr,count);
	if (dest && dest_sz)
		w(dest,dest_sz);
	xr_free		(dest);
}
void	IWriter::w_chunk(u32 type, void* data, u32 size)
{
	open_chunk	(type);
	if (type & CFS_CompressMark)	w_compressed(data,size);
	else							w			(data,size);
	close_chunk	();
}

//---------------------------------------------------
// base stream
IReader*	IReader::open_chunk(u32 ID)
{
	BOOL	bCompressed;
	u32	dwSize = find_chunk(ID,&bCompressed);
	if (dwSize!=0) {
		if (bCompressed) {
			BYTE*		dest;
			unsigned	dest_sz;
			_decompressLZ(&dest,&dest_sz,pointer(),dwSize);
			return xr_new<CTempReader>(dest,dest_sz);
		} else {
			return xr_new<IReader>(pointer(),dwSize);
		}
	} else return 0;
};
void	IReader::close()
{	xr_delete((IReader*)this); }

void	IReader::r	(void *p,int cnt)
{
	VERIFY		(Pos+cnt<=Size);
	Memory.mem_copy(p,pointer(),cnt);
	advance		(cnt);
};

//---------------------------------------------------
// temp stream
CTempReader::~CTempReader()
{	xr_free(data);	};
//---------------------------------------------------
// file stream
CFileReader::CFileReader(const char *name)
{
    data	= (char *)FileDownload(name,(u32 *)&Size);
    Pos		= 0;
};
CFileReader::~CFileReader()
{	xr_free(data);	};
//---------------------------------------------------
// compressed stream
CCompressedReader::CCompressedReader(const char *name, const char *sign)
{
    data	= (char *)FileDecompress(name,sign,(u32*)&Size);
    Pos		= 0;
}
CCompressedReader::~CCompressedReader()
{	xr_free(data);	};

