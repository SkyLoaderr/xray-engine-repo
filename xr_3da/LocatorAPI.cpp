// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LocatorAPI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocatorAPI::CLocatorAPI()
{
	
}

CLocatorAPI::~CLocatorAPI()
{
	
}

void CLocatorAPI::Register		(LPCSTR name, DWORD vfs, DWORD ptr, DWORD size, BOOL bCompressed)
{
	// Register file
	file				desc;
	desc.name			= strlwr(xr_strdup(name));
	desc.vfs			= vfs;
	desc.ptr			= ptr;
	desc.size			= size;
	desc.bCompressed	= bCompressed;
	files.insert		(desc); 
	
	// Try to register folder(s)
	string256			temp;	strcpy(temp,desc.name);
	string256			folder;
	while (temp[0]) 
	{
		_splitpath			(temp, 0, folder, 0, 0 );
		if (!Exist(folder))	
		{
			desc.name			= xr_strdup(folder);
			desc.vfs			= 0xffffffff;
			desc.ptr			= 0;
			desc.size			= 0;
			desc.bCompressed	= FALSE;
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
	A.vfs				= new CVirtualFileStream(path);
	archives.push_back	(A);

	// Create base path
	string256			base;
	strcpy				(base,path);
	*strext(base)		= 0;
	strcat				(base,"\\");

	// Read headers
	CStream*	hdr		= A.vfs->OpenChunk(1);
	while (!hdr->Eof())
	{
		string512		name,full;
		hdr->RstringZ	(name);
		strconcat		(full,base,name);

		DWORD vfs		= archives.size()-1;
		BOOL  bPacked	= (hdr->Rdword())?FALSE:TRUE;
		DWORD ptr		= hdr->Rdword();
		DWORD size		= hdr->Rdword();
		Register		(full,vfs,ptr,size,bPacked);
	}
	hdr->Close			();

	// Seek to zero for safety
	A.vfs->Seek			(0);
}

void CLocatorAPI::ProcessOne	(const char* path, _finddata_t& F)
{
	FILE_NAME	N;
	strcpy		(N,path);
	strcat		(N,F.name);
	strlwr		(N);
	
	if (F.attrib&_A_SUBDIR) {
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat		(N,"\\");
		Register	(N,0xffffffff,0,0,0);
		Recurse		(N);
	} else {
		if (strext(N) && 0==strcmp(strext(N),".xrp"))	ProcessArchive	(N);
		else											Register		(N,0xffffffff,0,0,0);
	}
}

void CLocatorAPI::Recurse		(const char* path)
{
    _finddata_t		sFile;
    int				hFile;

	FILE_NAME		N;
	strcpy			(N,path);
	strcat			(N,"*.*");

    R_ASSERT		((hFile=_findfirst(N, &sFile)) != -1);
	ProcessOne		(path,sFile);

    while			( _findnext( hFile, &sFile ) == 0 )
		ProcessOne	(path,sFile);

    _findclose		( hFile );
}

void CLocatorAPI::Initialize	()
{
	Log		("Initializing File System...");
	Recurse	("");
	Msg		("FS: %d files cached",files.size());
}
void CLocatorAPI::Destroy		()
{
	Log		("ShutDown: File System...");
	for		(set_files_it I=files.begin(); I!=files.end(); I++)
	{
		char* str = LPSTR(I->name);
		_FREE(str);
	}
	files.clear();
}

BOOL CLocatorAPI::Exist			(const char* F)
{
	FILE_NAME		N;
	strcpy			(N,F);
	strlwr			(N);
	file			desc;
	desc.name		= N;
	
	set_files_it	I = files.find(desc);
	return			I != files.end();
}

BOOL CLocatorAPI::Exist			(char* fn, const char* path, const char* name){
	strconcat		(fn, path, name);
	return Exist(fn);
}

BOOL CLocatorAPI::Exist			(char* fn, const char* path, const char* name, const char* ext){
	strconcat		(fn, path, name, ext);
	return Exist(fn);
}

void CLocatorAPI::List			(vector<char*>& dest, const char* path, DWORD flags)
{
	VERIFY			(flags);
	
	FILE_NAME		N;
	strcpy			(N,path);
	strlwr			(N);
	if (N[strlen(N)-1]!='\\') strcat(N,"\\");
	
	file			desc;
	desc.name		= N;
	set_files_it	I = files.find(desc);
	if (I==files.end())	return;
	
	int base_len	= strlen(N);
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

CStream* CLocatorAPI::Open	(const char* F)
{
	// Search entry
	FILE_NAME		N;
	strcpy			(N,F);
	strlwr			(N);
	file			desc_f;
	desc_f.name		= N;
	set_files_it	I = files.find(desc_f);
	if (I == files.end()) return NULL;

	// OK, analyse
	const file& desc	= *I;
	if (0xffffffff == desc.vfs)
	{
		// Normal file
		return new CFileStream	(F);
	} else {
		// Archived one
		LPVOID	ptr	= LPVOID(LPBYTE(archives[desc.vfs].vfs->Pointer()) + desc.ptr);
		if (desc.bCompressed)	
		{
			// Compressed
			BYTE*		dest;
			unsigned	size;

			_decompressLZ	(&dest,&size,ptr,desc.size);
			return new CTempStream	(dest,size);
		} else {
			// Plain (VFS)
			return new CStream		(ptr,desc.size);
		}
	}
}

void	CLocatorAPI::Close	(CStream* &fs)
{
	_DELETE	(fs);
}
