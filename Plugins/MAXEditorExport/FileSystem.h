//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef _INCDEF_FileSystem_H_
#define _INCDEF_FileSystem_H_

#include "FS.h"


// refs
struct st_ObjVer;

class FSPath{
public:

	char m_Root[MAX_PATH];
	char m_Path[MAX_PATH];
	char m_DefExt[MAX_PATH];
	char m_FilterString[1024];

public:

	void Init( char *_Root, char *_Add, char *_DefExt, char *_FilterString );
	void Update( char *_FileName );
#ifndef _MSC_VER
	void Update( AnsiString& _FileName );
#endif
	void VerifyPath();
};

class CFileSystem {
public:
	char 	m_Root[MAX_PATH];
	FSPath 	m_ExeRoot;
	FSPath 	m_GameRoot;
	FSPath 	m_GameSounds;
	FSPath 	m_GameCForms;
	FSPath 	m_GameMeshes;
	FSPath 	m_GameTextures;
	FSPath 	m_GameLevels;
	FSPath	m_GameKeys;
	FSPath 	m_Maps;
	FSPath 	m_Import;
	FSPath 	m_Meshes;
	FSPath 	m_Groups;
	FSPath 	m_OMotion;
	FSPath 	m_SMotion;
	FSPath 	m_OMotions;
	FSPath 	m_SMotions;
	FSPath 	m_Objects;
	FSPath 	m_Textures;
	FSPath 	m_TexturesThumbnail;
	FSPath 	m_Config;
	FSPath 	m_Temp;
public:
			CFileSystem		();
    void 	Init			();

	bool 	GetOpenName		(FSPath *initial, char *buffer);
	bool 	GetSaveName		(FSPath *initial, char *buffer);

    bool 	VerifyFileExists(const char *_FileName, bool bMessage = false);
    bool 	VerifyFileExists(FSPath *initial, const char *_FileName, bool bMessage = false);
    void 	DeleteFileByName(const char* nm);

#ifndef _MSC_VER
    int  	GetFileAge		(AnsiString& name);
    void 	SetFileAge		(AnsiString& name, int age);
#endif

    void 	GetFileVers		(const char* fn, st_ObjVer& v);
};

extern CFileSystem FS;


#endif /*_INCDEF_FileSystem_H_*/

/*
	int open( const char *_Name );
	int create( const char *_Name );
	long read( int handle, void *buffer, long size );
	long write( int handle, void *buffer, long size );
	void seek( int handle, long position );
	int	 fstell( int handle );
	bool eof( int handle );
	long pos( int handle );
	void close( int handle );
	long filesize( int handle );
    
	void initchunk( FSChunkDef *def, int handle );
	bool wopenchunk( FSChunkDef *def, int id );
	void wclosechunk( FSChunkDef *def );
	bool ropenchunk( FSChunkDef *def );
	void rclosechunk( FSChunkDef *def );
	bool rinchunk( FSChunkDef *def );

	bool w2openchunk( FSChunkDef *def, int id );
	void w2closechunk( FSChunkDef *def );

	__inline void writeword( int handle, WORD value ){
		this->write( handle, &value, sizeof(value) ); };
	__inline int readword( int handle ){
		WORD value; this->read( handle, &value, sizeof(value) );
		return (int)value; };

	__inline void writedword( int handle, DWORD value ){
		this->write( handle, &value, sizeof(value) ); };
	__inline long readdword( int handle ){
		DWORD value; this->read( handle, &value, sizeof(value) );
		return (long)value; };

	__inline void writefloat( int handle, float value ){
		this->write( handle, &value, sizeof(value) ); };
	__inline float readfloat( int handle ){
		float value; this->read( handle, &value, sizeof(value) );
		return value; };

	__inline void writebyte( int handle, BYTE value ){
		this->write( handle, &value, sizeof(value) ); };
	__inline BYTE readbyte( int handle ){
		BYTE value; this->read( handle, &value, sizeof(value) );
		return value; };

	__inline void writevector( int handle, float *values ){
		this->write( handle, values, sizeof(float)*3 ); };
	__inline void readvector( int handle, float *values ){
		this->read( handle, values, sizeof(float)*3 ); };

	__inline void writecolor4( int handle, float *values ){
		this->write( handle, values, sizeof(float)*4 ); };
	__inline void readcolor4( int handle, float *values ){
		this->read( handle, values, sizeof(float)*4 ); };

	void readstring( int handle, char *ptr, int maxsize );
	void readstring( int handle, AnsiString& ptr );
	void writestring( int handle, char *ptr );
	void writestring( int handle, AnsiString& ptr );
*/

