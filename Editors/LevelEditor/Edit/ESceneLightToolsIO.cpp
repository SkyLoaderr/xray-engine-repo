#include "stdafx.h"
#pragma hdrstop

#include "ESceneLightTools.h"
#include "scene.h"
#include "ui_main.h"

// chunks
static const u16 LIGHT_TOOLS_VERSION  	= 0x0000;
//----------------------------------------------------
static const u32 CHUNK_VERSION			= 0x1001;
static const u32 CHUNK_LCONTROLS		= 0x1002;
//----------------------------------------------------

bool ESceneCustomOTools::Load(IReader& F)
{
	if (!inherited::Load(F)) return false;

	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=LIGHTTOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",GetClassNameByClassID(ClassID));
        return false;
    }

	IReader* R 		= F.open_chunk(CHUNK_LCONTROLS); R_ASSERT(R);
    R->

    return true;
}
//----------------------------------------------------

void ESceneCustomOTools::Save(IWriter& F)
{
	inherited::Save	(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&LIGHTTOOLS_VERSION,sizeof(TOOLS_VERSION));

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
 
