//----------------------------------------------------
// file: Library.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "Library.h"
#include "Texture.h"
#include "SObject2.h"
#include "FileSystem.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

ELibrary Lib;

//----------------------------------------------------


void ELibrary::Init(){
	m_LandCurrent = 0;
	m_Current = 0;

	char N[MAX_PATH] = "Library.script";
	FS.m_Config.Update( N );

	XScr *_S = new XScr( N );
	XScrBlock *_B = new XScrBlock( _S );
	XScrBlock *_B1 = new XScrBlock( _B, 1 );

	XTokDef _Tokens [] = {
		XTokDef( "landscapemenu", 1 ),
		XTokDef( "objectmenu", 2 ),
		XTokDef( 0, 0 ) };

	for(int i=0;i<_B1->getTokenCount();i=_B1->nextPth(i)){
		XScrBlock *_B2 = 0;
		switch( _B1->symSearch(i,_Tokens) ){
		case 1:
			_B2 = new XScrBlock(_B1,i+1);
			InitLandscapeFolder( m_TexFolders, _B2 );
			delete _B2;
			break;
		case 2:
			_B2 = new XScrBlock(_B1,i+1);
			InitObjectFolder( m_ObjFolders, _B2 );
			delete _B2;
			break;
		}
	}

	delete _B1;
	delete _B;
	delete _S;

	NConsole.print( "Lib: initialized" );
	m_Valid = true;
}

bool ELibrary::InitLandscapeFolder( TexFolder& folder, XScrBlock *block ){
	
	_ASSERTE( block );

	XVarDef _Vars [] = {
		XVarDef( "name",  CTE_CHARPTR,  folder.m_Name ),
		XVarDef() };

	XTokDef _Tokens [] = {
		XTokDef( "file", 1 ),
		XTokDef( "folder", 2 ),
		XTokDef( 0, 0 ) };

	char namebuffer[MAX_PATH];
	XVarDef _Vars2 [] = {
		XVarDef( "name",  CTE_CHARPTR,  namebuffer ),
		XVarDef() };

	int j;
	ETexture *_T;

	for(int i=0;i<block->getTokenCount();i=block->nextPth(i)){
		XScrBlock *_B2 = 0;
		switch( block->symSearch(i,_Tokens) ){
		case 1:
			_B2 = new XScrBlock(block,i+1);
			for(j=0;j<_B2->getTokenCount();j=_B2->nextPth(j))
				_B2->varSearch( j, _Vars2 );
			_T = new ETexture( namebuffer );
			folder.m_Items.push_back(_T);
			m_Lands.push_back(_T);
			delete _B2;
			break;
		case 2:
			_B2 = new XScrBlock(block,i+1);
			folder.m_Subs.push_back(new TexFolder());
			InitLandscapeFolder( *folder.m_Subs.back(), _B2 );
			delete _B2;
			break;
		default:
			block->varSearch(i,_Vars);
			break;
		}
	}

	return true;
}

bool ELibrary::InitObjectFolder( ObjFolder& folder, XScrBlock *block ){

	_ASSERTE( block );

	XVarDef _Vars [] = {
		XVarDef( "name",  CTE_CHARPTR,  folder.m_Name ),
		XVarDef() };

	XTokDef _Tokens [] = {
		XTokDef( "object", 1 ),
		XTokDef( "folder", 2 ),
		XTokDef( 0, 0 ) };

	for(int i=0;i<block->getTokenCount();i=block->nextPth(i)){
		XScrBlock *_B2 = 0;
		switch( block->symSearch(i,_Tokens) ){
		case 1:
			_B2 = new XScrBlock(block,i+1);
			AddObjectToFolder( folder, _B2 );
			delete _B2;
			break;
		case 2:
			_B2 = new XScrBlock(block,i+1);
			folder.m_Subs.push_back(new ObjFolder());
			InitObjectFolder( *folder.m_Subs.back(), _B2 );
			delete _B2;
			break;
		default:
			block->varSearch(i,_Vars);
			break;
		}
	}

	return true;
}

