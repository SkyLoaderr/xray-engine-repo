//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef _INCDEF_FileSystem_H_
#define _INCDEF_FileSystem_H_


class FSPath{
public:
	
	char m_Root[MAX_PATH];
	char m_Path[MAX_PATH];
	char m_DefExt[MAX_PATH];
	char m_FilterString[1024];

public:

	void Init( char *_Root, char *_Add, char *_DefExt, char *_FilterString );
	void Update( char *_FileName );
	void VerifyPath();
};

#pragma pack(push,1)
struct FSChunkDef {
	int filehandle;
	int chunkid;
	long chunkstart;
	long chunksize; };
#pragma pack(pop)

class FileSystem {
public:

	char m_Root[MAX_PATH];
	FSPath m_GameRoot;
	FSPath m_GameData;
	FSPath m_Maps;
	FSPath m_Meshes;
	FSPath m_Pictures;
	FSPath m_LandPics;
	FSPath m_Config;
	FSPath m_Temp;

public:

	FileSystem();
	void Init( char *_ExeName );

	int open( char *_Name );
	int create( char *_Name );
	long read( int handle, void *buffer, long size );
	long write( int handle, void *buffer, long size );
	void seek( int handle, long position );
	bool eof( int handle );
	long pos( int handle );
	void close( int handle );
	long filesize( int handle );

	void __cdecl print( int handle, char *format, ... );

	void initchunk( FSChunkDef *def, int handle );
	bool wopenchunk( FSChunkDef *def, int id );
	void wclosechunk( FSChunkDef *def );
	bool ropenchunk( FSChunkDef *def );
	void rclosechunk( FSChunkDef *def );
	bool rinchunk( FSChunkDef *def );

	bool w2openchunk( FSChunkDef *def, int id );
	void w2closechunk( FSChunkDef *def );

	__forceinline void writeword( int handle, WORD value ){
		this->write( handle, &value, sizeof(value) ); };
	__forceinline int readword( int handle ){
		WORD value; this->read( handle, &value, sizeof(value) );
		return (int)value; };

	__forceinline void writedword( int handle, DWORD value ){
		this->write( handle, &value, sizeof(value) ); };
	__forceinline long readdword( int handle ){
		DWORD value; this->read( handle, &value, sizeof(value) );
		return (long)value; };

	__forceinline void writefloat( int handle, float value ){
		this->write( handle, &value, sizeof(value) ); };
	__forceinline float readfloat( int handle ){
		float value; this->read( handle, &value, sizeof(value) );
		return value; };

	__forceinline void writevector( int handle, float *values ){
		this->write( handle, values, sizeof(float)*3 ); };
	__forceinline void readvector( int handle, float *values ){
		this->read( handle, values, sizeof(float)*3 ); };

	__forceinline void writecolor4( int handle, float *values ){
		this->write( handle, values, sizeof(float)*4 ); };
	__forceinline void readcolor4( int handle, float *values ){
		this->read( handle, values, sizeof(float)*4 ); };

	void readstring( int handle, char *ptr, int maxsize );
	void writestring( int handle, char *ptr );

	bool getopenname( FSPath *initial, char *buffer );
	bool getsavename( FSPath *initial, char *buffer );
};


extern FileSystem FS;


#endif /*_INCDEF_FileSystem_H_*/


