//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "FileSystem.h"

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

bool EFS_Utils::GetOpenName(LPCSTR initial, xr_string& buffer, bool bMulti, LPCSTR offset, int start_flt_ext )
{
	string4096 buf;
	strcpy(buf,buffer.c_str());
	bool bRes = GetOpenName(initial,buf,sizeof(buf),bMulti,offset,start_flt_ext);
	if (bRes) buffer=(char*)buf;
	return bRes;
}

bool EFS_Utils::GetSaveName( LPCSTR initial, xr_string& buffer, LPCSTR offset, int start_flt_ext )
{
	string4096 buf;
	strcpy(buf,buffer.c_str());
	bool bRes = GetSaveName(initial,buf,sizeof(buf),offset,start_flt_ext);
	if (bRes) buffer=buf;
	return bRes;
}
//----------------------------------------------------

void EFS_Utils::MarkFile(LPCSTR fn, bool bDeleteSource)
{
	xr_string ext = strext(fn);
	ext.insert		(1,"~");
	xr_string backup_fn = EFS.ChangeFileExt(fn,ext.c_str());
	if (bDeleteSource){
		FS.file_rename(fn,backup_fn.c_str(),true);
	}else{
		FS.file_copy(fn,backup_fn.c_str());
	}
}

xr_string	EFS_Utils::AppendFolderToName(xr_string& tex_name, int depth, BOOL full_name)
{
	string1024 nm;
	strcpy(nm,tex_name.c_str());
	tex_name = AppendFolderToName(nm,depth,full_name);
	return tex_name;
}

