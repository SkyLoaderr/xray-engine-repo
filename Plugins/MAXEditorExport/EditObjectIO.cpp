//----------------------------------------------------
// file: CEditableObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "UI_Main.h"
#include "Shader.h"
#include "EditMesh.h"
#include "bone.h"
#include "motion.h"
#include "xr_trims.h"

#define EOBJ_CURRENT_VERSION		0x0010
//----------------------------------------------------
#define EOBJ_CHUNK_VERSION		  	0x0900
#define EOBJ_CHUNK_REFERENCE     	0x0902
#define EOBJ_CHUNK_FLAG           	0x0903
#define EOBJ_CHUNK_SURFACES			0x0905
#define EOBJ_CHUNK_EDITMESHES      	0x0910
#define EOBJ_CHUNK_LIB_VERSION     	0x0911
#define EOBJ_CHUNK_CLASSSCRIPT     	0x0912
#define EOBJ_CHUNK_BONES			0x0913
#define EOBJ_CHUNK_OMOTIONS			0x0914
#define EOBJ_CHUNK_ACTIVE_OMOTION	0x0915
#define EOBJ_CHUNK_SMOTIONS			0x0916
#define EOBJ_CHUNK_ACTIVE_SMOTION	0x0917
#define EOBJ_CHUNK_SURFACES_XRLC	0x0918
//----------------------------------------------------

bool CEditableObject::Load(const char* fname){
	AnsiString ext=ExtractFileExt(fname);
    ext=ext.LowerCase();
    if 	(ext==".lwo")    		return Import_LWO(fname,false);//(ELog.DlgMsg(mtConfirmation,"Optimize object?")==mrYes)?true:false);
    else if (ext==".object") 	return LoadObject(fname);
	return false;
}

bool CEditableObject::LoadObject(const char* fname){
    CStream* F;
    F = new CFileStream(fname);
    char MARK[8];
    F->Read(MARK,8);
    if (strcmp(MARK,"OBJECT")==0){
        _DELETE(F);
        F = new CCompressedStream(fname,"OBJECT");
    }
    CStream* OBJ = F->OpenChunk(CHUNK_OBJECT_BODY);
    R_ASSERT(OBJ);
    bool bRes = Load(*OBJ);
    OBJ->Close();
	_DELETE(F);
    if (bRes) m_LoadName = fname;
    return bRes;
}

void CEditableObject::SaveObject(const char* fname){
	if (IsModified()){
        // update transform matrix
        Fmatrix	mTransform,mScale,mTranslate,mRotate;
        mRotate.setHPB			(t_vRotate.y, t_vRotate.x, t_vRotate.z);
        mScale.scale			(t_vScale);
        mTranslate.translate	(t_vPosition);
        mTransform.mul			(mTranslate,mRotate);
        mTransform.mul			(mScale);
        TranslateToWorld		(mTransform);
    }

    CFS_Memory F;
    F.open_chunk(CHUNK_OBJECT_BODY);
    Save(F);
    F.close_chunk();

    FS.VerifyPath(fname);

    F.SaveTo(fname,0);
	m_LoadName = fname;
//    UI.ProgressStart(2,"Compressing...");
//    UI.ProgressUpdate(1);
//    F.SaveTo(fname,"OBJECT");
//    UI.ProgressEnd();
}


bool CEditableObject::Load(CStream& F){
    bool bRes = true;
	do{
        DWORD version = 0;
        char buf[255];
        char sh_name[255];
        R_ASSERT(F.ReadChunk(EOBJ_CHUNK_VERSION,&version));
        if (version!=EOBJ_CURRENT_VERSION){
            ELog.DlgMsg( mtError, "CEditableObject: unsupported file version. Object can't load.");
            bRes = false;
            break;
        }

        R_ASSERT(F.ReadChunk(EOBJ_CHUNK_FLAG, &m_DynamicObject));

        if (F.FindChunk	(EOBJ_CHUNK_CLASSSCRIPT)){
            F.RstringZ	(buf); m_ClassScript=buf;
        }

        // file version
        R_ASSERT(F.ReadChunk(EOBJ_CHUNK_LIB_VERSION, &m_ObjVer));
        // surfaces
        R_ASSERT(F.FindChunk(EOBJ_CHUNK_SURFACES));
        DWORD cnt = F.Rdword();
        m_Surfaces.resize(cnt);
        for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
            *s_it = new CSurface();
            F.RstringZ(buf);
            (*s_it)->SetName(buf);
            F.RstringZ(sh_name);
            (*s_it)->Set2Sided	(F.Rbyte());
            (*s_it)->SetFVF		(F.Rdword());
            cnt 				= F.Rdword();
            if (cnt>1) ELog.DlgMsg(mtError,"Object surface '%s' has more than one TC's.",buf);
            R_ASSERT(1<=cnt);
			F.RstringZ			(buf); (*s_it)->SetTexture(buf);
			F.RstringZ			(buf); (*s_it)->SetVMap(buf);
            (*s_it)->SetShader	(sh_name,Device.Shader.Create(sh_name,(*s_it)->_Texture()));
            (*s_it)->SetShaderXRLC("default");
        }

        // surfaces xrlc part
        if(F.FindChunk(EOBJ_CHUNK_SURFACES_XRLC))
	        for (s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
    	        F.RstringZ(buf); (*s_it)->SetShaderXRLC(buf);
        	}

        // Load meshes
        CStream* OBJ = F.OpenChunk(EOBJ_CHUNK_EDITMESHES);
        if(OBJ){
            CStream* M   = OBJ->OpenChunk(0);
            for (int count=1; M; count++) {
                CEditableMesh* mesh=new CEditableMesh(this);
                if (mesh->LoadMesh(*M))
                    m_Meshes.push_back(mesh);
                else{
                    _DELETE(mesh);
                    ELog.DlgMsg( mtError, "CEditableObject: Can't load mesh!", buf );
                    bRes = false;
                }
                M->Close();
                if (!bRes)	break;
                M = OBJ->OpenChunk(count);
            }
            OBJ->Close();
        }

        // bones
        if (F.FindChunk(EOBJ_CHUNK_BONES)){
            m_Bones.resize(F.Rdword());
            for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
                *b_it = new CBone();
                (*b_it)->Load(F);
            }
            UpdateBoneParenting();
        }

        // object motions
        if (F.FindChunk(EOBJ_CHUNK_OMOTIONS)){
            m_OMotions.resize(F.Rdword());
            for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++){
                *o_it = new COMotion();
                if (!(*o_it)->Load(F)){
                    ELog.Msg(mtError,"Motions has different version. Load failed.");
                    _DELETE(*o_it);
                    m_OMotions.clear();
                    break;
                }
            }
        }
        // skeleton motions
        if (F.FindChunk(EOBJ_CHUNK_SMOTIONS)){
            m_SMotions.resize(F.Rdword());
            for (SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end(); s_it++){
                *s_it = new CSMotion();
                if (!(*s_it)->Load(F)){
                    ELog.Msg(mtError,"Motions has different version. Load failed.");
                    _DELETE(*s_it);
                    m_SMotions.clear();
                    break;
                }
            }
        }

        ResetAnimation(false);
		if (F.FindChunk	(EOBJ_CHUNK_ACTIVE_OMOTION)){
        	F.RstringZ	(buf);
            SetActiveOMotion(FindOMotionByName(buf));
        }
		if (F.FindChunk	(EOBJ_CHUNK_ACTIVE_SMOTION)){
        	F.RstringZ	(buf);
            SetActiveSMotion(FindSMotionByName(buf));
        }

        if (!bRes) break;
        UpdateBox();
    }while(0);

    return bRes;
}

