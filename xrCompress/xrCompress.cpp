// xrCompress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma comment(lib,"x:\\xrCore.lib")

IWriter*				fs=0;
CMemoryWriter			fs_desc;

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
xr_multimap<u32,ALIAS>	aliases;


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

BOOL	testEqual		(LPCSTR path, IReader* base)
{
	IReader*	test	= FS.r_open	(path);
	if (test->length() != base->length())
	{
		return FALSE;
	}
	return 0==memcmp(test->pointer(),base->pointer(),base->length());
}

ALIAS*	testALIAS		(IReader* base, u32 crc, u32& a_tests)
{
	xr_multimap<u32,ALIAS>::iterator I = aliases.lower_bound(base->length());

	while (I!=aliases.end() && (I->first==base->length()))
	{
		if (I->second.crc == crc)
		{
			a_tests	++;
			if (testEqual(I->second.path,base))	
			{
				return	&I->second;
			}
		}
		I++;
	}
	return 0;
}

extern	u32		crc32_calc			(void* P, u32 size);

void	Compress			(LPCSTR path, LPCSTR base)
{
	filesTOTAL			++;

	printf				("\n%-80s   ",path);
	if (testSKIP(path))	{
		filesSKIP	++;
		printf(" - a SKIP");
		return;
	}

	string256		fn;		strconcat(fn,base,"\\",path);

	IReader*		src		=	FS.r_open	(fn);
	bytesSRC				+=	src->length	();
	u32			c_crc32		=	crc32_calc	(src->pointer(),src->length());
	u32			c_ptr		=	0;
	u32			c_size		=	0;
	u32			c_mode		=	0;
	u32			a_tests		=	0;

	ALIAS*		A			=	testALIAS	(src,c_crc32,a_tests);
	printf					("%3da ",a_tests);
	if (A) 
	{
		filesALIAS			++;
		printf				("ALIAS");

		// Alias found
		c_ptr				= A->c_ptr;
		c_size				= A->c_size;
		c_mode				= A->c_mode;
	} else {
		if (testVFS(path))	{
			filesVFS			++;
			printf				("VFS");

			// Write into BaseFS
			c_ptr				= fs->tell	();
			c_size				= src->length();
			c_mode				= 1;		// VFS file
			fs->w				(src->pointer(),c_size);
		} else {
			// Compress into BaseFS
			c_ptr				= fs->tell();
			c_size				= 0;
			u32 c_size_max		= rtc_csize		(src->length());
			u8*	c_data			= xr_alloc<u8>	(c_size_max);
			c_size				= rtc_compress	(c_data,c_size_max,src->pointer(),src->length());
			if ((c_size+64) >= u32(src->length()))
			{
				// Failed to compress - revert to VFS
				filesVFS			++;
				c_mode				= 1;		// VFS file
				c_size				= src->length();
				fs->w				(src->pointer(),c_size);
				printf				("VFS (R)");
			} else {
				// Compressed OK
				c_mode				= 0;		// Normal file
				fs->w				(c_data,c_size);
				printf				("%3.1f%%",	100.f*float(c_size)/float(src->length()));
			}

			// cleanup
			xr_free		(c_data);
		}
	}

	// Write description
	fs_desc.w_stringZ	(path	);
	fs_desc.w_u32		(c_mode	);
	fs_desc.w_u32		(c_ptr	);
	fs_desc.w_u32		(c_size	);

	if (0==A)	
	{
		// Register for future aliasing
		ALIAS			R;
		R.path			= xr_strdup	(fn);
		R.size			= src->length();
		R.crc			= c_crc32;
		R.c_mode		= c_mode;
		R.c_ptr			= c_ptr;
		R.c_size		= c_size;
		aliases.insert	(mk_pair(R.size,R));
	}

	FS.r_close	(src);
}

int __cdecl main	(int argc, char* argv[])
{
	Core._initialize("xrCompress");
	printf			("\n\n");

	if (argc!=2)	{
		printf("ERROR: u must pass folder name as parameter.\n");
		return 3;
	}
	printf			("[settings] SKIP: '*.key','build.*'\n");
	printf			("[settings] VFS:  'level.*'\n");
	printf			("\nCompressing files...");

	string256		folder;		strlwr(strconcat(folder,argv[1],"\\"));
	xr_vector<char*>*	list	= FS.file_list_open	(folder,FS_ListFiles);
	if (!list->empty())
	{
		u32				dwTimeStart	= timeGetTime();
		string256		fname;
		strconcat		(fname,argv[1],".xrp");
		unlink			(fname);
		fs				= FS.w_open	(fname);
		fs->open_chunk	(0);
		//***main process***: BEGIN

		for (u32 it=0; it<list->size(); it++)
			Compress((*list)[it],argv[1]);

		//***main process***: END
		fs->close_chunk	();
		bytesDST		= fs->tell	();
		fs->w_chunk		(1|CFS_CompressMark, fs_desc.pointer(),fs_desc.size());
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
	}
	FS.file_list_close	(list);

	Core._destroy		();
	return 0;
}
