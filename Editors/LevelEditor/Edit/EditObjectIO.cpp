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
    IReader* F;
    F = xr_new<CFileReader>(fname);
    char MARK[8];
    F->r(MARK,8);
    if (strcmp(MARK,"OBJECT")==0){
        xr_delete(F);
        F = xr_new<CCompressedReader>(fname,"OBJECT");
    }else{
    	F->seek(0);
    }
    IReader* OBJ = F->open_chunk(EOBJ_CHUNK_OBJECT_BODY);
    R_ASSERT(OBJ);
    bool bRes = Load(*OBJ);
    OBJ->close();
	xr_delete(F);
    if (bRes) m_LoadName = fname;
    return bRes;
}

void CEditableObject::SaveObject(const char* fname)
{
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

    CMemoryWriter F;
    F.open_chunk(EOBJ_CHUNK_OBJECT_BODY);
    Save(F);
    F.close_chunk();

	string256 save_nm,dir,path,nm;
    _splitpath( fname, dir, path, nm, 0 );
	strconcat(save_nm,dir,path,nm,".object");

    VerifyPath(save_nm);

    F.save_to(save_nm,0);
	m_LoadName = save_nm;
}


bool CEditableObject::Load(IReader& F){
    bool bRes = true;
	do{
        u32 version = 0;
        char buf[255];
        char sh_name[255];
        R_ASSERT(F.r_chunk(EOBJ_CHUNK_VERSION,&version));
        if (version!=EOBJ_CURRENT_VERSION){
            ELog.DlgMsg( mtError, "CEditableObject: unsupported file version. Object can't load.");
            bRes = false;
            break;
        }

        R_ASSERT(F.r_chunk(EOBJ_CHUNK_FLAGS, &m_Flags.flags));

        if (F.find_chunk	(EOBJ_CHUNK_CLASSSCRIPT)){
            F.r_stringZ	(buf); m_ClassScript=buf;
        }

        // file version
        R_ASSERT(F.r_chunk(EOBJ_CHUNK_LIB_VERSION, &m_ObjVer));
        // surfaces
        if (F.find_chunk(EOBJ_CHUNK_SURFACES3)){
            u32 cnt = F.r_u32();
            m_Surfaces.resize(cnt);
            for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
                *s_it 		= xr_new<CSurface>();
                F.r_stringZ	(buf);	(*s_it)->SetName		(buf);
                F.r_stringZ	(buf);	(*s_it)->SetShader		(buf);
                F.r_stringZ	(buf);	(*s_it)->SetShaderXRLC	(buf);
                F.r_stringZ	(buf);	(*s_it)->SetGameMtl		(buf);
                F.r_stringZ	(buf); 	(*s_it)->SetTexture		(buf);
                F.r_stringZ	(buf); 	(*s_it)->SetVMap		(buf);
                (*s_it)->m_Flags.set(F.r_u32());
                (*s_it)->SetFVF		(F.r_u32());
                cnt 				= F.r_u32();
                if (cnt>1) ELog.DlgMsg(mtError,"Object surface '%s' has more than one TC's.",buf);
                R_ASSERT(1<=cnt);
            }
        }else if (F.find_chunk(EOBJ_CHUNK_SURFACES2)){
            u32 cnt = F.r_u32();
            m_Surfaces.resize(cnt);
            for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
                *s_it 		= xr_new<CSurface>();
                F.r_stringZ	(buf);	(*s_it)->SetName		(buf);
                F.r_stringZ	(buf);	(*s_it)->SetShader		(buf);
                F.r_stringZ	(buf);	(*s_it)->SetShaderXRLC	(buf);
                F.r_stringZ	(buf); 	(*s_it)->SetTexture		(buf);
                F.r_stringZ	(buf); 	(*s_it)->SetVMap		(buf);
                (*s_it)->m_Flags.set(F.r_u32());
                (*s_it)->SetFVF		(F.r_u32());
                cnt 				= F.r_u32();
                if (cnt>1) ELog.DlgMsg(mtError,"Object surface '%s' has more than one TC's.",buf);
                R_ASSERT(1<=cnt);
            }
        }else{
            R_ASSERT(F.find_chunk(EOBJ_CHUNK_SURFACES));
            u32 cnt = F.r_u32();
            m_Surfaces.resize(cnt);
            for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
                *s_it = xr_new<CSurface>();
                F.r_stringZ(buf);
                (*s_it)->SetName(buf);
                F.r_stringZ(sh_name);
                (*s_it)->m_Flags.set(CSurface::sf2Sided,!!F.r_u8());
                (*s_it)->SetFVF		(F.r_u32());
                cnt 				= F.r_u32();
                if (cnt>1) ELog.DlgMsg(mtError,"Object surface '%s' has more than one TC's.",buf);
                R_ASSERT(1<=cnt);
                F.r_stringZ			(buf); (*s_it)->SetTexture(buf);
                F.r_stringZ			(buf); (*s_it)->SetVMap(buf);
                (*s_it)->SetShader		(sh_name);
                (*s_it)->SetShaderXRLC	("default");
            }

            // surfaces xrlc part
            if(F.find_chunk(EOBJ_CHUNK_SURFACES_XRLC))
                for (s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
                    F.r_stringZ(buf); (*s_it)->SetShaderXRLC(buf);
                }
        }

        // Load meshes
        IReader* OBJ = F.open_chunk(EOBJ_CHUNK_EDITMESHES);
        if(OBJ){
            IReader* M   = OBJ->open_chunk(0);
            for (int count=1; M; count++) {
                CEditableMesh* mesh=xr_new<CEditableMesh>(this);
                if (mesh->LoadMesh(*M))
                    m_Meshes.push_back(mesh);
                else{
                    xr_delete(mesh);
                    ELog.DlgMsg( mtError, "CEditableObject: Can't load mesh!", buf );
                    bRes = false;
                }
                M->close();
                if (!bRes)	break;
                M = OBJ->open_chunk(count);
            }
            OBJ->close();
        }

        // bones
        if (F.find_chunk(EOBJ_CHUNK_BONES)){
            m_Bones.resize(F.r_u32());
            for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
                *b_it = xr_new<CBone>();
                (*b_it)->Load(F);
            }
            UpdateBoneParenting();
        }

        // skeleton motions
        if (F.find_chunk(EOBJ_CHUNK_SMOTIONS)){
            m_SMotions.resize(F.r_u32());
            for (SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end(); s_it++){
                *s_it = xr_new<CSMotion>();
                if (!(*s_it)->Load(F)){
                    ELog.Msg(mtError,"Motions has different version. Load failed.");
                    xr_delete(*s_it);
                    m_SMotions.clear();
                    break;
                }
            }
        }

        // bone parts
        if (F.find_chunk(EOBJ_CHUNK_BONEPARTS)){
            m_BoneParts.resize(F.r_u32());
	        for (BPIt bp_it=m_BoneParts.begin(); bp_it!=m_BoneParts.end(); bp_it++){
    	        F.r_stringZ	(buf); bp_it->alias=buf;
	            bp_it->bones.resize(F.r_u32());
                F.r(bp_it->bones.begin(),bp_it->bones.size()*sizeof(int));
	        }
    	}

		if (F.find_chunk	(EOBJ_CHUNK_ACTORTRANSFORM)){
	        F.r_fvector3	(a_vPosition);
    	    F.r_fvector3	(a_vRotate);
		}

        ResetAnimation();

        if (!bRes) break;
        UpdateBox();
    }while(0);

    return bRes;
}

