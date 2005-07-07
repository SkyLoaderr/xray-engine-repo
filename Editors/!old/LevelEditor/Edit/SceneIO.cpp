//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "scene.h"
#include "ui_main.h"
#include "ui_leveltools.h"
#include "CustomObject.h"

// file: SceneChunks.h
#define CURRENT_FILE_VERSION    	0x00000005
#define CURRENT_LEVELOP_VERSION 	0x00000008
#define CURRENT_LEVELOP_BP_VERSION 	0x00000009
#define CURRENT_ENV_VERSION	 		0x00000007

#define CHUNK_VERSION       	0x9df3
//#define CHUNK_OBJECT        	0x7702	// obsolete
#define CHUNK_OBJECT_CLASS  	0x7703
//#define CHUNK_DETAILPATCHES 	0x7707	// obsolete
#define CHUNK_OBJECT_LIST		0x7708
#define CHUNK_CAMERA        	0x7709
#define CHUNK_SNAPOBJECTS   	0x7710
#define CHUNK_LEVELOP       	0x7711
#define CHUNK_OBJECT_COUNT  	0x7712
//#define CHUNK_DETAILOBJECTS 	0x7713  // obsolete
//#define CHUNK_AIMAP			0x7715	// obsolete
#define CHUNK_LEVEL_TAG			0x7777

#define CHUNK_TOOLS_GUID		0x7000
#define CHUNK_TOOLS_DATA		0x8000

// level options
#define CHUNK_LO_VERSION		0x7801
#define CHUNK_LO_NAMES 			0x7802
#define CHUNK_LO_BOP		 	0x7803
#define CHUNK_LO_PREFIX 		0x7804
#define CHUNK_LO_BP_VERSION		0x7849
#define CHUNK_BUILD_PARAMS		0x7850
//------------------------------------------------------------------------------------------------
// Level Options
//------------------------------------------------------------------------------------------------
void st_LevelOptions::Save( IWriter& F ){
    F.open_chunk( CHUNK_LO_VERSION );
	F.w_u32		( CURRENT_LEVELOP_VERSION );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_NAMES );
	F.w_stringZ	( m_FNLevelPath.c_str() );
    F.close_chunk();

    F.open_chunk( CHUNK_LO_PREFIX );
	F.w_stringZ	( m_LevelPrefix.c_str() );
    F.close_chunk();
    
    F.open_chunk( CHUNK_LO_BOP );
	F.w_stringZ	( m_BOPText.c_str() );
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

    R_ASSERT(F.find_chunk(CHUNK_LO_NAMES));
    F.r_stringZ 	(m_FNLevelPath);

    if (F.find_chunk(CHUNK_LO_PREFIX)) F.r_stringZ 	(m_LevelPrefix);

    R_ASSERT(F.find_chunk(CHUNK_LO_BOP));
    F.r_stringZ 	(m_BOPText); 

    vers = 0;
    if (F.find_chunk(CHUNK_LO_BP_VERSION))
	    vers = F.r_u32( );

    if (CURRENT_LEVELOP_BP_VERSION==vers){
	    if (F.find_chunk(CHUNK_BUILD_PARAMS)) 	
        	F.r(&m_BuildParams, sizeof(m_BuildParams));
    }else{
        ELog.DlgMsg	(mtError, "Skipping bad version of build params.");
    	m_BuildParams.Init();
    }
}

//------------------------------------------------------------------------------------------------
// Scene
//------------------------------------------------------------------------------------------------
BOOL EScene::LoadLevelPart(ESceneCustomMTools* M, LPCSTR full_name)
{
    IReader* R		= FS.r_open	(full_name);
    if (R){
        R_ASSERT	(R->find_chunk	(CHUNK_TOOLS_GUID));
        xrGUID		guid;
        R->r		(&guid,sizeof(guid));
        if (guid!=m_GUID){
            ELog.DlgMsg	(mtError,"Skipping invalid version of level part: '%s\\%s.part'",EFS.ExtractFileName(full_name).c_str(),M->ClassName());
        }else{
            IReader* chunk 	= R->open_chunk	(CHUNK_TOOLS_DATA+M->ClassID);
            M->Load			(*chunk);
            chunk->close	();
        }
        FS.r_close	(R);
	    return 		TRUE;
    }
    return 			FALSE;
}
BOOL EScene::LoadLevelPart(LPCSTR initial, LPCSTR map_name, ObjClassID cls)
{
	ESceneCustomMTools* M			= GetMTools(cls);
    xr_string 		full_name;
    if (initial)	FS.update_path	(full_name,initial,map_name);
    else			full_name		= map_name;
    xr_string fn					= EFS.ExtractFilePath(full_name.c_str())+EFS.ExtractFileName(full_name.c_str())+"\\"+M->ClassName()+".part";
    return			LoadLevelPart	(M,fn.c_str());
}

