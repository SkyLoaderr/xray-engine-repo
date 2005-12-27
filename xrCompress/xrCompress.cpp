// xrCompress.cpp : Defines the entry point for the console application.
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

IWriter*				fs				= 0;
CMemoryWriter			fs_desc;

u32						bytesSRC		= 0;
u32						bytesDST		= 0;
u32						filesTOTAL		= 0;
u32						filesSKIP		= 0;
u32						filesVFS		= 0;
u32						filesALIAS		= 0;
CStatTimer				t_compress;
u8*						c_heap			= NULL;
u32						dwTimeStart		= 0;

const u32				XRP_MAX_SIZE	= 1024*1024*640; // bytes



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

	if (0==stricmp(p_ext,".swatch"))return TRUE;
	if (0==stricmp(p_ext,".db"))	return TRUE;
	if (PatternMatch(p_ext,"*avi*"))return TRUE;
	if (0==stricmp(p_ext,".key"))	return TRUE;
	if (0==stricmp(p_ext,".tga"))	return TRUE;
	if (0==stricmp(p_ext,".txt"))	return TRUE;
	if (0==stricmp(p_ext,".smf"))	return TRUE;
	if (0==stricmp(p_ext,".uvm"))	return TRUE;
	if (0==stricmp(p_ext,".raw"))	return TRUE;
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
	if (0==stricmp(p_name,"level") && 0==stricmp(p_ext,".details"))				return TRUE;
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

void	Compress			(LPCSTR path, LPCSTR base, BOOL bFast)
{
	filesTOTAL		++;

	printf			("\n%-80s   ",path);
	if (testSKIP(path))	{
		filesSKIP	++;
		printf		(" - a SKIP");
		Msg			("%-80s   - SKIP",path);
		return;
	}

	string256		fn;				strconcat(fn,base,"\\",path);

	IReader*		src				=	FS.r_open	(fn);
	if (0==src){
		filesSKIP	++;
		printf		(" - CAN'T OPEN");
		Msg			("%-80s   - CAN'T OPEN",path);
		return;
	}
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
		Msg					("%-80s   - ALIAS (%s)",path,A->path);

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
			Msg					("%-80s   - VFS",path);
		} else {
			// Compress into BaseFS
			c_ptr				=	fs->tell();
			c_size_real			=	src->length();
			if (0!=c_size_real){
				u32 c_size_max		=	rtc_csize		(src->length());
				u8*	c_data			=	xr_alloc<u8>	(c_size_max);
				t_compress.Begin	();
				{
					// c_size_compressed	=	rtc_compress	(c_data,c_size_max,src->pointer(),c_size_real);
					c_size_compressed	= c_size_max;
					if (bFast){		
						R_ASSERT(LZO_E_OK == lzo1x_1_compress	((u8*)src->pointer(),c_size_real,c_data,&c_size_compressed,c_heap));
					}else{
						R_ASSERT(LZO_E_OK == lzo1x_999_compress	((u8*)src->pointer(),c_size_real,c_data,&c_size_compressed,c_heap));
					}
				}
				t_compress.End		();

				if ((c_size_compressed+16) >= c_size_real)
				{
					// Failed to compress - revert to VFS
					filesVFS			++;
					c_size_compressed	= c_size_real;
					fs->w				(src->pointer(),c_size_real);
					printf				("VFS (R)");
					Msg					("%-80s   - VFS (R)",path);
				} else {
					// Compressed OK - optimize
					if (!bFast){
						u8*		c_out	= xr_alloc<u8>	(c_size_real);
						u32		c_orig	= c_size_real;
						R_ASSERT		(LZO_E_OK	== lzo1x_optimize	(c_data,c_size_compressed,c_out,&c_orig, NULL));
						R_ASSERT		(c_orig		== c_size_real		);
						xr_free			(c_out);
					}
					fs->w				(c_data,c_size_compressed);
					printf				("%3.1f%%",	100.f*float(c_size_compressed)/float(src->length()));
					Msg					("%-80s   - OK (%3.1f%%)",path,100.f*float(c_size_compressed)/float(src->length()));
				}

				// cleanup
				xr_free		(c_data);
			}else{
				filesVFS				++;
				c_size_compressed		= c_size_real;
				//				fs->w					(src->pointer(),c_size_real);
				printf					("VFS (R)");
				Msg						("%-80s   - EMPTY",path);
			}
		}
	}

	// Write description
	fs_desc.w_stringZ	(path				);
	fs_desc.w_u32		(c_crc32			);
	fs_desc.w_u32		(c_ptr				);
	fs_desc.w_u32		(c_size_real		);
	fs_desc.w_u32		(c_size_compressed	);

	if (0==A)	
	{
		// Register for future aliasing
		ALIAS				R;
		R.path				= xr_strdup	(fn);
		R.crc				= c_crc32;
		R.c_ptr				= c_ptr;
		R.c_size_real		= c_size_real;
		R.c_size_compressed	= c_size_compressed;
		aliases.insert		(mk_pair(R.c_size_real,R));
	}

	FS.r_close	(src);
}