void CEditableObject::Save(CFS_Base& F){
	F.open_chunk	(EOBJ_CHUNK_VERSION);
	F.Wword			(EOBJ_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_CLASSSCRIPT);
	F.WstringZ		(m_ClassScript.c_str());
	F.close_chunk	();

    F.write_chunk	(EOBJ_CHUNK_FLAG,&m_DynamicObject,1);

    // object version
    F.write_chunk	(EOBJ_CHUNK_LIB_VERSION,&m_ObjVer,m_ObjVer.size());
    // meshes
    F.open_chunk	(EOBJ_CHUNK_EDITMESHES);
    int count = 0;
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++){
        F.open_chunk(count); count++;
        (*m)->SaveMesh(F);
        F.close_chunk();
    }
    F.close_chunk	();
    // surfaces
    F.open_chunk	(EOBJ_CHUNK_SURFACES);
    F.Wdword		(m_Surfaces.size());
    for (SurfaceIt sf_it=m_Surfaces.begin(); sf_it!=m_Surfaces.end(); sf_it++){
        F.WstringZ	((*sf_it)->_Name());
        F.WstringZ	((*sf_it)->_ShaderName());
        F.Wbyte		((*sf_it)->_2Sided());
        F.Wdword	((*sf_it)->_FVF());
        F.Wdword	(1);
		F.WstringZ	((*sf_it)->_Texture());
		F.WstringZ	((*sf_it)->_VMap());
    }
    F.close_chunk	();

    F.open_chunk	(EOBJ_CHUNK_SURFACES_XRLC);
    for (sf_it=m_Surfaces.begin(); sf_it!=m_Surfaces.end(); sf_it++){
        F.WstringZ	((*sf_it)->_ShaderXRLCName());
    }
    F.close_chunk	();

    // bones
    F.open_chunk	(EOBJ_CHUNK_BONES);
    F.Wdword		(m_Bones.size());
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++) (*b_it)->Save(F);
    F.close_chunk	();

    // object motions
    F.open_chunk	(EOBJ_CHUNK_OMOTIONS);
    F.Wdword		(m_OMotions.size());
    for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++) (*o_it)->Save(F);
    F.close_chunk	();

    // skeleton motions
    F.open_chunk	(EOBJ_CHUNK_SMOTIONS);
    F.Wdword		(m_SMotions.size());
    for (SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end(); s_it++) (*s_it)->Save(F);
    F.close_chunk	();

    if (m_ActiveOMotion){
	    F.open_chunk	(EOBJ_CHUNK_ACTIVE_OMOTION);
    	F.WstringZ		(m_ActiveOMotion->Name());
	    F.close_chunk	();
    }
    if (m_ActiveSMotion){
	    F.open_chunk	(EOBJ_CHUNK_ACTIVE_SMOTION);
    	F.WstringZ		(m_ActiveSMotion->Name());
	    F.close_chunk	();
    }

	t_bOnModified		= false;
}
//------------------------------------------------------------------------------
CSMotion* CEditableObject::LoadSMotion(const char* fname){
	if (FS.Exist(fname)){
    	CSMotion* M = new CSMotion();
        if (!M->LoadMotion(fname)){
        	_DELETE(M);
        }
        return M;
    }
	return 0;
}
//------------------------------------------------------------------------------
COMotion* CEditableObject::LoadOMotion(const char* fname){
	if (FS.Exist(fname)){
    	COMotion* M = new COMotion();
        if (!M->LoadMotion(fname)){
        	_DELETE(M);
        }
        return M;
    }
	return 0;
}
//------------------------------------------------------------------------------
bool CEditableObject::Reload()
{
	ClearGeometry();
    return Load(m_LoadName.c_str());
}

