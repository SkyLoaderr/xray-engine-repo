//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "xr_trims.h"
#include "xr_ini.h"
#include "cderr.h"

//----------------------------------------------------

void FSPath::Init( char *_Root, char *_Add, char *_DefExt, char *_FilterString ){
	VERIFY( _Root && _Add );
	strcpy( m_DefExt, _DefExt );
	strcpy( m_FilterString, _FilterString );
	strcpy( m_Root, _Root );
	strcpy( m_Path, _Root );
	strcat( m_Path, _Add );
    strlwr( m_Path );
	VerifyPath();
	ELog.Msg( mtInformation, "FS: path = %s", m_Path );
}

void FSPath::Update( char *_FileName )const{
	VERIFY( _FileName );
	char buffer[MAX_PATH];
	strcpy(buffer,_FileName);
	strcpy(_FileName, m_Path );
	strcat(_FileName, buffer );
}

#ifdef M_BORLAND
void FSPath::Update( AnsiString& _FileName )const{
	AnsiString buf;
    buf.sprintf("%s%s",m_Path,_FileName.c_str());
    _FileName=buf;
}
#endif

void FSPath::VerifyPath(){
	char tmp[MAX_PATH];
	for(int i=0;m_Path[i];i++){
		if( m_Path[i]!='\\' || i==0 )
			continue;
		memcpy( tmp, m_Path, i );
		tmp[i] = 0;
		CreateDirectory( tmp, 0 );
	}
}
//----------------------------------------------------


CFileSystem::CFileSystem( ){
	m_Local[0] = 0;
	m_Server[0] = 0;
    m_FindItems = 0;
    m_AccessLog = 0;
	_tzset();
}

CFileSystem::~CFileSystem(){
	_DELETE(m_AccessLog);
}