void	OpenPack			(LPCSTR tgt_folder, int num)
{
	VERIFY			(0==fs);

	string_path		fname;
	string128		s_num;
	strconcat		(fname,tgt_folder,".db_#",itoa(num,s_num,10));
	unlink			(fname);
	fs				= FS.w_open	(fname);
	fs_desc.clear	();
	aliases.clear	();
	dwTimeStart		= timeGetTime();
	fs->open_chunk	(0);
}

void	ClosePack			()
{
	fs->close_chunk	(); 
	// save list
	bytesDST		= fs->tell	();
	Log				("...Writing pack desc");
	fs->w_chunk		(1|CFS_CompressMark, fs_desc.pointer(),fs_desc.size());
	Msg				("Data size: %d. Desc size: %d.",bytesDST,fs_desc.size());
	FS.w_close		(fs);
	Log				("Pack saved.");
	u32	dwTimeEnd	= timeGetTime();
	printf			("\n\nFiles total/skipped/VFS/aliased: %d/%d/%d/%d\nOveral: %dK/%dK, %3.1f%%\nElapsed time: %d:%d\nCompression speed: %3.1f Mb/s",
		filesTOTAL,filesSKIP,filesVFS,filesALIAS,
		bytesDST/1024,bytesSRC/1024,
		100.f*float(bytesDST)/float(bytesSRC),
		((dwTimeEnd-dwTimeStart)/1000)/60,
		((dwTimeEnd-dwTimeStart)/1000)%60,
		float((float(bytesDST)/float(1024*1024))/(t_compress.GetElapsed_sec()))
		);
	Msg			("\n\nFiles total/skipped/VFS/aliased: %d/%d/%d/%d\nOveral: %dK/%dK, %3.1f%%\nElapsed time: %d:%d\nCompression speed: %3.1f Mb/s",
		filesTOTAL,filesSKIP,filesVFS,filesALIAS,
		bytesDST/1024,bytesSRC/1024,
		100.f*float(bytesDST)/float(bytesSRC),
		((dwTimeEnd-dwTimeStart)/1000)/60,
		((dwTimeEnd-dwTimeStart)/1000)%60,
		float((float(bytesDST)/float(1024*1024))/(t_compress.GetElapsed_sec()))
		);
}

void CompressList(LPCSTR in_name, xr_vector<char*>* list, xr_vector<char*>* fl_list, BOOL bFast)
{
	if (!list->empty() && in_name && in_name[0]){
		string256		caption;

		VERIFY			('\\'!=in_name[xr_strlen(in_name)-1]);
		string_path		tgt_folder;
		_splitpath		(in_name,0,0,tgt_folder,0);

		int pack_num	= 0;
		OpenPack		(tgt_folder,pack_num++);

		for (u32 it=0; it<fl_list->size(); it++){
			fs_desc.w_stringZ	((*fl_list)[it]);
			fs_desc.w_u32		(0		);	// crc
			fs_desc.w_u32		(0		);
			fs_desc.w_u32		(0		);
			fs_desc.w_u32		(0		);
		}

		c_heap			= xr_alloc<u8> (LZO1X_999_MEM_COMPRESS);
		//***main process***: BEGIN
		for (u32 it=0; it<list->size(); it++){
			sprintf		(caption,"Compress files: %d/%d - %d%%",it,list->size(),(it*100)/list->size());
			SetWindowText(GetConsoleWindow(),caption);
			if (fs->tell()>XRP_MAX_SIZE){
				ClosePack		();
				OpenPack		(tgt_folder,pack_num++);
			}
			Compress	((*list)[it],in_name,bFast);
		}
		ClosePack		();

		xr_free			(c_heap);
		//***main process***: END
	} else {
		Msg			("ERROR: folder not found.");
	}
}

void ProcessNormal(LPCSTR tgt_name, BOOL bFast)
{
	// collect files
	xr_vector<char*>*	list	= FS.file_list_open	("$target_folder$",FS_ListFiles);
	R_ASSERT2			(list,	"Unable to open folder!!!");
	// collect folders
	xr_vector<char*>*	fl_list	= FS.file_list_open	("$target_folder$",FS_ListFolders);
	R_ASSERT2			(fl_list,	"Unable to open folder!!!");
	// compress
	CompressList		(tgt_name,list,fl_list,bFast);
	// free lists
	FS.file_list_close	(fl_list);
	FS.file_list_close	(list);
}

