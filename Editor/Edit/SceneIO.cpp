//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "scene.h"
#include "ui_main.h"
#include "DPatch.h"
#include "DetailObjects.h"

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
}

void st_LevelOptions::Read(CStream& F){
	R_ASSERT(F.FindChunk(CHUNK_LO_VERSION));
    DWORD vers = F.Rdword( );
    if( vers != CURRENT_LEVELOP_VERSION ){
        Log->Msg( mtError, "Skipping bad version of level options..." );
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
}
//------------------------------------------------------------------------------------------------
// Scene
//------------------------------------------------------------------------------------------------
void EScene::Save(char *_FileName, bool bUndo){
	VERIFY( _FileName );
    CFS_Memory F;

    F.open_chunk	(CHUNK_VERSION);
    F.Wdword		(CURRENT_FILE_VERSION);
    F.close_chunk	();

    F.open_chunk	(CHUNK_LEVELOP);
	m_LevelOp.Save	(F);
	F.close_chunk	();

    F.open_chunk	(CHUNK_OBJECT_COUNT);
    F.Wdword		(ObjCount());
	F.close_chunk	();
    
    if (m_DetailPatches->ObjCount()){
		F.open_chunk	(CHUNK_DETAILPATCHES);
    	m_DetailPatches->Save(F);
		F.close_chunk	();
    }

    if (m_DetailObjects->Valid()){
		F.open_chunk	(CHUNK_DETAILOBJECTS);
    	m_DetailObjects->Save(F);
		F.close_chunk	();
    }

    if (!bUndo){
		F.open_chunk	(CHUNK_CAMERA);
        F.Wvector		(UI->Device.m_Camera.GetHPB());
        F.Wvector		(UI->Device.m_Camera.GetPosition());
		F.close_chunk	();
    }

	// snap list
    F.open_chunk	(CHUNK_SNAPOBJECTS);
    F.Wdword		(m_SnapObjects.size());
    for(ObjectIt _F=m_SnapObjects.begin();_F!=m_SnapObjects.end();_F++)
        F.WstringZ	((*_F)->GetName());
    F.close_chunk	();

    F.open_chunk	(CHUNK_OBJECT_LIST);
    int count = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            F.open_chunk(count); count++;
              	F.open_chunk	(CHUNK_OBJECT_CLASS);
                F.Wdword		((*_F)->ClassID());
            	F.close_chunk	();
              	F.open_chunk	(CHUNK_OBJECT_BODY);
            	(*_F)->Save		(F);
            	F.close_chunk	();
            F.close_chunk();
        }
    }
	F.close_chunk	();

    // back up previous
    if (!bUndo) FS.BackupFile	(_FileName);

    // save data
    F.SaveTo		(_FileName,0);
}
//--------------------------------------------------------------------------------------------------

