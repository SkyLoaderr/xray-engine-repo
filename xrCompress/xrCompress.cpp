// xrCompress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fs.h"

CFS_File*				fs=0;
CFS_Memory				fs_desc;

DWORD					bytesSRC=0,bytesDST=0;
DWORD					filesTOTAL=0,filesSKIP=0,filesVFS=0;

BOOL	testSKIP		(LPCSTR path)
{
	string256			p_name;
	string256			p_ext;
	_splitpath			(path, 0, 0, p_name, p_ext );
	
	if (0==stricmp(p_ext,".key"))	return TRUE;
	if (0==stricmp(p_name,"build"))	return TRUE;
	return FALSE;
}
BOOL	testVFS			(LPCSTR path)
{
	string256			p_name;
	string256			p_ext;
	_splitpath			(path, 0, 0, p_name, p_ext );
	
	if (0==stricmp(p_name,"level"))	return TRUE;
	return FALSE;
}

void Compress			(LPCSTR path)
{
	filesTOTAL			++;

	printf				("\n%-80s   ",path);
	if (testSKIP(path))	{
		filesSKIP	++;
		printf("SKIP");
		return;
	}
	if (testVFS(path))	{
		filesVFS	++;
		printf("VFS");

		// Write into BaseFS
		CVirtualFileStream	src(path);
		DWORD		c_ptr	= fs->tell();
		DWORD		c_size	= src.Length();
		fs->write			(src.Pointer(),c_size);
		bytesSRC			+= c_size;
		bytesDST			+= c_size;
		
		// Write description
		fs_desc.WstringZ	(path	);
		fs_desc.Wdword		(1		);	// VFS file
		fs_desc.Wdword		(c_ptr	);
		fs_desc.Wdword		(c_size	);
	} else {
		// Compress into BaseFS
		CVirtualFileStream	src(path);
		DWORD		c_ptr	= fs->tell();
		BYTE*		c_data	= 0;
		unsigned	c_size	= 0;
		_compressLZ			(&c_data,&c_size,src.Pointer(),src.Length());
		fs->write			(c_data,c_size);
		printf				("%3.1f%%",100.f*float(c_size)/float(src.Length()));
		bytesSRC			+= src.Length();
		bytesDST			+= c_size;
		
		// Write description
		fs_desc.WstringZ	(path	);
		fs_desc.Wdword		(0		);	// Normal file
		fs_desc.Wdword		(c_ptr	);
		fs_desc.Wdword		(c_size	);
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
		DWORD			dwTimeStart	= timeGetTime();
		string256		fname;
		strconcat		(fname,"..\\",argv[1],".xrp");
		fs				= new CFS_File(fname);
		fs->open_chunk	(0);
		Recurse			("");
		fs->close_chunk	();
		fs->write_chunk	(1|CFS_CompressMark, fs_desc.pointer(),fs_desc.size());
		delete fs;
		DWORD			dwTimeEnd	= timeGetTime();
		printf			("\n\nFiles total/skipped/VFS: %d/%d/%d\nOveral ratio: %3.1f%%\nElapsed time: %d:%d\n",
			filesTOTAL,filesSKIP,filesVFS,
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

