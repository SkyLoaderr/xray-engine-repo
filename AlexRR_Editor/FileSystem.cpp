//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "FileSystem.h"


//----------------------------------------------------

FileSystem FS;

//----------------------------------------------------

void FSPath::Init( char *_Root, char *_Add, char *_DefExt, char *_FilterString ){
	_ASSERTE( _Root && _Add );
	strcpy( m_DefExt, _DefExt );
	strcpy( m_FilterString, _FilterString );
	strcpy( m_Root, _Root );
	strcpy( m_Path, _Root );
	strcat( m_Path, _Add );
	VerifyPath();
	NConsole.print( "FS: path = %s", m_Path );
}

void FSPath::Update( char *_FileName ){
	_ASSERTE( _FileName );
	char buffer[MAX_PATH];
	strcpy(buffer,_FileName);
	strcpy(_FileName, m_Path );
	strcat(_FileName, buffer );
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


FileSystem::FileSystem(){
	m_Root[0] = 0;
}

void FileSystem::Init( char *_ExeName ){
	
	_ASSERTE( _ExeName );
	_splitpath( _ExeName, m_Root, 0, 0, 0 );
	_splitpath( _ExeName, 0, m_Root+strlen(m_Root), 0, 0 );

	m_GameData.Init( m_Root, "game\\data\\",          "",     "" );
	m_GameRoot.Init( m_Root, "game\\",                "",     "" );
	m_Meshes.Init( m_Root,   "meshes\\",              "",     "" );
	m_Maps.Init( m_Root,     "maps\\",                "lv2",  "Levels" );
	m_Pictures.Init( m_Root, "pictures\\",            "bmp",  "Bitmaps" );
	m_Config.Init( m_Root,   "config\\",              "",     "" );
	m_LandPics.Init( m_Root, "pictures\\landscape\\", "bmp",  "Bitmaps" );
	m_Temp.Init( m_Root,     "temp\\",                "",     "" );
}

//----------------------------------------------------

int FileSystem::open( char *_Name ){
	_ASSERTE( _Name );
	int handle = _open( _Name,
		_O_BINARY|_O_RDONLY);
	_ASSERTE( handle > 0 );
	return handle;
}

int FileSystem::create( char *_Name ){
	_ASSERTE( _Name );
	int handle = _open( _Name, 
		_O_BINARY|_O_RDWR|_O_CREAT|_O_TRUNC,
		_S_IREAD|_S_IWRITE);
	_ASSERTE( handle > 0 );
	return handle;
}

long FileSystem::read( int handle, void *buffer, long size ){
	return _read( handle, buffer, size );
}

long FileSystem::write( int handle, void *buffer, long size ){
	return _write( handle, buffer, size );
}

void FileSystem::seek( int handle, long position ){
	_lseek( handle, position, SEEK_SET );
}

long FileSystem::pos( int handle ){
	return _lseek( handle, 0, SEEK_CUR );
}

bool FileSystem::eof( int handle ){
	return !!_eof( handle );
}

void FileSystem::close( int handle ){
	_close( handle );
}

long FileSystem::filesize( int handle ){
	return _filelength( handle );
}

//----------------------------------------------------

void __cdecl FileSystem::print( int handle, char *format, ... ){

	char buf[4096];
	va_list l; 

	va_start( l, format );
	vsprintf( buf, format, l );

	if( buf[0] )
		write( handle, buf, strlen( buf ) );
}

//----------------------------------------------------

void FileSystem::initchunk( FSChunkDef *def, int handle ){
	memset( def, 0, sizeof(FSChunkDef) );
	def->filehandle = handle;
}

bool FileSystem::ropenchunk( FSChunkDef *def ){
	def->chunkstart = pos( def->filehandle );
	def->chunkid = readword( def->filehandle );
	def->chunksize = readdword( def->filehandle );
	return true;
}

void FileSystem::rclosechunk( FSChunkDef *def ){
	long fullsize = 6 + def->chunksize;
	seek( def->filehandle, def->chunkstart + fullsize );
}

bool FileSystem::rinchunk( FSChunkDef *def ){
	long _low = def->chunkstart+6;
	long _high = _low + def->chunksize;
	long _pos = pos( def->filehandle );
	return ( (_pos>=_low ) && (_pos<_high) );
}

bool FileSystem::wopenchunk( FSChunkDef *def, int id ){
	def->chunkstart = pos( def->filehandle );
	def->chunkid = id;
	def->chunksize = 0;
	writeword( def->filehandle, id );
	writedword( def->filehandle, 0 );
	return true;
}

void FileSystem::wclosechunk( FSChunkDef *def ){
	long savepos = pos( def->filehandle );
	def->chunksize = savepos - def->chunkstart - 6;
	seek( def->filehandle, def->chunkstart + 2 );
	writedword( def->filehandle, def->chunksize );
	seek( def->filehandle, savepos );
}

bool FileSystem::w2openchunk( FSChunkDef *def, int id ){
	def->chunkstart = pos( def->filehandle );
	def->chunkid = id;
	def->chunksize = 0;
	writedword( def->filehandle, id );
	writedword( def->filehandle, 0 );
	return true;
}

void FileSystem::w2closechunk( FSChunkDef *def ){
	long savepos = pos( def->filehandle );
	def->chunksize = savepos - def->chunkstart - 8;
	seek( def->filehandle, def->chunkstart + 4 );
	writedword( def->filehandle, def->chunksize );
	seek( def->filehandle, savepos );
}

//----------------------------------------------------

bool FileSystem::getopenname( FSPath *initial, char *buffer ){
	_ASSERTE( initial && buffer );

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.hwndOwner = 0;
	ofn.lpstrDefExt = initial->m_DefExt;
	ofn.lpstrFile = buffer;
	ofn.lpstrFilter = initial->m_FilterString;
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

bool FileSystem::getsavename( FSPath *initial, char *buffer ){

	_ASSERTE( initial && buffer );

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.hwndOwner = 0;
	ofn.lpstrDefExt = initial->m_DefExt;
	ofn.lpstrFile = buffer;
	ofn.lpstrFilter = initial->m_FilterString;
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

//----------------------------------------------------

void FileSystem::readstring( int handle, char *ptr, int maxsize ){

	_ASSERTE( maxsize <= 4096 );

	char b[4096];
	long p = this->pos( handle );
	this->read( handle, b, 4096 );
	int l = strlen(b) + 1;
	_ASSERTE( l<maxsize );
	memcpy( ptr, b, l );
	this->seek( handle, p+l );
}

void FileSystem::writestring( int handle, char *ptr ){
	this->write( handle, ptr, strlen(ptr)+1 );
}

//----------------------------------------------------