//----------------------------------------------------
// file: Library.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "Library.h"
#include "SObject2.h"
#include "FileSystem.h"
#include "SceneClassList.h"
#include "Scene.h"

//----------------------------------------------------
ELibrary Lib;
//----------------------------------------------------
#define CURRENT_LIBRARY_VERSION 0x4c494201
#define CHUNK_VERSION       0x9df3
#define CHUNK_OBJECT        0x7702
#define CHUNK_OBJECT_CLASS  0x7703
#define CHUNK_OBJECT_BODY   0x7704
//----------------------------------------------------

SceneObject *NewLibObjectFromClassID( int _ClassID ){
	switch( _ClassID ){
//		case OBJCLASS_DUMMY: return new SceneObject();
		case OBJCLASS_SOBJECT2: return new SObject2(true);
//		case OBJCLASS_LIGHT: return new Light();
//		case OBJCLASS_SOUND: return new CSound();
//		case OBJCLASS_PCLIPPER: return new PClipper();
        default: throw -1;
	}
}
//----------------------------------------------------
void ELibrary::ReloadLibrary(){
    VERIFY(!Scene.ObjCount());
    Clear();
    Init();
}
//----------------------------------------------------
bool ELibrary::ReadObject( FSChunkDef *chunk ){
	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );
	SceneObject *currentobject = 0;
	while( FS.rinchunk( chunk ) ){
		FS.ropenchunk( &current );
		switch( current.chunkid ){
		case CHUNK_OBJECT_CLASS:
			if( currentobject==0 )
				currentobject = NewLibObjectFromClassID( FS.readdword(chunk->filehandle) );
			break;
		case CHUNK_OBJECT_BODY:
			if( currentobject )
				currentobject->Load( chunk );
			break;
		default:
			Log.Msg( "ELibrary: undefined chunk 0x%04X", current.chunkid );
			break; }
		FS.rclosechunk( &current );
	}
	if( currentobject ){ m_Objects.push_back( currentobject ); return true; }
	return false;
}

//----------------------------------------------------
bool ELibrary::Load(){
  	char _FileName[MAX_PATH]="library";
    FS.m_Config.Update(_FileName);

	int handle;
	FSChunkDef current;
    DWORD version = 0;

	_ASSERTE( _FileName );
	handle = FS.open( _FileName );

	Log.Msg( "ELibrary: loading %s...", _FileName );

	FS.initchunk( &current, handle );

	while( !FS.eof( handle ) ){
		bool undefined_chunk = false;
		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case CHUNK_VERSION:
			version = FS.readdword( handle );
            if (version!=CURRENT_LIBRARY_VERSION){
    			Log.Msg( "ELibrary: unsuported file version. Level don't load.");
                return false;
            }
            break;

		case CHUNK_OBJECT:
			ReadObject( &current );
			break;

		default:
			undefined_chunk = true;
			break;
		}
		if( undefined_chunk )
			Log.Msg( "ELibrary: undefined chunk 0x%04X", current.chunkid );

		FS.rclosechunk( &current );
	}

	Log.Msg( "ELibrary: %d objects loaded", m_Objects.size() );

	FS.close( handle );
	return true;
}

void ELibrary::Save(){
  	char _FileName[MAX_PATH]="library";
    FS.m_Config.Update(_FileName);

	int handle;
	FSChunkDef ver,object,objectclass,objectbody;

	_ASSERTE( _FileName );
	handle = FS.create( _FileName );

	FS.initchunk( &ver, handle );
	FS.initchunk( &object, handle );
	FS.initchunk( &objectclass, handle );
	FS.initchunk( &objectbody, handle );

	FS.wopenchunk( &ver, CHUNK_VERSION );
	FS.writedword( handle, CURRENT_LIBRARY_VERSION );
	FS.wclosechunk( &ver );

	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++){

		FS.wopenchunk( &object, CHUNK_OBJECT );

		FS.wopenchunk( &objectclass, CHUNK_OBJECT_CLASS );
		FS.writedword( handle, (*_F)->ClassID() );
		FS.wclosechunk( &objectclass );

		FS.wopenchunk( &objectbody, CHUNK_OBJECT_BODY );
		(*_F)->Save( handle );
		FS.wclosechunk( &objectbody );

		FS.wclosechunk( &object );
	}

	FS.close( handle );
}

void ELibrary::Init(){
	m_Current = 0;

    Load( );

	Log.Msg( "Lib: initialized" );
	m_Valid = true;
}

void ELibrary::Clear(){
	m_Valid = false;

	m_Current = 0;

	ObjectIt o = m_Objects.begin();
	for(;o!=m_Objects.end();o++)
		SAFE_DELETE( (*o) );

	m_Objects.clear();

	Log.Msg( "Lib: cleared" );
}

ELibrary::ELibrary(){
	m_Valid = false;
	m_Current = 0;
}

ELibrary::~ELibrary(){
	_ASSERTE( m_Valid == false );
	_ASSERTE( m_Objects.size() == 0 );
}

//----------------------------------------------------

SceneObject *ELibrary::SearchObject( int classfilter, char *name ){

	_ASSERTE( name );

	ObjectIt o = m_Objects.begin();
	for(;o!=m_Objects.end();o++)
		if( (*o)->ClassID()==classfilter &&
			0==stricmp((*o)->GetName(),name) )
				return (*o);

	return 0;
}

