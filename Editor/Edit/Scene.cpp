//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "SceneClassList.h"
#include "Texture.h"
#include "FileSystem.h"
#include "ui_main.h"
#include "SceneChunks.h"
#include "Texturizer.h"

//----------------------------------------------------

EScene Scene;

void EScene::Init(){
	m_LandGrid.set( 2.f, 2.f );
	m_LastAvailObject = 0;
	ResetGlobals();
	InitDefaultText();
	Log.Msg( "Scene: initialized" );
	m_Valid = true;
    m_Modified = false;
}

void EScene::Clear(){
	Log.Msg( "Scene: cleared" );
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

bool EScene::ContainsObject( SceneObject* object ){
	_ASSERTE( object );
	_ASSERTE( m_Valid );
    ObjectIt it = find( m_Objects.begin(), m_Objects.end(), object );
    if (it!=m_Objects.end()) return true;
    return false;
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
	Fmatrix identity;
	identity.identity();
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (((*_F)->ClassID()==classfilter)||(classfilter==OBJCLASS_NONE)) && (*_F)->Visible() ){
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
		if( (((*_F)->ClassID()==classfilter)||(classfilter<0)) && (*_F)->Visible() ){
			(*_F)->Select( flag );
			count++;
		}
	return count;
}

int EScene::ShowObjects( bool flag, int classfilter, bool bOnlySelected ){
	int count = 0;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (((*_F)->ClassID()==classfilter)||(classfilter<0))){
            if (bOnlySelected){
                if ((*_F)->Selected()){
                    (*_F)->Show( flag );
	    		    count++;
                }
            }else{
    			(*_F)->Show( flag );
	    		count++;
            }
		}
	return count;
}

int EScene::ObjectCount( int classfilter ){
	if(classfilter<0)
		return m_Objects.size();
	int count = 0;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (((*_F)->ClassID()==classfilter)||(classfilter<0)))
			count++;
	return count;
}

