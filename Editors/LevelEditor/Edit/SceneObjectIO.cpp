//----------------------------------------------------
// file: CSceneObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SceneObject.h"
#include "Library.h"
#include "Shader.h"
#include "EditMesh.h"
#include "bone.h"
#include "motion.h"
// export spawn
#include "xrServer_Objects_ALife_All.h"
#include "net_utils.h"
#include "xrMessages.h"
#include "builder.h"

//----------------------------------------------------
#define SCENEOBJ_CURRENT_VERSION		0x0011
//----------------------------------------------------
#define SCENEOBJ_CHUNK_VERSION		  	0x0900
#define SCENEOBJ_CHUNK_REFERENCE     	0x0902
#define SCENEOBJ_CHUNK_PLACEMENT     	0x0904
#define SCENEOBJ_CHUNK_FLAGS			0x0905
//#define SCENEOBJ_CHUNK_OMOTIONS			0xF914
//#define SCENEOBJ_CHUNK_ACTIVE_OMOTION	0xF915
//#define SCENEOBJ_CHUNK_SOUNDS			0xF920
//#define SCENEOBJ_CHUNK_ACTIVE_SOUND		0xF921
bool CSceneObject::Load(IReader& F)
{
    bool bRes = true;
	do{
        u32 version = 0;
        char buf[1024];
        R_ASSERT(F.r_chunk(SCENEOBJ_CHUNK_VERSION,&version));
        if ((version!=0x0010)&&(version!=SCENEOBJ_CURRENT_VERSION)){
            ELog.DlgMsg( mtError, "CSceneObject: unsupported file version. Object can't load.");
            bRes = false;
            break;
        }

        if (version==0x0010){
	        R_ASSERT(F.find_chunk(SCENEOBJ_CHUNK_PLACEMENT));
    	    F.r_fvector3(FPosition);
	        F.r_fvector3(FRotation);
    	    F.r_fvector3(FScale);
        }

		CCustomObject::Load(F);

        R_ASSERT(F.find_chunk(SCENEOBJ_CHUNK_REFERENCE));
        m_Version	= F.r_s32();
        F.r_s32		(); // advance (old read vers)
        F.r_stringZ	(buf);
        if (!SetReference(buf)){
            ELog.Msg( mtError, "CSceneObject: '%s' not found in library", buf );
            bRes = false;
            break;
        }
        if(!CheckVersion())
            ELog.Msg( mtError, "CSceneObject: '%s' different file version! Some objects will work incorrectly.", buf );

        // flags
        if (F.find_chunk(SCENEOBJ_CHUNK_FLAGS)){
        	m_Flags.set(F.r_u32());
        }

        if (!bRes) break;
    }while(0);

    return bRes;
}

void CSceneObject::Save(IWriter& F){
	CCustomObject::Save(F);

	F.open_chunk	(SCENEOBJ_CHUNK_VERSION);
	F.w_u16			(SCENEOBJ_CURRENT_VERSION);
	F.close_chunk	();

    // reference object version
    F.open_chunk	(SCENEOBJ_CHUNK_REFERENCE); R_ASSERT2(m_pReference,"Empty SceneObject REFS");
    F.w_s32			(m_pReference->m_Version);
    F.w_s32			(0); // reserved
    F.w_stringZ		(m_ReferenceName.c_str());
    F.close_chunk	();

    F.open_chunk	(SCENEOBJ_CHUNK_FLAGS);
	F.w_u32			(m_Flags.flags);
    F.close_chunk	();
}
//----------------------------------------------------

bool CSceneObject::ExportGame(SExportStreams& F)
{
    return false;
}
//----------------------------------------------------

