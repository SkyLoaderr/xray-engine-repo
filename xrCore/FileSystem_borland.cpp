//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#ifdef M_BORLAND
bool EFS_Utils::GetOpenName(LPCSTR initial, AnsiString& buffer, bool bMulti, LPCSTR offset, int start_flt_ext ){
	string4096 buf;
	strcpy(buf,buffer.c_str());
	bool bRes = GetOpenName(initial,buf,sizeof(buf),bMulti,offset,start_flt_ext);
	if (bRes) buffer=buf;
	return bRes;
}

bool EFS_Utils::GetSaveName( LPCSTR initial, AnsiString& buffer, LPCSTR offset, int start_flt_ext ){
	string4096 buf;
	strcpy(buf,buffer.c_str());
	bool bRes = GetSaveName(initial,buf,sizeof(buf),offset,start_flt_ext);
	if (bRes) buffer=buf;
	return bRes;
}
//----------------------------------------------------

void EFS_Utils::MarkFile(const AnsiString& fn, bool bDeleteSource)
{
	AnsiString ext = ExtractFileExt(fn);
	ext.Insert("~",2);
	AnsiString backup_fn = ChangeFileExt(fn,ext);
	if (bDeleteSource){
		FS.file_rename(fn.c_str(),backup_fn.c_str(),true);
	}else{
		FS.file_copy(fn.c_str(),backup_fn.c_str());
	}
}

void EFS_Utils::BackupFile(LPCSTR initial, const AnsiString& fname)
{
	R_ASSERT(initial);
	AnsiString src_name; 
	FS.update_path(src_name,initial,fname.c_str());
	if (FS.exist(src_name.c_str())){
		AnsiString 			dst_name;
		FS_Path* P 			= FS.get_path(initial);
		string64			t_stemp;
		dst_name.sprintf	("%s%s.%s_%s",P->m_Add,fname.c_str(),Core.UserName,timestamp(t_stemp));
		FS.update_path		("$server_backup$",dst_name);
		FS.file_copy		(src_name.c_str(),dst_name.c_str());
		WriteAccessLog		(dst_name.c_str(),"Backup");
	}
}

AnsiString&	EFS_Utils::UpdateTextureNameWithFolder(AnsiString& tex_name)
{
	string1024 nm;
	strcpy(nm,tex_name.c_str());
	tex_name = UpdateTextureNameWithFolder(nm);
	return tex_name;
}

void EFS_Utils::WriteAccessLog(LPSTR fn, LPSTR start_msg)
{
	string1024 buf;
	string128 dt_buf, tm_buf;
	sprintf(buf, "%s:   '%s' from computer: '%s' by user: '%s' at %s %s",start_msg,fn,Core.CompName,Core.UserName,_strdate(dt_buf),_strtime(tm_buf));
	int hf = open( m_AccessLog, _O_WRONLY|_O_APPEND|_O_BINARY );
	if( hf<=0 )
		hf = open( m_AccessLog,
		_O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY,
		_S_IREAD | _S_IWRITE );

	_write( hf, buf, strlen(buf) );
	_close( hf );
}

void EFS_Utils::RegisterAccess(LPSTR fn, LPSTR start_msg, bool bLog)
{
	CInifile*	ini = CInifile::Create(m_LastAccessFN,false);
	ini->w_string("last_access",fn,Core.CompName);
	CInifile::Destroy(ini);
	if (bLog) 	WriteAccessLog(fn,start_msg);
}

BOOL EFS_Utils::CheckLocking(LPCSTR initial, LPSTR fname, bool bOnlySelf, bool bMsg)
{
	string256 fn; strcpy(fn,fname);
	if (initial) FS.update_path(fn,initial,fn);

	if (bOnlySelf) return (m_LockFiles.find(fn)!=m_LockFiles.end());
	if (FS.exist(fn)){
		HANDLE handle=CreateFile(fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		CloseHandle(handle);
		if (bMsg&&(INVALID_HANDLE_VALUE==handle))
			ELog.DlgMsg(mtError,"Access denied.\nFile: '%s'\ncurrently locked by user: '%s'.",fn,GetLockOwner(0,fn));
		return (INVALID_HANDLE_VALUE==handle);
	}else return FALSE;
}

BOOL EFS_Utils::LockFile(LPCSTR initial, LPSTR fname, bool bLog)
{
	string256 fn; strcpy(fn,fname);
	if (initial) FS.update_path(fn,initial,fn);

	BOOL bRes=false;
	if (m_LockFiles.find(fn)==m_LockFiles.end()){
		HANDLE handle=CreateFile(fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (INVALID_HANDLE_VALUE!=handle){
			LPSTR lp_fn=fn;
			std::pair<HANDLEPairIt, bool> I=m_LockFiles.insert(std::mk_pair(lp_fn,handle));
			R_ASSERT(I.second);
			RegisterAccess(fn,"Lock",bLog);
			bRes=true;
		}
	}
	return bRes;
}

BOOL EFS_Utils::UnlockFile(LPCSTR initial, LPSTR fname, bool bLog)
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

LPCSTR EFS_Utils::GetLockOwner(LPCSTR initial, LPSTR fname)
{
	string256 fn; strcpy(fn,fname);
	if (initial) FS.update_path(fn,initial,fn);

	CInifile*	ini = CInifile::Create(m_LastAccessFN,true);
	static string256 comp;
	strcpy(comp,ini->line_exist("last_access",fn)?ini->r_string("last_access",fn):"unknown");
	CInifile::Destroy(ini);

	return comp;
}
#endif