SceneObject* EScene::ReadObject( CStream* F ){
	SceneObject *currentobject = 0;
    DWORD clsid=0;
    R_ASSERT(F->FindChunk(CHUNK_OBJECT_CLASS));
    clsid = F->Rdword();
	currentobject  = NewObjectFromClassID( clsid );

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
	Log->Msg( mtInformation, "EScene: loading %s...", _FileName );
    
    if (FS.Exist(_FileName,true)){
        CStream* F;
        F = new CFileStream(_FileName);
		char MARK[8];
        F->Read(MARK,8);
        if (strcmp(MARK,"LEVEL")==0){
        	_DELETE(F);
            F = new CCompressedStream(_FileName,"LEVEL");
        }
    	
//      CCompressedStream F(_FileName,"LEVEL");
//        CFileStream F(_FileName);

        // Version
        R_ASSERT(F->ReadChunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION){
            Log->DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            UI->UpdateScene();
        	_DELETE(F);
            return false;
        }

        // Lev. ops.
        CStream* LOP = F->OpenChunk(CHUNK_LEVELOP);
        if (LOP){
	        m_LevelOp.Read(*LOP);
    	    UpdateSkydome();
        	LOP->Close();
        }else{
			Log->DlgMsg( mtError, "Skipping old version of level options.\nCheck level options after loading." );
	    }

        //
        if (F->FindChunk(CHUNK_CAMERA)){
        	Fvector hpb, pos;
	        F->Rvector	(hpb);
    	    F->Rvector	(pos);
            UI->Device.m_Camera.Set(hpb,pos);
		}

        // Objects
        DWORD obj_cnt 	= 0;
	    if (F->FindChunk	(CHUNK_OBJECT_COUNT)){
	    	obj_cnt		= F->Rdword();
        }
        
        CStream* OBJ = F->OpenChunk(CHUNK_OBJECT_LIST);
	    UI->ProgressStart(obj_cnt,"Load scene...");
        if (OBJ){
	        CStream* O   = OBJ->OpenChunk(0);
    	    for (int count=1; O; count++) {
			    UI->ProgressInc();
        	    SceneObject* obj = ReadObject(O);
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
                *_F 	= FindObjectByName(buf,OBJCLASS_EDITOBJECT);
                VERIFY	(*_F);
            }
            UpdateSnapList();
        }

        // detail patches
	    CStream* DP = F->OpenChunk(CHUNK_DETAILPATCHES);
		if (DP){
	    	m_DetailPatches->Load(*DP);
            DP->Close();
        }

        // detail objects
        // объ€зательно после загрузки snap листа 
	    CStream* DO = F->OpenChunk(CHUNK_DETAILOBJECTS);
		if (DO){
	    	m_DetailObjects->Load(*DO);
            DO->Close();
        }

	    UI->ProgressEnd();

        // update scene groups list
        UpdateGroups();
        
        Log->Msg( mtInformation, "EScene: %d objects loaded", ObjCount() );

        UI->UpdateScene();
        UI->Device.UpdateFog();

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
                        F.Wdword		((*_F)->ClassID());
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
	Log->Msg( mtInformation, "EScene: loading %s...", _FileName );

    bool res = true;
    
    if (FS.Exist(_FileName)){
        CFileStream F(_FileName);

        // Version
        R_ASSERT(F.ReadChunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION){
            Log->DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            UI->UpdateScene();
            return false;
        }

        // Objects
        CStream* OBJ = F.OpenChunk(CHUNK_OBJECT_LIST); 
        if (OBJ){
            CStream* O   = OBJ->OpenChunk(0);
            for (int count=1; O; count++) {
                SceneObject* obj = ReadObject(O);
                O->Close();
                O = OBJ->OpenChunk(count);
                if (obj){
                	lst.push_back(obj);
                }else{
                    Log->DlgMsg(mtError,"EScene. Failed to load selection.");
                    res = false;
                    break;
                }
            }
            OBJ->Close();
        }
        SynchronizeObjects();
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
        	SceneObject* obj = *it;
            char buf[MAX_OBJ_NAME];
            GenObjectName(obj->ClassID(),buf);
            strcpy(obj->GetName(),buf);
			AddObject(obj, false);
			obj->Select(true);
            // group utils
            if (obj->IsInGroup()){
            	int idx = obj->GetGroupIndex();
				obj->Ungroup();
                int idx_subst;
            	if (group_subst.find(idx)!=group_subst.end()){
    	            idx_subst = group_subst[idx];
                }else{
                	idx_subst = GroupGetEmptyIndex();
                    group_subst[idx] = idx_subst;
                }
                obj->Group(idx_subst);
            }
        }
        // update scene groups list
        UpdateGroups();

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
	GetTempFileName( FS.m_Temp.m_Path, "clip", 0, sceneclipdata->m_FileName );
	SaveSelection( classfilter, sceneclipdata->m_FileName );

	GlobalUnlock( hmem );

	int clipformat = RegisterClipboardFormat( "CF_XRAY_CLASS_LIST" );
	if( OpenClipboard( 0 ) ){
		SetClipboardData( clipformat, hmem );
		CloseClipboard();
	} else {
		Log->DlgMsg( mtError, "Failed to open clipboard" );
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
			Log->DlgMsg( mtError, "No data in clipboard" );
		}

		CloseClipboard();

	} else {
		Log->DlgMsg( mtError, "Failed to open clipboard" );
		return false;
	}

	return true;
}

int EScene::CutSelection( EObjClass classfilter ){
	CopySelection( classfilter );
	return RemoveSelection( classfilter );
}
//----------------------------------------------------

