// xrCompress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fs.h"

CFS_File*				fs=0;
CFS_Memory				fs_desc;

u32						bytesSRC=0,bytesDST=0;
u32						filesTOTAL=0,filesSKIP=0,filesVFS=0,filesALIAS=0;

struct	ALIAS
{
	LPCSTR			path;
	u32				size;
	u32				crc;
	u32				c_mode;
	u32				c_ptr;
	u32				c_size;
};
multimap<u32,ALIAS>		aliases;

BOOL	testSKIP		(LPCSTR path)
{
	string256			p_name;
	string256			p_ext;
	_splitpath			(path, 0, 0, p_name, p_ext );
	
	if (0==stricmp(p_ext,".key"))	return TRUE;
	if (0==stricmp(p_name,"build"))	return TRUE;
	if ('~'==p_ext[1])				return TRUE;
	if ('_'==p_ext[1])				return TRUE;
	return FALSE;
}
BOOL	testVFS			(LPCSTR path)
{
	string256			p_name;
	string256			p_ext;
	_splitpath			(path, 0, 0, p_name, p_ext );
	
	if (0==stricmp(p_name,"level"))	{
		if (0==stricmp(p_ext,".game"))		return FALSE;
		if (0==stricmp(p_ext,".hom"))		return FALSE;
		if (0==stricmp(p_ext,".ltx"))		return FALSE;
		if (0==stricmp(p_ext,".spawn"))		return FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL	testEqual		(LPCSTR path, CVirtualFileStream& base)
{
	CVirtualFileStream	test	(path);
	if (test.Length() != base.Length())
	{
		return FALSE;
	}
	return 0==memcmp(test.Pointer(),base.Pointer(),base.Length());
}

ALIAS*	testALIAS		(CVirtualFileStream& base, u32& a_tests)
{
	multimap<u32,ALIAS>::iterator I = aliases.lower_bound(base.Length());

	while (I!=aliases.end() && (I->first==base.Length()))
	{
		a_tests	++;
		if (testEqual(I->second.path,base))	
		{
			return	&I->second;
		}
		I++;
	}
	return 0;
}

void Compress			(LPCSTR path)
{
	filesTOTAL			++;

	printf				("\n%-80s   ",path);
	if (testSKIP(path))	{
		filesSKIP	++;
		printf(" - a SKIP");
		return;
	}

	CVirtualFileStream		src	(path);
	u32			c_ptr		= 0;
	u32			c_size		= 0;
	u32			c_mode		= 0;
	u32			a_tests		= 0;

	ALIAS*		A			= testALIAS(src,a_tests);
	printf				("%3da ",a_tests);
	if (A) 
	{
		filesALIAS			++;
		printf				("ALIAS");

		// Alias found
		c_ptr				= A->c_ptr;
		c_size				= A->c_size;
		c_mode				= A->c_mode;
		bytesSRC			+= A->size;
		bytesDST			+= 0;
	} else {
		if (testVFS(path))	{
			filesVFS			++;
			printf				("VFS");

			// Write into BaseFS
			c_ptr				= fs->tell	();
			c_size				= src.Length();
			c_mode				= 1;		// VFS file
			fs->write			(src.Pointer(),c_size);
			bytesSRC			+= c_size;
			bytesDST			+= c_size;
		} else {
			// Compress into BaseFS
			c_ptr				= fs->tell();
			c_size				= 0;
			c_mode				= 0;		// Normal file
			BYTE*		c_data	= 0;
			_compressLZ			(&c_data,&c_size,src.Pointer(),src.Length());
			fs->write			(c_data,c_size);
			printf				("%3.1f%%",100.f*float(c_size)/float(src.Length()));
			bytesSRC			+= src.Length();
			bytesDST			+= c_size;
		}
	}

	// Write description
	fs_desc.WstringZ	(path	);
	fs_desc.Wdword		(c_mode	);
	fs_desc.Wdword		(c_ptr	);
	fs_desc.Wdword		(c_size	);

	if (0==A)	
	{
		// Register for future aliasing
		ALIAS			R;
		R.path			= strdup	(path);
		R.size			= src.Length();
		R.c_mode		= c_mode;
		R.c_ptr			= c_ptr;
		R.c_size		= c_size;
		aliases.insert	(make_pair(R.size,R));
	}
}

void Recurse		(const char* path);
void ProcessOne		(_finddata_t& F, const char* path)
{
	string512		N;
	strcpy			(N,path);
	strcat			(N,F.name);
	
	if (F.attrib&_A_SUBDIR) {
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat		(N,"\\");
		Recurse		(N);
	} else {
		Compress	(strlwr(N));
	}
}

void Recurse		(const char* path)
{
    _finddata_t		sFile;
    int				hFile;
	
	string512		N;
	strcpy			(N,path);
	strcat			(N,"*.*");
	
    hFile=_findfirst(N, &sFile);
	ProcessOne		(sFile,path);
	
    while			( _findnext( hFile, &sFile ) == 0 )
		ProcessOne	(sFile,path);
	
    _findclose		( hFile );
}

int __cdecl main	(int argc, char* argv[])
{
	InitMath		();
	printf			("\n\n");

	if (argc!=2)	{
		printf("ERROR: u must pass folder name as parameter.\n");
		return 3;
	}
	printf			("[settings] SKIP: '*.key','build.*'\n");
	printf			("[settings] VFS:  'level.*'\n");
	printf			("\nCompressing files...");
	if (0==chdir(argv[1]))
	{
		u32				dwTimeStart	= timeGetTime();
		string256		fname;
		strconcat		(fname,"..\\",argv[1],".xrp");
		unlink			(fname);
		fs				= new CFS_File(fname);
		fs->open_chunk	(0);
		Recurse			("");
		fs->close_chunk	();
		fs->write_chunk	(1|CFS_CompressMark, fs_desc.pointer(),fs_desc.size());
		delete fs;
		u32			dwTimeEnd	= timeGetTime();
		printf			("\n\nFiles total/skipped/VFS/aliased: %d/%d/%d/%d\nOveral: %dK/%dK, %3.1f%%\nElapsed time: %d:%d\n",
			filesTOTAL,filesSKIP,filesVFS,filesALIAS,
			bytesDST/1024,bytesSRC/1024,
			100.f*float(bytesDST)/float(bytesSRC),
			((dwTimeEnd-dwTimeStart)/1000)/60,
			((dwTimeEnd-dwTimeStart)/1000)%60
			);
	} else {
		printf("ERROR: folder not found.\n");
		return 3;
	}

	return 0;
}

