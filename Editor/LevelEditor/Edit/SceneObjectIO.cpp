//----------------------------------------------------
// file: CSceneObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SceneObject.h"
#include "Library.h"
#include "UI_Main.h"
#include "Shader.h"
#include "EditMesh.h"
#include "bone.h"
#include "motion.h"

//----------------------------------------------------
bool CSceneObject::Load(CStream& F){
    bool bRes = true;
	do{
        DWORD version = 0;
        char buf[1024];
        R_ASSERT(F.ReadChunk(EOBJ_CHUNK_VERSION,&version));
        if ((version!=0x0010)&&(version!=EOBJ_CURRENT_VERSION)){
            ELog.DlgMsg( mtError, "CSceneObject: unsupported file version. Object can't load.");
            bRes = false;
            break;
        }

        if (version==0x0010){
	        R_ASSERT(F.FindChunk(EOBJ_CHUNK_PLACEMENT));
    	    F.Rvector(PPosition);
	        F.Rvector(PRotate);
    	    F.Rvector(PScale);
        }

		CCustomObject::Load(F);

        R_ASSERT(F.FindChunk(EOBJ_CHUNK_REFERENCE));
        F.Read(&m_ObjVer, sizeof(m_ObjVer));
        F.RstringZ(buf);
        if (!SetReference(buf)){
            ELog.Msg( mtError, "CSceneObject: '%s' not found in library", buf );
            bRes = false;
            break;
        }
        if(!CheckVersion())
            ELog.Msg( mtError, "CSceneObject: '%s' different file version! Some objects will work incorrectly.", buf );

        if (!bRes) break;
    }while(0);

    return bRes;
}

void CSceneObject::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(EOBJ_CHUNK_VERSION);
	F.Wword			(EOBJ_CURRENT_VERSION);
	F.close_chunk	();

    // reference object version
    F.open_chunk	(EOBJ_CHUNK_REFERENCE);
    F.write			(&m_pRefs->m_ObjVer,sizeof(m_pRefs->m_ObjVer));
    F.WstringZ		(m_pRefs->GetName());
    F.close_chunk	();
}