int EScene::InvertSelection( int classfilter ){
	int count = 0;
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if((((*_F)->ClassID()==classfilter)||(classfilter<0)) && (*_F)->Visible() ){
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
	Fvector& start,
	Fvector& direction,
	int classfilter, SPickInfo* pinf )
{
	if( !valid() )
		return 0;

	Fmatrix identity;
	identity.identity();

	float nearest_dist = 0;
	SceneObject *nearest_object = 0;

	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (((*_F)->ClassID()==classfilter)||(classfilter==OBJCLASS_NONE)) && (*_F)->Visible() ){
			float d0 = 0;
			if( (*_F)->RTL_Pick(&d0,start,direction,identity,pinf) ){
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
        if((((*_F)->ClassID()==classfilter)||(classfilter<0)) && (*_F)->Visible() ){
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

void EScene::Render( float _Aspect, Fmatrix *_Camera ){

	if( !valid() )
		return;

	if( locked() )
		return;

	Fmatrix identity;
	identity.identity();

	if( UI.bRenderWire ){
		UI.d3d()->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_WIREFRAME);
	} else {
		UI.d3d()->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_SOLID);
	}

	SetLights();
	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++)
		if( (*_F)->Visible())
   			(*_F)->Render( identity );

    Texturizer.Render();

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
	for(;o!=m_Objects.end();){
        SceneObject* _O = *o; o++;
		SAFE_DELETE( _O );
    }
	m_Objects.clear();
}

//----------------------------------------------------
bool EScene::IfModified(){
    if (m_Modified && (m_Objects.size()||UI.GetEditFileName()[0])){
        int mr = MessageDlg("The scene has been modified.\n Do yo want to save your changes?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo << mbCancel, 0);
        switch(mr){
        case mrYes: UI.Command(COMMAND_SAVE); m_Modified = false; break;
        case mrNo: m_Modified = false; break;
        case mrCancel: return false;
        }
    }
    return true;
}

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

	Log.Msg( "EScene: loading %s...", _FileName );

	FS.initchunk( &current, handle );

	while( !FS.eof( handle ) ){
		bool undefined_chunk = false;
		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case CHUNK_VERSION:
			version = FS.readdword( handle );
            if (version!=CURRENT_FILE_VERSION){
    			Log.Msg( "EScene: unsuported file version. Level don't load.");
                return false;
            }
            break;

		case CHUNK_GLOBALS:
			ReadGlobals( &current );
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
			Log.Msg( "EScene: undefined chunk 0x%04X", current.chunkid );

		FS.rclosechunk( &current );
	}

	Log.Msg( "EScene: %d objects loaded", m_Objects.size() );

	FS.close( handle );
    m_Modified = false;
	return true;
}

void EScene::Save(char *_FileName){

	int handle;
	FSChunkDef ver,globals,cktext;
	FSChunkDef object,objectclass,objectbody;

	_ASSERTE( _FileName );
	handle = FS.create( _FileName );

	FS.initchunk( &ver, handle );
	FS.initchunk( &globals, handle );
	FS.initchunk( &cktext, handle );
	FS.initchunk( &object, handle );
	FS.initchunk( &objectclass, handle );
	FS.initchunk( &objectbody, handle );

	FS.wopenchunk( &ver, CHUNK_VERSION );
	FS.writedword( handle, CURRENT_FILE_VERSION );
	FS.wclosechunk( &ver );

	FS.wopenchunk( &globals, CHUNK_GLOBALS );
	WriteGlobals( &globals );
	FS.wclosechunk( &globals );

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
    m_Modified = false;
}

bool EScene::WriteGlobals( FSChunkDef *chunk ){

	FS.writedword( chunk->filehandle, CURRENT_GLOBALS_VERSION );
	FS.writestring( chunk->filehandle, m_FNLevelPath );
	FS.writestring( chunk->filehandle, m_LevelName );
// Build options
/*	FS.writedword( chunk->filehandle, m_bTesselate );
	FS.writedword( chunk->filehandle, m_bConvertProgressive );
	FS.writedword( chunk->filehandle, m_bLightMaps );

	FS.writefloat( chunk->filehandle, m_maxedge );
	FS.writedword( chunk->filehandle, m_VB_maxSize );
	FS.writedword( chunk->filehandle, m_VB_maxVertices );

	FS.writefloat( chunk->filehandle, m_pixels_per_meter );
	FS.writefloat( chunk->filehandle, m_maxsize );
	FS.writefloat( chunk->filehandle, m_relevance );
	FS.writefloat( chunk->filehandle, m_viewdist );
*/
//------------------------------------------------------------------------------
	return true;
}

bool EScene::ReadGlobals( FSChunkDef *chunk ){
	if( FS.readdword( chunk->filehandle ) != CURRENT_GLOBALS_VERSION ){
		Log.Msg( "Skipping bad version of globals..." );
		return true;
	}

	FS.readstring( chunk->filehandle, m_FNLevelPath, MAX_PATH );
	FS.readstring( chunk->filehandle, m_LevelName, MAX_PATH );
// Build options
/*	m_bTesselate            = FS.readdword( chunk->filehandle );
	m_bConvertProgressive   = FS.readdword( chunk->filehandle );
	m_bLightMaps            = FS.readdword( chunk->filehandle );

	m_maxedge               = FS.readfloat( chunk->filehandle );
	m_VB_maxSize            = FS.readdword( chunk->filehandle );
	m_VB_maxVertices        = FS.readdword( chunk->filehandle );
	m_pixels_per_meter      = FS.readfloat( chunk->filehandle );
	m_maxsize               = FS.readfloat( chunk->filehandle );
	m_relevance             = FS.readfloat( chunk->filehandle );
	m_viewdist              = FS.readfloat( chunk->filehandle );
*/
//------------------------------------------------------------------------------
    return true;
}

void EScene::InitDefaultText(){
	char deffilename[MAX_PATH];
	int handle,s;

	m_BOPText[0] = 0;

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
}

void EScene::ResetGlobals(){
	strcpy(m_FNLevelPath,"level");
	strcpy(m_LevelName,"Unnamed place");
// Build options
	m_bTesselate            = TRUE;
	m_bConvertProgressive   = TRUE;
	m_bLightMaps            = TRUE;

	m_maxedge               = 1.f;
	m_VB_maxSize            = 64;
	m_VB_maxVertices        = 4096;
	m_pixels_per_meter      = 20;
	m_maxsize               = 16;
	m_relevance             = 10;
	m_viewdist              = 100;
//------------------------------------------------------------------------------
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
			Log.Msg( "EScene: undefined chunk 0x%04X", current.chunkid );
			break; }

		FS.rclosechunk( &current );
	}

	if( currentobject ){
		m_Objects.push_back( currentobject );
		return true; }

	return false;
}

//----------------------------------------------------

