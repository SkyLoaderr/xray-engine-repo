// FS.h: interface for the CFS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FS_H__F9718331_BA98_4DD3_96FC_C455405D54FB__INCLUDED_)
#define AFX_FS_H__F9718331_BA98_4DD3_96FC_C455405D54FB__INCLUDED_

#include "lzhuf.h"

extern ENGINE_API void *	FileDownload	(const char *fn, DWORD* pdwSize=NULL);
extern ENGINE_API void		FileCompress	(const char *fn, const char* sign, void* data, DWORD size);
extern ENGINE_API void *	FileDecompress	(const char *fn, const char* sign, DWORD* size=NULL);

#define CFS_CompressMark	(1ul << 31ul)
#define CFS_AlignMark		(1ul << 30ul)

const char term[2]={13,10};

class ENGINE_API CFS_Base {
private:
	std::stack<int>	chunk_pos;
	int				align_correction;

	IC DWORD		correction	(DWORD p)
	{
		if (p%16) {
			return ((p%16)+1)*16 - p;
		} return 0;
	}
public:
	virtual ~CFS_Base() {
		while (!chunk_pos.empty())
			close_chunk();
	}

	// kernel
	virtual void	write	(const void* ptr, DWORD count)	= 0;
	virtual void	seek	(DWORD pos)						= 0;
	virtual DWORD	tell	()								= 0;

	// generalized writing functions
	IC void	Wdword(DWORD d)
	{	write(&d,sizeof(DWORD));	}
	IC void	Wword(WORD d)
	{	write(&d,sizeof(WORD));		}
	IC void	Wfloat(float d)
	{	write(&d,sizeof(float));	}
	IC void	Wbyte(BYTE d)
	{	write(&d,sizeof(BYTE));		}
	IC void	Wstring(const char *p)
	{	write(p,strlen(p));
		write(term,2);		 	    }
	IC void	WstringZ(const char *p)
	{	write(p,strlen(p)+1);		}
	IC void	Wvector(const Fvector &v)
	{	write(&v,3*sizeof(float));	}
	IC void	Wcolor(const Fcolor &v)
	{	write(&v,4*sizeof(float));	}

	// generalized chunking
	IC DWORD	align		()
	{
		DWORD bytes = correction(tell());
		DWORD copy  = bytes;
		while (bytes) { Wbyte(0); bytes--; }
		return copy;
	}
	IC void		open_chunk	(DWORD type)
	{
		Wdword(type);
		chunk_pos.push(tell());
		Wdword(0);	// the place for 'size'
		if (type&CFS_AlignMark)	align_correction = align();
		else					align_correction = 0;
	}
	IC void	close_chunk	()
	{
		VERIFY(!chunk_pos.empty());

		int pos  = tell();
		seek	(chunk_pos.top());
		Wdword	(pos-chunk_pos.top()-4-align_correction);
		seek	(pos);
		chunk_pos.pop();
	}
	IC void write_compressed(void* ptr, DWORD count)
	{
		BYTE*		dest	= 0;
		unsigned	dest_sz	= 0;
		_compressLZ(&dest,&dest_sz,ptr,count);
		if (dest && dest_sz)
			write(dest,dest_sz);
		free(dest);
	}
	IC void write_chunk(DWORD type, void* data, DWORD size)
	{
		open_chunk	(type);
		if (type & CFS_CompressMark)	write_compressed(data,size);
		else							write			(data,size);
		close_chunk	();
	}
};

class ENGINE_API CFS_Memory : public CFS_Base
{
	BYTE*		data;
	DWORD		position;
	DWORD		mem_size;
	DWORD		file_size;
public:
	CFS_Memory() {
		data		= 0;
		position	= 0;
		mem_size	= 0;
		file_size	= 0;
	}
	virtual ~CFS_Memory();

	// kernel
	virtual void	write	(const void* ptr, DWORD count);

