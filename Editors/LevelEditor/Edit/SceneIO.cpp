//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "scene.h"
#include "DetailObjects.h"
#include "ESceneAIMapTools.h"
#include "ui_main.h"

// file: SceneChunks.h
#define CURRENT_FILE_VERSION    	0x00000005
#define CURRENT_LEVELOP_VERSION 	0x00000008
#define CURRENT_LEVELOP_BP_VERSION 	0x00000008
#define CURRENT_ENV_VERSION	 		0x00000007

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
#define CHUNK_AIMAP			0x7715

// level options
#define CHUNK_LO_VERSION		0x7801
#define CHUNK_LO_NAMES 			0x7802
#define CHUNK_LO_BOP		 	0x7803
#define CHUNK_LO_SKYDOME	 	0x7804
#define CHUNK_LO_ENVS		 	0x7805
#define CHUNK_LO_DOCLUSTERSIZE	0x7806
#define CHUNK_LO_BP_VERSION		0x7849
#define CHUNK_BUILD_PARAMS		0x7850

//------------------------------------------------------------------------------------------------
// Environment
//------------------------------------------------------------------------------------------------
void st_Environment::Save( IWriter& F ){
    F.w_float	( m_ViewDist );
    F.w_float	( m_Fogness );
    F.w_fcolor	( m_FogColor );
    F.w_fcolor	( m_AmbColor );
    F.w_fcolor	( m_SkyColor );
}

void st_Environment::Read(IReader& F){
    m_ViewDist 	= F.r_float();
    m_Fogness  	= F.r_float();
    F.r_fcolor 	( m_FogColor );
    F.r_fcolor 	( m_AmbColor );
    F.r_fcolor 	( m_SkyColor );
}
//------------------------------------------------------------------------------------------------
// Level Options
//------------------------------------------------------------------------------------------------
void st_LevelOptions::Save( IWriter& F ){
    F.open_chunk( CHUNK_LO_VERSION );
	F.w_u32		( CURRENT_LEVELOP_VERSION );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_NAMES );
	F.w_stringZ	( m_FNLevelPath.c_str() );
	F.w_stringZ	( m_LevelName.c_str() );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_BOP );
	F.w_stringZ	( m_BOPText.c_str() );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_SKYDOME );
    F.w_stringZ	( m_SkydomeObjName.c_str() );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_ENVS );
	F.w_u32		( m_CurEnv );
	F.w_u32		( m_Envs.size() );
	for (EnvIt e_it=m_Envs.begin(); e_it!=m_Envs.end(); e_it++)
    	e_it->Save(F);
    F.close_chunk();

    F.open_chunk( CHUNK_LO_DOCLUSTERSIZE );
	F.w_float	( m_DOClusterSize );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_BP_VERSION );
	F.w_u32		( CURRENT_LEVELOP_BP_VERSION );
    F.close_chunk();

    F.open_chunk( CHUNK_BUILD_PARAMS );
	F.w			( &m_BuildParams, sizeof(m_BuildParams) );
    F.close_chunk();
}