void EScene::Save(LPCSTR initial, LPCSTR map_name, bool bUndo)
{
	VERIFY(map_name);

    CTimer T; 		T.Start();
    xr_string full_name;
    if (initial)	FS.update_path	(full_name,initial,map_name);
    else			full_name		= map_name;
    
    xr_string part_prefix;
    
	if (!bUndo){ 
    	EFS.UnlockFile	(0,full_name.c_str(),false);
    	EFS.MarkFile	(full_name.c_str(),true);
    	part_prefix		= EFS.ExtractFilePath(full_name.c_str())+EFS.ExtractFileName(full_name.c_str())+"\\";
    }

    IWriter* F		= FS.w_open		(full_name.c_str()); R_ASSERT(F);

    F->open_chunk	(CHUNK_VERSION);
    F->w_u32		(CURRENT_FILE_VERSION);
    F->close_chunk	();

    F->open_chunk	(CHUNK_LEVELOP);
	m_LevelOp.Save	(*F);
	F->close_chunk	();

    F->open_chunk	(CHUNK_TOOLS_GUID);
    F->w			(&m_GUID,sizeof(m_GUID));
    F->close_chunk	();

    m_ModifName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    m_ModifTime		= time(NULL);

    F->open_chunk	(CHUNK_LEVEL_TAG);
	F->w_stringZ	(m_OwnerName);
	F->w_stringZ	(m_ModifName);
	F->w			(&m_CreateTime,sizeof(m_CreateTime));
	F->w			(&m_ModifTime,sizeof(m_ModifTime));
    F->close_chunk	();
    
    F->open_chunk	(CHUNK_OBJECT_COUNT);
    F->w_u32		(ObjCount());
	F->close_chunk	();

    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    CMemoryWriter 	w_cache;
    for (; _I!=_E; _I++)
        if (_I->second&&_I->second->IsNeedSave()){
            if (bUndo && _I->second->m_bEnabled){
	         	_I->second->Save(w_cache);
	        	F->open_chunk	(CHUNK_TOOLS_DATA+_I->first);
				F->w			(w_cache.pointer(),w_cache.size());
	        	F->close_chunk	();
            }else{
                if (_I->second->m_bEnabled){
                    xr_string		part_name = part_prefix+_I->second->ClassName()+".part";

		         	_I->second->Save(w_cache);

//.					EFS.UnlockFile	(0,part_name.c_str(),false);
					EFS.MarkFile	(part_name.c_str(),true);

                    IWriter* FF		= FS.w_open	(part_name.c_str());

                    FF->open_chunk	(CHUNK_TOOLS_GUID);
                    FF->w			(&m_GUID,sizeof(m_GUID));
                    FF->close_chunk	();

                    FF->open_chunk	(CHUNK_TOOLS_DATA+_I->first);
                    FF->w			(w_cache.pointer(),w_cache.size());
                    FF->close_chunk	();

                    FS.w_close		(FF);

//.					EFS.LockFile	(0,part_name.c_str(),false);
                }
            }
			w_cache.clear	();
        }
        
    if (!bUndo){
		F->open_chunk	(CHUNK_CAMERA);
        F->w_fvector3	(Device.m_Camera.GetHPB());
        F->w_fvector3	(Device.m_Camera.GetPosition());
        F->close_chunk	();
    }

	// snap list
    F->open_chunk		(CHUNK_SNAPOBJECTS);
    F->w_u32			(m_ESO_SnapObjects.size());
    for(ObjectIt _F=m_ESO_SnapObjects.begin();_F!=m_ESO_SnapObjects.end();_F++)
        F->w_stringZ	((*_F)->Name);
    F->close_chunk		();

    // save data
    FS.w_close			(F);
	if (!bUndo){ 
    	EFS.LockFile	(0,full_name.c_str(),false);
    	m_RTFlags.set	(flRT_Unsaved,FALSE);
    	Msg				("Saving time: %3.2f sec",T.Stop());
    }
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

bool EScene::ReadObject(IReader& F, CCustomObject*& O)
{
    ObjClassID clsid=OBJCLASS_DUMMY;
    R_ASSERT(F.find_chunk(CHUNK_OBJECT_CLASS));
    clsid = ObjClassID(F.r_u32());
	O = GetOTools(clsid)->CreateObject(0,0);

    IReader* S = F.open_chunk(CHUNK_OBJECT_BODY);
    R_ASSERT(S);
    bool bRes = O->Load(*S);
    S->close();

	if (!bRes) xr_delete(O);
	return bRes;
}
//----------------------------------------------------

bool EScene::ReadObjects(IReader& F, u32 chunk_id, TAppendObject on_append, SPBItem* pb)
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
            if (pb) pb->Inc();
        }
        OBJ->close();
    }
    return bRes;
}
//----------------------------------------------------

