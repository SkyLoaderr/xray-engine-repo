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

#define EOBJ_CURRENT_VERSION		0x0010
//----------------------------------------------------
#define EOBJ_CHUNK_VERSION		  	0x0900
#define EOBJ_CHUNK_REFERENCE     	0x0902
#define EOBJ_CHUNK_FLAG           	0x0903
#define EOBJ_CHUNK_PLACEMENT     	0x0904
#define EOBJ_CHUNK_SURFACES			0x0905
#define EOBJ_CHUNK_EDITMESHES      	0x0910
#define EOBJ_CHUNK_LIB_VERSION     	0x0911
#define EOBJ_CHUNK_CLASSSCRIPT     	0x0912
#define EOBJ_CHUNK_BONES			0x0913
#define EOBJ_CHUNK_OMOTIONS			0x0914
#define EOBJ_CHUNK_ACTIVE_OMOTION	0x0915
#define EOBJ_CHUNK_SMOTIONS			0x0916
#define EOBJ_CHUNK_ACTIVE_SMOTION	0x0917

//----------------------------------------------------
bool CSceneObject::Load(CStream& F){
    bool bRes = true;
	do{
        DWORD version = 0;
        char buf[1024];
        R_ASSERT(F.ReadChunk(EOBJ_CHUNK_VERSION,&version));
        if (version!=EOBJ_CURRENT_VERSION){
            ELog.DlgMsg( mtError, "CSceneObject: unsupported file version. Object can't load.");
            bRes = false;
            break;
        }

		CCustomObject::Load(F);

        R_ASSERT(F.FindChunk(EOBJ_CHUNK_PLACEMENT));
        F.Rvector(vPosition);
        F.Rvector(vRotate);
        F.Rvector(vScale);

        R_ASSERT(F.FindChunk(EOBJ_CHUNK_REFERENCE));
        F.Read(&m_ObjVer, sizeof(m_ObjVer));
        F.RstringZ(buf);
        CEditableObject* O = Lib->GetEditObject(buf);
        if (!(m_pRefs=O)){
            ELog.Msg( mtError, "CSceneObject: '%s' not found in library", buf );
            bRes = false;
            break;
        }
        if(!CheckVersion())
            ELog.Msg( mtError, "CSceneObject: '%s' different file version! Some objects will work incorrectly.", buf );

        if (!bRes) break;
        UpdateTransform();
    }while(0);

    return bRes;
}

void CSceneObject::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(EOBJ_CHUNK_VERSION);
	F.Wword			(EOBJ_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_PLACEMENT);
    F.Wvector		(vPosition);
    F.Wvector		(vRotate);
    F.Wvector		(vScale);
	F.close_chunk	();

    // reference object version
    F.open_chunk	(EOBJ_CHUNK_REFERENCE);
    F.write			(&m_pRefs->m_ObjVer,sizeof(m_pRefs->m_ObjVer));
    F.WstringZ		(m_pRefs->GetName());
    F.close_chunk	();
}

