//----------------------------------------------------
// file: CEditableObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#include "bone.h"
#include "motion.h"

#ifdef _EDITOR
 #include "xr_ini.h"
 #include "exportskeleton.h"
 #include "Shader.h"
 #include "xr_trims.h"

bool CEditableObject::Load(const char* fname){
	AnsiString ext=ExtractFileExt(fname);
    ext=ext.LowerCase();
    if 	(ext==".lwo")    		return Import_LWO(fname,false);
    else if (ext==".object") 	return LoadObject(fname);
	return false;
}
#endif

bool CEditableObject::LoadObject(const char* fname){
    CStream* F;
    F = new CFileStream(fname);
    char MARK[8];
    F->Read(MARK,8);
    if (strcmp(MARK,"OBJECT")==0){
        _DELETE(F);
        F = new CCompressedStream(fname,"OBJECT");
    }
    CStream* OBJ = F->OpenChunk(EOBJ_CHUNK_OBJECT_BODY);
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
        if (!fsimilar(t_vRotate.magnitude(),0)||!fsimilar(t_vScale.magnitude(),1.73205f)||!fsimilar(t_vPosition.magnitude(),0)){
            mRotate.setHPB			(t_vRotate.y, t_vRotate.x, t_vRotate.z);
            mScale.scale			(t_vScale);
            mTranslate.translate	(t_vPosition);
            mTransform.mul			(mTranslate,mRotate);
            mTransform.mulB			(mScale);
	        TranslateToWorld		(mTransform);
            t_vRotate.set			(0,0,0);
            t_vPosition.set			(0,0,0);
            t_vScale.set			(1,1,1);
        }
    }

    CFS_Memory F;
    F.open_chunk(EOBJ_CHUNK_OBJECT_BODY);
    Save(F);
    F.close_chunk();

	string256 save_nm,dir,path,nm;
    _splitpath( fname, dir, path, nm, 0 );
	strconcat(save_nm,dir,path,nm,".object");

    Engine.FS.VerifyPath(save_nm);

    F.SaveTo(save_nm,0);
	m_LoadName = save_nm;
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

        R_ASSERT(F.ReadChunk(EOBJ_CHUNK_FLAGS, &m_dwFlags));

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
#ifdef _EDITOR
            (*s_it)->SetShader	(sh_name,Device.Shader.Create(sh_name,(*s_it)->_Texture()));
#else
            (*s_it)->ED_SetShader(sh_name);
#endif
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

        // bone parts
        if (F.FindChunk(EOBJ_CHUNK_BONEPARTS)){
            m_BoneParts.resize(F.Rdword());
	        for (BPIt bp_it=m_BoneParts.begin(); bp_it!=m_BoneParts.end(); bp_it++){
    	        F.RstringZ	(buf); bp_it->alias=buf;
	            bp_it->bones.resize(F.Rdword());
                F.Read(bp_it->bones.begin(),bp_it->bones.size()*sizeof(int));
	        }
    	}

		if (F.FindChunk	(EOBJ_CHUNK_ACTORTRANSFORM)){
	        F.Rvector	(a_vPosition);
    	    F.Rvector	(a_vRotate);
		}

        ResetAnimation();

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

    F.write_chunk	(EOBJ_CHUNK_FLAGS,&m_dwFlags,sizeof(m_dwFlags));

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
    if (!m_Bones.empty()){
	    F.open_chunk	(EOBJ_CHUNK_BONES);
    	F.Wdword		(m_Bones.size());
	    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++) (*b_it)->Save(F);
    	F.close_chunk	();
    }

    // skeleton motions
    if (!m_SMotions.empty()){
        F.open_chunk	(EOBJ_CHUNK_SMOTIONS);
        F.Wdword		(m_SMotions.size());
        for (SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end(); s_it++) (*s_it)->Save(F);
        F.close_chunk	();
    }

    // bone parts
    if (!m_BoneParts.empty()){
        F.open_chunk	(EOBJ_CHUNK_BONEPARTS);
        F.Wdword		(m_BoneParts.size());
        for (BPIt bp_it=m_BoneParts.begin(); bp_it!=m_BoneParts.end(); bp_it++){
            F.WstringZ	(bp_it->alias.c_str());
            F.Wdword	(bp_it->bones.size());
            F.write		(bp_it->bones.begin(),bp_it->bones.size()*sizeof(int));
        }
        F.close_chunk	();
    }                                 

    if (IsDynamic()){
		F.open_chunk	(EOBJ_CHUNK_ACTORTRANSFORM);
        F.Wvector		(a_vPosition);
        F.Wvector		(a_vRotate);
		F.close_chunk	();
    }

	bOnModified		= false;
}
//------------------------------------------------------------------------------
CSMotion* CEditableObject::LoadSMotion(const char* fname){
	if (Engine.FS.Exist(fname)){
    	CSMotion* M = new CSMotion();
        if (!M->LoadMotion(fname)){
        	_DELETE(M);
        }
        return M;
    }
	return 0;
}
//------------------------------------------------------------------------------
#ifdef _EDITOR
bool CEditableObject::ExportSkeletonOGF(LPCSTR fn){
	CFS_Memory F;
    if (PrepareSV(F)){
    	F.SaveTo(fn,0);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
bool CEditableObject::ExportObjectOGF(LPCSTR fname)
{
	CFS_Memory F;
    if (PrepareOGF(F)){
    	F.SaveTo(fname,0);
        return true;
    }
    return false;
}
#endif
