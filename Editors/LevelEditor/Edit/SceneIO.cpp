//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "scene.h"
#include "DetailObjects.h"
#include "ui_main.h"

// file: SceneChunks.h
#define CURRENT_FILE_VERSION    0x00000005
#define CURRENT_LEVELOP_VERSION 0x00000008
#define CURRENT_ENV_VERSION	 	0x00000007

#define CHUNK_VERSION       0x9df3
#define CHUNK_OBJECT        0x7702
#define CHUNK_OBJECT_CLASS  0x7703
#define CHUNK_DETAILPATCHES 0x7707
#define CHUNK_OBJECT_LIST	0x7708
#define CHUNK_CAMERA        0x7709
#define CHUNK_SNAPOBJECTS   0x7710
#define CHUNK_LEVELOP       0x7711
#define CHUNK_OBJECT_COUNT  0x7712
#define CHUNK_DETAILOBJECTS 0x7713

// level options
#define CHUNK_LO_VERSION		0x7801
#define CHUNK_LO_NAMES 			0x7802
#define CHUNK_LO_BOP		 	0x7803
#define CHUNK_LO_SKYDOME	 	0x7804
#define CHUNK_LO_ENVS		 	0x7805
#define CHUNK_LO_DOCLUSTERSIZE	0x7806
#define CHUNK_BUILD_PARAMS		0x7850

//------------------------------------------------------------------------------------------------
// Environment
//------------------------------------------------------------------------------------------------
void st_Environment::Save( CFS_Base& F ){
    F.Wfloat	( m_ViewDist );
    F.Wfloat	( m_Fogness );
    F.Wcolor	( m_FogColor );
    F.Wcolor	( m_AmbColor );
    F.Wcolor	( m_SkyColor );
}

void st_Environment::Read(CStream& F){
    m_ViewDist 	= F.Rfloat();
    m_Fogness  	= F.Rfloat();
    F.Rcolor   	( m_FogColor );
    F.Rcolor   	( m_AmbColor );
    F.Rcolor	( m_SkyColor );
}
//------------------------------------------------------------------------------------------------
// Level Options
//------------------------------------------------------------------------------------------------
void st_LevelOptions::Save( CFS_Base& F ){
    F.open_chunk( CHUNK_LO_VERSION );
	F.Wdword	( CURRENT_LEVELOP_VERSION );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_NAMES );
	F.WstringZ	( m_FNLevelPath.c_str() );
	F.WstringZ	( m_LevelName.c_str() );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_BOP );
	F.WstringZ	( m_BOPText.c_str() );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_SKYDOME );
    F.WstringZ	( m_SkydomeObjName.c_str() );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_ENVS );
	F.Wdword	( m_CurEnv );
	F.Wdword	( m_Envs.size() );
	for (EnvIt e_it=m_Envs.begin(); e_it!=m_Envs.end(); e_it++)
    	e_it->Save(F);
    F.close_chunk();

    F.open_chunk( CHUNK_LO_DOCLUSTERSIZE );
	F.Wfloat	( m_DOClusterSize );
    F.close_chunk();

    F.open_chunk( CHUNK_BUILD_PARAMS );
	F.write		( &m_BuildParams, sizeof(m_BuildParams) );
    F.close_chunk();
}

void st_LevelOptions::Read(CStream& F){
	R_ASSERT(F.FindChunk(CHUNK_LO_VERSION));
    DWORD vers = F.Rdword( );
    if( vers != CURRENT_LEVELOP_VERSION ){
        ELog.Msg( mtError, "Skipping bad version of level options..." );
        return;
    }

    char buf[4096];
    R_ASSERT(F.FindChunk(CHUNK_LO_NAMES));
    F.RstringZ 	(buf); m_FNLevelPath=buf;
    F.RstringZ 	(buf); m_LevelName=buf;

    R_ASSERT(F.FindChunk(CHUNK_LO_BOP));
    F.RstringZ 	(buf); m_BOPText=buf;

    R_ASSERT(F.FindChunk(CHUNK_LO_SKYDOME));
	F.RstringZ	(buf); m_SkydomeObjName=buf;

    R_ASSERT(F.FindChunk(CHUNK_LO_ENVS));
	m_CurEnv	= F.Rdword( );
    m_Envs.resize(F.Rdword( ));
	for (EnvIt e_it=m_Envs.begin(); e_it!=m_Envs.end(); e_it++)
    	e_it->Read(F);

    R_ASSERT(F.FindChunk(CHUNK_LO_DOCLUSTERSIZE));
    m_DOClusterSize = F.Rfloat();

    if (F.FindChunk(CHUNK_BUILD_PARAMS))
		F.Read	( &m_BuildParams, sizeof(m_BuildParams) );
}
//------------------------------------------------------------------------------------------------
// Scene
//------------------------------------------------------------------------------------------------
void EScene::Save(char *_FileName, bool bUndo){
	VERIFY( _FileName );
    CFS_Memory F;

//	Msg("0: %d",F.tell());
    F.open_chunk	(CHUNK_VERSION);
    F.Wdword		(CURRENT_FILE_VERSION);
    F.close_chunk	();

//	Msg("1: %d",F.tell());
    F.open_chunk	(CHUNK_LEVELOP);
	m_LevelOp.Save	(F);
	F.close_chunk	();

//	Msg("2: %d",F.tell());
    F.open_chunk	(CHUNK_OBJECT_COUNT);
    F.Wdword		(ObjCount());
	F.close_chunk	();

//	Msg("3: %d",F.tell());
    if (m_DetailObjects->Valid()){
		F.open_chunk	(CHUNK_DETAILOBJECTS);
    	m_DetailObjects->Save(F);
		F.close_chunk	();
    }

//	Msg("4: %d",F.tell());
    if (!bUndo){
		F.open_chunk	(CHUNK_CAMERA);
        F.Wvector		(Device.m_Camera.GetHPB());
        F.Wvector		(Device.m_Camera.GetPosition());
        F.close_chunk	();
    }

//	Msg("5: %d",F.tell());
	// snap list
    F.open_chunk	(CHUNK_SNAPOBJECTS);
    F.Wdword		(m_SnapObjects.size());
    for(ObjectIt _F=m_SnapObjects.begin();_F!=m_SnapObjects.end();_F++)
        F.WstringZ	((*_F)->Name);
    F.close_chunk	();

//	Msg("6: %d",F.tell());
    F.open_chunk	(CHUNK_OBJECT_LIST);
    int count = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
//        int sz0 = F.tell();
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            F.open_chunk(count); count++;
            SaveObject(*_F,&F);
            F.close_chunk();
        }
