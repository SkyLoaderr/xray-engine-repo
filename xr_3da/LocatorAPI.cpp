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

void CLocatorAPI::Register		(LPCSTR name, DWORD vfs, DWORD ptr, BOOL bCompressed)
{
	file				desc;
	desc.name			= strlwr(strdup(name));
	desc.vfs			= vfs;
	desc.ptr			= ptr;
	desc.bCompressed	= bCompressed;
	files.insert		(desc); 
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
}

void CLocatorAPI::ProcessOne	(_finddata_t& F, const char* path)
{
	FILE_NAME	N;
	strcpy		(N,path);
	strcat		(N,F.name);
	strlwr		(N);
	
	if (F.attrib&_A_SUBDIR) {
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat		(N,"\\");
		Register	(N,0xffffffff,0,0);
		Recurse		(N);
	} else {
		if (strext(N) && 0==strcmp(strext(N),".xrp"))	ProcessArchive	(N);
		else											Register		(N,0xffffffff,F.size,0);
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
	ProcessOne		(sFile,path);

    while			( _findnext( hFile, &sFile ) == 0 )
		ProcessOne	(sFile,path);

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
	for		(set_cstr_it I=files.begin(); I!=files.end(); I++)
	{
		char* str = *I;
		_FREE(str);
	}
	files.clear();
}

BOOL CLocatorAPI::Exist			(const char* F)
{
	FILE_NAME		N;
	strcpy			(N,F);
	strlwr			(N);

	set_cstr_it		I = files.find(N);
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

	set_cstr_it		I = files.find(N);
	if (I==files.end())	return;

	int base_len	= strlen(N);
	for (++I; I!=files.end(); I++)
	{
		char* entry = *I;
		if (0!=strncmp(entry,N,base_len))	break;	// end of list
		char* end_symbol = entry+strlen(entry)-1;
		if ((*end_symbol) !='\\')	{
			// file
			if ((flags&FS_ListFiles) == 0)	continue;
			R_ASSERT(0);
		} else {
			// folder
			if ((flags&FS_ListFolders) == 0)continue;
			const char* entry_begin = entry+base_len;

			if (strstr(entry_begin,"\\")!=end_symbol)	continue;	// folder in folder

			dest.push_back	(strdup(entry_begin));
		}
	}
	return;
}