void st_LevelOptions::Read(IReader& F)
{
	R_ASSERT(F.find_chunk(CHUNK_LO_VERSION));
    DWORD vers = F.r_u32( );
    if( vers != CURRENT_LEVELOP_VERSION ){
        ELog.DlgMsg( mtError, "Skipping bad version of level options." );
        return;
    }

    char buf[4096];
    R_ASSERT(F.find_chunk(CHUNK_LO_NAMES));
    F.r_stringZ 	(m_FNLevelPath);
    F.r_stringZ 	(m_LevelName);

    R_ASSERT(F.find_chunk(CHUNK_LO_BOP));
    F.r_stringZ 	(m_BOPText); 

    R_ASSERT(F.find_chunk(CHUNK_LO_SKYDOME));
	F.r_stringZ		(m_SkydomeObjName);

    R_ASSERT(F.find_chunk(CHUNK_LO_ENVS));
	m_CurEnv	= F.r_u32( );
    m_Envs.resize(F.r_u32( ));
	for (EnvIt e_it=m_Envs.begin(); e_it!=m_Envs.end(); e_it++)
    	e_it->Read(F);

    R_ASSERT(F.find_chunk(CHUNK_LO_DOCLUSTERSIZE));
    m_DOClusterSize = F.r_float();

    vers = 0;
    if (F.find_chunk(CHUNK_LO_BP_VERSION))
	    vers = F.r_u32( );

    if (CURRENT_LEVELOP_BP_VERSION==vers){
	    if (F.find_chunk(CHUNK_BUILD_PARAMS)){
			F.r		( &m_BuildParams, sizeof(m_BuildParams) );
        }
    }else{
        ELog.DlgMsg( mtError, "Skipping bad version of build params." );
    }
}
//------------------------------------------------------------------------------------------------
// Scene
//------------------------------------------------------------------------------------------------
void EScene::Save(LPCSTR initial, LPCSTR map_name, bool bUndo)
{
	VERIFY( map_name );

    AnsiString full_name = (initial)?FS.update_path(full_name,initial,map_name):AnsiString(map_name);
    
    CMemoryWriter F;

//	Msg("0: %d",F.tell());
    F.open_chunk	(CHUNK_VERSION);
    F.w_u32		(CURRENT_FILE_VERSION);
    F.close_chunk	();

//	Msg("1: %d",F.tell());
    F.open_chunk	(CHUNK_LEVELOP);
	m_LevelOp.Save	(F);
	F.close_chunk	();

//	Msg("2: %d",F.tell());
    F.open_chunk	(CHUNK_OBJECT_COUNT);
    F.w_u32		(ObjCount());
	F.close_chunk	();

//	Msg("3: %d",F.tell());
    if (GetMTools(OBJCLASS_DO)->IsNeedSave()){
		F.open_chunk	(CHUNK_DETAILOBJECTS);
    	GetMTools(OBJCLASS_DO)->Save(F);
		F.close_chunk	();
    }

    if (GetMTools(OBJCLASS_AIMAP)->IsNeedSave()){
        F.open_chunk	(CHUNK_AIMAP);
        GetMTools(OBJCLASS_AIMAP)->Save	(F);
        F.close_chunk	();
    }

//	Msg("4: %d",F.tell());
    if (!bUndo){
		F.open_chunk	(CHUNK_CAMERA);
        F.w_fvector3		(Device.m_Camera.GetHPB());
        F.w_fvector3		(Device.m_Camera.GetPosition());
        F.close_chunk	();
    }

//	Msg("5: %d",F.tell());
	// snap list
    F.open_chunk	(CHUNK_SNAPOBJECTS);
    F.w_u32			(m_ESO_SnapObjects.size());
    for(ObjectIt _F=m_ESO_SnapObjects.begin();_F!=m_ESO_SnapObjects.end();_F++)
        F.w_stringZ	((*_F)->Name);
    F.close_chunk	();

//	Msg("6: %d",F.tell());
    F.open_chunk	(CHUNK_OBJECT_LIST);
    int count = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
//        int sz0 = F.tell();
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            F.open_chunk(count); count++;
            SaveObject(*_F,F);
            F.close_chunk();
        }
//        int sz1 = F.tell();
//    	Msg("TARGET: %s, size: %d",GetNameByClassID(it->first),sz1-sz0);
    }
	F.close_chunk	();
//	Msg("TOTAL: %d",F.tell());

	if (!bUndo) EFS.UnlockFile		(0,full_name.c_str(),false);
    // back up previous
    if (!bUndo) EFS.MarkFile		(full_name.c_str(),false);
    // save data
    F.save_to						(full_name.c_str());
	if (!bUndo) EFS.LockFile		(0,full_name.c_str(),false);
}
//--------------------------------------------------------------------------------------------------

void EScene::SaveObject( CCustomObject* O, IWriter& F )
{
    F.open_chunk	(CHUNK_OBJECT_CLASS);
    F.w_u32		(O->ClassID);
    F.close_chunk	();
    F.open_chunk	(CHUNK_OBJECT_BODY);
    O->Save			(F);
    F.close_chunk	();
}
//--------------------------------------------------------------------------------------------------

void EScene::SaveObjects( ObjectList& lst, u32 chunk_id, IWriter& F )
{
    F.open_chunk	(chunk_id);
    int count 		= 0;
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
        F.open_chunk(count); count++;
        SaveObject	(*_F,F);
        F.close_chunk();
    }
	F.close_chunk	();
}
//--------------------------------------------------------------------------------------------------

bool EScene::ReadObject(IReader& F, CCustomObject*& O){
    DWORD clsid=0;
    R_ASSERT(F.find_chunk(CHUNK_OBJECT_CLASS));
    clsid = F.r_u32();
	O = NewObjectFromClassID(clsid,0,0);

    IReader* S = F.open_chunk(CHUNK_OBJECT_BODY);
    R_ASSERT(S);
    bool bRes = O->Load(*S);
    S->close();

	if (!bRes) xr_delete(O);
	return bRes;
}
//----------------------------------------------------

