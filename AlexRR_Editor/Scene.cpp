//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "Scene.h"
#include "SceneChunks.h"
#include "SceneClassList.h"
#include "Texture.h"
#include "FileSystem.h"

//----------------------------------------------------

EScene Scene;

//----------------------------------------------------

void EScene::Init(){
	m_LandGrid.set( 2.f, 2.f );
	m_LastAvailObject = 0;
	ResetGlobals();
	InitDefaultText();
	ResetDefaultLight();
	NConsole.print( "Scene: initialized" );
	m_Valid = true;
}

void EScene::Clear(){
	NConsole.print( "Scene: cleared" );
	m_LastAvailObject = 0;
	m_Valid = false;
}

void EScene::AddObject( SceneObject* object ){
	_ASSERTE( object );
	_ASSERTE( m_Valid );
	m_Objects.push_back( object );
}

void EScene::RemoveObject( SceneObject* object ){
	_ASSERTE( object );
	_ASSERTE( m_Valid );
	m_Objects.remove( object );
}

SceneObject *EScene::QuadPick( int x, int z, int classfilter ){
	if( !valid() )
		return 0;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID() == classfilter && (*_F)->Visible() && (*_F)->QuadPick(x,z) )
			return (*_F);
	return 0;
}

int EScene::BoxPickSelect( ICullPlane *planes, int classfilter, bool flag ){
	int count = 0;
	IMatrix identity;
	identity.identity();
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID() == classfilter && (*_F)->Visible() ){
			if( (*_F)->BoxPick( planes, identity ) ){
				(*_F)->Select( flag );
				count++;
			}
		}
	return count;
}

int EScene::SelectObjects( bool flag, int classfilter ){
	int count = 0;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID() == classfilter && (*_F)->Visible() ){
			(*_F)->Select( flag );
			count++;
		}
	return count;
}

int EScene::ObjectCount( int classfilter ){
	if(classfilter<0)
		return m_Objects.size();
	int count = 0;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID() == classfilter )
			count++;
	return count;
}

int EScene::InvertSelection( int classfilter ){
	int count = 0;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID() == classfilter && (*_F)->Visible() ){
			(*_F)->Select( !(*_F)->Selected() );
			count++;
		}
	return count;
}

int EScene::RemoveSelection( int classfilter ){
	int count = 0;
	ObjectList newlist;
	ObjectIt _F = FirstObj();

	if( classfilter >= 0 ){
		for(;_F!=LastObj();_F++){
			if( (*_F)->ClassID() == classfilter ){
				if( !(*_F)->Selected() ){
					newlist.push_back( (*_F) );
				} else {
					count ++;
					SAFE_DELETE( (*_F) );
				}
			} else {
				newlist.push_back( (*_F) );
			}
		}
	} else {
		for(;_F!=LastObj();_F++){
			if( !(*_F)->Selected() ){
				newlist.push_back( (*_F) );
			} else {
				count ++;
				SAFE_DELETE( (*_F) );
			}
		}
	}
	
	ReplaceList( newlist );
	return count;
}

bool EScene::FullRemove( SceneObject *object ){
	RemoveObject( object );
	SAFE_DELETE( object );
	return true;
};

SceneObject *EScene::RTL_Pick(
	IVector& start,
	IVector& direction,
	int classfilter )
{
	if( !valid() )
		return 0;

	IMatrix identity;
	identity.identity();
	
	float nearest_dist = 0;
	SceneObject *nearest_object = 0;

	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID() == classfilter && (*_F)->Visible() ){
			float d0 = 0;
			if( (*_F)->RTL_Pick(&d0,start,direction,identity) ){
				if( nearest_object ){
					if( d0 < nearest_dist ){
						nearest_dist = d0;
						nearest_object = (*_F);
					}
				}
				else{
					nearest_dist = d0;
					nearest_object = (*_F);
				}

			}
		}
			
	return nearest_object;
}

