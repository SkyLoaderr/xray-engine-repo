//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "CustomObject.h"
#include "xr_trims.h"
#include <io.h>
#include <utime.h>

CFileSystem FS;
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

void FSPath::Update( AnsiString& _FileName )const{
	AnsiString buf;
    buf.sprintf("%s%s",m_Path,_FileName.c_str());
    _FileName=buf;
}

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
	m_Root[0] = 0;
    m_FindItems = 0;
	_tzset();
}

CFileSystem::~CFileSystem(){
}

void CFileSystem::OnCreate(){
//	VERIFY( _ExeName );
//	_splitpath( _ExeName, m_Root, 0, 0, 0 );
//	_splitpath( _ExeName, 0, m_Root+strlen(m_Root), 0, 0 );
	strcpy(m_Root,"x:\\");
    strcpy(m_Server,"\\\\X-Ray\\stalker$\\");

	m_ExeRoot.Init  	(m_Root, 	"",               		"",     	"" );
    m_GameLevels.Init	(m_Server, 	"game\\data\\levels\\",	"",     	"" );
    m_GameSounds.Init	(m_Server, 	"game\\data\\sounds\\",	"wav",		"Wave (*.wav)|*.wav" );
	m_GameRoot.Init 	(m_Server, 	"game\\",         		"",     	"" );
	m_GameCForms.Init	(m_Server, 	"game\\data\\cforms\\",	"vcf",		"Collision form (*.vcf)|*.vcf" );
	m_GameMeshes.Init	(m_Server, 	"game\\data\\meshes\\",	"ogf",		"Render model (*.ogf)|*.ogf" );
    m_GameDO.Init		(m_Server,  "game\\data\\meshes\\",	"do",		"Detail object (*.do)|*.do" );
    m_GameTextures.Init	(m_Server,	"game\\data\\textures\\","dds",		"Textures (*.dds)|*.dds" );

	m_Groups.Init   	(m_Server, 	"objects\\",       		"mesh", 	"Groups (*.group)|*.group" );
    m_Objects.Init  	(m_Server, 	"objects\\",       		"object",	"Editor objects (*.object,*.lwo)|*.object;*.lwo" );
	m_Import.Init  		(m_Root, 	"import\\",       		"object",	"Import objects (*.object,*.lwo)|*.object;*.lwo" );
	m_OMotion.Init		(m_Root, 	"import\\", 		   	"anm",		"Object animation (*.anim)|*.anim" );
	m_OMotions.Init		(m_Root, 	"import\\", 		    "anms",	    "Object animation list (*.anims)|*.anims" );
	m_SMotion.Init		(m_Root, 	"import\\", 		    "skl",		"Skeleton motion file (*.skl)|*.skl" );
	m_SMotions.Init		(m_Root, 	"import\\", 		    "skls",		"Skeleton motions file (*.skls)|*.skls" );
	m_Maps.Init     	(m_Server, 	"maps\\",         		"lv2",  	"Levels (*.lv2)|*.lv2" );
	m_Textures.Init 	(m_Server, 	"textures\\",     		"bmp",  	"Textures (*.bmp;*.tga)|*.bmp;*.tga" );
	m_Temp.Init     	(m_Root, 	"temp\\",         		"",     	"" );
}

//----------------------------------------------------

bool CFileSystem::GetOpenName( FSPath *initial, char *buffer ){
	VERIFY( initial && buffer );

    bool res;
    AnsiString flt;
    TOpenDialog* od = new TOpenDialog(0);
    flt = initial->m_FilterString;
    od->Filter = flt;
    od->FilterIndex = 1;
    od->InitialDir = initial->m_Path;
    od->DefaultExt = initial->m_DefExt;
    od->FileName = buffer;
    od->Options << ofNoChangeDir << ofFileMustExist	<< ofPathMustExist;
    res = od->Execute();
    if (res) strcpy(buffer, od->FileName.c_str());
    _DELETE(od);
    return res;
}

bool CFileSystem::GetSaveName( FSPath *initial, char *buffer ){
	VERIFY( initial && buffer );

    bool res;
    AnsiString flt;
    TSaveDialog* od = new TSaveDialog(0);
    flt = initial->m_FilterString;
    od->Filter = flt;
    od->FilterIndex = 1;
    od->InitialDir = initial->m_Path;
    od->DefaultExt = initial->m_DefExt;
    od->FileName = buffer;
    od->Options << ofOverwritePrompt;
    res = od->Execute();
    if (res) strcpy(buffer, od->FileName.c_str());
    _DELETE(od);
    return res;
}
//----------------------------------------------------

bool CFileSystem::GetOpenName( FSPath *initial, AnsiString& buffer ){
	VERIFY( initial );
    bool res;
    AnsiString flt;
    TOpenDialog* od = new TOpenDialog(0);
    flt = initial->m_FilterString;
    od->Filter = flt;
    od->FilterIndex = 1;
    od->InitialDir = initial->m_Path;
    od->DefaultExt = initial->m_DefExt;
    od->FileName = buffer;
    od->Options << ofNoChangeDir << ofFileMustExist	<< ofPathMustExist;
    res = od->Execute();
    if (res) buffer = od->FileName;
    _DELETE(od);
    return res;
}

bool CFileSystem::GetSaveName( FSPath *initial, AnsiString& buffer ){
	VERIFY( initial );

    bool res;
    AnsiString flt;
    TSaveDialog* od = new TSaveDialog(0);
    flt = initial->m_FilterString;
    od->Filter = flt;
    od->FilterIndex = 1;
    od->InitialDir = initial->m_Path;
    od->DefaultExt = initial->m_DefExt;
    od->FileName = buffer;
    od->Options << ofOverwritePrompt;
    res = od->Execute();
    if (res) buffer = od->FileName;
    _DELETE(od);
    return res;
}

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
	AnsiString fn;
    fn = _FileName;
    initial->Update(fn);
	return Exist(fn.c_str(),bMessage);
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

void CFileSystem::ProcessOne(_finddata_t& F, const char* path, bool bOnlyDir)
{
	FILE_NAME	N;
	FILE_NAME	M;
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
		m_FindItems->insert(make_pair(strlwr(N+path_size),F.time_write));
	}
}

void CFileSystem::Recurse(const char* path)
{
    _finddata_t		sFile;
    int				hFile;

	FILE_NAME		N;
	FILE_NAME		dst;

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

