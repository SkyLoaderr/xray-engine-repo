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

#define BACKUP_FILE_LEVEL 5

void EFS_Utils::BackupFile(LPCSTR initial, LPCSTR fname, bool bMsg)
{
	R_ASSERT(initial);
	xr_string src_name; 
	FS.update_path(src_name,initial,fname);
	if (FS.exist(src_name.c_str())){
		xr_string			dst_name,dst_path,del_name;
		FS_Path* P 			= FS.get_path(initial);
		string64			t_stemp;
        dst_name			= xr_string(P->m_Add)+fname+"."+Core.UserName+"_"+timestamp(t_stemp);
		FS.update_path		(dst_name,"$server_backup$",dst_name.c_str());
        FS.update_path		(dst_path,"$server_backup$",P->m_Add);

        // удалить лишние бэкап файлы
        FS_QueryMap lst;
        xr_string mask	= EFS.ChangeFileExt(fname,".*"); xr_strlwr(mask);
		if (FS.file_list	(lst, dst_path.c_str(), FS_ListFiles, mask.c_str())>=BACKUP_FILE_LEVEL){
        	do{
            	FS_QueryPairIt  min_it  = lst.begin();
                FS_QueryPairIt  it		= lst.begin();
                FS_QueryPairIt	_E		= lst.end();
                for (; it!=_E; it++) if (it->second.modif<min_it->second.modif) min_it = it;
                del_name				= dst_path+min_it->first;
                FS.file_delete			(del_name.c_str());
                lst.erase	(min_it);
            }while(lst.size()>=BACKUP_FILE_LEVEL);
        }

		// copy backup
		FS.file_copy		(src_name.c_str(),dst_name.c_str());
		WriteAccessLog		(dst_name.c_str(),"Backup");
	}else{
    	if (bMsg)			Log("!Can't backup file:",fname);
    }
}

xr_string	EFS_Utils::AppendFolderToName(xr_string& tex_name, int depth, BOOL full_name)
{
	string1024 nm;
	strcpy(nm,tex_name.c_str());
	tex_name = AppendFolderToName(nm,depth,full_name);
	return tex_name;
}

void EFS_Utils::WriteAccessLog(LPCSTR fn, LPCSTR start_msg)
{
	string1024		buf;
	string256		dt_buf, tm_buf;
	sprintf			(buf, "%s:   '%s' from computer: '%s' by user: '%s' at %s %s",start_msg,fn,Core.CompName,Core.UserName,_strdate(dt_buf),_strtime(tm_buf));
	xr_string		m_AccessLog;
	FS.update_path	(m_AccessLog,"$server_data_root$","access.log");
	int hf 			= open( m_AccessLog.c_str(), _O_WRONLY|_O_APPEND|_O_BINARY );
	if( hf<=0 )
		hf = open( m_AccessLog.c_str(),
		_O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY,
		_S_IREAD | _S_IWRITE );

	_write( hf, buf, xr_strlen(buf) );
    char el[2]={0x0d,0x0a};
	_write( hf, el, 2 );
	_close( hf );
}

void EFS_Utils::RegisterAccess(LPCSTR fn, LPCSTR start_msg, bool bLog)
{
	xr_string		m_LastAccessFN;
	FS.update_path	(m_LastAccessFN,"$server_data_root$","access.ini");
	CInifile*	ini	= CInifile::Create(m_LastAccessFN.c_str(),false);
	ini->w_string	("last_access",fn,Core.CompName);
	CInifile::Destroy(ini);
	if (bLog) 	WriteAccessLog(fn,start_msg);
}

BOOL EFS_Utils::CheckLocking(LPCSTR initial, LPCSTR fname, bool bOnlySelf, bool bMsg, shared_str* owner)
{
	string256 fn; strcpy(fn,fname);
	if (initial) FS.update_path(fn,initial,fn);

	if (bOnlySelf) return (m_LockFiles.find(fn)!=m_LockFiles.end());
	if (FS.exist(fn)){
		HANDLE handle=CreateFile(fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		CloseHandle(handle);
        if (INVALID_HANDLE_VALUE==handle){
			if (bMsg)	Msg("#!Access denied. File: '%s'currently locked by user: '%s'.",fn,GetLockOwner(0,fn));
            if (owner) 	*owner = GetLockOwner(0,fn);
        }
		return (INVALID_HANDLE_VALUE==handle);
	}
    return FALSE;
}

BOOL EFS_Utils::LockFile(LPCSTR initial, LPCSTR fname, bool bLog)
{
	string256 fn; strcpy(fn,fname);
	if (initial) FS.update_path(fn,initial,fn);

	BOOL bRes=false;
	if (m_LockFiles.find(fn)==m_LockFiles.end()){
		HANDLE handle=CreateFile(fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (INVALID_HANDLE_VALUE!=handle){
			LPSTR lp_fn=fn;
			std::pair<HANDLEPairIt, bool> I=m_LockFiles.insert(mk_pair(lp_fn,handle));
			R_ASSERT(I.second);
			RegisterAccess(fn,"Lock",bLog);
			bRes=true;
		}
	}
	return bRes;
}

BOOL EFS_Utils::UnlockFile(LPCSTR initial, LPCSTR fname, bool bLog)
{
	string256 fn; strcpy(fn,fname);
	if (initial) FS.update_path(fn,initial,fn);

	HANDLEPairIt it = m_LockFiles.find(fn);
	if (it!=m_LockFiles.end()){
		m_LockFiles.erase(it);
		if (bLog){
			WriteAccessLog(fn,"Unlock");
		}
		return CloseHandle(it->second);
	}
	return false;
}

shared_str EFS_Utils::GetLockOwner(LPCSTR initial, LPCSTR fname)
{
	string256 fn; strcpy(fn,fname);
	if (initial) FS.update_path(fn,initial,fn);

	xr_string		m_LastAccessFN;
	FS.update_path	(m_LastAccessFN,"$server_data_root$","access.ini");
	CInifile*	ini = CInifile::Create(m_LastAccessFN.c_str(),true);
	static string256 comp;
	strcpy(comp,ini->line_exist("last_access",fn)?ini->r_string("last_access",fn):"unknown");
	CInifile::Destroy(ini);

	return comp;
}