void ELibrary::Clear(){

	m_Valid = false;
	
	m_LandCurrent = 0;
	m_Current = 0;

	ObjectIt o = m_Objects.begin();
	for(;o!=m_Objects.end();o++)
		SAFE_DELETE( (*o) );

	m_Objects.clear();

	TextureIt t = m_Lands.begin();
	for(;t!=m_Lands.end();t++)
		SAFE_DELETE( (*t) );

	m_Lands.clear();

	NConsole.print( "Lib: cleared" );
}

ELibrary::ELibrary(){
	m_Valid = false;
	m_Current = 0;
	m_LandCurrent = 0;
}

ELibrary::~ELibrary(){
	_ASSERTE( m_Valid == false );
	_ASSERTE( m_Objects.size() == 0 );
}

//----------------------------------------------------

bool ELibrary::AddObjectToFolder( ObjFolder& folder, XScrBlock *block ){
	
	_ASSERTE( block );

	SObject2 *obj = new SObject2();
	folder.m_Items.push_back( obj );
	m_Objects.push_back( obj );

	XVarDef _Vars [] = {
		XVarDef( "name",			CTE_CHARPTR,  obj->GetName() ),
		XVarDef( "class",			CTE_CHARPTR,  obj->GetClassName() ),
		XVarDef( "script",			CTE_CHARPTR,  obj->GetClassScript() ),
		XVarDef( "makeunique",		CTE_INT,      &obj->m_MakeUnique ),
		XVarDef( "dynamic",			CTE_INT,      &obj->m_DynamicList ),
		XVarDef() };

	XTokDef _Tokens [] = {
		XTokDef( "addmesh", 1 ),
		XTokDef( 0, 0 ) };

	for(int i=0;i<block->getTokenCount();i=block->nextPth(i)){
		XScrBlock *_B2 = 0;
		switch( block->symSearch(i,_Tokens) ){
		case 1:
			_B2 = new XScrBlock(block,i+1);
			AddFileToObject( obj, _B2 );
			delete _B2;
			break;
		default:
			block->varSearch(i,_Vars);
			break;
		}
	}

	obj->UpdateClassScript();
	obj->UpdateBox();

	NConsole.print( "Lib object: %s valid", obj->GetName() );
	return true;
}

//----------------------------------------------------

bool ELibrary::AddFileToObject( SObject2 *obj, XScrBlock *block ){

	_ASSERTE( block );
	_ASSERTE( obj );

	Mesh *mesh = new Mesh();
	SObject2Mesh& mref = obj->AddMesh( mesh );
	
	XTokDef _Tokens [] = {
		XTokDef( "optionpack", 1 ),
		XTokDef( 0, 0 ) };

	XVarDef _Vars [] = {
		XVarDef( "name",			CTE_CHARPTR,  mref.m_Name ),
		XVarDef( "filename",		CTE_CHARPTR,  mref.m_FileName ),
		XVarDef() };

	for(int i=0;i<block->getTokenCount();i=block->nextPth(i)){
		XScrBlock *_B2 = 0;
		switch( block->symSearch(i,_Tokens) ){
		case 1:
			_B2 = new XScrBlock(block,i+1);
			mref.m_Ops.ReadScript( _B2 );
			delete _B2;
			break;
		default:
			block->varSearch(i,_Vars);
			break;
		}
	}

	_ASSERTE( mref.m_Name[0] );
	_ASSERTE( mref.m_FileName[0] );

	NConsole.print( "Loading mesh '%s' from %s...", mref.m_Name, mref.m_FileName );
	return mesh->Load( mref.m_FileName );
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

ETexture *ELibrary::SearchTexture( char *name ){
	
	_ASSERTE( name );
	
	TextureIt t = m_Lands.begin();
	for(;t!=m_Lands.end();t++)
		if(0==stricmp((*t)->name(),name) )
			return (*t);

	return 0;
}
