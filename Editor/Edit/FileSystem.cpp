//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "CustomObject.h"
#include <io.h>

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
	hFindHandle	= -1;
    m_FindItems = 0;
}

CFileSystem::~CFileSystem(){
	if (hFindHandle!=-1) _findclose(hFindHandle);
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
BOOL CFileSystem::GetFileAge( const AnsiString& name, int FT ){
	int hfile	= _open(name.c_str(),O_BINARY|O_RDONLY);
	if	(hfile<=0) return FALSE;
	int handle	= _get_osfhandle(hfile);
//	BOOL res	= GetFileTime(HANDLE(handle),0,0,&FT);
	_close		(hfile);
//	return		res;
}
//----------------------------------------------------
void CFileSystem::SetFileAge( const AnsiString& name, int FT ){
	int hfile	= _open(name.c_str(),O_BINARY|O_RDWR); R_ASSERT(hfile>0);
	int handle	= _get_osfhandle(hfile);
//	BOOL res	= SetFileTime(HANDLE(handle),0,0,&FT); R_ASSERT(res);
	_close		(hfile);
}
//----------------------------------------------------
void CFileSystem::SetFileAgeFrom(const AnsiString& src_name, const AnsiString& dest_name){
	FILETIME FT;
//	BOOL res = GetFileAge(src_name,FT); R_ASSERT(res);
//	SetFileAge(dest_name,FT);
}
//----------------------------------------------------
int CFileSystem::CompareFileAge(const AnsiString& fn1, const AnsiString& fn2){
	int FT1, FT2;
	BOOL fn1_res = GetFileAge(fn1,FT1);
	BOOL fn2_res = GetFileAge(fn2,FT2);
	if (!fn1_res||!fn2_res) return -1;
//	return (0==CompareFileTime(&FT1,&FT2));
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

int CFileSystem::RestoreBackup(const AnsiString& fn){
	AnsiString ext = ExtractFileExt(fn);
    ext.Insert("~",2);
	AnsiString backup_fn = ChangeFileExt(fn,ext);
    if (Exist(backup_fn.c_str(),false)){
    	if (CompareFileAge(backup_fn,fn)==0){
			CopyFileTo(backup_fn.c_str(),fn.c_str(),true);
            return 1;
        }else return -1;
    }else return 0;
}

bool CFileSystem::CreateNullFile(const char* fn){
    CFS_Memory F;
	F.SaveTo(fn,0);
    return true;
}

LPCSTR CFileSystem::FindFirst(LPSTR mask){
	if (hFindHandle!=-1) _findclose(hFindHandle);
	hFindHandle = _findfirst(mask, &FData);
    if (hFindHandle!=-1) return FData.name;
    return 0;
}

LPCSTR CFileSystem::FindNext(){
	VERIFY(hFindHandle!=-1);
    if (_findnext(hFindHandle, &FData)!=-1) return FData.name;
    return 0;
}

void CFileSystem::ProcessOne(_finddata_t& F, const char* path)
{
	FILE_NAME	N;
	strcpy		(N,path);
	strcat		(N,F.name);

    FILETIME 	FT;
	int hfile	= _open(F.name,O_BINARY|O_RDONLY);
	int handle	= _get_osfhandle(hfile);
	BOOL res	= GetFileTime(HANDLE(handle),0,0,&FT);

	if (F.attrib&_A_SUBDIR) {
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat(N,"\\");
        if (!bFiles) m_FindItems->insert(make_pair(strlwr(N),F.time_write));
		Recurse(N);
	} else {
		if (bFiles) m_FindItems->insert(make_pair(strlwr(N),F.time_write));
	}
}

void CFileSystem::Recurse(const char* path)
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

int CFileSystem::GetFiles(LPCSTR path, FindDataMap& items)
{
	bFiles			= true;
	m_FindItems		= &items;
	Recurse			(path);
    return m_FindItems->size();
}

int CFileSystem::GetDirectories(LPCSTR path, FindDataMap& items)
{
	bFiles			= false;
	m_FindItems		= &items;
	Recurse			(path);
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

