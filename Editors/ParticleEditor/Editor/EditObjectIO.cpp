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
 #include "exportskeleton.h"
 #include "Shader.h"

bool CEditableObject::Load(const char* fname)
{
	AnsiString ext=ExtractFileExt(fname);
    ext=ext.LowerCase();
    if 	(ext==".lwo")    		return Import_LWO(fname,false);
    else return LoadObject(fname);
	return false;
}
#endif

bool CEditableObject::LoadObject(const char* fname)
{
    IReader* F = FS.r_open(fname); R_ASSERT(F);
    IReader* OBJ = F->open_chunk(EOBJ_CHUNK_OBJECT_BODY);
    R_ASSERT(OBJ);
    bool bRes = Load(*OBJ);
    OBJ->close();
	FS.r_close(F);
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

    CMemoryWriter 	F;
    F.open_chunk	(EOBJ_CHUNK_OBJECT_BODY);
    Save			(F);
    F.close_chunk	();

    F.save_to		(fname);
	m_LoadName 		= fname;
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
        R_ASSERT(F.find_chunk(EOBJ_CHUNK_LIB_VERSION));
        m_Version			= F.r_s32();
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
        IReader* B_CHUNK = F.open_chunk(EOBJ_CHUNK_BONES2);
		if (B_CHUNK){
            int chunk = 0;
        	IReader* O;
            while (0!=(O=B_CHUNK->open_chunk(chunk++))){
            	m_Bones.push_back(xr_new<CBone>());
                m_Bones.back()->Load_1(*O);
                O->close();
            }
            B_CHUNK->close();
            UpdateBoneParenting();
        }else if (F.find_chunk(EOBJ_CHUNK_BONES)){
            m_Bones.resize(F.r_u32());
            for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
                *b_it = xr_new<CBone>();
                (*b_it)->Load_0(F);
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
                F.r(&*bp_it->bones.begin(),bp_it->bones.size()*sizeof(int));
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

void CEditableObject::Save(IWriter& F)
{
	F.open_chunk	(EOBJ_CHUNK_VERSION);
	F.w_u16			(EOBJ_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_CLASSSCRIPT);
	F.w_stringZ		(m_ClassScript.c_str());
	F.close_chunk	();

    F.w_chunk		(EOBJ_CHUNK_FLAGS,&m_Flags.flags,sizeof(m_Flags.flags));

    // object version
    F.open_chunk	(EOBJ_CHUNK_LIB_VERSION);
    F.w_s32			(m_Version);
    F.w_s32			(0);
    F.close_chunk	();

//    Log("1: ",F.tell());
    // meshes
    F.open_chunk	(EOBJ_CHUNK_EDITMESHES);
    int count = 0;
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++){
        F.open_chunk(count); count++;
        (*m)->SaveMesh(F);
        F.close_chunk();
    }
    F.close_chunk	();
//    Log("2: ",F.tell());
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
//    Log("3: ",F.tell());

    // bones
    if (!m_Bones.empty()){
	    F.open_chunk	(EOBJ_CHUNK_BONES2);
	    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        	F.open_chunk	(b_it-m_Bones.begin());
        	(*b_it)->Save	(F);
	    	F.close_chunk	();
        }
    	F.close_chunk	();
    }
//    Log("4: ",F.tell());

    // skeleton motions
    if (!m_SMotions.empty()){
        F.open_chunk	(EOBJ_CHUNK_SMOTIONS);
        F.w_u32		(m_SMotions.size());
        for (SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end(); s_it++) (*s_it)->Save(F);
        F.close_chunk	();
    }
//    Log("5: ",F.tell());

    // bone parts
    if (!m_BoneParts.empty()){
        F.open_chunk	(EOBJ_CHUNK_BONEPARTS);
        F.w_u32			(m_BoneParts.size());
        for (BPIt bp_it=m_BoneParts.begin(); bp_it!=m_BoneParts.end(); bp_it++){
            F.w_stringZ	(bp_it->alias.c_str());
            F.w_u32		(bp_it->bones.size());
            F.w			(&*bp_it->bones.begin(),bp_it->bones.size()*sizeof(int));
        }
        F.close_chunk	();
    }
//    Log("6: ",F.tell());

    if (IsDynamic()){
		F.open_chunk	(EOBJ_CHUNK_ACTORTRANSFORM);
        F.w_fvector3		(a_vPosition);
        F.w_fvector3		(a_vRotate);
		F.close_chunk	();
    }
//    Log("7: ",F.tell());

	bOnModified		= false;
}
//------------------------------------------------------------------------------
CSMotion* CEditableObject::LoadSMotion(const char* fname){
	if (FS.exist(fname)){
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
bool CEditableObject::ExportOGF(LPCSTR fn){
	CMemoryWriter F;
    if (PrepareOGF(F)){
    	F.save_to(fn);
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
    F.save_to(fname);
	return true;
}
//------------------------------------------------------------------------------
#endif