bool EScene::OnLoadAppendObject(CCustomObject* O)
{
	AppendObject	(O,false);
    return true;
}
//----------------------------------------------------

bool EScene::Load(LPCSTR initial, LPCSTR map_name, bool bUndo)
{
    DWORD version = 0;

	if (!map_name||(0==map_name[0])) return false;
    
    xr_string full_name;
    if (initial)	FS.update_path	(full_name,initial,map_name);
    else			full_name 		= map_name;
    
	ELog.Msg( mtInformation, "EScene: loading '%s'", map_name);
    if (FS.exist(full_name.c_str())){
        CTimer T; T.Start();
        IReader* F 	= FS.r_open(full_name.c_str());
        // Version
        R_ASSERT	(F->r_chunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION){
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            UI->UpdateScene();
            FS.r_close(F);
            return false;
        }

        xr_string 	part_prefix;
        if (!bUndo)
            part_prefix		= EFS.ExtractFilePath(full_name.c_str())+EFS.ExtractFileName(full_name.c_str())+"\\";
        
        // Lev. ops.
        IReader* LOP = F->open_chunk(CHUNK_LEVELOP);
        if (LOP){
	        m_LevelOp.Read	(*LOP);
        	LOP->close		();
        }else{
			ELog.DlgMsg		(mtError, "Skipping old version of level options.\nCheck level options after loading.");
	    }

        //
        if (F->find_chunk(CHUNK_CAMERA)){
        	Fvector hpb, pos;
	        F->r_fvector3	(hpb);
    	    F->r_fvector3	(pos);
            Device.m_Camera.Set(hpb,pos);
			Device.m_Camera.SetStyle(Device.m_Camera.GetStyle());
        }

	    if (F->find_chunk(CHUNK_TOOLS_GUID)){
		    F->r			(&m_GUID,sizeof(m_GUID));
        }
        
        if (F->find_chunk(CHUNK_LEVEL_TAG)){
            F->r_stringZ	(m_OwnerName);
            F->r_stringZ	(m_ModifName);
            F->r			(&m_CreateTime,sizeof(m_CreateTime));
            F->r			(&m_ModifTime,sizeof(m_ModifTime));
        }else{
            m_OwnerName		= "";
            m_ModifName		= "";
            m_CreateTime	= 0;
            m_ModifTime		= 0;
        }
    
        DWORD obj_cnt 		= 0;
        if (F->find_chunk(CHUNK_OBJECT_COUNT))
        	obj_cnt 		= F->r_u32();

        SPBItem* pb 		= UI->ProgressStart(obj_cnt,"Loading objects...");
        ReadObjects			(*F,CHUNK_OBJECT_LIST,OnLoadAppendObject,pb);
        UI->ProgressEnd		(pb);

        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second){
			    IReader* chunk 		= F->open_chunk(CHUNK_TOOLS_DATA+_I->first);
            	if (chunk){
	                _I->second->Load(*chunk);
    	            chunk->close	();
                }else{
                    if (_I->second->m_bEnabled && (_I->first!=OBJCLASS_DUMMY)){
                        xr_string 		part_name = part_prefix+_I->second->ClassName()+".part";
                        LoadLevelPart	(_I->second,part_name.c_str());
                    }
                }
            }

        // snap list
        if (F->find_chunk(CHUNK_SNAPOBJECTS)){
        	shared_str 	buf;
            int cnt 	= F->r_u32();
            if (cnt){
                for (int i=0; i<cnt; i++){
                    F->r_stringZ	(buf);
                    CCustomObject* O = FindObjectByName(buf.c_str(),OBJCLASS_SCENEOBJECT);
                    if (!O)		ELog.Msg(mtError,"EScene: Can't find snap object '%s'.",buf.c_str());
                    else		m_ESO_SnapObjects.push_back(O);
                }
            }
            UpdateSnapList();
        }
/*
        { // old version
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
		}      
*/          
        Msg("EScene: %d objects loaded, %3.2f sec", ObjCount(), T.Stop() );

    	UI->UpdateScene(true); 

		FS.r_close(F);

        SynchronizeObjects();

	    if (!bUndo) m_RTFlags.set(flRT_Unsaved|flRT_Modified,FALSE);
        
		return true;
    }else{
    	ELog.Msg(mtError,"Can't find file: '%s'",map_name);
    }
	return false;
}

