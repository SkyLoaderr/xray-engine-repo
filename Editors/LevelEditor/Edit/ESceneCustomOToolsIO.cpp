#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomOTools.h"
#include "scene.h"
#include "ui_main.h"

// chunks
static const u16 TOOLS_VERSION  		= 0x0000;
//----------------------------------------------------
static const u32 CHUNK_VERSION			= 0x0001;
static const u32 CHUNK_OBJECT_COUNT		= 0x0002;
static const u32 CHUNK_OBJECTS			= 0x0003;
static const u32 CHUNK_FLAGS			= 0x0004;
//----------------------------------------------------

bool ESceneCustomOTools::OnLoadSelectionAppendObject(CCustomObject* obj)
{
    string256 buf;
    Scene.GenObjectName	(obj->ClassID,buf,obj->Name);
    obj->Name			= buf;
    Scene.AppendObject	(obj, false);
    return true;
}
//----------------------------------------------------

bool ESceneCustomOTools::OnLoadAppendObject(CCustomObject* O)
{
	Scene.AppendObject	(O,false);
	UI.ProgressInc		();
    return true;
}
//----------------------------------------------------

bool ESceneCustomOTools::LoadSelection(IReader& F)
{
	u16 version 	= 0;

    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

    int count		= 0;
	F.r_chunk		(CHUNK_OBJECT_COUNT,&count);

    UI.ProgressStart(count,AnsiString().sprintf("Loading %s's...",ClassDesc()).c_str());
    Scene.ReadObjects(F,CHUNK_OBJECTS,OnLoadSelectionAppendObject);
    UI.ProgressEnd	();

    return true;
}
//----------------------------------------------------

void ESceneCustomOTools::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&TOOLS_VERSION,sizeof(TOOLS_VERSION));

	F.open_chunk	(CHUNK_OBJECTS);
    int count		= 0;
    for(ObjectIt it = m_Objects.begin();it!=m_Objects.end();it++){
    	if ((*it)->Selected()){
	        F.open_chunk(count++);
    	    Scene.SaveObject(*it,F);
        	F.close_chunk();
        }
    }
	F.close_chunk	();

	F.w_chunk		(CHUNK_OBJECT_COUNT,&count,sizeof(count));
}
//----------------------------------------------------

bool ESceneCustomOTools::Load(IReader& F)
{
	u16 version 	= 0;

    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

    if (F.find_chunk(CHUNK_FLAGS))
    	m_Flags.set	(F.r_u32());

    int count		= 0;
	F.r_chunk		(CHUNK_OBJECT_COUNT,&count);

    UI.ProgressStart(count,AnsiString().sprintf("Loading %s...",ClassDesc()).c_str());
    Scene.ReadObjects(F,CHUNK_OBJECTS,OnLoadAppendObject);
    UI.ProgressEnd	();

    return true;
}
//----------------------------------------------------

void ESceneCustomOTools::Save(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&TOOLS_VERSION,sizeof(TOOLS_VERSION));

	F.open_chunk	(CHUNK_FLAGS);
    F.w_u32			(m_Flags.get());
	F.close_chunk	();

    int count		= m_Objects.size();
	F.w_chunk		(CHUNK_OBJECT_COUNT,&count,sizeof(count));

	F.open_chunk	(CHUNK_OBJECTS);
    count			= 0;
    for(ObjectIt it = m_Objects.begin();it!=m_Objects.end();it++){
        F.open_chunk(count++);
        Scene.SaveObject(*it,F);
        F.close_chunk();
    }
	F.close_chunk	();
}
//----------------------------------------------------

bool ESceneCustomOTools::Export(LPCSTR fn)
{
	return false;
}
//----------------------------------------------------
 
bool ESceneCustomOTools::ExportGame(SExportStreams& F)
{
    for(ObjectIt it = m_Objects.begin();it!=m_Objects.end();it++)
        (*it)->ExportGame(F);
	return true;
}
//----------------------------------------------------
 