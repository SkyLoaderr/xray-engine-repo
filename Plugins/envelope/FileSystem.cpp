//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "FileSystem.h"
//----------------------------------------------------

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
//	NConsole.print("FS: path = %s", m_Path );
}

void FSPath::Update( char *_FileName ){
	VERIFY( _FileName );
	char buffer[MAX_PATH];
	strcpy(buffer,_FileName);
	strcpy(_FileName, m_Path );
	strcat(_FileName, buffer );
}

#ifndef _MSC_VER
void FSPath::Update( AnsiString& _FileName ){
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
	m_Root[0] = 0;
	Init();
}

void CFileSystem::Init(){
//	NConsole.print("Init path.");
//	VERIFY( _ExeName );
//	_splitpath( _ExeName, m_Root, 0, 0, 0 );
//	_splitpath( _ExeName, 0, m_Root+strlen(m_Root), 0, 0 );
	strcpy(m_Root,"x:\\");

	m_ExeRoot.Init  ( m_Root, "",               		"",     	"" );
    m_GameLevels.Init(m_Root, "game\\data\\levels\\",	"",     	"" );
    m_GameSounds.Init(m_Root, "game\\data\\sounds\\",	"wav",		"Wave (*.wav)|*.wav" );
	m_GameRoot.Init ( m_Root, "game\\",         		"",     	"" );
	m_GameCForms.Init(m_Root, "game\\data\\cforms\\",	"vcf",		"Collision form (*.vcf)|*.vcf" );
	m_GameMeshes.Init(m_Root, "game\\data\\meshes\\",	"ogf",		"Render model (*.ogf)|*.ogf" );
    m_GameTextures.Init(m_Root,"game\\data\\textures\\","dds",		"Textures (*.dds)|*.dds" );
	m_GameKeys.Init	(m_Root,  "game\\data\\meshes\\",	"key",		"XRay model keys (*.key)" );

	m_Groups.Init   ( m_Root, "meshes\\",       		"group",	"Groups (*.group)|*.group" );
	m_Meshes.Init   ( m_Root, "meshes\\",       		"mesh", 	"Meshes (*.mesh)|*.mesh" );
    m_Objects.Init  ( m_Root, "meshes\\",       		"object",	"Editor objects (*.object,*.lwo)|*.object;*.lwo" );
	m_Import.Init  	( m_Root, "import\\",       		"object",	"Import objects (*.object,*.lwo)|*.object;*.lwo" );
	m_OMotion.Init	( m_Root, "import\\", 		      	"anm",		"Object animation (*.anim)|*.anim" );
	m_OMotions.Init	( m_Root, "import\\", 		      	"anms",	    "Object animation list (*.anims)|*.anims" );
	m_SMotion.Init	( m_Root, "import\\", 		      	"skl",		"Skeleton motion file (*.skl)|*.skl" );
	m_SMotions.Init	( m_Root, "import\\", 		      	"skls",		"Skeleton motions file (*.skls)|*.skls" );
	m_Maps.Init     ( m_Root, "maps\\",         		"lv2",  	"Levels (*.lv2)|*.lv2" );
	m_Textures.Init ( m_Root, "textures\\",     		"bmp",  	"Textures (*.bmp;*.tga)|*.bmp;*.tga" );
    m_TexturesThumbnail.Init( m_Root, "textures\\thumbnail\\",	"",  		"" );
	m_Config.Init   ( m_Root, "config\\",       		"",     	"" );
	m_Temp.Init     ( m_Root, "temp\\",         		"",     	"" );
}

#ifndef _MSC_VER
//----------------------------------------------------
void CFileSystem::SetFileAge( AnsiString& name, int age ){
 	int iFileHandle;
	iFileHandle = FileOpen(name, fmOpenWrite);
	FileSetDate(iFileHandle,age);
    FileClose(iFileHandle);
}
//----------------------------------------------------
int CFileSystem::GetFileAge( AnsiString& name ){
    return FileAge( name );
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
    od->Options << ofNoChangeDir;
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
    res = od->Execute();
    if (res) strcpy(buffer, od->FileName.c_str());
    _DELETE(od);
    return res;
}
//----------------------------------------------------

bool CFileSystem::VerifyFileExists( const char *_FileName, bool bMessage ){
    bool bRes = FileExists(_FileName);
    if (bMessage&&!bRes){
        Log->Msg(mtError,"Can't find required file: '%s'",_FileName);
        return false;
    };
    return bRes;
}
//----------------------------------------------------

bool CFileSystem::VerifyFileExists( FSPath *initial, const char *_FileName, bool bMessage){
	AnsiString fn;
    fn = _FileName;
    initial->Update(fn);
	return VerifyFileExists(fn.c_str(),bMessage);
}

void CFileSystem::DeleteFileByName(const char* nm){
	DeleteFile(nm);
}

void CFileSystem::GetFileVers(const char* fn, st_ObjVer& v){
    int handle	= open(fn, O_RDONLY);
    v.f_size 	= filelength(handle);
    v.f_age 	= GetFileAge(fn);
    close(handle);
}
#endif

#ifdef _MSC_VER
bool CFileSystem::GetOpenName( FSPath *initial, char *buffer ){
	VERIFY( initial && buffer );

	char flt[1024];ZeroMemory(flt,sizeof(char)*1024);
	char ext[256];
	sprintf(ext,"*.%s",initial->m_DefExt);
	strcpy(flt,initial->m_FilterString);
	strcpy(flt+strlen(flt)+1,ext);

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.hwndOwner = 0;
	ofn.lpstrDefExt = initial->m_DefExt;
	ofn.lpstrFile = buffer;
	ofn.lpstrFilter = flt;
	ofn.lStructSize = sizeof(ofn);
	ofn.nMaxFile = MAX_PATH;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = initial->m_Path;
	ofn.Flags = 
		OFN_HIDEREADONLY|
		OFN_FILEMUSTEXIST|
		OFN_NOCHANGEDIR;
	return !!GetOpenFileName( &ofn );
}

bool CFileSystem::GetSaveName( FSPath *initial, char *buffer ){

	VERIFY( initial && buffer );

	char flt[1024]; ZeroMemory(flt,sizeof(char)*1024);
	char ext[256];
	sprintf(ext,"*.%s",initial->m_DefExt);
	strcpy(flt,initial->m_FilterString);
	strcpy(flt+strlen(flt)+1,ext);

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.hwndOwner = 0;
	ofn.lpstrDefExt = initial->m_DefExt;
	ofn.lpstrFile = buffer;
	ofn.lpstrFilter = flt;
	ofn.lStructSize = sizeof(ofn);
	ofn.nMaxFile = MAX_PATH;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = initial->m_Path;
	ofn.Flags = 
		OFN_HIDEREADONLY|
		OFN_OVERWRITEPROMPT|
		OFN_NOCHANGEDIR;
	return !!GetSaveFileName( &ofn );
}
#endif

