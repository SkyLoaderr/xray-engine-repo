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
#define SCENEOBJ_CURRENT_VERSION		0x0011
//----------------------------------------------------
#define SCENEOBJ_CHUNK_VERSION		  	0x0900
#define SCENEOBJ_CHUNK_REFERENCE     	0x0902
#define SCENEOBJ_CHUNK_PLACEMENT     	0x0904
#define SCENEOBJ_CHUNK_OMOTIONS			0xF914
//----------------------------------------------------
COMotion* CSceneObject::LoadOMotion(const char* fname){
	if (Engine.FS.Exist(fname)){
    	COMotion* M = new COMotion();
        if (!M->LoadMotion(fname)){
        	_DELETE(M);
        }
        return M;
    }
	return 0;
}
//------------------------------------------------------------------------------

bool CSceneObject::Load(CStream& F){
    bool bRes = true;
	do{
        DWORD version = 0;
        char buf[1024];
        R_ASSERT(F.ReadChunk(SCENEOBJ_CHUNK_VERSION,&version));
        if ((version!=0x0010)&&(version!=SCENEOBJ_CURRENT_VERSION)){
            ELog.DlgMsg( mtError, "CSceneObject: unsupported file version. Object can't load.");
            bRes = false;
            break;
        }

        if (version==0x0010){
	        R_ASSERT(F.FindChunk(SCENEOBJ_CHUNK_PLACEMENT));
    	    F.Rvector(FPosition);
	        F.Rvector(FRotation);
    	    F.Rvector(FScale);
        }

		CCustomObject::Load(F);

        R_ASSERT(F.FindChunk(SCENEOBJ_CHUNK_REFERENCE));
        F.Read(&m_ObjVer, sizeof(m_ObjVer));
        F.RstringZ(buf);
        if (!SetReference(buf)){
            ELog.Msg( mtError, "CSceneObject: '%s' not found in library", buf );
            bRes = false;
            break;
        }
        if(!CheckVersion())
            ELog.Msg( mtError, "CSceneObject: '%s' different file version! Some objects will work incorrectly.", buf );

        // object motions
        if (F.FindChunk(SCENEOBJ_CHUNK_OMOTIONS)){
            m_OMotions.resize(F.Rdword());
            for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++){
                *o_it = new COMotion();
                if (!(*o_it)->Load(F)){
                    ELog.Msg(mtError,"SceneObject: '%s' - motions has different version. Load failed.",Name);
                    _DELETE(*o_it);
                    m_OMotions.clear();
                    break;
                }
            }
        }

        if (!bRes) break;
    }while(0);

    return bRes;
}

void CSceneObject::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(SCENEOBJ_CHUNK_VERSION);
	F.Wword			(SCENEOBJ_CURRENT_VERSION);
	F.close_chunk	();

    // reference object version
    F.open_chunk	(SCENEOBJ_CHUNK_REFERENCE); R_ASSERT2(m_pRefs,"Empty SceneObject REFS");
    F.write			(&m_pRefs->m_ObjVer,sizeof(m_pRefs->m_ObjVer));
    F.WstringZ		(m_pRefs->GetName());
    F.close_chunk	();

    // object motions
    if (!m_OMotions.empty()){
	    F.open_chunk	(SCENEOBJ_CHUNK_OMOTIONS);
    	F.Wdword		(m_OMotions.size());
	    for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++) (*o_it)->Save(F);
    	F.close_chunk	();
    }

}