//---------------------------------------------------------------------------------------
//copy/paste utils
//---------------------------------------------------------------------------------------
void EScene::SaveSelection( ObjClassID classfilter, LPCSTR initial, LPCSTR fname )
{
	VERIFY( fname );

    xr_string full_name;
    if (initial)	FS.update_path	(full_name,initial,fname);
    else			full_name 		= fname;

    IWriter* F		= FS.w_open(full_name.c_str());  R_ASSERT(F);

    F->open_chunk	(CHUNK_VERSION);
    F->w_u32	   	(CURRENT_FILE_VERSION);
    F->close_chunk	();

    CMemoryWriter 	w_cache;
    if (OBJCLASS_DUMMY==classfilter){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second&&_I->second->IsNeedSave()){
                F->open_chunk	(CHUNK_TOOLS_DATA+_I->first);
                _I->second->SaveSelection(w_cache);
                F->w			(w_cache.pointer(),w_cache.size());
                w_cache.clear	();
                F->close_chunk	();
            }
    }else{
    	ESceneCustomMTools* mt = GetMTools(classfilter); VERIFY(mt);
        F->open_chunk	(CHUNK_TOOLS_DATA+classfilter);
        mt->SaveSelection(w_cache);
        F->w			(w_cache.pointer(),w_cache.size());
        w_cache.clear	();
        F->close_chunk	();
    }
        
    FS.w_close		(F);
}

//----------------------------------------------------
bool EScene::OnLoadSelectionAppendObject(CCustomObject* obj)
{
    string256 buf;
    GenObjectName	(obj->ClassID,buf,obj->Name);
    obj->Name		= buf;
    AppendObject	(obj, false);
    obj->Select		(true);
    return true;
}
//----------------------------------------------------

bool EScene::LoadSelection( LPCSTR initial, LPCSTR fname )
{
    DWORD version = 0;

	VERIFY( fname );

    xr_string 	full_name;
    if (initial)	FS.update_path	(full_name,initial,fname);
    else			full_name 		= fname;

	ELog.Msg( mtInformation, "EScene: loading part %s...", fname );

    bool res = true;

    if (FS.exist(full_name.c_str())){
		SelectObjects( false );

        IReader* F = FS.r_open(full_name.c_str());

        // Version
        R_ASSERT(F->r_chunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION){
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            UI->UpdateScene();
            FS.r_close(F);
            return false;
        }

        // Objects
        if (!ReadObjects(*F,CHUNK_OBJECT_LIST,OnLoadSelectionAppendObject,0)){
            ELog.DlgMsg(mtError,"EScene. Failed to load selection.");
            res = false;
        }

        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second){
			    IReader* chunk 		= F->open_chunk(CHUNK_TOOLS_DATA+_I->first);
            	if (chunk){
	                _I->second->LoadSelection(*chunk);
    	            chunk->close	();
                }
            }
/*
        { // old version
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
        }
*/
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

void EScene::CopySelection( ObjClassID classfilter )
{
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
}

void EScene::PasteSelection()
{
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
	}
}