bool EScene::ReadObjects(IReader& F, u32 chunk_id, TAppendObject on_append)
{
	R_ASSERT(on_append);
	bool bRes = true;
    IReader* OBJ 	= F.open_chunk(chunk_id);
    if (OBJ){
        IReader* O   = OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            CCustomObject* obj=0;
            if (ReadObject(*O, obj)){ 	if (!on_append(obj)) xr_delete(obj);}
            else						bRes = false;
            O->close();
            O = OBJ->open_chunk(count);
        }
        OBJ->close();
    }
    return bRes;
}
//----------------------------------------------------

bool EScene::OnLoadAppendObject(CCustomObject* O)
{
	AddObject		(O,false);
	UI.ProgressInc	();
    return true;
}
//----------------------------------------------------

bool EScene::Load(LPCSTR initial, LPCSTR map_name)
{
    DWORD version = 0;

	VERIFY(map_name);
    AnsiString full_name = (initial)?FS.update_path(full_name,initial,map_name):AnsiString(map_name);
	ELog.Msg( mtInformation, "EScene: loading %s...", map_name);

    if (FS.exist(full_name.c_str())){
        IReader* F = FS.r_open(full_name.c_str());
        // Version
        R_ASSERT(F->r_chunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION){
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            UI.UpdateScene();
            FS.r_close(F);
            return false;
        }

        // Lev. ops.
        IReader* LOP = F->open_chunk(CHUNK_LEVELOP);
        if (LOP){
	        m_LevelOp.Read	(*LOP);
    	    UpdateSkydome	();
        	LOP->close		();
        }else{
			ELog.DlgMsg( mtError, "Skipping old version of level options.\nCheck level options after loading." );
	    }

        //
        if (F->find_chunk(CHUNK_CAMERA)){
        	Fvector hpb, pos;
	        F->r_fvector3	(hpb);
    	    F->r_fvector3	(pos);
            Device.m_Camera.Set(hpb,pos);
			Device.m_Camera.SetStyle(Device.m_Camera.GetStyle());
        }

        DWORD obj_cnt 	= 0;
        if (F->find_chunk(CHUNK_OBJECT_COUNT))
        	obj_cnt = F->r_u32();

        UI.ProgressStart(obj_cnt,"Loading scene...");
        ReadObjects		(*F,CHUNK_OBJECT_LIST,OnLoadAppendObject);
        UI.ProgressEnd	();

        // snap list
        if (F->find_chunk(CHUNK_SNAPOBJECTS)){
        	m_ESO_SnapObjects.resize(F->r_u32());
		    AnsiString buf;
		   	for(ObjectIt _F=m_ESO_SnapObjects.begin();_F!=m_ESO_SnapObjects.end();_F++){
    	    	F->r_stringZ(buf);
                *_F 	= FindObjectByName(buf.c_str(),OBJCLASS_SCENEOBJECT);
                VERIFY	(*_F);
            }
            UpdateSnapList();
        }

	    IReader* DO = F->open_chunk(CHUNK_DETAILOBJECTS);
		if (DO){
	    	GetMTools(OBJCLASS_DO)->Load(*DO);
            DO->close();
        }

	    IReader* AIM = F->open_chunk(CHUNK_AIMAP);
		if (AIM){
	    	GetMTools(OBJCLASS_AIMAP)->Load(*AIM);
            AIM->close();
        }

        ELog.Msg( mtInformation, "EScene: %d objects loaded", ObjCount() );

        UI.UpdateScene(true);

		FS.r_close(F);

        SynchronizeObjects();

		return true;
    }else{
    	ELog.Msg(mtError,"Can't find file: ",map_name);
    }
	return false;
}

