//----------------------------------------------------
// file: CEditObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "FileSystem.h"
#include "Library.h"
#include "UI_Main.h"
#include "Shader.h"
#include "EditMesh.h"
#include "xrShader.h"
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

bool CEditObject::Load(const char* fname){
	AnsiString ext=ExtractFileExt(fname);
    ext=ext.LowerCase();
    if 	(ext==".lwo")    		return Import_LWO(fname,false);//(Log->DlgMsg(mtConfirmation,"Optimize object?")==mrYes)?true:false);
    else if (ext==".object") 	return LoadObject(fname);
	return false;
}

bool CEditObject::LoadObject(const char* fname){
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
    return bRes;
}

void CEditObject::SaveObject(const char* fname){
    CFS_Memory F;
    F.open_chunk(CHUNK_OBJECT_BODY);
    Save(F);
    F.close_chunk();
    
    F.SaveTo(fname,0);
//    UI->ProgressStart(2,"Compressing...");
//    UI->ProgressUpdate(1);
//    F.SaveTo(fname,"OBJECT");
//    UI->ProgressEnd();
}


bool CEditObject::Load(CStream& F){
    bool bRes = true;
	do{
        DWORD version = 0;
        char buf[1024];        
        R_ASSERT(F.ReadChunk(EOBJ_CHUNK_VERSION,&version));
        if (version!=EOBJ_CURRENT_VERSION){
            Log->DlgMsg( mtError, "CEditObject: unsupported file version. Object can't load.");
            bRes = false;
            break;
        }
    
		SceneObject::Load(F);

        R_ASSERT(F.FindChunk(EOBJ_CHUNK_PLACEMENT));
        F.Rvector(vPosition);
        F.Rvector(vRotate);
        F.Rvector(vScale);
    
        R_ASSERT(F.ReadChunk(EOBJ_CHUNK_FLAG, &m_DynamicObject));

        if (F.FindChunk	(EOBJ_CHUNK_CLASSSCRIPT)){
            F.RstringZ	(buf); m_ClassScript=buf;
        }
    
        if (F.FindChunk(EOBJ_CHUNK_REFERENCE)){
            VERIFY(m_LibRef==0 && m_Meshes.size()==0);
            F.Read(&m_ObjVer, sizeof(m_ObjVer));
            F.RstringZ(buf);
            CLibObject* LO = Lib->SearchObject(buf);
            if (!LO){
                Log->Msg( mtError, "CEditObject: '%s' not found in library", buf );
	            bRes = false;
    	        break;
            }
            if (0==(m_LibRef=LO->GetReference())){
                Log->Msg( mtError, "CEditObject: '%s' can't load", buf );
	            bRes = false;
    	        break;
            }
            if(!CheckVersion())
                Log->Msg( mtError, "CEditObject: '%s' different file version! Some objects will work incorrectly.", buf );
        }else{
            // file version
            R_ASSERT(F.ReadChunk(EOBJ_CHUNK_LIB_VERSION, &m_ObjVer));
            // surfaces
            R_ASSERT(F.FindChunk(EOBJ_CHUNK_SURFACES));
            DWORD cnt = F.Rdword();
            m_Surfaces.resize(cnt);
            char sh_name[64];
            for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
                *s_it = new st_Surface();
                F.RstringZ((*s_it)->name);
                F.RstringZ(sh_name);
                (*s_it)->sideflag 	= F.Rbyte();
                (*s_it)->dwFVF 		= F.Rdword();
                cnt 				= F.Rdword();
                (*s_it)->textures.resize(cnt);
                (*s_it)->vmaps.resize(cnt);
                for (AStringIt n_it=(*s_it)->textures.begin(); n_it!=(*s_it)->textures.end(); n_it++){
                    F.RstringZ		(buf); *n_it = buf;
                }
                for (AStringIt v_it=(*s_it)->vmaps.begin(); v_it!=(*s_it)->vmaps.end(); v_it++){
                    F.RstringZ		(buf); *v_it = buf;
                }
                (*s_it)->shader 	= UI->Device.Shader.Create(sh_name,(*s_it)->textures);
                SH_ShaderDef* sh_base = SHLib->FindShader(AnsiString(sh_name));
                if (sh_base)        (*s_it)->has_alpha = (sh_base->Passes_Count)?sh_base->Passes[0].Flags.bABlend:false;
            }

            // Load meshes
            CStream* OBJ = F.OpenChunk(EOBJ_CHUNK_EDITMESHES);
            if(OBJ){
                CStream* M   = OBJ->OpenChunk(0);
                for (int count=1; M; count++) {
                    CEditMesh* mesh=new CEditMesh(this);
                    if (mesh->LoadMesh(*M))
                        m_Meshes.push_back(mesh);
                    else{
                        _DELETE(mesh);
                        Log->DlgMsg( mtError, "CEditObject: Can't load mesh!", buf );
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
			            Log->Msg(mtError,"Motions has different version. Load failed.");
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
			            Log->Msg(mtError,"Motions has different version. Load failed.");
                        _DELETE(*s_it);
                        m_SMotions.clear();
                        break;
                    }
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
        UpdateTransform();
    }while(0);
                                         
    return bRes;
}

void CEditObject::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(EOBJ_CHUNK_VERSION);
	F.Wword			(EOBJ_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_PLACEMENT);
    F.Wvector		(vPosition);
    F.Wvector		(vRotate);
    F.Wvector		(vScale);
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_CLASSSCRIPT);
	F.WstringZ		(m_ClassScript.c_str());
	F.close_chunk	();
    
    F.write_chunk	(EOBJ_CHUNK_FLAG,&m_DynamicObject,1);

    if( IsReference() ){
    	// reference object version
        F.open_chunk	(EOBJ_CHUNK_REFERENCE);
        F.write			(&m_LibRef->m_ObjVer,sizeof(m_LibRef->m_ObjVer));
        F.WstringZ		(m_LibRef->m_Name);
        F.close_chunk	();
    } else {
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
            F.WstringZ	((*sf_it)->name);
            F.WstringZ	((*sf_it)->shader?(*sf_it)->shader->shader->cName:"");
            F.Wbyte		((*sf_it)->sideflag);
            F.Wdword	((*sf_it)->dwFVF);
            F.Wdword	((*sf_it)->textures.size());
            for (AStringIt n_it=(*sf_it)->textures.begin(); n_it!=(*sf_it)->textures.end(); n_it++)
                F.WstringZ(n_it->c_str());
            for (AStringIt v_it=(*sf_it)->vmaps.begin(); v_it!=(*sf_it)->vmaps.end(); v_it++)
                F.WstringZ(v_it->c_str());
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
    }

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
}
//------------------------------------------------------------------------------
CSMotion* CEditObject::LoadSMotion(const char* fname){
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
COMotion* CEditObject::LoadOMotion(const char* fname){
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