	virtual void	seek	(DWORD pos)	
	{	position = pos;		}
	virtual DWORD	tell	() 
	{	return position;	}

	// specific
	BYTE*	pointer	()	{ return data; }
	DWORD	size	()	{ return file_size;	}
	void	clear	()  { file_size=0; position=0;	}
	void	SaveTo	(const char* fn, const char* sign)
	{
    	if (sign) FileCompress(fn,sign,pointer(),size());
        else {
        #ifdef _EDITOR
        	int H = open(fn,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,S_IREAD|S_IWRITE);
        #else
        	int H = _open(fn,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,S_IREAD|S_IWRITE);
        #endif
            R_ASSERT(H>0);
            _write(H,pointer(),size());
            _close(H);
        }
	}
};

class ENGINE_API CFS_File : public CFS_Base
{
private:
	FILE*			hf;
public:
	CFS_File		(const char *name)
	{
		R_ASSERT(name);
		R_ASSERT(name[0]);
		hf = fopen(name, "wb");
		R_ASSERT(hf);
		setvbuf(hf, 0, _IOFBF, 24*1024 );
	}
	
	virtual ~CFS_File()
	{
		fclose(hf);
	}
	
	// kernel
	virtual void	write	(const void* ptr, DWORD count) {
		fwrite(ptr,count,1,hf);
	};
	virtual void	seek	(DWORD pos) {
		fseek(hf,pos,SEEK_SET);	
	};
	virtual DWORD	tell	() {
		return ftell(hf);
	};
};
//------------------------------------------------------------------------------------
class ENGINE_API CStream {
protected:
	char *		data;
	int			Pos;
	int			Size;

	IC DWORD		correction	(DWORD p)
	{
		if (p%16) {
			return ((p%16)+1)*16 - p;
		} return 0;
	}
	IC BOOL isTerm(char a)		{ return (a==13)||(a==10); };
public:
	CStream		()
	{
		Pos		= 0;
	}
	CStream		(void *_data, int _size)
	{
		data	= (char *)_data;
		Size	= _size;
		Pos		= 0;
	};
	virtual ~CStream() {};

	IC int	Elapsed(void)			// сколько байт осталось
	{	return Size-Pos;		};
	IC BOOL	Eof(void)				// есть ли еще данные для чтения
	{	return Elapsed()<=0;	};
	IC int	Tell(void)				// позиция указателя чтения
	{	return Pos;				};
	IC int	Length(void)			// размер файла
	{	return Size;			};
	IC void* Pointer(void)			// указатель на память
	{	return &(data[Pos]);	};
	IC void	Seek(int ptr)			// перемещение указателя
	{	Pos=ptr;	VERIFY((Pos<=Size) && (Pos>=0));	};
	IC void	Advance(int cnt)		// перемещение указателя
	{	Pos+=cnt;	VERIFY((Pos<=Size) && (Pos>=0));	};
	IC void	Read(void *p,int cnt)	// главная ф-я чтения
	{	VERIFY(Pos+cnt<=Size);
		CopyMemory(p,Pointer(),cnt);
		Advance(cnt);
	};
	IC void	Rstring(char *dest)	// читает строку до CR/LF
	{
		char *src = (char *) data;
		while (!Eof()) {
			if (isTerm(src[Pos])) {
				*dest = 0;
				Pos++;
				if (!Eof() && isTerm(src[Pos])) Pos++;
				return;
			}
			*dest++ = src[Pos++];
		}
		*dest	=	0;
	};
	IC void	RstringZ(char *dest)	// читает C-Style строку (ANSIZ)
	{
		char *src = (char *) data;
		while ((src[Pos]!=0) && (!Eof())) *dest++ = src[Pos++];
		*dest	=	0;
		Pos		++;
	};
	IC void	SkipStringZ()
	{
		char *src = (char *) data;
		while ((src[Pos]!=0) && (!Eof())) Pos++;
		Pos		++;
	};
	IC DWORD	Rdword(void)
	{	DWORD tmp;	Read(&tmp,sizeof(tmp)); return tmp;	};
	IC BYTE		Rbyte(void)
	{	BYTE tmp;	Read(&tmp,sizeof(tmp));	return tmp; };
	IC char		Rchar(void)
	{	char tmp;	Read(&tmp,sizeof(tmp));	return tmp; };
	IC int		Rint(void)
	{	int tmp;	Read(&tmp,sizeof(tmp));	return tmp; };
	IC WORD		Rword(void)
	{	WORD tmp;	Read(&tmp,sizeof(tmp));	return tmp; };
	IC float	Rfloat(void)
	{	float tmp;	Read(&tmp,sizeof(tmp)); return tmp;	};
	IC void	Rvector(Fvector &v)
	{	Read(&v,3*sizeof(float));	}
	IC void	Rcolor(Fcolor &v)
	{	Read(&v,4*sizeof(float));	}

