//----------------------------------------------------
// file: SceneCopyPaste.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "UI_Main.h"
#include "Scene.h"
#include "FileSystem.h"
#include "SceneChunks.h"

//----------------------------------------------------

void EScene::SaveSelection( int classfilter, char *filename ){

	int handle;
	FSChunkDef ver,globals;
	FSChunkDef object,objectclass,objectbody;

	_ASSERTE( filename );
	handle = FS.create( filename );

	FS.initchunk( &ver, handle );
	FS.initchunk( &globals, handle );
	FS.initchunk( &object, handle );
	FS.initchunk( &objectclass, handle );
	FS.initchunk( &objectbody, handle );

	FS.wopenchunk( &ver, CHUNK_VERSION );
	FS.writedword( handle, CURRENT_FILE_VERSION );
	FS.wclosechunk( &ver );

	ObjectIt _F = m_Objects.begin();
	for(;_F!=m_Objects.end();_F++){

		if( classfilter >= 0 && (*_F)->ClassID() != classfilter )
			continue;

		if( (*_F)->Selected() ){

			FS.wopenchunk( &object, CHUNK_OBJECT );

			FS.wopenchunk( &objectclass, CHUNK_OBJECT_CLASS );
			FS.writedword( handle, (*_F)->ClassID() );
			FS.wclosechunk( &objectclass );
			
			FS.wopenchunk( &objectbody, CHUNK_OBJECT_BODY );
			(*_F)->Save( handle );
			FS.wclosechunk( &objectbody );

			FS.wclosechunk( &object );
		}
	}

	FS.close( handle );
}

//----------------------------------------------------

bool EScene::LoadSelection( char *filename ){

	int handle;
	FSChunkDef current;
	DWORD version = 0;

	_ASSERTE( filename );
	handle = FS.open( filename );

    Scene.SelectObjects( false, OBJCLASS_NONE );

	FS.initchunk( &current, handle );

	while( !FS.eof( handle ) ){
		FS.ropenchunk( &current );
		switch( current.chunkid ){
		case CHUNK_VERSION:
			version = FS.readdword( handle );
            if (version!=CURRENT_FILE_VERSION)
    			Log.Msg( "Unsupport clipboard data" );
			break;
		case CHUNK_OBJECT:
			ReadObject( &current );
			if( !m_Objects.empty() ){
				m_Objects.back()->Select( true );
                strcat(m_Objects.back()->GetName(),"_copy");
            }
			break;
		default:
			break;
		}
		FS.rclosechunk( &current );
	}
	
	FS.close( handle );
	return true;
}

//----------------------------------------------------

#pragma pack(push,1)
struct SceneClipData {
	int m_ClassFilter;
	char m_FileName[MAX_PATH];
};
#pragma pack(pop)

int EScene::CopySelection( int classfilter ){

	HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, sizeof(SceneClipData) );
	SceneClipData *sceneclipdata = (SceneClipData *)GlobalLock(hmem);

	sceneclipdata->m_ClassFilter = classfilter;
	GetTempFileName( FS.m_Temp.m_Path, "clip", 0, sceneclipdata->m_FileName );
	SaveSelection( classfilter, sceneclipdata->m_FileName );

	GlobalUnlock( hmem );

	int clipformat = RegisterClipboardFormat( "CF_XRAY_CLASS_LIST" );
	if( OpenClipboard( 0 ) ){
		SetClipboardData( clipformat, hmem );
		CloseClipboard();
	} else {
		Log.Msg( "Failed to open clipboard" );
		GlobalFree( hmem );
	}

	return true;
}

int EScene::PasteSelection(){
	int clipformat = RegisterClipboardFormat( "CF_XRAY_CLASS_LIST" );
	if( OpenClipboard( 0 ) ){

		HGLOBAL hmem = GetClipboardData(clipformat);
		if( hmem ){
			SceneClipData *sceneclipdata = (SceneClipData *)GlobalLock(hmem);
			LoadSelection( sceneclipdata->m_FileName );
			GlobalUnlock( hmem );
		} else {
			Log.Msg( "No data in clipboard" );
		}

		CloseClipboard();

	} else {
		Log.Msg( "Failed to open clipboard" );
		return false;
	}

	return true;
}

int EScene::CutSelection( int classfilter ){
	CopySelection( classfilter );
	return RemoveSelection( classfilter );
}

//----------------------------------------------------