void EScene::CutSelection( ObjClassID classfilter )
{
	CopySelection( classfilter );
	RemoveSelection( classfilter );
}
//----------------------------------------------------

void EScene::LoadCompilerError(LPCSTR fn)
{
	IReader* F	= FS.r_open(fn);

    Tools->ClearDebugDraw();
    Fvector 		pt[3];
    if (F->find_chunk(10)){ // lc error (TJ)
        Tools->m_DebugDraw.m_Points.resize(F->r_u32());
        F->r(Tools->m_DebugDraw.m_Points.begin(),sizeof(CLevelTools::SDebugDraw::Point)*Tools->m_DebugDraw.m_Points.size());
    }else if (F->find_chunk(0)){ // lc error (TJ)
    	u32 cnt			= F->r_u32();
        for (u32 k=0;k<cnt; k++){ F->r(pt,sizeof(Fvector)); Tools->m_DebugDraw.AppendPoint(pt[0]); }
    }
    if (F->find_chunk(11)){ // lc error (multiple edges)
        Tools->m_DebugDraw.m_Lines.resize(F->r_u32());
        F->r(Tools->m_DebugDraw.m_Lines.begin(),sizeof(CLevelTools::SDebugDraw::Line)*Tools->m_DebugDraw.m_Lines.size());
    }else if (F->find_chunk(1)){ // lc error (multiple edges)
    	u32 cnt			= F->r_u32();
        for (u32 k=0;k<cnt; k++){ F->r(pt,sizeof(Fvector)*2); Tools->m_DebugDraw.AppendLine(pt[0],pt[1]); }
    }
    if (F->find_chunk(12)){ // lc error (invalid faces)
        Tools->m_DebugDraw.m_WireFaces.resize(F->r_u32());
        F->r(Tools->m_DebugDraw.m_WireFaces.begin(),sizeof(CLevelTools::SDebugDraw::Face)*Tools->m_DebugDraw.m_WireFaces.size());
    }else if (F->find_chunk(2)){ // lc error (invalid faces)
    	u32 cnt			= F->r_u32();
        for (u32 k=0;k<cnt; k++){ F->r(pt,sizeof(Fvector)*3); Tools->m_DebugDraw.AppendWireFace(pt[0],pt[1],pt[2]); }
    }
    FS.r_close(F);
}

void EScene::SaveCompilerError(LPCSTR fn)
{
	IWriter*		fs	= FS.w_open(fn);  R_ASSERT(fs);
	IWriter&		err = *fs;

	// t-junction
	err.open_chunk	(10);
	err.w_u32		(Tools->m_DebugDraw.m_Points.size());
	err.w			(Tools->m_DebugDraw.m_Points.begin(), Tools->m_DebugDraw.m_Points.size()*sizeof(CLevelTools::SDebugDraw::Point));
	err.close_chunk	();

	// m-edje
	err.open_chunk	(11);
	err.w_u32		(Tools->m_DebugDraw.m_Lines.size());
	err.w			(Tools->m_DebugDraw.m_Lines.begin(), Tools->m_DebugDraw.m_Lines.size()*sizeof(CLevelTools::SDebugDraw::Line));
	err.close_chunk	();

	// invalid
	err.open_chunk	(12);
	err.w_u32		(Tools->m_DebugDraw.m_WireFaces.size());
	err.w			(Tools->m_DebugDraw.m_WireFaces.begin(), Tools->m_DebugDraw.m_WireFaces.size()*sizeof(CLevelTools::SDebugDraw::Face));
	err.close_chunk	();

    FS.w_close		(fs);
}