int EScene::SelectionCount( int classfilter, bool testflag ){
	int count = 0;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->ClassID() == classfilter && (*_F)->Visible() ){
			if( (*_F)->Selected() == testflag)
				count++;
		}
	return count;
}

//----------------------------------------------------

EScene::EScene(){
	m_Valid = false;
	m_Locked = 0;
}

EScene::~EScene(){
	_ASSERTE( m_Valid == false );
}

//----------------------------------------------------

void EScene::Render( float _Aspect, IMatrix *_Camera ){
	
	if( !valid() )
		return;

	if( locked() )
		return;

	IMatrix identity;
	identity.identity();

	if( UI.wmode() ){
		UI.d3d()->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_WIREFRAME);
	} else {
		UI.d3d()->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_SOLID);
	}

	SetLights();
	TM.BindNone();

	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->Visible() )
			(*_F)->Render( identity );

	TM.BindNone();
	ClearLights();
}

void EScene::Update( float dT ){

	if( !valid() )
		return;

	if( locked() )
		return;

	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		(*_F)->RTL_Update( dT );
}

void EScene::clearobjects(){
	ObjectIt o = m_Objects.begin();
	for(;o!=m_Objects.end();o++)
		SAFE_DELETE( (*o) );
	m_Objects.clear();
}

//----------------------------------------------------

void EScene::Unload(){
	m_LastAvailObject = 0;
	clearobjects();
}

bool EScene::Load(char *_FileName){

	int handle;
	FSChunkDef current;
	DWORD version = 0;
	DWORD textsize = 0;

	_ASSERTE( _FileName );
	handle = FS.open( _FileName );

	NConsole.print( "EScene: loading %s...", _FileName );

	FS.initchunk( &current, handle );

	while( !FS.eof( handle ) ){
		bool undefined_chunk = false;
		FS.ropenchunk( &current );
		switch( current.chunkid ){
		
		case CHUNK_VERSION:
			version = FS.readdword( handle );
			break;
		
		case CHUNK_GLOBALS:
			ReadGlobals( &current );
			break;

		case CHUNK_DEFLIGHT:
			ReadDefaultLight( &current );
			break;

		case CHUNK_TEXT:
			textsize = FS.readdword( handle );
			FS.read( handle, m_BOPText, textsize );
			m_BOPText[textsize] = 0;
			break;

		case CHUNK_OBJECT:
			ReadObject( &current );
			break;

		default:
			undefined_chunk = true;
			break;
		}
		if( undefined_chunk )
			NConsole.print( "EScene: undefined chunk 0x%04X", current.chunkid );
		
		FS.rclosechunk( &current );
	}

	NConsole.print( "EScene: %d objects loaded", m_Objects.size() );
	
	FS.close( handle );
	return true;
}

