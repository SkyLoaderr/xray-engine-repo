F// xrCompress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lzo\lzo1x.h"
#include <mmsystem.h>

#pragma warning(disable:4995)
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default:4995)

#pragma comment			(lib,"x:\\xrCore.lib")

extern int ProcessDifference();

IWriter*				fs=0;
CMemoryWriter			fs_desc;

u32						bytesSRC=0,bytesDST=0;
u32						filesTOTAL=0,filesSKIP=0,filesVFS=0,filesALIAS=0;
CTimer					t_compress;
u64						t_compress_total;
u8*						c_heap	= NULL;



DEFINE_VECTOR(_finddata_t,FFVec,FFIt);
IC bool pred_str_ff(const _finddata_t& x, const _finddata_t& y)
{	
	return xr_strcmp(x.name,y.name)<0;	
}


struct	ALIAS
{
	LPCSTR			path;
	u32				crc;
	u32				c_ptr;
	u32				c_size_real;
	u32				c_size_compressed;
};
xr_multimap<u32,ALIAS>	aliases;


BOOL	testSKIP		(LPCSTR path)
{
	string256			p_name;
	string256			p_ext;
	_splitpath			(path, 0, 0, p_name, p_ext );

	if (0==stricmp(p_ext,".key"))	return TRUE;
	if (0==stricmp(p_ext,".tga"))	return TRUE;
	if (0==stricmp(p_ext,".txt"))	return TRUE;
	if (0==stricmp(p_ext,".smf"))	return TRUE;
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

	// if (0==stricmp(p_name,"level") && (0==stricmp(p_ext,".") || 0==p_ext[0]) )	return TRUE;	// level.
	if (0==stricmp(p_name,"level") && 0==stricmp(p_ext,".ai"))					return TRUE;
	if (0==stricmp(p_name,"level") && 0==stricmp(p_ext,".gct"))					return TRUE;
	if (0==stricmp(p_ext,".ogg"))												return TRUE;
	if (0==stricmp(p_name,"game") && 0==stricmp(p_ext,".graph"))				return TRUE;
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

void	Compress			(LPCSTR path, LPCSTR base)
{
	filesTOTAL			++;

	printf				("\n%-80s   ",path);
	if (testSKIP(path))	{
		filesSKIP	++;
		printf(" - a SKIP");
		return;
	}

	string256		fn;				strconcat(fn,base,"\\",path);

	IReader*		src				=	FS.r_open	(fn);
	bytesSRC						+=	src->length	();
	u32			c_crc32				=	crc32		(src->pointer(),src->length());
	u32			c_ptr				=	0;
	u32			c_size_real			=	0;
	u32			c_size_compressed	=	0;
	u32			a_tests				=	0;

	ALIAS*		A					=	testALIAS	(src,c_crc32,a_tests);
	printf							("%3da ",a_tests);
	if (A) 
	{
		filesALIAS			++;
		printf				("ALIAS");

		// Alias found
		c_ptr				= A->c_ptr;
		c_size_real			= A->c_size_real;
		c_size_compressed	= A->c_size_compressed;
	} else {
		if (testVFS(path))	{
			filesVFS			++;

			// Write into BaseFS
			c_ptr				= fs->tell	();
			c_size_real			= src->length();
			c_size_compressed	= src->length();
			fs->w				(src->pointer(),c_size_real);
			printf				("VFS");
		} else {
			// Compress into BaseFS
			c_ptr				=	fs->tell();
			c_size_real			=	src->length();
			if (0!=c_size_real){
				u32 c_size_max		=	rtc_csize		(src->length());
				u8*	c_data			=	xr_alloc<u8>	(c_size_max);
				t_compress.Start	();
				{
					// c_size_compressed	=	rtc_compress	(c_data,c_size_max,src->pointer(),c_size_real);
					c_size_compressed	= c_size_max;
					R_ASSERT			(LZO_E_OK == lzo1x_999_compress	((u8*)src->pointer(),c_size_real,c_data,&c_size_compressed,c_heap));
				}
				t_compress_total	+=	t_compress.GetElapsed_clk();

				if ((c_size_compressed+16) >= c_size_real)
				{
					// Failed to compress - revert to VFS
					filesVFS			++;
					c_size_compressed	= c_size_real;
					fs->w				(src->pointer(),c_size_real);
					printf				("VFS (R)");
				} else {
					// Compressed OK - optimize
					{
						u8*		c_out	= xr_alloc<u8>	(c_size_real);
						u32		c_orig	= c_size_real;
						R_ASSERT		(LZO_E_OK	== lzo1x_optimize	(c_data,c_size_compressed,c_out,&c_orig, NULL));
						R_ASSERT		(c_orig		== c_size_real		);
						xr_free			(c_out);
					}
					fs->w				(c_data,c_size_compressed);
					printf				("%3.1f%%",	100.f*float(c_size_compressed)/float(src->length()));
				}

				// cleanup
				xr_free		(c_data);
			}else{
				filesVFS				++;
				c_size_compressed		= c_size_real;
				//				fs->w					(src->pointer(),c_size_real);
				printf					("VFS (R)");
			}
		}
	}

	// Write description
	fs_desc.w_stringZ	(path				);
	fs_desc.w_u32		(c_ptr				);
	fs_desc.w_u32		(c_size_real		);
	fs_desc.w_u32		(c_size_compressed	);

	if (0==A)	
	{
		// Register for future aliasing
		ALIAS			R;
		R.path				= xr_strdup	(fn);
		R.crc				= c_crc32;
		R.c_ptr				= c_ptr;
		R.c_size_real		= c_size_real;
		R.c_size_compressed	= c_size_compressed;
		aliases.insert		(mk_pair(R.c_size_real,R));
	}

	FS.r_close	(src);
}

int __cdecl main	(int argc, char* argv[])
{
	Core._initialize("xrCompress",0,FALSE);
	printf			("\n\n");

	LPCSTR params = GetCommandLine();

	if(strstr(params,"-diff")){
		ProcessDifference();

	}else{
		if (argc<2)	{
			printf("ERROR: u must pass folder name as parameter.\n");
			printf("or use -diff /? option to get information about creating difference.\n");
			return 3;
		}
		printf			("[settings] SKIP: '*.key','build.*'\n");
		printf			("[settings] VFS:  'level.*'\n");

		string_path		folder;		strlwr(strconcat(folder,argv[1],"\\"));
		printf			("\nCompressing files (%s)...",folder);

		FS._initialize	(CLocatorAPI::flTargetFolderOnly,folder);

		xr_vector<char*>*	list	= FS.file_list_open	("$target_folder$",FS_ListFiles);
		R_ASSERT2			(list,	"Unable to open folder!!!");

		if (!list->empty())
		{
			u32				dwTimeStart	= timeGetTime();
			string256		fname;
			strconcat		(fname,argv[1],".xrp");
			unlink			(fname);

			// collect folders
			xr_vector<char*>*	fl_list	= FS.file_list_open	("$target_folder$",FS_ListFolders);
			R_ASSERT2			(fl_list,	"Unable to open folder!!!");



			for (u32 it=0; it<fl_list->size(); it++){
				fs_desc.w_stringZ	((*fl_list)[it]);
				fs_desc.w_u32		(0		);
				fs_desc.w_u32		(0		);
				fs_desc.w_u32		(0		);
			}
			FS.file_list_close	(fl_list);

			fs				= FS.w_open	(fname);
			fs->open_chunk	(0);
			//***main process***: BEGIN
			c_heap			= xr_alloc<u8> (LZO1X_999_MEM_COMPRESS);
			for (u32 it=0; it<list->size(); it++){
				Compress	((*list)[it],argv[1]);
			}
			xr_free			(c_heap);
			fs->close_chunk	();
			//***main process***: END

			// save list
			bytesDST		= fs->tell	();
			fs->w_chunk		(1|CFS_CompressMark, fs_desc.pointer(),fs_desc.size());
			FS.w_close		(fs);
			u32	dwTimeEnd	= timeGetTime();
			printf			("\n\nFiles total/skipped/VFS/aliased: %d/%d/%d/%d\nOveral: %dK/%dK, %3.1f%%\nElapsed time: %d:%d\nCompression speed: %3.1f Mb/s",
				filesTOTAL,filesSKIP,filesVFS,filesALIAS,
				bytesDST/1024,bytesSRC/1024,
				100.f*float(bytesDST)/float(bytesSRC),
				((dwTimeEnd-dwTimeStart)/1000)/60,
				((dwTimeEnd-dwTimeStart)/1000)%60,
				float((float(bytesDST)/float(1024*1024))/(float(t_compress_total)*float(CPU::cycles2seconds)))
				);
		} else {
			printf("ERROR: folder not found.\n");
		}
		FS.file_list_close	(list);
	}

	Core._destroy		();
	return 0;
}

/*
if(strstr(params,"-dpack")){
bDiff = true;
FS_orig = xr_new<CLocatorAPI>	();
FS_orig->m_Flags.set(CLocatorAPI::flReady,TRUE);
FS_orig->_initialize(CLocatorAPI::flReady);

string_path		N;
_finddata_t		sFile;
intptr_t		hFile;
FFVec			rec_files;
// find all files
strconcat(N,Core.ApplicationPath,"\\*.xp*");
if (-1==(hFile=_findfirst(N, &sFile))){
return		false;
}
// загоняем в вектор для того *.xp* приходили в сортированном порядке
rec_files.push_back(sFile);
while			( _findnext( hFile, &sFile ) == 0 )
rec_files.push_back(sFile);
_findclose		( hFile );

std::sort		(rec_files.begin(), rec_files.end(), pred_str_ff);

for (FFIt it=rec_files.begin(); it!=rec_files.end(); it++)
FS_orig->register_archieve( (*it).name );

list_orig	= FS_orig->file_list_open	("",FS_ListFiles);
fl_list_orig	= FS_orig->file_list_open("",FS_ListFolders);
strcat(root_pth,"");
}


if(bDiff){
for(u32 i=0; i<list_orig->size();++i){
file_comparer fc(list_orig->at(i),FS_orig);
xr_vector<char*>::iterator it = std::find_if(list->begin(),list->end(),fc);
if(it != list->end()){
printf("skip file %s\n",list_orig->at(i));
list->erase(it);
}
}
};

if(bDiff){
for(u32 i=0; i<fl_list_orig->size();++i){
file_comparer fc(fl_list_orig->at(i));
xr_vector<char*>::iterator it = std::find_if(fl_list->begin(),fl_list->end(),fc);
if(it != fl_list->end()){
printf("skip folder %s\n",fl_list_orig->at(i));
fl_list->erase(it);
}
}
};


if(bDiff){
FS_orig->file_list_close(list_orig);
FS_orig->file_list_close(fl_list_orig);
xr_delete(FS_orig);
}
*/