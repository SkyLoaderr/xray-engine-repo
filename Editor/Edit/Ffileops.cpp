#include "pch.h"
#pragma hdrstop

#include "lzhuf.h"

#pragma pack(push)
#pragma pack(1)
struct _file {
        DWORD   flags;
        DWORD   ptr;
        DWORD   size;
        char    name[MAX_PATH];
};
#pragma pack(pop)

static char		cacheNAME[MAX_PATH]="";// name of cached PAK
static _file*	cacheHDR	=0;		// header in PAK
static DWORD	cacheCNT	=0;		// files in cached PAK

BOOL	OpenPAK(char *name) {
	if (stricmp(cacheNAME,name)==0) return true;

	int hf;
	hf = _open(name,O_RDONLY|O_BINARY);
	if (hf==-1) return false;

	DWORD	sign;
	_read(hf,&sign,4);
	if (sign!=0x1980) return false;
	strcpy(cacheNAME,name);
	free(cacheHDR); cacheHDR = 0;
	_read(hf,&cacheCNT,4);
	cacheHDR = (_file *)malloc(cacheCNT*sizeof(_file));
	VERIFY(cacheHDR);
	_read(hf,cacheHDR,cacheCNT*sizeof(_file));
	_close(hf);
//	Log	("* Opening PAK: ",name);
//	DToF		("*    Files:     ",cacheCNT);
	return true;
}

static int __cdecl _compare(const void *e1, const void *e2)
{
	char *	key	= (char *) e1;
	_file*	f	= (_file*) e2;

	return stricmp(key,f->name);
}

void Fatal(char *e, char *d) {
	Log.Msg("! FATAL ERROR: %s, %s",e,d);
	throw -1;
}

void *	LoadPAK(char *name, DWORD *pdwSize)
{
	_file *f = (_file *)bsearch(name, cacheHDR, cacheCNT, sizeof(_file),_compare);
	if (f==NULL) Fatal("File not found in PAK",name);
	int hf = _open(cacheNAME,O_RDONLY|O_BINARY);
	if (hf==-1) Fatal("Can't open PAK",cacheNAME);
	_lseek(hf,f->ptr,SEEK_SET);

	void *buf = NULL;
	if (f->flags&1) {
		// WARN: Don't need to allocate memory for buf - it will be done automatically
		unsigned s = _readLZ(hf,buf,f->size);
		if (pdwSize) *pdwSize = s;
	} else {
		buf = malloc(f->size);
		if (buf==NULL) Fatal("Can't allocate memory for reading",name);
		_read(hf,buf,f->size);
		if (pdwSize) *pdwSize = f->size;
	}
	_close(hf);
	return buf;
}

BOOL InPAK(char *name) {
	return (bsearch(name, cacheHDR, cacheCNT, sizeof(_file),_compare)!=NULL);
}

void *DownloadFile(char *fn, DWORD *pdwSize)
{
	int		hFile;
	DWORD	size;

//	Log("* Loading file: ",fn);
	hFile=_open(fn,O_RDONLY|O_BINARY);
	if (hFile==-1) {
		// *** File does'nt exist at all
		// *** Try load it from .PAK file
		char path		[_MAX_PATH];
		char drive		[_MAX_DRIVE];
		char dir		[_MAX_DIR];
		char fname		[_MAX_FNAME];
		char ext		[_MAX_EXT];

		int p=0;
		for (int i=0; fn[i]; i++)
		{
			path[p++]=tolower(fn[i]);
			if (fn[i]=='\\' && fn[i+1]=='\\') i++;
		}
		path[p++]=0;
		_splitpath		( path, drive, dir, fname, ext );

		p=i=0;
		while (dir[i]) {
			if (dir[i]=='\\') {
				char bn[_MAX_PATH];
				CopyMemory(bn,dir,i);
				bn[i]=0;
				strcat(bn,".pak");
				if (OpenPAK(bn)) {
					char buf[_MAX_PATH];
					strcpy(buf,&(dir[i+1]));
					strcat(buf,fname);
					if (ext[1])	strcat(buf,ext);
					return LoadPAK(buf,pdwSize);
				}
			}
			i++;
		}
		Fatal("Can't open required file: ",fn);
		return 0;
	} else {
		size = filelength(hFile);
		void *buf = malloc(size);
		if (buf==NULL) Fatal("Can't allocate enought memory to load file",fn);
		_read(hFile,buf,size);
		_close(hFile);
		if (pdwSize) *pdwSize = size;
		return buf;
	}
}


BOOL FileExists(char *fn)
{
	int		hFile;

	hFile=_open(fn,O_RDONLY|O_BINARY);
	if (hFile==-1) {
		// *** File does'nt exist at all
		// *** Try load it from .PAK file
		char path		[_MAX_PATH];
		char drive		[_MAX_DRIVE];
		char dir		[_MAX_DIR];
		char fname		[_MAX_FNAME];
		char ext		[_MAX_EXT];

		int p=0;
		for (int i=0; fn[i]; i++)
		{
			path[p++]=tolower(fn[i]);
			if (fn[i]=='\\' && fn[i+1]=='\\') i++;
		}
		path[p++]=0;
		_splitpath		( path, drive, dir, fname, ext );

		p=i=0;
		while (dir[i]) {
			if (dir[i]=='\\') {
				char bn[_MAX_PATH];
				CopyMemory(bn,dir,i);
				bn[i]=0;
				strcat(bn,".pak");
				if (OpenPAK(bn)) {
					char buf[_MAX_PATH];
					strcpy(buf,&(dir[i+1]));
					strcat(buf,fname);
					if (ext[1])	strcat(buf,ext);
					return InPAK(buf);
				}
			}
			i++;
		}
		return false;
	} else {
		_close(hFile);
		return true;
	}
}