//        int sz1 = F.tell();
//    	Msg("TARGET: %s, size: %d",GetNameByClassID(it->first),sz1-sz0);
    }
	F.close_chunk	();
//	Msg("TOTAL: %d",F.tell());

    // back up previous
    if (!bUndo) Engine.FS.BackupFile	(_FileName);

    // save data
	if (!bUndo) Engine.FS.UnlockFile	(0,_FileName,false);
    F.SaveTo							(_FileName,0);
	if (!bUndo) Engine.FS.LockFile		(0,_FileName,false);
}
//--------------------------------------------------------------------------------------------------

void EScene::SaveObject( CCustomObject* O, CFS_Base* F ){
	R_ASSERT(F);
    F->open_chunk	(CHUNK_OBJECT_CLASS);
    F->Wdword		(O->ClassID);
    F->close_chunk	();
    F->open_chunk	(CHUNK_OBJECT_BODY);
    O->Save			(*F);
    F->close_chunk	();
}
//--------------------------------------------------------------------------------------------------

CCustomObject* EScene::ReadObject( CStream* F ){
	CCustomObject *currentobject = 0;
    DWORD clsid=0;
    R_ASSERT(F->FindChunk(CHUNK_OBJECT_CLASS));
    clsid = F->Rdword();
	currentobject  = NewObjectFromClassID(clsid,0,0);

    CStream* S = F->OpenChunk(CHUNK_OBJECT_BODY);
    R_ASSERT(S);
    bool bRes = currentobject->Load(*S);
    S->Close();

	if (!bRes) _DELETE(currentobject);
	return currentobject;
}
//----------------------------------------------------

bool EScene::Load(char *_FileName){
    DWORD version = 0;

	VERIFY( _FileName );
	ELog.Msg( mtInformation, "EScene: loading %s...", _FileName );

    if (Engine.FS.Exist(_FileName,true)){
        CStream* F;
        F = new CFileStream(_FileName);
		char MARK[8];
        F->Read(MARK,8);
        if (strcmp(MARK,"LEVEL")==0){
        	_DELETE(F);
            F = new CCompressedStream(_FileName,"LEVEL");
        }

        // Version
        R_ASSERT(F->ReadChunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION){
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            UI.UpdateScene();
        	_DELETE(F);
            return false;
        }

        // Lev. ops.
        CStream* LOP = F->OpenChunk(CHUNK_LEVELOP);
        if (LOP){
	        m_LevelOp.Read	(*LOP);
    	    UpdateSkydome	();
        	LOP->Close		();
        }else{
			ELog.DlgMsg( mtError, "Skipping old version of level options.\nCheck level options after loading." );
	    }

        //
        if (F->FindChunk(CHUNK_CAMERA)){
        	Fvector hpb, pos;
	        F->Rvector	(hpb);
    	    F->Rvector	(pos);
            Device.m_Camera.Set(hpb,pos);
			Device.m_Camera.SetStyle(Device.m_Camera.GetStyle());
        }

        // Objects
        DWORD obj_cnt 	= 0;
	    if (F->FindChunk	(CHUNK_OBJECT_COUNT)){
	    	obj_cnt		= F->Rdword();
        }

        CStream* OBJ = F->OpenChunk(CHUNK_OBJECT_LIST);
	    UI.ProgressStart(obj_cnt,"Load scene...");
        if (OBJ){
	        CStream* O   = OBJ->OpenChunk(0);
    	    for (int count=1; O; count++) {
			    UI.ProgressInc();
        	    CCustomObject* obj = ReadObject(O);
                if (obj) AddObject(obj, false);
            	O->Close();
	            O = OBJ->OpenChunk(count);
    	    }
	        OBJ->Close();
        }

        // snap list
        if (F->FindChunk(CHUNK_SNAPOBJECTS)){
        	m_SnapObjects.resize(F->Rdword());
		    char buf[4096];
		   	for(ObjectIt _F=m_SnapObjects.begin();_F!=m_SnapObjects.end();_F++){
    	    	F->RstringZ(buf);
                *_F 	= FindObjectByName(buf,OBJCLASS_SCENEOBJECT);
                VERIFY	(*_F);
            }
            UpdateSnapList();
        }

        // detail objects
        // объ€зательно после загрузки snap листа
	    CStream* DO = F->OpenChunk(CHUNK_DETAILOBJECTS);
		if (DO){
	    	m_DetailObjects->Load(*DO);
            DO->Close();
        }

	    UI.ProgressEnd();

        // update scene groups list
//S        UpdateGroups();

        ELog.Msg( mtInformation, "EScene: %d objects loaded", ObjCount() );

        UI.UpdateScene();
        Device.UpdateFog();

		_DELETE(F);

        SynchronizeObjects();

		return true;
    }
	return false;
}