void EScene::Save(char *_FileName){

	int handle;
	FSChunkDef ver,globals,cktext,deflight;
	FSChunkDef object,objectclass,objectbody;

	_ASSERTE( _FileName );
	handle = FS.create( _FileName );
	
	FS.initchunk( &ver, handle );
	FS.initchunk( &globals, handle );
	FS.initchunk( &cktext, handle );
	FS.initchunk( &object, handle );
	FS.initchunk( &objectclass, handle );
	FS.initchunk( &objectbody, handle );
	FS.initchunk( &deflight, handle );

	FS.wopenchunk( &ver, CHUNK_VERSION );
	FS.writedword( handle, CURRENT_FILE_VERSION );
	FS.wclosechunk( &ver );

	FS.wopenchunk( &globals, CHUNK_GLOBALS );
	WriteGlobals( &globals );
	FS.wclosechunk( &globals );

	FS.wopenchunk( &deflight, CHUNK_DEFLIGHT );
	WriteDefaultLight( &deflight );
	FS.wclosechunk( &deflight );

	FS.wopenchunk( &cktext, CHUNK_TEXT );
	FS.writedword( handle, strlen(m_BOPText) );
	FS.write( handle, m_BOPText, strlen(m_BOPText) );
	FS.wclosechunk( &cktext );

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

bool EScene::WriteGlobals( FSChunkDef *chunk ){
	
	FS.writedword( chunk->filehandle, CURRENT_GLOBALS_VERSION );

	FS.writestring( chunk->filehandle, m_FNLtx );
	FS.writestring( chunk->filehandle, m_FNLights );
	FS.writestring( chunk->filehandle, m_FNForms );
	FS.writestring( chunk->filehandle, m_FNMeshList );
	FS.writestring( chunk->filehandle, m_FNMap );
	FS.writestring( chunk->filehandle, m_FNVisibility );

	FS.writestring( chunk->filehandle, m_FNLevelPath );
	FS.writestring( chunk->filehandle, m_FNTexPath );

	FS.writestring( chunk->filehandle, m_LevelName );

	FS.writeword( chunk->filehandle, m_BOPVisMap );
	FS.writeword( chunk->filehandle, m_BOPLightMaps );
	FS.writeword( chunk->filehandle, m_BOPCheck );
	FS.writeword( chunk->filehandle, m_BOPCollapseMaps );

	FS.writefloat( chunk->filehandle, m_BOPVisQuad );

	FS.writeword( chunk->filehandle, m_BOPOptimize );
	FS.writefloat( chunk->filehandle, m_BOPConnectDist );
	FS.writefloat( chunk->filehandle, m_BOPAvObjSize );

	FS.writefloat( chunk->filehandle, m_BOPVisCellSize.x );
	FS.writefloat( chunk->filehandle, m_BOPVisCellSize.z );

	FS.writefloat( chunk->filehandle, m_BOPIndentLow.x );
	FS.writefloat( chunk->filehandle, m_BOPIndentLow.z );
	FS.writefloat( chunk->filehandle, m_BOPIndentHigh.x );
	FS.writefloat( chunk->filehandle, m_BOPIndentHigh.z );

	return true;
}

bool EScene::ReadGlobals( FSChunkDef *chunk ){
	
	if( FS.readdword( chunk->filehandle ) != CURRENT_GLOBALS_VERSION ){
		NConsole.print( "Skipping bad version of globals..." );
		return true;
	}

	FS.readstring( chunk->filehandle, m_FNLtx, MAX_PATH );
	FS.readstring( chunk->filehandle, m_FNLights, MAX_PATH );
	FS.readstring( chunk->filehandle, m_FNForms, MAX_PATH );
	FS.readstring( chunk->filehandle, m_FNMeshList, MAX_PATH );
	FS.readstring( chunk->filehandle, m_FNMap, MAX_PATH );
	FS.readstring( chunk->filehandle, m_FNVisibility, MAX_PATH );

	FS.readstring( chunk->filehandle, m_FNLevelPath, MAX_PATH );
	FS.readstring( chunk->filehandle, m_FNTexPath, MAX_PATH );

	FS.readstring( chunk->filehandle, m_LevelName, MAX_PATH );

	m_BOPVisMap = FS.readword( chunk->filehandle );
	m_BOPLightMaps = FS.readword( chunk->filehandle );
	m_BOPCheck = FS.readword( chunk->filehandle );
	m_BOPCollapseMaps = FS.readword( chunk->filehandle );

	m_BOPVisQuad = FS.readfloat( chunk->filehandle );

	m_BOPOptimize = FS.readword( chunk->filehandle );
	m_BOPConnectDist = FS.readfloat( chunk->filehandle );
	m_BOPAvObjSize = FS.readfloat( chunk->filehandle );

	m_BOPVisCellSize.x = FS.readfloat( chunk->filehandle );
	m_BOPVisCellSize.z = FS.readfloat( chunk->filehandle );

	m_BOPIndentLow.x = FS.readfloat( chunk->filehandle );
	m_BOPIndentLow.z = FS.readfloat( chunk->filehandle );
	m_BOPIndentHigh.x = FS.readfloat( chunk->filehandle );
	m_BOPIndentHigh.z = FS.readfloat( chunk->filehandle );

	return true;
}

void EScene::InitDefaultText(){
	
	char deffilename[MAX_PATH];
	int handle,s;

	m_BOPText[0] = 0;
	m_BOPAddFiles[0] = 0;

	strcpy(deffilename,"default.scenetext");
	FS.m_Config.Update( deffilename );
	handle = FS.open( deffilename );

	s = FS.filesize( handle );
	if( s >= sizeof(m_BOPText) ){
		FS.close( handle );
		return; }
	FS.read( handle, m_BOPText, s );
	m_BOPText[s] = 0;
	FS.close( handle );

	strcpy(deffilename,"default.meshlist");
	FS.m_Config.Update( deffilename );
	handle = FS.open( deffilename );

	s = FS.filesize( handle );
	if( s >= sizeof(m_BOPAddFiles) ){
		FS.close( handle );
		return; }
	FS.read( handle, m_BOPAddFiles, s );
	m_BOPAddFiles[s] = 0;
	FS.close( handle );

	strcpy( m_BOPLMapPrefix, "level" );
}

void EScene::ResetGlobals(){

	strcpy(m_FNLtx,"level.ltx");
	strcpy(m_FNLights,"lights");
	strcpy(m_FNForms,"static.vcf");
	strcpy(m_FNMeshList,"meshlist");
	strcpy(m_FNMap,"map");
	strcpy(m_FNVisibility,"visibility");

	strcpy(m_FNLevelPath,"game\\data\\level");
	strcpy(m_FNTexPath,"game\\data\\maps");

	strcpy(m_LevelName,"Unnamed place");

	m_BOPVisMap = true;
	m_BOPLightMaps = false;
	m_BOPCheck = false;
	m_BOPCollapseMaps = false;

	m_BOPVisQuad = 2.f;

	m_BOPOptimize = 1;
	m_BOPConnectDist = 1.f;
	m_BOPAvObjSize = 15.f;

	m_BOPVisCellSize.set( 2.f, 2.f );
	m_BOPIndentLow.set( 6.f, 6.f );
	m_BOPIndentHigh.set( 6.f, 6.f );
}

void EScene::WriteDefaultLight( FSChunkDef *chunk ){
	FS.writecolor4( chunk->filehandle, m_GlobalAmbient.val );
	FS.writecolor4( chunk->filehandle, m_GlobalDiffuse.val );
	FS.writecolor4( chunk->filehandle, m_GlobalSpecular.val );
	FS.writevector( chunk->filehandle, m_GlobalLightDirection.a );
}

void EScene::ReadDefaultLight( FSChunkDef *chunk ){
	FS.readcolor4( chunk->filehandle, m_GlobalAmbient.val );
	FS.readcolor4( chunk->filehandle, m_GlobalDiffuse.val );
	FS.readcolor4( chunk->filehandle, m_GlobalSpecular.val );
	FS.readvector( chunk->filehandle, m_GlobalLightDirection.a );
}


bool EScene::ReadObject( FSChunkDef *chunk ){

	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	SceneObject *currentobject = 0;

	while( FS.rinchunk( chunk ) ){

		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case CHUNK_OBJECT_CLASS:
			if( currentobject==0 )
				currentobject = NewObjectFromClassID( FS.readdword(chunk->filehandle) );
			break;

		case CHUNK_OBJECT_BODY:
			if( currentobject )
				currentobject->Load( chunk );
			break;

		default:
			NConsole.print( "EScene: undefined chunk 0x%04X", current.chunkid );
			break; }

		FS.rclosechunk( &current );
	}

	if( currentobject ){
		m_Objects.push_back( currentobject );
		return true; }

	return false;
}

//----------------------------------------------------

