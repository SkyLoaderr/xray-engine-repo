#ifndef FS_internalH
#define FS_internalH
#pragma once

#include "lzhuf.h"

void* __stdcall FileDownload	(LPCSTR fn, u32* pdwSize=NULL);
void			FileCompress	(const char *fn, const char* sign, void* data, u32 size);
void * 			FileDecompress	(const char *fn, const char* sign, u32* size=NULL);

class CFileWriter : public IWriter
{
private:
	FILE*			hf;
public:
	CFileWriter		(const char *name)
	{
		fName		= xr_strdup(name);
		R_ASSERT	(name);
		R_ASSERT	(name[0]);
		VerifyPath	(fName);
		hf			= fopen(fName, "wb");
		R_ASSERT3	(hf, "Can't write file. File may be open or in use.", fName );
		setvbuf		(hf, 0, _IOFBF, 24*1024 );
	}

	virtual ~CFileWriter()
	{
		fclose		(hf);
	}

	// kernel
	virtual void	w			(const void* _ptr, u32 count) 
    { 
		if (0!=count){
			const u32 mb_sz = 0x1000000;
			u8* ptr 		= (u8*)_ptr;
			for (int req_size = count; req_size>mb_sz; req_size-=mb_sz, ptr+=mb_sz){
				size_t W = fwrite(ptr,mb_sz,1,hf);
				R_ASSERT2(W==1,"Can't write mem block to file. Disk maybe full.");
			}
			if (req_size)	{
				size_t W = fwrite(ptr,req_size,1,hf); 
				R_ASSERT2(W==1,"Can't write mem block to file. Disk maybe full.");
			}
		}
    };
	virtual void	seek		(u32 pos)	{	fseek(hf,pos,SEEK_SET);		};
	virtual u32		tell		()			{	return ftell(hf);			};
};

// It automatically frees memory after destruction
class CTempReader : public IReader
{
public:
	CTempReader(void *_data, int _size) : IReader(_data,_size)
	{}
	virtual ~CTempReader();
};
class CFileReader : public IReader
{
public:
	CFileReader(const char *name);
	virtual ~CFileReader();
};
class CCompressedReader : public IReader
{
public:
	CCompressedReader(const char *name, const char *sign);
	virtual ~CCompressedReader();
};
class CVirtualFileReader : public IReader
{
private:
   void			*hSrcFile, *hSrcMap;
public:
				CVirtualFileReader(const char *cFileName);
	virtual		~CVirtualFileReader();
};

#endif