// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "FS_internal.h"

CLocatorAPI	FS;

//////////////////////////////////////////////////////////////////////
// FS_Path
//////////////////////////////////////////////////////////////////////
FS_Path::FS_Path	(LPCSTR _Root, LPCSTR _Add, LPCSTR _DefExt, LPCSTR _FilterCaption)
{
	VERIFY			(_Root);
	string256		temp;
    strcpy			(temp,_Root);
    if (_Add) 		strcat(temp,_Add);
	m_Path			= xr_strdup(temp);
	m_DefExt		= _DefExt?xr_strdup(_DefExt):0;
	m_FilterCaption	= _FilterCaption?xr_strdup(_FilterCaption):0;
	m_Add			= _Add?xr_strdup(_Add):0;
	m_Root			= _Root?xr_strdup(_Root):0;
	VerifyPath		(m_Path);
}

LPCSTR FS_Path::_update(LPSTR _fname)const
{
	VERIFY(_fname);
	string256		temp;
	strcpy			(temp,_fname);
	return strconcat(_fname,m_Path,temp);
}

LPCSTR FS_Path::_update(LPSTR dest, LPCSTR src)const
{
	R_ASSERT(dest);
    R_ASSERT(src);
	return strconcat(dest,m_Path,src);
}

#ifdef __BORLANDC__
const AnsiString& FS_Path::_update(AnsiString& _fname)const
{
	return _fname=AnsiString(m_Path)+_fname;
}
const AnsiString& FS_Path::_update(AnsiString& dest, LPCSTR src)const
{
    R_ASSERT(src);
	return dest=AnsiString(m_Path)+src;
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLocatorAPI::CLocatorAPI()
{
}

CLocatorAPI::~CLocatorAPI()
{
}

void CLocatorAPI::Register		(LPCSTR name, u32 vfs, u32 ptr, u32 size, BOOL bCompressed, u32 modif)
{
	// Register file
	file				desc;
	desc.name			= strlwr(xr_strdup(name));
	desc.vfs			= vfs;
	desc.ptr			= ptr;
	desc.size			= size;
    desc.modif			= modif;
	desc.bCompressed	= bCompressed;

	// if file already exist - update info
	set_files_it	I	= files.find(desc);
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
	string256			temp;	strcpy(temp,desc.name);
	string256			folder;
	while (temp[0]) 
	{
		_splitpath			(temp, 0, folder, 0, 0 );
		if (!exist(folder))	
		{
			desc.name			= xr_strdup(folder);
			desc.vfs			= 0xffffffff;
			desc.ptr			= 0;
			desc.size			= 0;
			desc.bCompressed	= FALSE;
            desc.modif			= 0;
			files.insert		(desc); 
		}
		strcpy(temp,folder);
		if (strlen(temp))		temp[strlen(temp)-1]=0;
	}
}

void CLocatorAPI::ProcessArchive(const char* path)
{
	// Open archive
	archive				A;
	A.vfs				= xr_new<CVirtualFileReader> (path);
	archives.push_back	(A);

	// Create base path
	string256			base;
	strcpy				(base,path);
	*strext(base)		= 0;
	strcat				(base,"\\");

	// Read headers
	IReader*	hdr		= A.vfs->open_chunk(1);
	while (!hdr->eof())
	{
		string512		name,full;
		hdr->r_stringZ	(name);
		strconcat		(full,base,name);

		size_t vfs		= archives.size()-1;
		BOOL  bPacked	= (hdr->r_u32())?FALSE:TRUE;
		u32 ptr			= hdr->r_u32();
		u32 size		= hdr->r_u32();
		Register		(full,(u32)vfs,ptr,size,bPacked,0);
	}
	hdr->close			();

	// Seek to zero for safety
	A.vfs->seek			(0);
}

void CLocatorAPI::ProcessOne	(const char* path, LPVOID _F)
{
	_finddata_t& F	= *((_finddata_t*)_F);

	string256	N;
	strcpy		(N,path);
	strcat		(N,F.name);
	strlwr		(N);
	
	if (F.attrib&_A_HIDDEN)			return;

	if (F.attrib&_A_SUBDIR) {
		if (bNoRecurse)				return;
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat		(N,"\\");
		Register	(N,0xffffffff,0,F.size,0,(u32)F.time_write);
		Recurse		(N);
	} else {
		if (strext(N) && 0==strncmp(strext(N),".xp",3) && !bNoRecurse)	ProcessArchive	(N);
		else															Register		(N,0xffffffff,0,F.size,0,(u32)F.time_write);
	}
}

DEFINE_VECTOR(_finddata_t,FFVec,FFIt);
IC bool pred_str_ff(const _finddata_t& x, const _finddata_t& y)
{	
	return strcmp(x.name,y.name)<0;	
}

bool CLocatorAPI::Recurse		(const char* path)
{
    _finddata_t		sFile;
    intptr_t		hFile;

	string256		N;
	strcpy			(N,path);
	strcat			(N,"*.*");

	FFVec			rec_files;
	rec_files.reserve(256);

	if (-1==(hFile=_findfirst(N, &sFile))){
    	Log			("Wrong path: ",path);
    	return		false;
    }
	rec_files.push_back(sFile);
	while			( _findnext( hFile, &sFile ) == 0 )
		rec_files.push_back(sFile);
	_findclose		( hFile );

	sort			(rec_files.begin(), rec_files.end(), pred_str_ff);

	for (FFIt it=rec_files.begin(); it!=rec_files.end(); it++)
		ProcessOne	(path,it);
    return true;
}

void CLocatorAPI::_initialize	()
{
	Log				("Initializing File System...");
	DWORD	M1		= Memory.mem_usage();

	// scan root directory
	bNoRecurse		= TRUE;
	Recurse			("");

	string256		buf;
	IReader* F		= r_open("","fs.ltx");
	string256		id, temp, root, add, def, capt;
	LPCSTR			lp_add, lp_def, lp_capt;
    string16		recurse;
	while(!F->eof()){
		F->r_string	(buf);
        _GetItem(buf,0,id,'=');
        _GetItem(buf,1,temp,'=');
		int cnt		= _GetItemCount(temp);  R_ASSERT(cnt>=2);
        _GetItem	(temp,0,recurse);
        _GetItem	(temp,1,root);
        _GetItem	(temp,2,add);
        _GetItem	(temp,3,def);
        _GetItem	(temp,4,capt);
		strlwr		(id);
		strlwr		(root);
		lp_add		=(cnt>=3)?strlwr(add):0;
		lp_def		=(cnt>=4)?def:0;
		lp_capt		=(cnt>=5)?capt:0;
		PathPairIt p_it = pathes.find(root);
		pair<PathPairIt, bool> I;
        FS_Path* P	= xr_new<FS_Path>((p_it!=pathes.end())?p_it->second->m_Path:root,lp_add,lp_def,lp_capt);
		bNoRecurse	= !CInifile::IsBOOL(recurse);
		if (Recurse(P->m_Path)){
            I		= pathes.insert(make_pair(xr_strdup(id),P));
            R_ASSERT(I.second);
        }
	}
	r_close			(F);
	DWORD	M2		= Memory.mem_usage();
	Msg				("FS: %d files cached, %dKb memory used.",files.size(),(M2-M1)/1024);
}
void CLocatorAPI::_destroy		()
{
	Log				("ShutDown: File System...");
	for				(set_files_it I=files.begin(); I!=files.end(); I++)
	{
		char* str	= LPSTR(I->name);
		xr_free		(str);
	}
	files.clear		();
	for		(PathPairIt p_it=pathes.begin(); p_it!=pathes.end(); p_it++)
    {
		char* str	= LPSTR(p_it->first);
		xr_free		(str);
		xr_delete	(p_it->second);
    }
	pathes.clear	();
}

BOOL CLocatorAPI::exist			(const char* F)
{
	string256		N;
	strcpy			(N,F);
	strlwr			(N);
	file			desc;
	desc.name		= N;
	
	set_files_it	I = files.find(desc);
	return			I != files.end();
}

BOOL CLocatorAPI::exist			(char* fn, const char* path, const char* name){
	strconcat		(fn, path, name);
	return exist(fn);
}

BOOL CLocatorAPI::exist			(char* fn, const char* path, const char* name, const char* ext){
	strconcat		(fn, path, name, ext);
	return exist(fn);
}

void CLocatorAPI::List			(vector<char*>& dest, const char* path, u32 flags)
{
	VERIFY			(flags);
	
	string256		N;
	strcpy			(N,path);
	strlwr			(N);
	if (N[strlen(N)-1]!='\\') strcat(N,"\\");
	
	file			desc;
	desc.name		= N;
	set_files_it	I = files.find(desc);
	if (I==files.end())	return;
	
	size_t base_len	= strlen(N);
	for (++I; I!=files.end(); I++)
	{
		const file& entry = *I;
		if (0!=strncmp(entry.name,N,base_len))	break;	// end of list
		const char* end_symbol = entry.name+strlen(entry.name)-1;
		if ((*end_symbol) !='\\')	{
			// file
			if ((flags&FS_ListFiles) == 0)	continue;
			R_ASSERT(0);
		} else {
			// folder
			if ((flags&FS_ListFolders) == 0)continue;
			const char* entry_begin = entry.name+base_len;
			
			if (strstr(entry_begin,"\\")!=end_symbol)	continue;	// folder in folder
			
			dest.push_back	(xr_strdup(entry_begin));
		}
	}
	return;
}

#ifdef __BORLANDC__
void CLocatorAPI::List			(FileMap& dest, const char* path, u32 flags)
{
	VERIFY			(flags);
	
	string256		N;
	strcpy			(N,path);
	strlwr			(N);
	if (N[strlen(N)-1]!='\\') strcat(N,"\\");
	
	file			desc;
	desc.name		= N;
	set_files_it	I = files.find(desc);
	if (I==files.end())	return;
	
	size_t base_len	= strlen(N);
	for (++I; I!=files.end(); I++)
	{
		const file& entry = *I;
		if (0!=strncmp(entry.name,N,base_len))	break;	// end of list
		const char* end_symbol = entry.name+strlen(entry.name)-1;
		if ((*end_symbol) !='\\')	{
			// file
			if ((flags&FS_ListFiles) == 0)	continue;
			R_ASSERT(0);
		} else {
			// folder
			if ((flags&FS_ListFolders) == 0)continue;
			const char* entry_begin = entry.name+base_len;
			
			if (strstr(entry_begin,"\\")!=end_symbol)	continue;	// folder in folder
			dest[entry_begin] = entry.modif;
		}
	}
	return;
}
#endif

IReader* CLocatorAPI::r_open	(LPCSTR path, LPCSTR _fname)
{
	// correct path
	string512		fname;
	strcpy			(fname,_fname);
	strlwr			(fname);
	if (path&&path[0]) update_path(path,fname);

	// Search entry
	file			desc_f;
	desc_f.name		= fname;
	set_files_it	I = files.find(desc_f);
	if (I == files.end()) return NULL;

	// OK, analyse
	const file& desc	= *I;
	if (0xffffffff == desc.vfs)
	{
		// Normal file
		if (desc.size<256*1024)	return xr_new<CFileReader>			(fname);
		else					return xr_new<CVirtualFileReader>	(fname);
	} else {
		// Archived one
		LPVOID	ptr	= LPVOID(LPBYTE(archives[desc.vfs].vfs->pointer()) + desc.ptr);
		if (desc.bCompressed)	
		{
			// Compressed
			BYTE*		dest;
			unsigned	size;

			_decompressLZ	(&dest,&size,ptr,desc.size);
			return xr_new<CTempReader>	(dest,size);
		} else {
			// Plain (VFS)
			return xr_new<IReader>		(ptr,desc.size);
		}
	}
}

void	CLocatorAPI::r_close	(IReader* &fs)
{
	xr_delete	(fs);
}

IWriter* CLocatorAPI::w_open	(LPCSTR path, LPCSTR _fname)
{
	string512	fname;
	strconcat	(fname,_fname,".$");
	strlwr		(fname);
	if (path&&path[0]) update_path(path,fname);
	return xr_new<CFileWriter>(fname);
}

void	CLocatorAPI::w_close(IWriter* &fs, bool bDiscard)
{
	R_ASSERT	(fs->fName&&fs->fName[0]);
	string256	temp_name;
	strcpy		(temp_name,fs->fName);
	fs->fName[strlen(fs->fName)-2]=0;
	if (bDiscard){
		xr_delete	(fs);
		unlink		(temp_name);
	}else{
		xr_delete	(fs);
		unlink		(fs->fName);
		rename		(temp_name,fs->fName);
        struct _stat st;
        _stat		(fs->fName,&st);
		Register	(fs->fName,0xffffffff,0,0,0,(u32)st.st_mtime);
	}
}

bool CLocatorAPI::path_exist		(LPCSTR path)
{
    PathPairIt P 			= pathes.find(path); 
    return					(P!=pathes.end());
}

FS_Path*	CLocatorAPI::get_path		(LPCSTR path)
{
    PathPairIt P 			= pathes.find(path); 
    R_ASSERT2(P!=pathes.end(),path);
    return P->second;
}

void	CLocatorAPI::update_path		(LPCSTR initial, LPSTR path)
{
    get_path(initial)->_update(path);
}

void	CLocatorAPI::update_path		(LPSTR dest, LPCSTR initial, LPCSTR src)
{
    get_path(initial)->_update(dest,src);
}

#ifdef __BORLANDC__
void CLocatorAPI::update_path		(LPCSTR initial, AnsiString& path)
{
    get_path(initial)->_update(path);
}
void CLocatorAPI::update_path		(AnsiString& dest, LPCSTR initial, LPCSTR src)
{
    get_path(initial)->_update(dest,src);
}
#endif    