	// Set file pointer to start of chunk data (0 for root chunk)
	void	Rewind	(void)
	{	Seek(0); }

	DWORD		FindChunk(DWORD ID, BOOL* bCompressed=0)	// поиск XR Chunk'ов - возврат - размер или 0
	{
		DWORD	dwSize,dwType;

		Rewind();
		while (!Eof()) {
			dwType = Rdword();
			dwSize = Rdword();
			if (dwType&CFS_AlignMark) Advance(correction(Tell()));
			if ((dwType&(~CFS_CompressMark)) == ID) {
				if (bCompressed) *bCompressed = dwType&CFS_CompressMark;
				return dwSize;
			}
			else				Advance(dwSize);
		}
		return 0;
	};
	BOOL		ReadChunk(DWORD ID, void *dest)	// чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
	{
		DWORD	dwSize = FindChunk(ID);
		if (dwSize!=0) {
			Read(dest,dwSize);
			return TRUE;
		} else return FALSE;
	};
	CStream*	OpenChunk(DWORD ID);
	void		Close();
};

// It automatically frees memory after destruction
class ENGINE_API CTempStream : public CStream
{
public:
	CTempStream(void *_data, int _size) : CStream(_data,_size)
	{}
	virtual ~CTempStream();
};
class ENGINE_API CFileStream : public CStream
{
public:
	CFileStream(const char *name)
	{
		data	= (char *)FileDownload(name,(DWORD *)&Size);
		Pos		= 0;
	};
	virtual ~CFileStream();
};
class ENGINE_API CCompressedStream : public CStream
{
public:
	CCompressedStream(const char *name, const char *sign)
	{
		data	= (char *)FileDecompress(name,sign,(DWORD*)&Size);
		Pos		= 0;
	}
	virtual ~CCompressedStream();
};
class ENGINE_API CVirtualFileStream : public CStream
{
private:
   HANDLE	hSrcFile,hSrcMap;
public:
	CVirtualFileStream(const char *cFileName) {
		// Open the file
		hSrcFile = CreateFile(cFileName, GENERIC_READ, FILE_SHARE_READ,
			0, OPEN_EXISTING, 0, 0);
		R_ASSERT(hSrcFile!=INVALID_HANDLE_VALUE);
		Size = (int)GetFileSize(hSrcFile, NULL);
		R_ASSERT(Size);

		hSrcMap = CreateFileMapping (hSrcFile, 0, PAGE_READONLY, 0, 0, 0);
		R_ASSERT(hSrcMap!=INVALID_HANDLE_VALUE);

		data = (char*)MapViewOfFile (hSrcMap, FILE_MAP_READ, 0, 0, 0);
		R_ASSERT(data);
	}
	virtual ~CVirtualFileStream() {
        UnmapViewOfFile ((void*)data);
		CloseHandle		(hSrcMap);
		CloseHandle		(hSrcFile);
	}
};

#endif // !defined(AFX_FS_H__F9718331_BA98_4DD3_96FC_C455405D54FB__INCLUDED_)