void CEditableObject::Save(IWriter& F){
	F.open_chunk	(EOBJ_CHUNK_VERSION);
	F.w_u16			(EOBJ_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_CLASSSCRIPT);
	F.w_stringZ		(m_ClassScript.c_str());
	F.close_chunk	();

    F.w_chunk		(EOBJ_CHUNK_FLAGS,&m_Flags.flags,sizeof(m_Flags.flags));

    // object version
    F.w_chunk		(EOBJ_CHUNK_LIB_VERSION,&m_ObjVer,m_ObjVer.size());
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
    F.open_chunk	(EOBJ_CHUNK_SURFACES3);
    F.w_u32		(m_Surfaces.size());
    for (SurfaceIt sf_it=m_Surfaces.begin(); sf_it!=m_Surfaces.end(); sf_it++){
        F.w_stringZ	((*sf_it)->_Name			());
        F.w_stringZ	((*sf_it)->_ShaderName		());
        F.w_stringZ	((*sf_it)->_ShaderXRLCName	());
        F.w_stringZ	((*sf_it)->_GameMtlName		());
		F.w_stringZ	((*sf_it)->_Texture			());
		F.w_stringZ	((*sf_it)->_VMap			());
        F.w_u32	((*sf_it)->m_Flags.get		());
        F.w_u32	((*sf_it)->_FVF				());
        F.w_u32	(1);
    }
    F.close_chunk	();

    // bones
    if (!m_Bones.empty()){
	    F.open_chunk	(EOBJ_CHUNK_BONES);
    	F.w_u32		(m_Bones.size());
	    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++) (*b_it)->Save(F);
    	F.close_chunk	();
    }

    // skeleton motions
    if (!m_SMotions.empty()){
        F.open_chunk	(EOBJ_CHUNK_SMOTIONS);
        F.w_u32		(m_SMotions.size());
        for (SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end(); s_it++) (*s_it)->Save(F);
        F.close_chunk	();
    }

    // bone parts
    if (!m_BoneParts.empty()){
        F.open_chunk	(EOBJ_CHUNK_BONEPARTS);
        F.w_u32			(m_BoneParts.size());
        for (BPIt bp_it=m_BoneParts.begin(); bp_it!=m_BoneParts.end(); bp_it++){
            F.w_stringZ	(bp_it->alias.c_str());
            F.w_u32		(bp_it->bones.size());
            F.w			(bp_it->bones.begin(),bp_it->bones.size()*sizeof(int));
        }
        F.close_chunk	();
    }

    if (IsDynamic()){
		F.open_chunk	(EOBJ_CHUNK_ACTORTRANSFORM);
        F.w_fvector3		(a_vPosition);
        F.w_fvector3		(a_vRotate);
		F.close_chunk	();
    }

	bOnModified		= false;
}
//------------------------------------------------------------------------------
CSMotion* CEditableObject::LoadSMotion(const char* fname){
	if (Engine.FS.Exist(fname)){
    	CSMotion* M = xr_new<CSMotion>();
        if (!M->LoadMotion(fname)){
        	xr_delete(M);
        }
        return M;
    }
	return 0;
}
//------------------------------------------------------------------------------
#ifdef _EDITOR
bool CEditableObject::ExportSkeletonOGF(LPCSTR fn){
	CMemoryWriter F;
    if (PrepareSV(F)){
    	F.save_to(fn,0);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
bool CEditableObject::ExportObjectOGF(LPCSTR fname)
{
	CMemoryWriter F;
    if (PrepareOGF(F)){
    	F.save_to(fname,0);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
bool CEditableObject::ExportHOMPart(const Fmatrix& parent, IWriter& F)
{
	Fvector v;
    for (EditMeshIt m_it=m_Meshes.begin(); m_it!=m_Meshes.end(); m_it++){
        for (SurfFacesPairIt sf_it=(*m_it)->m_SurfFaces.begin(); sf_it!=(*m_it)->m_SurfFaces.end(); sf_it++){
            BOOL b2Sided = sf_it->first->m_Flags.is(CSurface::sf2Sided);
            IntVec& i_lst= sf_it->second;
            for (IntIt i_it=i_lst.begin(); i_it!=i_lst.end(); i_it++){
                st_Face& face = (*m_it)->m_Faces[*i_it];
                for (int k=0; k<3; k++){
                    parent.transform_tiny(v,(*m_it)->m_Points[face.pv[k].pindex]);
                    F.w_fvector3	(v);
                }
                F.w_u32(b2Sided);
            }
        }
    }
	return true;
}
//------------------------------------------------------------------------------
bool CEditableObject::ExportHOM(LPCSTR fname)
{
    CMemoryWriter F;
    F.open_chunk(0);
    F.w_u32(0);
    F.close_chunk();
    F.open_chunk(1);
    ExportHOMPart(Fidentity,F);
    F.close_chunk();
    F.save_to(fname,0);
	return true;
}
//------------------------------------------------------------------------------
#endif
