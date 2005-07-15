// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default:4995)

#include "FS_internal.h"

CLocatorAPI*	xr_FS	= NULL;

#ifdef _EDITOR
#define FSLTX	"fs.ltx"
#else
#define FSLTX	"fsgame.ltx"
#endif

//////////////////////////////////////////////////////////////////////
// FS_Path
//////////////////////////////////////////////////////////////////////
FS_Path::FS_Path	(LPCSTR _Root, LPCSTR _Add, LPCSTR _DefExt, LPCSTR _FilterCaption, u32 flags)
{
	VERIFY			(_Root&&_Root[0]);
	string_path		temp;
    strcpy			(temp,_Root); 
    if (_Add) 		strcat(temp,_Add);
	if (temp[0] && temp[xr_strlen(temp)-1]!='\\') strcat(temp,"\\");
	m_Path			= xr_strlwr(xr_strdup(temp));
	m_DefExt		= _DefExt?xr_strlwr(xr_strdup(_DefExt)):0;
	m_FilterCaption	= _FilterCaption?xr_strlwr(xr_strdup(_FilterCaption)):0;
	m_Add			= _Add?xr_strlwr(xr_strdup(_Add)):0;
	m_Root			= _Root?xr_strlwr(xr_strdup(_Root)):0;
    m_Flags.assign	(flags);
#ifdef _EDITOR
	// Editor(s)/User(s) wants pathes already created in "real" file system :)
	VerifyPath		(m_Path);
#endif
}

FS_Path::~FS_Path	()
{
	xr_free	(m_Root);
	xr_free	(m_Path);
	xr_free	(m_Add);
	xr_free	(m_DefExt);
	xr_free	(m_FilterCaption);
}

void	FS_Path::_set	(LPSTR add)
{
	// m_Add
	R_ASSERT		(add);
	xr_free			(m_Add);
	m_Add			= xr_strlwr(xr_strdup(add));

	// m_Path
	string_path		temp;
	strconcat		(temp,m_Root,m_Add);
	if (temp[xr_strlen(temp)-1]!='\\') strcat(temp,"\\");
	xr_free			(m_Path);
	m_Path			= xr_strlwr(xr_strdup(temp));
}


LPCSTR FS_Path::_update(LPSTR dest, LPCSTR src)const
{
	R_ASSERT(dest);
    R_ASSERT(src);
	string_path		temp;
	strcpy			(temp,src);
	return xr_strlwr(strconcat(dest,m_Path,temp));
}