void ProcessLTX(LPCSTR tgt_name, LPCSTR params, BOOL bFast)
{
	xr_string		ltx_name;
	LPCSTR ltx_nm	= strstr(params,".ltx");				VERIFY(ltx_nm!=0);
	string_path		ltx_fn;
	string_path		tmp;
	strncpy			(tmp,params,ltx_nm-params); tmp[ltx_nm-params]=0;
	_Trim			(tmp);
	strcat			(tmp,".ltx");
	strcpy			(ltx_fn,tmp);

	// append ltx path (if exist)
	string_path		fn,dr,di;
	_splitpath		(ltx_fn,dr,di,0,0);
	strconcat		(fn,dr,di);  
	if (0!=fn[0])
		FS.append_path	("ltx_path",fn,0,false);
	
	if (!FS.exist(ltx_fn)&&!FS.exist(ltx_fn,"$app_root$",tmp)) 
		Debug.fatal	("ERROR: Can't find ltx file: '%s'",ltx_fn);

	CInifile ltx	(ltx_fn);
	printf			("Processing LTX...\n");

	xr_vector<char*> list;
	xr_vector<char*> fl_list;
	CInifile::Sect& sect	= ltx.r_section("config");
	for (CInifile::SectIt s_it=sect.begin(); s_it!=sect.end(); s_it++){
		BOOL bRecurse	= CInifile::IsBOOL(s_it->second.c_str());
		u32 mask		= bRecurse?FS_ListFiles:FS_ListFiles|FS_RootOnly;

		string_path path;
		LPCSTR _path		= 0==xr_strcmp(s_it->first.c_str(),".\\")?"":s_it->first.c_str();
		strcpy				(path,_path);
		u32 path_len		= xr_strlen(path);
		if ((0!=path_len)&&(path[path_len-1]!='\\')) strcat(path,"\\");

		printf				("- Append path: '%s'\n",path);
		xr_vector<char*>*	i_list	= FS.file_list_open	("$target_folder$",path,mask);
		R_ASSERT3			(i_list,	"Unable to open file list:", path);
		// collect folders
		xr_vector<char*>*	i_fl_list	= FS.file_list_open	("$target_folder$",path,FS_ListFolders);
		R_ASSERT3			(i_fl_list,	"Unable to open folder list:", path);
		
		xr_vector<char*>::iterator it	= i_list->begin();
		xr_vector<char*>::iterator itE	= i_list->end();
		xr_string tmp_path;
		for (;it!=itE;++it){ 
			tmp_path		= xr_string(path)+xr_string(*it);
			list.push_back	(xr_strdup(tmp_path.c_str()));
		}
		it					= i_fl_list->begin();
		itE					= i_fl_list->end();
		for (;it!=itE;++it){ 
			tmp_path		= xr_string(path)+xr_string(*it);
			fl_list.push_back(xr_strdup(tmp_path.c_str()));
		}

		// free lists
		FS.file_list_close	(i_fl_list);
		FS.file_list_close	(i_list);
	}
	// compress
	CompressList	(tgt_name,&list,&fl_list,bFast);

	// free
	xr_vector<char*>::iterator it	= list.begin();
	xr_vector<char*>::iterator itE	= list.end();
	for (;it!=itE;++it) xr_free(*it);
	it				= fl_list.begin();
	itE				= fl_list.end();
	for (;it!=itE;++it) xr_free(*it);
}

int __cdecl main	(int argc, char* argv[])
{
	Core._initialize("xrCompress",0,FALSE);
	printf			("\n\n");

	LPCSTR params = GetCommandLine();

	if(strstr(params,"-diff")){
		ProcessDifference	();
	}else{
		if (argc<2)	{
			printf("ERROR: u must pass folder name as parameter.\n");
			printf("-diff /? option to get information about creating difference.\n");
			printf("-fast - fast compression.\n");
			printf("	 file_name.ltx - pathes to compress.\n");
			printf("\n");
			printf("LTX format:\n");
			printf("	[config]\n");
			printf("	;<path>     = <recurse>\n");
			printf("	.\\         = false\n");
			printf("	textures\   = true\n");
			
			Core._destroy();
			return 3;
		}
		printf			("[settings] SKIP: '*.key','build.*'\n");
		printf			("[settings] VFS:  'level.*'\n");

		string_path		folder;		strlwr(strconcat(folder,argv[1],"\\"));
		printf			("\nCompressing files (%s)...\n\n",folder);

		FS._initialize	(CLocatorAPI::flTargetFolderOnly|CLocatorAPI::flScanAppRoot,folder);

		BOOL bFast		= 0!=strstr(params,"-fast");

		LPCSTR p		= strstr(params,"-ltx");
		if(0!=p){
			ProcessLTX		(argv[1],p+4,bFast);
		}else{
			ProcessNormal	(argv[1],bFast);
		}
	}

	Core._destroy		();
	return 0;
}