void CFileSystem::OnCreate(){
//	VERIFY( _ExeName );
//	_splitpath( _ExeName, m_Root, 0, 0, 0 );
//	_splitpath( _ExeName, 0, m_Root+strlen(m_Root), 0, 0 );
	strcpy(m_Local,"x:\\");
    strcpy(m_Server,"\\\\X-Ray\\stalker$\\");

	m_LocalRoot.Init  	(m_Local, 	"",               		"",     			"" );
	m_ServerRoot.Init  	(m_Server, 	"",               		"",     			"" );
    m_GameLevels.Init	(m_Server, 	"game\\data\\levels\\",	"",     			"" );
    m_GameSounds.Init	(m_Server, 	"game\\data\\sounds\\",	"*.wav",			"Wave (*.wav)" );
	m_GameRoot.Init 	(m_Server, 	"game\\",         		"",     			"" );
	m_GameCForms.Init	(m_Server, 	"game\\data\\cforms\\",	"*.vcf",			"Collision form (*.vcf)" );
	m_GameMeshes.Init	(m_Server, 	"game\\data\\meshes\\",	"*.ogf",			"Render model (*.ogf)" );
    m_GameDO.Init		(m_Server,  "game\\data\\meshes\\",	"*.do",				"Detail object (*.do)" );
    m_GameTextures.Init	(m_Server,	"game\\data\\textures\\","*.dds",			"Textures (*.dds)" );
	m_GameKeys.Init		(m_Server,  "game\\data\\meshes\\",	"*.key",			"XRay model keys (*.key)" );

	m_Groups.Init   	(m_Server, 	"objects\\",       		"*.group", 			"Groups (*.group)" );
    m_Objects.Init  	(m_Server, 	"objects\\",       		"*.object;*.lwo",	"Editor objects (*.object,*.lwo)" );
	m_Import.Init  		(m_Local, 	"import\\",       		"*.object;*.lwo", 	"Import objects (*.object,*.lwo)" );
	m_OMotion.Init		(m_Local, 	"import\\", 		   	"*.anm",			"Object animation (*.anm)" );
	m_OMotions.Init		(m_Local, 	"import\\", 		    "*.anms",	    	"Object animation list (*.anms)" );
	m_SMotion.Init		(m_Local, 	"import\\", 		    "*.skl",			"Skeleton motion file (*.skl)" );
	m_SMotions.Init		(m_Local, 	"import\\", 		    "*.skls",			"Skeleton motions file (*.skls)" );
	m_Maps.Init     	(m_Server, 	"maps\\",         		"*.level",  		"Levels (*.level)" );
	m_Textures.Init 	(m_Server, 	"textures\\",     		"*.bmp;*.tga",		"Textures (*.bmp;*.tga)" );
	m_Temp.Init     	(m_Local, 	"temp\\",         		"",     			"" );

    strcpy				(m_LastAccessFN,"access.ini"); 	m_ServerRoot.Update(m_LastAccessFN);
    string256 fn; strcpy(fn,"access.log"); m_ServerRoot.Update(fn);
    m_AccessLog			= new CLog();
	m_AccessLog->Create	(fn,true);
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

bool CFileSystem::GetOpenName( FSPath& initial, char *buffer, int sz_buf, bool bMulti, LPCSTR offset ){
	VERIFY(buffer&&(sz_buf>0));
	char flt[1024];ZeroMemory(flt,sizeof(char)*1024);
	strcpy(flt,initial.m_FilterString);
	strcpy(flt+strlen(flt)+1,initial.m_DefExt);

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
    ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner 		= GetForegroundWindow();
	ofn.lpstrDefExt 	= initial.m_DefExt;
	ofn.lpstrFile 		= buffer;
	ofn.nMaxFile 		= sz_buf;
	ofn.lpstrFilter 	= flt;
	ofn.nFilterIndex 	= 1;
    ofn.lpstrTitle      = "Open a File";

    string512 path; strcpy(path,(offset&&offset[0])?offset:initial.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags =
    	OFN_PATHMUSTEXIST|
    	OFN_FILEMUSTEXIST|
		OFN_HIDEREADONLY|
		OFN_FILEMUSTEXIST|
		OFN_NOCHANGEDIR|(bMulti?OFN_ALLOWMULTISELECT|OFN_EXPLORER:0);
	bool bRes = !!GetOpenFileName( &ofn );
    if (!bRes){
	    DWORD err = CommDlgExtendedError();
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

bool CFileSystem::GetSaveName( FSPath& initial, char *buffer, int sz_buf, LPCSTR offset ){
	VERIFY(buffer&&(sz_buf>0));
	char flt[1024];ZeroMemory(flt,sizeof(char)*1024);
	strcpy(flt,initial.m_FilterString);
	strcpy(flt+strlen(flt)+1,initial.m_DefExt);

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.hwndOwner 		= GetForegroundWindow();
	ofn.lpstrDefExt 	= initial.m_DefExt;
	ofn.lpstrFile 		= buffer;
	ofn.lpstrFilter 	= flt;
	ofn.lStructSize 	= sizeof(ofn);
	ofn.nMaxFile 		= sz_buf;
	ofn.nFilterIndex 	= 1;
    ofn.lpstrTitle      = "Save a File";
    string512 path; strcpy(path,(offset&&offset[0])?offset:initial.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags 			= OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;

	bool bRes = !!GetSaveFileName( &ofn );
    if (!bRes){
	    DWORD err = CommDlgExtendedError();
	    switch(err){
        case FNERR_BUFFERTOOSMALL: 	ELog.DlgMsg(mtError,"Too many file selected."); break;
        }
	}
	return bRes;
}
//----------------------------------------------------
#ifdef M_BORLAND
bool CFileSystem::GetOpenName(FSPath& initial, AnsiString& buffer, bool bMulti, LPCSTR offset ){
	string4096 buf;
    strcpy(buf,buffer.c_str());
	bool bRes = GetOpenName(initial,buf,sizeof(buf),bMulti,offset);
    if (bRes) buffer=buf;
	return bRes;
}

bool CFileSystem::GetSaveName( FSPath& initial, AnsiString& buffer, LPCSTR offset ){
	string4096 buf;
    strcpy(buf,buffer.c_str());
	bool bRes = GetSaveName(initial,buf,sizeof(buf),offset);
    if (bRes) buffer=buf;
	return bRes;
}
#endif
//----------------------------------------------------

bool CFileSystem::Exist(LPCSTR _FileName, bool bMessage){
    bool bRes = FileExists(_FileName);
    if (bMessage&&!bRes){
        ELog.Msg(mtError,"Can't find required file: '%s'",_FileName);
        return false;
    };
    return bRes;
}
//----------------------------------------------------

bool CFileSystem::Exist(char* fn, const char* path, const char* name, bool bMessage){
	strcpy(fn,path);
    strcat(fn,name);
	return Exist(fn,bMessage);
}
//----------------------------------------------------

bool CFileSystem::Exist(char* fn, const char* path, const char* name, const char* ext, bool bMessage){
	strcpy(fn,path);
    strcat(fn,name);
    strcat(fn,ext);
	return Exist(fn,bMessage);
}
//----------------------------------------------------


bool CFileSystem::Exist( FSPath *initial, const char *_FileName, bool bMessage){
	string256 fn;
    strcpy(fn, _FileName);
    initial->Update(fn);
	return Exist(fn,bMessage);
}
//----------------------------------------------------

void CFileSystem::DeleteFileByName(const char* nm){
	DeleteFile(nm);
}
//----------------------------------------------------

void CFileSystem::CopyFileTo(LPCSTR src, LPCSTR dest, bool bOverwrite){
	CopyFile(src,dest,!bOverwrite);
}
//----------------------------------------------------

//----------------------------------------------------
// Age routines
//----------------------------------------------------

int CFileSystem::GetFileAge( const AnsiString& name ){
    _finddata_t		sFile;
    int	hFile		= _findfirst(name.c_str(), &sFile);
	if (hFile<=0) 	return -1;
    return			sFile.time_write;
}
//----------------------------------------------------
void CFileSystem::SetFileAge( const AnsiString& name, int age ){
    utimbuf 	tm;
    tm.actime	= age;
    tm.modtime	= age;
    _utime(name.c_str(),&tm);
}
//----------------------------------------------------
//
//----------------------------------------------------
int	CFileSystem::FileLength(LPCSTR fn){
	if (Exist(fn)){
        int fh = open(fn, _O_RDWR);
        int sz = filelength(fh);
		close(fh);
        return sz;
    }
    return 0;
}
//----------------------------------------------------
#ifdef M_BORLAND
void CFileSystem::MarkFile(const AnsiString& fn){
	AnsiString ext = ExtractFileExt(fn);
    ext.Insert("~",2);
	AnsiString backup_fn = ChangeFileExt(fn,ext);
	DeleteFile(backup_fn);
	RenameFile(fn,backup_fn);
}

void CFileSystem::BackupFile(const AnsiString& fn){
	AnsiString ext = ExtractFileExt(fn);
    ext.Insert("~",2);
	AnsiString backup_fn = ChangeFileExt(fn,ext);
	CopyFileTo(fn.c_str(),backup_fn.c_str(),true);
}

bool CFileSystem::RestoreBackup(const AnsiString& fn){
	AnsiString ext = ExtractFileExt(fn);
    ext.Insert("~",2);
	AnsiString backup_fn = ChangeFileExt(fn,ext);
    if (Exist(backup_fn.c_str(),false)){
		CopyFileTo(backup_fn.c_str(),fn.c_str(),true);
		return true;
    }
    return false;
}

bool CFileSystem::CreateNullFile(const char* fn){
    CFS_Memory F;
	F.SaveTo(fn,0);
    return true;
}
#endif

void CFileSystem::ProcessOne(_finddata_t& F, const char* path, bool bOnlyDir)
{
	string256	N;
	strcpy		(N,path);
	strcat		(N,F.name);

	if (F.attrib&_A_SUBDIR) {
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat(N,"\\");
		Recurse(N);
	} else {
    	if (bOnlyDir) return;
    	if (bClampExt) if (strext(N)) *strext(N)=0;
		m_FindItems->insert(std::make_pair(strlwr(N+path_size),F.time_write));
	}
}

void CFileSystem::Recurse(const char* path)
{
    _finddata_t		sFile;
    int				hFile;

	string256		N;
	string256		dst;

    strcpy			(N,path);
    strcat			(N,"*.*");
    R_ASSERT		((hFile=_findfirst(N, &sFile)) != -1);
    ProcessOne		(sFile,path,true);
    while			( _findnext( hFile, &sFile ) == 0 )
        ProcessOne(sFile,path,true);
    _findclose		( hFile );

    int cnt			= _GetItemCount(ext_mask,',');
    for (int i=0; i<cnt; i++){
	    strcpy		(N,path);
    	strcat		(N,_GetItem(ext_mask,i,dst,','));
    	if ((hFile=_findfirst(N, &sFile))==-1) continue;
        ProcessOne(sFile,path,false);
	    while		( _findnext( hFile, &sFile ) == 0 )
	        ProcessOne(sFile,path,false);
	    _findclose	( hFile );
	}

/*
    strcpy			(N,path);
    strcat			(N,"*.*");

    R_ASSERT		((hFile=_findfirst(N, &sFile)) != -1);
    ProcessOne		(sFile,path);

    while			( _findnext( hFile, &sFile ) == 0 )
        ProcessOne(sFile,path);

    _findclose		( hFile );
*/
}

int CFileSystem::GetFiles(LPCSTR path, FileMap& items, bool clamp_path, bool clamp_ext, LPCSTR ext_m)
{
    ext_mask		= strdup(ext_m);
	m_FindItems		= &items;
    bClampExt		= clamp_ext;
    path_size		= clamp_path?strlen(path):0;
	Recurse			(path);
    _FREE			(ext_mask);
    return m_FindItems->size();
}

void CFileSystem::VerifyPath(LPCSTR path)
{
	char tmp[MAX_PATH];
	for(int i=0;path[i];i++){
		if( path[i]!='\\' || i==0 )
			continue;
		memcpy( tmp, path, i );
		tmp[i] = 0;
		CreateDirectory( tmp, 0 );
	}
}

LPSTR CFileSystem::UpdateTextureNameWithFolder(LPSTR tex_name)
{
    if (_GetItemCount(tex_name,'_')>1){
        string256 _fn;
        string256 fld;
        _GetItem(tex_name,0,fld,'_');
        sprintf(_fn,"%s\\%s",fld,tex_name);
		strcpy(tex_name,_fn);
    }
	return tex_name;
}

void CFileSystem::GetCompAndUser(string64& computer, string64& user)
{
	// names
	DWORD		comp_sz = sizeof(computer);
	GetComputerName(computer,&comp_sz);
	DWORD		user_sz = sizeof(user);
	GetUserName	(user,&user_sz);
}

void CFileSystem::RegisterAccess(LPSTR fn)
{
	string64 computer;
    string64 user;
	GetCompAndUser(computer, user);

    CInifile*	ini = CInifile::Create(m_LastAccessFN,false);
	ini->WriteString("last_access",fn,computer);
    CInifile::Destroy(ini);

	string128 dt_buf, tm_buf;
	m_AccessLog->Msg(mtInformation,"Lock:   '%s' from computer: '%s' by user: '%s' at %s %s",fn,computer,user,_strdate(dt_buf),_strtime(tm_buf));
}

BOOL CFileSystem::CheckLocking(FSPath *initial, LPSTR fname, bool bOnlySelf, bool bMsg)
{
	string256 fn; strcpy(fn,fname);
	if (initial) initial->Update(fn);

	if (bOnlySelf) return (m_LockFiles.find(fn)!=m_LockFiles.end());
	HANDLE handle=CreateFile(fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    CloseHandle(handle);
    if (bMsg&&(INVALID_HANDLE_VALUE==handle))
		ELog.DlgMsg(mtError,"Access denied.\nFile: '%s'\ncurrently locked by user: '%s'.",fn,GetLockOwner(0,fn));
    return (INVALID_HANDLE_VALUE==handle);
}

BOOL CFileSystem::LockFile(FSPath *initial, LPSTR fname, bool bLog)
{
	string256 fn; strcpy(fn,fname);
	if (initial) initial->Update(fn);

    BOOL bRes=false;
	if (m_LockFiles.find(fn)==m_LockFiles.end()){
		HANDLE handle=CreateFile(fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    	if (INVALID_HANDLE_VALUE!=handle){
        	LPSTR lp_fn=fn;
			std::pair<HANDLEPairIt, bool> I=m_LockFiles.insert(std::make_pair(lp_fn,handle));
            R_ASSERT(I.second);
            if (bLog) RegisterAccess(fname);
            bRes=true;
        }
    }
    return bRes;
}

BOOL CFileSystem::UnlockFile(FSPath *initial, LPSTR fname, bool bLog)
{
	string256 fn; strcpy(fn,fname);
	if (initial) initial->Update(fn);

	HANDLEPairIt it = m_LockFiles.find(fn);
	if (it!=m_LockFiles.end()){
		m_LockFiles.erase(it);
        if (bLog){
			string64 computer; string64 user;
			GetCompAndUser(computer, user);
			string128 dt_buf, tm_buf;
    		m_AccessLog->Msg(mtInformation,"Unlock: '%s' from computer: '%s' by user: '%s' at %s %s",fn,computer,user,_strdate(dt_buf),_strtime(tm_buf));
        }
    	return CloseHandle(it->second);
    }
    return false;
}

LPCSTR CFileSystem::GetLockOwner(FSPath *initial, LPSTR fname)
{
	string256 fn; strcpy(fn,fname);
	if (initial) initial->Update(fn);

    CInifile*	ini = CInifile::Create(m_LastAccessFN,true);
	static string256 comp;
    strcpy(comp,ini->ReadSTRING("last_access",fn));
    CInifile::Destroy(ini);

	return comp;
}

CStream* CFileSystem::Open(LPCSTR fn)
{
	if (!Exist(fn)) return 0;
	return new CFileStream(fn);	
}

void CFileSystem::Close(CStream*& F)
{
	_DELETE(F);
}