void FS_Path::_update(xr_string& dest, LPCSTR src)const
{
    R_ASSERT(src);
    dest			= xr_string(m_Path)+src;
    xr_strlwr		(dest);
}
void FS_Path::rescan_path_cb	()
{
	m_Flags.set(flNeedRescan,TRUE);
    FS.m_Flags.set(CLocatorAPI::flNeedRescan,TRUE);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLocatorAPI::CLocatorAPI()
{
	FThread				= 0;
    m_Flags.zero		();
	// get page size
	SYSTEM_INFO			sys_inf;
	GetSystemInfo		(&sys_inf);
	dwAllocGranularity	= sys_inf.dwAllocationGranularity;
    m_iLockRescan		= 0;
	dwOpenCounter		= 0;
}

CLocatorAPI::~CLocatorAPI()
{
	VERIFY				(0==m_iLockRescan);
}

void CLocatorAPI::Register		(LPCSTR name, u32 vfs, u32 ptr, u32 size_real, u32 size_compressed, u32 modif)
{
	// Register file
	file				desc;
	desc.name			= xr_strlwr(xr_strdup(name));
	desc.vfs			= vfs;
	desc.ptr			= ptr;
	desc.size_real		= size_real;
	desc.size_compressed= size_compressed;
    desc.modif			= modif & (~u32(0x3));

	// if file already exist - update info
	files_it	I		= files.find(desc);
	if (I!=files.end()){
		char* str		= LPSTR(I->name);
		xr_free			(str);
		files.erase		(I);
		files.insert	(desc); 
		return;
	}

	// otherwise insert file
	files.insert		(desc); 
	
	// Try to register folder(s)
	string_path			temp;	strcpy(temp,desc.name);
	string_path			path;
	string_path			folder;
	while (temp[0]) 
	{
		_splitpath		(temp, path, folder, 0, 0 );
        strcat			(path,folder);
		if (!exist(path))	
		{
			desc.name			= xr_strdup(path);
			desc.vfs			= 0xffffffff;
			desc.ptr			= 0;
			desc.size_real		= 0;
			desc.size_compressed= 0;
            desc.modif			= u32(-1);
            std::pair<files_it,bool> I = files.insert(desc); 
            R_ASSERT(I.second);
		}
		strcpy(temp,folder);
		if (xr_strlen(temp))		temp[xr_strlen(temp)-1]=0;
	}
}

IReader* open_chunk(void* ptr, u32 ID)	
{
	BOOL			res;
	u32				dwType, dwSize;
	DWORD			read_byte;
	u32 pt			= SetFilePointer(ptr,0,0,FILE_BEGIN); VERIFY(pt!=INVALID_SET_FILE_POINTER);
	while (true){
		res			= ReadFile	(ptr,&dwType,4,&read_byte,0); VERIFY(res&&(read_byte==4));
		res			= ReadFile	(ptr,&dwSize,4,&read_byte,0); VERIFY(res&&(read_byte==4));
		if ((dwType&(~CFS_CompressMark)) == ID) {
			u8* src_data	= xr_alloc<u8>(dwSize);
			res				= ReadFile	(ptr,src_data,dwSize,&read_byte,0); VERIFY(res&&(read_byte==dwSize));
			if (dwType&CFS_CompressMark) {
				BYTE*			dest;
				unsigned		dest_sz;
				_decompressLZ	(&dest,&dest_sz,src_data,dwSize);
				xr_free			(src_data);
				return xr_new<CTempReader>(dest,dest_sz,0);
			} else {
				return xr_new<CTempReader>(src_data,dwSize,0);
			}
		}else{ 
			pt		= SetFilePointer(ptr,dwSize,0,FILE_CURRENT); 
			if (pt==INVALID_SET_FILE_POINTER) return 0;
		}
	}
	return 0;
};

void CLocatorAPI::ProcessArchive(const char* _path)
{
	// find existing archive
	shared_str path		= _path;
	for (archives_it it=archives.begin(); it!=archives.end(); ++it)
		if (it->path==path)	return;

	// open archive
	archives.push_back	(archive());
	archive& A		= archives.back();
	A.path			= path;
	// Open the file
	A.hSrcFile		= CreateFile		(*path, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	R_ASSERT							(A.hSrcFile!=INVALID_HANDLE_VALUE);
	A.hSrcMap		= CreateFileMapping	(A.hSrcFile, 0, PAGE_READONLY, 0, 0, 0);
	R_ASSERT							(A.hSrcMap!=INVALID_HANDLE_VALUE);
	A.size			= GetFileSize		(A.hSrcFile,0);
	R_ASSERT							(A.size>0);

	// Create base path
	string_path			base;
	strcpy				(base,*path);
	if (strext(base))	*strext(base)	= 0;
	strcat				(base,"\\");

	// Read headers
	IReader* hdr		= open_chunk(A.hSrcFile,1); R_ASSERT(hdr);
	RStringVec	fv;
	while (!hdr->eof())
	{
		string_path		name,full;
		hdr->r_stringZ	(name,sizeof(name));
		strconcat		(full,base,name);
		size_t vfs		= archives.size()-1;
		u32 ptr			= hdr->r_u32();
		u32 size_real	= hdr->r_u32();
		u32 size_compr	= hdr->r_u32();
		//del
//		if(ptr)
//			fv.push_back	(full);

		Register		(full,(u32)vfs,ptr,size_real,size_compr,0);
	}
	hdr->close			();
}

void CLocatorAPI::ProcessOne	(const char* path, void* _F)
{
	_finddata_t& F	= *((_finddata_t*)_F);

	string_path	N;
	strcpy		(N,path);
	strcat		(N,F.name);
	xr_strlwr	(N);
	
	if (F.attrib&_A_HIDDEN)			return;

	if (F.attrib&_A_SUBDIR) {
		if (bNoRecurse)				return;
		if (0==xr_strcmp(F.name,"."))	return;
		if (0==xr_strcmp(F.name,"..")) return;
		strcat		(N,"\\");
		Register	(N,0xffffffff,0,F.size,F.size,(u32)F.time_write);
		Recurse		(N);
	} else {
		if (strext(N) && 0==strncmp(strext(N),".xp",3))		ProcessArchive	(N);
		else												Register		(N,0xffffffff,0,F.size,F.size,(u32)F.time_write);
	}
}

DEFINE_VECTOR(_finddata_t,FFVec,FFIt);
IC bool pred_str_ff(const _finddata_t& x, const _finddata_t& y)
{	
	return xr_strcmp(x.name,y.name)<0;	
}

bool CLocatorAPI::Recurse		(const char* path)
{
    _finddata_t		sFile;
    intptr_t		hFile;

	string_path		N;
	strcpy			(N,path);
	strcat			(N,"*.*");

	FFVec			rec_files;
	rec_files.reserve(256);

	// find all files    
	if (-1==(hFile=_findfirst(N, &sFile))){
    	// Log		("! Wrong path: ",path);
    	return		false;
    }
    // �������� � ������ ��� ���� *.xp* ��������� � ������������� �������
	rec_files.push_back(sFile);
	while			( _findnext( hFile, &sFile ) == 0 )
		rec_files.push_back(sFile);
	_findclose		( hFile );

	std::sort		(rec_files.begin(), rec_files.end(), pred_str_ff);

	for (FFIt it=rec_files.begin(); it!=rec_files.end(); it++)
		ProcessOne	(path,&*it);

	// insert self
    if (path&&path[0])
		Register	(path,0xffffffff,0,0,0,0);

    return true;
}

void CLocatorAPI::_initialize	(u32 flags, LPCSTR target_folder)
{
	if (m_Flags.is(flReady))return;

	Log				("Initializing File System...");
	u32	M1			= Memory.mem_usage();

	m_Flags.set		(flags,TRUE);

	// append application path

	if (m_Flags.is(flScanAppRoot)){
		append_path		("$app_root$",Core.ApplicationPath,0,FALSE);
    }
	if (m_Flags.is(flTargetFolderOnly)){
		append_path		("$target_folder$",target_folder,0,TRUE);
	}else{
		// scan root directory
		bNoRecurse		= TRUE;
		string_path		buf;
		IReader* F		= 0;
		Recurse			("");
		F				= r_open(FSLTX); 
		if (!F&&m_Flags.is(flScanAppRoot))
			F			= r_open("$app_root$",FSLTX); 
		R_ASSERT3		(F,"Can't open file:", FSLTX);
		// append all pathes    
		string_path		id, temp, root, add, def, capt;
		LPCSTR			lp_add, lp_def, lp_capt;
		string16		b_v;
		while(!F->eof()){
			F->r_string	(buf,sizeof(buf));
			_GetItem(buf,0,id,'=');
			if (id[0]==';') continue;
			_GetItem(buf,1,temp,'=');
			int cnt		= _GetItemCount(temp);  R_ASSERT(cnt>=3);
			u32 fl		= 0;
			_GetItem	(temp,0,b_v);	if (CInifile::IsBOOL(b_v)) fl |= FS_Path::flRecurse;
			_GetItem	(temp,1,b_v);	if (CInifile::IsBOOL(b_v)) fl |= FS_Path::flNotif;
			_GetItem	(temp,2,root);
			_GetItem	(temp,3,add);
			_GetItem	(temp,4,def);
			_GetItem	(temp,5,capt);
			xr_strlwr	(id);			if (!m_Flags.is(flBuildCopy)&&(0==xr_strcmp(id,"$build_copy$"))) continue;
			xr_strlwr	(root);
			lp_add		=(cnt>=4)?xr_strlwr(add):0;
			lp_def		=(cnt>=5)?def:0;
			lp_capt		=(cnt>=6)?capt:0;
			PathPairIt p_it = pathes.find(root);
			std::pair<PathPairIt, bool> I;
			FS_Path* P	= xr_new<FS_Path>((p_it!=pathes.end())?p_it->second->m_Path:root,lp_add,lp_def,lp_capt,fl);
			bNoRecurse	= !(fl&FS_Path::flRecurse);
			Recurse		(P->m_Path);
			I			= pathes.insert(mk_pair(xr_strdup(id),P));
			
			//disable file caching if no network drive ($server_root$ begins with \\x-ray)
			if(0==xr_strcmp(id,"$server_root$") && root!=strstr(root,"\\\\x-ray") )
				m_Flags.set(flCacheFiles,FALSE);
			
			R_ASSERT	(I.second);
		}
		r_close			(F);
	};

	if(strstr(Core.Params,"-pack ")){
			string512 pack_name;
			sscanf					(strstr(Core.Params,"-pack ")+6,"%[^ ] ",pack_name);
			update_path(pack_name,"$server_root$",pack_name);
			register_archieve		(pack_name);
	};

	u32	M2		= Memory.mem_usage();
	Msg				("FS: %d files cached, %dKb memory used.",files.size(),(M2-M1)/1024);

#ifdef __BORLANDC__
	m_Flags.set		(flEventNotificator,TRUE);
#endif
	if (m_Flags.is(flEventNotificator)){
	    // set event handlers
	    SetEventNotification();
    }
	m_Flags.set		(flReady,TRUE);

	CreateLog		(0!=strstr(Core.Params,"-nolog"));
}

void CLocatorAPI::_destroy		()
{
	CloseLog		();

	if (m_Flags.is(flEventNotificator)){
        // clear event handlers
        ClearEventNotification		();
    }

	for				(files_it I=files.begin(); I!=files.end(); I++)
	{
		char* str	= LPSTR(I->name);
		xr_free		(str);
	}
	files.clear		();
	for				(PathPairIt p_it=pathes.begin(); p_it!=pathes.end(); p_it++)
    {
		char* str	= LPSTR(p_it->first);
		xr_free		(str);
		xr_delete	(p_it->second);
    }
	pathes.clear	();
	for				(archives_it a_it=archives.begin(); a_it!=archives.end(); a_it++)
    {
		CloseHandle	(a_it->hSrcMap);
		CloseHandle	(a_it->hSrcFile);
    }
    archives.clear	();
}

const CLocatorAPI::file* CLocatorAPI::exist			(const char* fn)
{
	files_it it		= file_find_it(fn);
	return (it!=files.end())?&(*it):0;
}

const CLocatorAPI::file* CLocatorAPI::exist			(const char* path, const char* name)
{
	string_path		temp;       
    update_path		(temp,path,name);
	return exist(temp);
}

const CLocatorAPI::file* CLocatorAPI::exist			(char* fn, const char* path, const char* name)
{
    update_path		(fn,path,name);
	return exist(fn);
}

const CLocatorAPI::file* CLocatorAPI::exist			(char* fn, const char* path, const char* name, const char* ext)
{
	string_path nm;
	strconcat		(nm,name,ext);
    update_path		(fn,path,nm);
	return exist(fn);
}

xr_vector<char*>* CLocatorAPI::file_list_open			(const char* initial, const char* folder, u32 flags)
{
	string_path		N;
	R_ASSERT		(initial&&initial[0]);
	update_path		(N,initial,folder);
	return			file_list_open(N,flags);
}

xr_vector<char*>* CLocatorAPI::file_list_open			(const char* _path, u32 flags)
{
	R_ASSERT		(_path);
	VERIFY			(flags);
	// ��������� ����� �� ��������������� ����
	check_pathes	();

	xr_string		N;
	if (path_exist(_path))	update_path	(N,_path,"");
	else					N=_path		;

	file			desc;
	desc.name		= N.c_str();
	files_it	I 	= files.find(desc);
	if (I==files.end())	return 0;
	
	xr_vector<char*>*	dest	= xr_new<xr_vector<char*> > ();

	size_t base_len	= N.size();// xr_strlen(N);
	for (++I; I!=files.end(); I++)
	{
		const file& entry = *I;
		if (0!=strncmp(entry.name,N.c_str(),base_len))	break;	// end of list
		const char* end_symbol = entry.name+xr_strlen(entry.name)-1;
		if ((*end_symbol) !='\\')	{
			// file
			if ((flags&FS_ListFiles) == 0)	continue;

			const char* entry_begin = entry.name+base_len;
			if ((flags&FS_RootOnly)&&strstr(entry_begin,"\\"))	continue;	// folder in folder
			dest->push_back			(xr_strdup(entry_begin));
            LPSTR fname 			= dest->back();
            if (flags&FS_ClampExt)	if (0!=strext(fname)) *strext(fname)=0;
		} else {
			// folder
			if ((flags&FS_ListFolders) == 0)continue;
			const char* entry_begin = entry.name+base_len;
			
			if ((flags&FS_RootOnly)&&(strstr(entry_begin,"\\")!=end_symbol))	continue;	// folder in folder
			
			dest->push_back	(xr_strdup(entry_begin));
		}
	}
	return dest;
}

void	CLocatorAPI::file_list_close	(xr_vector<char*>* &lst)
{
	if (lst) 
	{
		for (xr_vector<char*>::iterator I=lst->begin(); I!=lst->end(); I++)
			xr_free	(*I);
		xr_delete	(lst);
	}
}

struct fl_mask{
	bool 	b_cmp_nm;
    bool	b_cmp_ext;
	shared_str nm;
    shared_str ext;
    fl_mask():b_cmp_nm(false),b_cmp_ext(false){};
};

DEFINE_VECTOR(fl_mask,FLMaskVec,FLMaskVecIt);
int CLocatorAPI::file_list(FS_QueryMap& dest, LPCSTR path, u32 flags, LPCSTR mask)
{
	R_ASSERT		(path);
	VERIFY			(flags);
	// ��������� ����� �� ��������������� ����
    check_pathes	();
               
	xr_string		N;
	if (path_exist(path))	update_path(N,path,"");
    else						N=path;

	file			desc;
	desc.name		= N.c_str();
	files_it	I 	= files.find(desc);
	if (I==files.end())	return 0;

    BOOL b_mask 	= FALSE;
	FLMaskVec 		masks;
	if (mask){
		int cnt		= _GetItemCount(mask);
		string64	buf;
		for (int k=0; k<cnt; k++){
        	_GetItem(mask,k,buf);
            fl_mask item;
        	// name
			string64 nm;
        	_GetItem(buf,0,nm,'.'); 
			if (nm&&nm[0]&&(nm[0]!='*'))	{	item.nm=nm; item.b_cmp_nm=true;		}
			// extension
            LPCSTR ext	= strext(buf);
			if (ext&&ext[0]&&(ext[1]!='*'))	{	item.ext=ext; item.b_cmp_ext=true; 	}
            masks.push_back(item);
        }
        b_mask		= !masks.empty();
	}

	size_t base_len	= N.size();
	for (++I; I!=files.end(); I++)
	{
		const file& entry = *I;
		if (0!=strncmp(entry.name,N.c_str(),base_len))	break;	// end of list
		LPCSTR end_symbol = entry.name+xr_strlen(entry.name)-1;
		if ((*end_symbol) !='\\')	{
			// file
			if ((flags&FS_ListFiles) == 0)	continue;
			LPCSTR entry_begin 		= entry.name+base_len;
			if ((flags&FS_RootOnly)&&strstr(entry_begin,"\\"))	continue;	// folder in folder
			// check extension
			if (b_mask){
                xr_string 		nm;
                _GetItem			(entry_begin,0,nm,'.');
				LPCSTR ext			= strext(entry_begin);
                bool bOK			= false;
                for (FLMaskVecIt it=masks.begin(); it!=masks.end(); it++){
                	bool bNM		= true;
                    bool bEXT		= true;
                    if (it->b_cmp_nm)	if (nm!=*it->nm) 					bNM =false;
                    if (it->b_cmp_ext)	if (!ext||(0!=xr_strcmp(ext,*it->ext)))bEXT=false;
                    bOK				= bNM&&bEXT;
                    if (bOK)		break;
                }
                if (!bOK)			continue;
			}
			xr_string fn			= entry_begin;
			// insert file entry
			if (flags&FS_ClampExt)fn= EFS.ChangeFileExt(fn,"");
			u32 fl = (entry.vfs!=0xffffffff?FS_QueryItem::flVFS:0);
			dest.insert(mk_pair(fn.c_str(),FS_QueryItem(entry.size_real,entry.modif,fl)));
		} else {
			// folder
			if ((flags&FS_ListFolders) == 0)	continue;
			LPCSTR entry_begin 		= entry.name+base_len;

			if ((flags&FS_RootOnly)&&(strstr(entry_begin,"\\")!=end_symbol))	continue;	// folder in folder
			u32 fl = FS_QueryItem::flSubDir|(entry.vfs?FS_QueryItem::flVFS:0);
			dest.insert(mk_pair(entry_begin,FS_QueryItem(entry.size_real,entry.modif,fl)));
		}
	}
	return dest.size();
}

bool CLocatorAPI::file_find(FS_QueryItem& dest, LPCSTR path, LPCSTR name, bool clamp_ext)
{
	R_ASSERT			(path);
	// ��������� ����� �� ��������������� ����
    check_pathes		();

	xr_string			N;
	if (path_exist(path))	update_path(N,path,name);
    else						N=xr_string(path)+name;

	file				desc;
	desc.name			= N.c_str();
	files_it	I 		= files.find(desc);
	if (I==files.end())	return false;

	size_t base_len		= N.size();
    const file& entry 	= *I;
    xr_string fn		= (clamp_ext)?EFS.ChangeFileExt(entry.name+base_len,""):xr_string(entry.name+base_len);
    u32 fl 				= (entry.vfs!=0xffffffff?FS_QueryItem::flVFS:0);
    dest.set			(entry.size_real,entry.modif,fl);
    return true;
}

IReader* CLocatorAPI::r_open	(LPCSTR path, LPCSTR _fname)
{
	IReader* R		= 0;
	// ��������� ����� �� ��������������� ����
    check_pathes	();

	// correct path
	string_path		fname;
	strcpy			(fname,_fname);
	xr_strlwr		(fname);
	if (path&&path[0]) update_path(fname,path,fname);

	// Search entry
	file			desc_f;
	desc_f.name		= fname;
	files_it	I 	= files.find(desc_f);
	if (I == files.end()) return NULL;

	dwOpenCounter	++;

	const	file& desc	= *I;
	LPCSTR	source_name = &fname[0];

	// OK, analyse
	if (0xffffffff == desc.vfs)
	{
		// Normal file, 100% full path - check cache
		// Release don't need this at all
#ifdef	DEBUG
		if (m_Flags.is(flCacheFiles)){
			string_path	fname_copy;
			if (pathes.size()>1)
			{
				LPCSTR		path_base		= get_path	("$server_root$")->m_Path;
				u32			len_base		= xr_strlen	(path_base);
				LPCSTR		path_file		= fname;
				u32			len_file		= xr_strlen	(path_file);
				if (len_file>len_base)		
				{
					if (0==memcmp(path_base,fname,len_base))	{
						BOOL		bCopy	= FALSE;

						string_path	fname_in_cache	;
						update_path	(fname_in_cache,"$cache$",path_file+len_base);
						files_it	fit	= file_find_it(fname_in_cache);
						if (fit!=files.end())	
						{
							// use
							const file&	fc	= *fit;
							if ((fc.size_real == desc.size_real)&&(fc.modif==desc.modif))	{
								// use
							} else {
								// copy & use
								Msg			("copy: db[%X],cache[%X] - '%s', ",desc.modif,fc.modif,fname);
								bCopy		= TRUE;
							}
						} else {
							// copy & use
							bCopy	= TRUE;
						}

						// copy if need
						if (bCopy)			
						{
							IReader*	_src;
							if (desc.size_real<256*1024)	_src = xr_new<CFileReader>			(fname);
							else							_src = xr_new<CVirtualFileReader>	(fname);
							IWriter*	_dst	= xr_new<CFileWriter>			(fname_in_cache);
							_dst->w				(_src->pointer(),_src->length());
							xr_delete			(_dst);
							xr_delete			(_src);
							set_file_age		(fname_in_cache,desc.modif);
							Register			(fname_in_cache,0xffffffff,0,desc.size_real,desc.size_real,desc.modif);
						}

						// Use
						source_name	= &fname_copy[0];
						strcpy		(fname_copy,fname);
						strcpy		(fname,fname_in_cache);
					}
				}
			}
		}
#endif
		if (desc.size_real<256*1024)	R = xr_new<CFileReader>			(fname);
		else							R = xr_new<CVirtualFileReader>	(fname);
	} else {
		// Archived one
		archive& A						= archives[desc.vfs];
		u32 start						= (desc.ptr/dwAllocGranularity)*dwAllocGranularity;
		u32 end							= (desc.ptr+desc.size_compressed)/dwAllocGranularity;
		if ((desc.ptr+desc.size_compressed)%dwAllocGranularity)	end+=1;
		end								*= dwAllocGranularity;
		if (end>A.size)					end = A.size;
		u32 sz							= (end-start);
		u8* ptr							= (u8*)MapViewOfFile(A.hSrcMap, FILE_MAP_READ, 0, start, sz); VERIFY(ptr);
		u32 ptr_offs					= desc.ptr-start;
		if (desc.size_real != desc.size_compressed)	{
			// Compressed
			u8*			dest			= xr_alloc<u8>(desc.size_real);
			rtc_decompress				(dest,desc.size_real,ptr+ptr_offs,desc.size_compressed);
			R = xr_new<CTempReader>		(dest,desc.size_real,0);
			UnmapViewOfFile				(ptr);
		} else {
//			R_ASSERT2(data,cFileName);
			// Plain (VFS)
			R = xr_new<CPackReader>		(ptr,ptr+ptr_offs,desc.size_real);
		}
	}

#ifdef DEBUG
	if ( R && m_Flags.is(flBuildCopy|flReady) ){
		string_path	cpy_name;
		string_path	e_cpy_name;
		FS_Path* 	P; 
		if (source_name==strstr(source_name,(P=get_path("$server_root$"))->m_Path)||
        	source_name==strstr(source_name,(P=get_path("$server_data_root$"))->m_Path)){
			update_path			(cpy_name,"$build_copy$",source_name+xr_strlen(P->m_Path));
			IWriter* W = w_open	(cpy_name);
            if (W){
                W->w				(R->pointer(),R->length());
                w_close				(W);
                set_file_age(cpy_name,get_file_age(source_name));
                if (m_Flags.is(flEBuildCopy)){
                    LPCSTR ext		= strext(cpy_name);
                    if (ext){
                        IReader* R		= 0;
                        if (0==xr_strcmp(ext,".dds")){
                            P			= get_path("$game_textures$");               
                            update_path	(e_cpy_name,"$textures$",source_name+xr_strlen(P->m_Path));
                            // tga
                            *strext		(e_cpy_name) = 0;
                            strcat		(e_cpy_name,".tga");
                            r_close		(R=r_open(e_cpy_name));
                            // thm
                            *strext		(e_cpy_name) = 0;
                            strcat		(e_cpy_name,".thm");
                            r_close		(R=r_open(e_cpy_name));
                        }else if (0==xr_strcmp(ext,".ogg")){
                            P			= get_path("$game_sounds$");                               
                            update_path	(e_cpy_name,"$sounds$",source_name+xr_strlen(P->m_Path));
                            // wav
                            *strext		(e_cpy_name) = 0;
                            strcat		(e_cpy_name,".wav");
                            r_close		(R=r_open(e_cpy_name));
                            // thm
                            *strext		(e_cpy_name) = 0;
                            strcat		(e_cpy_name,".thm");
                            r_close		(R=r_open(e_cpy_name));
                        }else if (0==xr_strcmp(ext,".object")){
                            strcpy		(e_cpy_name,source_name);
                            // object thm
                            *strext		(e_cpy_name) = 0;
                            strcat		(e_cpy_name,".thm");
                            R			= r_open(e_cpy_name);
                            if (R)		r_close	(R);
                        }
                    }
                }
            }else{
            	Log			("!Can't build:",source_name);
            }
		}
	}
#endif
	return R;
}

void	CLocatorAPI::r_close	(IReader* &fs)
{
	xr_delete	(fs);
}

IWriter* CLocatorAPI::w_open	(LPCSTR path, LPCSTR _fname)
{
	string_path	fname;
	xr_strlwr(strcpy(fname,_fname));//,".$");
	if (path&&path[0]) update_path(fname,path,fname);
    CFileWriter* W 	= xr_new<CFileWriter>(fname); 
#ifdef _EDITOR
	if (!W->valid()) xr_delete(W);
#endif    
	return W;
}

void	CLocatorAPI::w_close(IWriter* &S)
{
	if (S){
        R_ASSERT	(S->fName.size());
        string_path	fname;
        strcpy		(fname,*S->fName);
        xr_delete	(S);
        struct _stat st;
        _stat		(fname,&st);
        Register	(fname,0xffffffff,0,st.st_size,st.st_size,(u32)st.st_mtime);
    }
}

CLocatorAPI::files_it CLocatorAPI::file_find_it(LPCSTR fname)
{
	// ��������� ����� �� ��������������� ����
    check_pathes	();

	file			desc_f;
	desc_f.name		= xr_strlwr(xr_strdup(fname));
    files_it I		= files.find(desc_f);
    xr_free			(desc_f.name);
	return I;
}

BOOL CLocatorAPI::dir_delete(LPCSTR path,LPCSTR nm,BOOL remove_files)
{
	string_path	fpath;
	if (path&&path[0]) 	update_path(fpath,path,nm);
    else				strcpy(fpath,nm);

    files_set 	folders;
    files_it I;
	// remove files
    I					= file_find_it(fpath);
    if (I!=files.end()){
        size_t base_len			= xr_strlen(fpath);
        for (; I!=files.end(); ){
            files_it cur_item	= I;
            const file& entry 	= *cur_item;
            I					= cur_item; I++;
            if (0!=strncmp(entry.name,fpath,base_len))	break;	// end of list
			const char* end_symbol = entry.name+xr_strlen(entry.name)-1;
			if ((*end_symbol) !='\\'){
//		        const char* entry_begin = entry.name+base_len;
				if (!remove_files) return FALSE;
		    	unlink		(entry.name);
				files.erase	(cur_item);
	        }else{
            	folders.insert(entry);
            }
        }
    }
    // remove folders
    files_set::reverse_iterator r_it = folders.rbegin();
    for (;r_it!=folders.rend();r_it++){
	    const char* end_symbol = r_it->name+xr_strlen(r_it->name)-1;
    	if ((*end_symbol) =='\\'){
        	_rmdir		(r_it->name);
            files.erase	(*r_it);
        }
    }
    return TRUE;
}

void CLocatorAPI::file_delete(LPCSTR path, LPCSTR nm)
{
	string_path	fname;
	if (path&&path[0]) 	update_path(fname,path,nm);
    else				strcpy(fname,nm);

    const files_it I	= file_find_it(fname);
    if (I!=files.end()){
	    // remove file
    	unlink			(I->name);
		char* str		= LPSTR(I->name);
		xr_free			(str);
	    files.erase		(I);
    }
}

void CLocatorAPI::file_copy(LPCSTR src, LPCSTR dest)
{
	if (exist(src)){
        IReader* S		= r_open(src);
        if (S){
            IWriter* D	= w_open(dest);
            if (D){
                D->w	(S->pointer(),S->length());
                w_close	(D);
            }
            r_close		(S);
        }
	}
}

void CLocatorAPI::file_rename(LPCSTR src, LPCSTR dest, bool bOwerwrite)
{
	files_it	S		= file_find_it(src);
	if (S!=files.end()){
		files_it D		= file_find_it(dest);
		if (D!=files.end()){ 
	        if (!bOwerwrite) return;
            unlink		(D->name);
			char* str	= LPSTR(D->name);
			xr_free		(str);
			files.erase	(D);
        }

        file new_desc	= *S;
		// remove existing item
		char* str		= LPSTR(S->name);
		xr_free			(str);
		files.erase		(S);
		// insert updated item
        new_desc.name	= xr_strlwr(xr_strdup(dest));
		files.insert	(new_desc); 
        
        // physically rename file
        VerifyPath		(dest);
        rename			(src,dest);
	}
}

int	CLocatorAPI::file_length(LPCSTR src)
{
	files_it	I		= file_find_it(src);
	return (I!=files.end())?I->size_real:-1;
}

bool CLocatorAPI::path_exist(LPCSTR path)
{
    PathPairIt P 			= pathes.find(path); 
    return					(P!=pathes.end());
}

FS_Path* CLocatorAPI::append_path(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive)
{
	VERIFY			(root&&root[0]);
	VERIFY			(false==path_exist(path_alias));
	FS_Path* P		= xr_new<FS_Path>(root,add,LPCSTR(0),LPCSTR(0),0);
	bNoRecurse		= !recursive;
	Recurse			(P->m_Path);
	pathes.insert	(mk_pair(xr_strdup(path_alias),P));
	return P;
}

FS_Path* CLocatorAPI::get_path(LPCSTR path)
{
    PathPairIt P 			= pathes.find(path); 
    R_ASSERT2(P!=pathes.end(),path);
    return P->second;
}

LPCSTR CLocatorAPI::update_path(LPSTR dest, LPCSTR initial, LPCSTR src)
{
    return get_path(initial)->_update(dest,src);
}

void CLocatorAPI::update_path(xr_string& dest, LPCSTR initial, LPCSTR src)
{
    return get_path(initial)->_update(dest,src);
}

u32 CLocatorAPI::get_file_age(LPCSTR nm)
{
	// ��������� ����� �� ��������������� ����
    check_pathes	();

	files_it I 		= file_find_it(nm);
    return (I!=files.end())?I->modif:u32(-1);
}

void CLocatorAPI::set_file_age(LPCSTR nm, u32 age)
{
	// ��������� ����� �� ��������������� ����
    check_pathes	();

    // set file
    _utimbuf	tm;
    tm.actime	= age;
    tm.modtime	= age;
    int res 	= _utime(nm,&tm);
    if (0!=res){
    	Msg			("!Can't set file age: '%s'. Error: '%s'",nm,_sys_errlist[errno]);
    }else{
        // update record
        files_it I 		= file_find_it(nm);
        if (I!=files.end()){
            file& F		= (file&)*I;
            F.modif		= age;
        }
    }
}

void CLocatorAPI::rescan_path(LPCSTR full_path, BOOL bRecurse)
{
	file desc; 
    desc.name		= full_path;
	files_it	I 	= files.lower_bound(desc);
	if (I==files.end())	return;
	
	size_t base_len			= xr_strlen(full_path);
	for (; I!=files.end(); ){
    	files_it cur_item	= I;
		const file& entry 	= *cur_item;
    	I					= cur_item; I++;
		if (0!=strncmp(entry.name,full_path,base_len))	break;	// end of list
		const char* entry_begin = entry.name+base_len;
        if (!bRecurse&&strstr(entry_begin,"\\")) continue;
        // erase item
		char* str		= LPSTR(cur_item->name);
		xr_free			(str);
		files.erase		(cur_item);
	}
    bNoRecurse	= !bRecurse;
    Recurse		(full_path);
}

void  CLocatorAPI::rescan_pathes()
{
	m_Flags.set(flNeedRescan,FALSE);
	for (PathPairIt p_it=pathes.begin(); p_it!=pathes.end(); p_it++)
    {
    	FS_Path* P	= p_it->second;
        if (P->m_Flags.is(FS_Path::flNeedRescan)){
			rescan_path(P->m_Path,P->m_Flags.is(FS_Path::flRecurse));
			P->m_Flags.set(FS_Path::flNeedRescan,FALSE);
        }
    }
}

void CLocatorAPI::check_pathes()
{
	if (m_Flags.is(flNeedRescan)&&(0==m_iLockRescan)){
    	lock_rescan		();
    	rescan_pathes	();
    	unlock_rescan	();
    }
}

void CLocatorAPI::register_archieve(LPCSTR path)
{
	ProcessArchive(path);
}

BOOL CLocatorAPI::can_write_to_folder(LPCSTR path)
{
	if (path&&path[0]){
		string_path		temp;       
        LPCSTR fn		= "$!#%TEMP%#!$.$$$";
	    strconcat		(temp,path,path[xr_strlen(path)-1]!='\\'?"\\":"",fn);
		FILE* hf		= fopen	(temp, "wb");
		if (hf==0)		return FALSE;
        else{
        	fclose 		(hf);
	    	unlink		(temp);
            return 		TRUE;
        }
    }else{
    	return 			FALSE;
    }
}

BOOL CLocatorAPI::can_write_to_alias(LPCSTR path)
{
	string_path			temp;       
    update_path			(temp,path,"");
	return can_write_to_folder(temp);
}

BOOL CLocatorAPI::can_modify_file(LPCSTR fname)
{
	FILE* hf			= fopen	(fname, "r+b");
    if (hf){	
    	fclose			(hf);
        return 			TRUE;
    }else{
    	return 			FALSE;
    }
}

BOOL CLocatorAPI::can_modify_file(LPCSTR path, LPCSTR name)
{
	string_path			temp;       
    update_path			(temp,path,name);
	return can_modify_file(temp);
}

