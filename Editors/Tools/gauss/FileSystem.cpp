//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "cderr.h"

EFS_Utils EFS;
//----------------------------------------------------
EFS_Utils::EFS_Utils( )
{
}

EFS_Utils::~EFS_Utils()
{
}

void EFS_Utils::OnCreate()
{
    FS.update_path			(m_LastAccessFN,_server_data_root_,"access.ini");
    FS.update_path			(m_AccessLog,	_server_data_root_,"access.log");
}                                               
//----------------------------------------------------
#ifdef _MSC_VER
#define utimbuf _utimbuf
LPCSTR ExtractFileExt(const string& fn){
	return strext(fn.c_str());
}
bool FileExists(LPCSTR fn){
	int handle	= open(fn, O_RDONLY);
    return handle>-1;
}
#endif

LPCSTR MakeFilter(string1024& dest, LPCSTR info, LPCSTR ext)
{
	ZeroMemory(dest,sizeof(dest));
    if (ext){
        int icnt=_GetItemCount(ext,';');
		LPSTR dst=dest;
        if (icnt>1){
            strconcat(dst,info," (",ext,")");
            dst+=(strlen(dst)+1);
            strcpy(dst,ext);
            dst+=(strlen(ext)+1);
        }
        for (int i=0; i<icnt; i++){
            string64 buf;
            _GetItem(ext,i,buf,';');
            strconcat(dst,info," (",buf,")");
            dst+=(strlen(dst)+1);
            strcpy(dst,buf);
            dst+=(strlen(buf)+1);
        }
    }
	return dest;
}

//------------------------------------------------------------------------------
// start_flt_ext = -1-all 0..n-indices
//------------------------------------------------------------------------------
bool EFS_Utils::GetOpenName( LPCSTR initial, char *buffer, int sz_buf, bool bMulti, LPCSTR offset, int start_flt_ext )
{
	VERIFY(buffer&&(sz_buf>0));
	FS_Path& P			= *FS.get_path(initial);
	string1024 flt;
	MakeFilter(flt,P.m_FilterCaption,P.m_DefExt);

	OPENFILENAME ofn;
	Memory.mem_fill		( &ofn, 0, sizeof(ofn) );
    ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner 		= GetForegroundWindow();
	ofn.lpstrDefExt 	= P.m_DefExt;
	ofn.lpstrFile 		= buffer;
	ofn.nMaxFile 		= sz_buf;
	ofn.lpstrFilter 	= flt;
	ofn.nFilterIndex 	= start_flt_ext;
    ofn.lpstrTitle      = "Open a File";

    string512 path; strcpy(path,(offset&&offset[0])?offset:P.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags =
    	OFN_PATHMUSTEXIST|
    	OFN_FILEMUSTEXIST|
		OFN_HIDEREADONLY|
		OFN_FILEMUSTEXIST|
		OFN_NOCHANGEDIR|(bMulti?OFN_ALLOWMULTISELECT|OFN_EXPLORER:0);
	bool bRes = !!GetOpenFileName( &ofn );
    if (!bRes){
	    u32 err = CommDlgExtendedError();
	    switch(err){
        case FNERR_BUFFERTOOSMALL: 	ELog.DlgMsg(mtError,"Too many file selected."); break;
        }
	}
    if (bRes&&bMulti){
		int cnt		= _GetItemCount(buffer,0x0);
        if (cnt>1){
            string64  	buf;
            string64  	dir;
            string4096 	fns;
            strcpy(dir, buffer);

            strcpy		(fns,dir);
            strcat		(fns,"\\");
            strcat		(fns,_GetItem(buffer,1,buf,0x0));
            for (int i=2; i<cnt; i++){
                strcat	(fns,",");
                strcat	(fns,dir);
                strcat	(fns,"\\");
                strcat	(fns,_GetItem(buffer,i,buf,0x0));
            }
            strcpy		(buffer,fns);
        }
    }
    return bRes;
}

bool EFS_Utils::GetSaveName( LPCSTR initial, char *buffer, int sz_buf, LPCSTR offset, int start_flt_ext )
{
	VERIFY(buffer&&(sz_buf>0));
	FS_Path& P			= *FS.get_path(initial);
	string1024 flt;
	MakeFilter(flt,P.m_FilterCaption,P.m_DefExt);
	OPENFILENAME ofn;
	Memory.mem_fill		( &ofn, 0, sizeof(ofn) );
	ofn.hwndOwner 		= GetForegroundWindow();
	ofn.lpstrDefExt 	= P.m_DefExt;
	ofn.lpstrFile 		= buffer;
	ofn.lpstrFilter 	= flt;
	ofn.lStructSize 	= sizeof(ofn);
	ofn.nMaxFile 		= sz_buf;
	ofn.nFilterIndex 	= start_flt_ext+2;
    ofn.lpstrTitle      = "Save a File";
    string512 path; strcpy(path,(offset&&offset[0])?offset:P.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags 			= OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;

	bool bRes = !!GetSaveFileName( &ofn );
    if (!bRes){
	    u32 err = CommDlgExtendedError();
	    switch(err){
        case FNERR_BUFFERTOOSMALL: 	ELog.DlgMsg(mtError,"Too many file selected."); break;
        }
	}
	return bRes;
}
//----------------------------------------------------
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
#endif
//----------------------------------------------------

//----------------------------------------------------
#ifdef M_BORLAND
void EFS_Utils::MarkFile(const AnsiString& fn, bool bDeleteSource)
{
	AnsiString ext = ExtractFileExt(fn);
    ext.Insert("~",2);
	AnsiString backup_fn = ChangeFileExt(fn,ext);
	if (bDeleteSource){
    	FS.file_delete(backup_fn.c_str());
		FS.file_rename(fn.c_str(),backup_fn.c_str());
    }else{
		FS.file_rename(fn.c_str(),backup_fn.c_str());
    }
}

void EFS_Utils::BackupFile(LPCSTR initial, const AnsiString& fname)
{
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
#endif

AnsiString&	EFS_Utils::UpdateTextureNameWithFolder(AnsiString& tex_name)
{
	string1024 nm;
    strcpy(nm,tex_name.c_str());
    tex_name = UpdateTextureNameWithFolder(nm);
    return tex_name;
}

LPSTR EFS_Utils::UpdateTextureNameWithFolder(LPSTR tex_name)
{
	string256 _fn;
	strcpy(tex_name,UpdateTextureNameWithFolder(tex_name, _fn));
	return tex_name;
}

LPSTR EFS_Utils::UpdateTextureNameWithFolder(LPCSTR src_name, LPSTR dest_name)
{
    if (_GetItemCount(src_name,'_')>1){
        string256 fld;
        _GetItem(src_name,0,fld,'_');
        sprintf(dest_name,"%s\\%s",fld,src_name);
    }else{
		strcpy(dest_name,src_name);
    }
	return dest_name;
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
			std::pair<HANDLEPairIt, bool> I=m_LockFiles.insert(std::make_pair(lp_fn,handle));
            R_ASSERT(I.second);
            RegisterAccess(fname,"Lock",bLog);
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

LPCSTR EFS_Utils::GenerateName(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name)
{
    int cnt = 0;
	string256 fn;
    if (base_name)	strconcat	(fn,base_path,base_name,def_ext);
	else 			sprintf		(fn,"%s%02d%s",base_path,cnt++,def_ext);
	while (FS.exist(fn))
	    if (base_name)	sprintf(fn,"%s%s%02d%s",base_path,base_name,cnt++,def_ext);
        else 			sprintf(fn,"%s%02d%s",base_path,cnt++,def_ext);
    strcpy(out_name,fn);
	return out_name;
}

