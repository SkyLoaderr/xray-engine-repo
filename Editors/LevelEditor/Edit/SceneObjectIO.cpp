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
// export spawn
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
#define SCENEOBJ_CHUNK_OMOTIONS			0xF914
#define SCENEOBJ_CHUNK_ACTIVE_OMOTIONS	0xF915
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

        // flags
        if (F.FindChunk(SCENEOBJ_CHUNK_FLAGS)){
        	m_dwFlags	= F.Rdword();
        }

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
            if (F.FindChunk(SCENEOBJ_CHUNK_ACTIVE_OMOTIONS)){
            	string256 buf; F.RstringZ(buf);
                SetActiveOMotion(FindOMotionByName(buf));
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

    F.open_chunk	(SCENEOBJ_CHUNK_FLAGS);
	F.Wdword		(m_dwFlags);
    F.close_chunk	();

    // object motions
    if (IsOMotionable()){
	    F.open_chunk	(SCENEOBJ_CHUNK_OMOTIONS);
    	F.Wdword		(m_OMotions.size());
	    for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++) (*o_it)->Save(F);
    	F.close_chunk	();

        if (IsOMotionActive()){
	        F.open_chunk	(SCENEOBJ_CHUNK_ACTIVE_OMOTIONS);
    	    F.WstringZ		(m_ActiveOMotion->Name());
    		F.close_chunk	();
        }
    }

}
//----------------------------------------------------

bool CSceneObject::ExportSpawn(CFS_Base& F, int chunk_id)
{
    if (IsDynamic()&&IsFlag(eDummy)){
    	R_ASSERT(m_pRefs);
    	// export spawn packet
        NET_Packet Packet;
        Packet.w_begin		(M_SPAWN);
        Packet.w_string		("M_DUMMY");
        Packet.w_string		(Name);
        Packet.w_u8 		(0xFE);
        Packet.w_vec3		(PPosition);
        Fvector a; a.set	(0,0,0);
        Packet.w_vec3		(a);
        Packet.w_u16		(0);
        WORD fl 			= M_SPAWN_OBJECT_ACTIVE;//(m_Flags.bActive)?M_SPAWN_OBJECT_ACTIVE:0;
        Packet.w_u16		(fl);

        u32	position		= Packet.w_tell	();
        Packet.w_u16		(0);
        // spawn info
        Packet.w_u8 		((1<<0)|(1<<1)|(1<<3));
		// verify path
		// esAnimated
        string256 anm_name;	strconcat(anm_name,Name,".anms");
        Packet.w_string		(anm_name);
        strconcat			(anm_name,Builder.m_LevelPath.m_Path,Name,".anms");
        Engine.FS.VerifyPath(anm_name);
        SaveOMotions		(anm_name);
		// esModel
        string256 mdl_name;	strconcat(mdl_name,Name,".ogf");
        Packet.w_string		(mdl_name);
        strconcat			(mdl_name,Builder.m_LevelPath.m_Path,Name,".ogf");
        if (m_pRefs->IsSkeleton()) m_pRefs->ExportSkeletonOGF(mdl_name); else m_pRefs->ExportObjectOGF(mdl_name);
		// esSound
        string256 snd_name;	strconcat(snd_name,Name,".wav");
        Packet.w_string		(snd_name);
        strconcat			(snd_name,Builder.m_LevelPath.m_Path,Name,".wav");
		Engine.FS.CreateNullFile(snd_name);
        // data size
        u16 size			= u16(Packet.w_tell()-position);
        Packet.w_seek		(position,&size,sizeof(u16));

	    F.open_chunk		(chunk_id);
        F.write				(Packet.B.data,Packet.B.count);
        F.close_chunk		();
    	return true;
    }
    return false;
}
//----------------------------------------------------