//---------------------------------------------------------------------------------------
//copy/paste utils
//---------------------------------------------------------------------------------------
void EScene::SaveSelection( int classfilter, char *filename ){
	VERIFY( filename );
    CFS_Memory F;

    F.open_chunk	(CHUNK_VERSION);
    F.Wdword		(CURRENT_FILE_VERSION);
    F.close_chunk	();

    F.open_chunk	(CHUNK_OBJECT_LIST);
    int count = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
                if( (*_F)->Selected() ){
                    F.open_chunk(count); count++;
                        F.open_chunk	(CHUNK_OBJECT_CLASS);
                        F.Wdword		((*_F)->ClassID);
                        F.close_chunk	();
                        F.open_chunk	(CHUNK_OBJECT_BODY);
                        (*_F)->Save		(F);
                        F.close_chunk	();
                    F.close_chunk();
                }
            }
    }
	F.close_chunk	();
    F.SaveTo		(filename,0);//"LEVEL");
}

//----------------------------------------------------

bool EScene::LoadSelection(const char *_FileName,ObjectList& lst){
    DWORD version = 0;

	VERIFY( _FileName );
	ELog.Msg( mtInformation, "EScene: loading %s...", _FileName );

    bool res = true;

    if (Engine.FS.Exist(_FileName)){
        CFileStream F(_FileName);

        // Version
        R_ASSERT(F.ReadChunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION){
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            UI.UpdateScene();
            return false;
        }

        // Objects
        CStream* OBJ = F.OpenChunk(CHUNK_OBJECT_LIST);
        if (OBJ){
            CStream* O   = OBJ->OpenChunk(0);
            for (int count=1; O; count++) {
                CCustomObject* obj = ReadObject(O);
                O->Close();
                O = OBJ->OpenChunk(count);
                if (obj){
                	lst.push_back(obj);
                }else{
                    ELog.DlgMsg(mtError,"EScene. Failed to load selection.");
                    res = false;
                    break;
                }
            }
            OBJ->Close();
        }
    }

    return res;
}
//----------------------------------------------------

bool EScene::LoadSelection( const char *_FileName ){
    map<int,int> group_subst;

    ObjectList lst;
    if (LoadSelection(_FileName,lst)){
        SelectObjects( false );
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	CCustomObject* obj = *it;
            char buf[MAX_OBJ_NAME];
    		GenObjectName(obj->ClassID,buf,obj->Name);
            obj->Name=buf;
			AddObject(obj, false);
			obj->Select(true);
        }
		SynchronizeObjects();
        return true;
    }
	return false;
}
//----------------------------------------------------

#pragma pack(push,1)
struct SceneClipData {
	int m_ClassFilter;
	char m_FileName[MAX_PATH];
};
#pragma pack(pop)

int EScene::CopySelection( EObjClass classfilter ){
	HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, sizeof(SceneClipData) );
	SceneClipData *sceneclipdata = (SceneClipData *)GlobalLock(hmem);

	sceneclipdata->m_ClassFilter = classfilter;
	GetTempFileName( Engine.FS.m_Temp.m_Path, "clip", 0, sceneclipdata->m_FileName );
	SaveSelection( classfilter, sceneclipdata->m_FileName );

	GlobalUnlock( hmem );

	int clipformat = RegisterClipboardFormat( "CF_XRAY_CLASS_LIST" );
	if( OpenClipboard( 0 ) ){
		SetClipboardData( clipformat, hmem );
		CloseClipboard();
	} else {
		ELog.DlgMsg( mtError, "Failed to open clipboard" );
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
			ELog.DlgMsg( mtError, "No data in clipboard" );
		}

		CloseClipboard();

	} else {
		ELog.DlgMsg( mtError, "Failed to open clipboard" );
		return false;
	}

	return true;
}

int EScene::CutSelection( EObjClass classfilter ){
	CopySelection( classfilter );
	return RemoveSelection( classfilter );
}
//----------------------------------------------------

