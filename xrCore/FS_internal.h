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
	virtual void	w			(const void* ptr, u32 count) { fwrite(ptr,count,1,hf);};
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
   HANDLE	hSrcFile,hSrcMap;
public:
	CVirtualFileReader(const char *cFileName) {
		// Open the file
		hSrcFile = CreateFile(cFileName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
			0, OPEN_EXISTING, 0, 0);
		R_ASSERT(hSrcFile!=INVALID_HANDLE_VALUE);
		Size = (int)GetFileSize(hSrcFile, NULL);
		R_ASSERT(Size);

		hSrcMap = CreateFileMapping (hSrcFile, 0, PAGE_READONLY, 0, 0, 0);
		R_ASSERT(hSrcMap!=INVALID_HANDLE_VALUE);

		data = (char*)MapViewOfFile (hSrcMap, FILE_MAP_READ, 0, 0, 0);
		R_ASSERT(data);
	}
	virtual ~CVirtualFileReader() {
        UnmapViewOfFile ((void*)data);
		CloseHandle		(hSrcMap);
		CloseHandle		(hSrcFile);
	}
};

#endif