//---------------------------------------------------------------------------------------
//copy/paste utils
//---------------------------------------------------------------------------------------
void EScene::SaveSelection( int classfilter, LPCSTR initial, LPCSTR fname )
{
	VERIFY( fname );
    AnsiString full_name = (initial)?FS.update_path(full_name,initial,fname):AnsiString(fname);

    CMemoryWriter F;

    F.open_chunk	(CHUNK_VERSION);
    F.w_u32			(CURRENT_FILE_VERSION);
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
                        F.w_u32		((*_F)->ClassID);
                        F.close_chunk	();
                        F.open_chunk	(CHUNK_OBJECT_BODY);
                        (*_F)->Save		(F);
                        F.close_chunk	();
                    F.close_chunk();
                }
            }
    }
	F.close_chunk	();

    if ((classfilter==OBJCLASS_DUMMY)||(classfilter==OBJCLASS_DO)){
        if (GetMTools(OBJCLASS_DO)->IsNeedSave()){
            F.open_chunk	(CHUNK_DETAILOBJECTS);
            GetMTools(OBJCLASS_DO)->Save(F);
            F.close_chunk	();
        }
    }

    if ((classfilter==OBJCLASS_DUMMY)||(classfilter==OBJCLASS_AIMAP)){
        if (GetMTools(OBJCLASS_AIMAP)->IsNeedSave()){
            F.open_chunk	(CHUNK_AIMAP);
            GetMTools(OBJCLASS_AIMAP)->Save	(F);
            F.close_chunk	();
        }
    }

    F.save_to		(full_name.c_str());
}

//----------------------------------------------------
bool EScene::OnLoadSelectionAppendObject(CCustomObject* obj)
{
    string256 buf;
    GenObjectName	(obj->ClassID,buf,obj->Name);
    obj->Name		= buf;
    AddObject		(obj, false);
    obj->Select		(true);
    return true;
}
//----------------------------------------------------

bool EScene::LoadSelection( LPCSTR initial, LPCSTR fname )
{
    DWORD version = 0;

	VERIFY( fname );
    AnsiString full_name = (initial)?FS.update_path(full_name,initial,fname):AnsiString(fname);

	ELog.Msg( mtInformation, "EScene: loading %s...", fname );

    bool res = true;

    if (FS.exist(full_name.c_str())){
		SelectObjects( false );

        IReader* F = FS.r_open(full_name.c_str());

        // Version
        R_ASSERT(F->r_chunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION){
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            UI.UpdateScene();
            FS.r_close(F);
            return false;
        }

        // Objects
        if (!ReadObjects(*F,CHUNK_OBJECT_LIST,OnLoadSelectionAppendObject)){
            ELog.DlgMsg(mtError,"EScene. Failed to load selection.");
            res = false;
        }

        IReader* DO = F->open_chunk(CHUNK_DETAILOBJECTS);
        if (DO){
        	GetMTools(OBJCLASS_DO)->Clear	();
            GetMTools(OBJCLASS_DO)->Load	(*DO);
            DO->close();
        }

        IReader* AIM = F->open_chunk(CHUNK_AIMAP);
        if (AIM){
        	GetMTools(OBJCLASS_AIMAP)->Clear();
            GetMTools(OBJCLASS_AIMAP)->Load	(*AIM);
            AIM->close();
        }

        // Synchronize
		SynchronizeObjects();
		FS.r_close(F);
    }
	return res;
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
	GetTempFileName( FS.get_path(_temp_)->m_Path, "clip", 0, sceneclipdata->m_FileName );
	SaveSelection( classfilter, 0, sceneclipdata->m_FileName );

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
			LoadSelection( 0, sceneclipdata->m_FileName );
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

int EScene::CutSelection( EObjClass classfilter )
{
	CopySelection( classfilter );
	return RemoveSelection( classfilter );
}
//----------------------------------------------------

void EScene::LoadCompilerError(LPCSTR fn)
{
	IReader* F	= FS.r_open(fn);

    m_CompilerErrors.Clear();
    if (F->find_chunk(0)){ // lc error (TJ)
        m_CompilerErrors.m_TJVerts.resize(F->r_u32());
        F->r(m_CompilerErrors.m_TJVerts.begin(),sizeof(ERR::Vert)*m_CompilerErrors.m_TJVerts.size());
    }
    if (F->find_chunk(1)){ // lc error (multiple edges)
        m_CompilerErrors.m_MultiEdges.resize(F->r_u32());
        F->r(m_CompilerErrors.m_MultiEdges.begin(),sizeof(ERR::Edge)*m_CompilerErrors.m_MultiEdges.size());
    }
    if (F->find_chunk(2)){ // lc error (invalid faces)
        m_CompilerErrors.m_InvalidFaces.resize(F->r_u32());
        F->r(m_CompilerErrors.m_InvalidFaces.begin(),sizeof(ERR::Face)*m_CompilerErrors.m_InvalidFaces.size());
    }
    FS.r_close(F);
}

