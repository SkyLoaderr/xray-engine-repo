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
#include "xrServer_Entities.h"
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
#define SCENEOBJ_CHUNK_ACTIVE_OMOTION	0xF915
#define SCENEOBJ_CHUNK_SOUNDS			0xF920
#define SCENEOBJ_CHUNK_ACTIVE_SOUND		0xF921
//----------------------------------------------------
COMotion* CSceneObject::LoadOMotion(LPCSTR fname)
{
	if (FS.exist(fname)){
    	COMotion* M = xr_new<COMotion>();
        if (!M->LoadMotion(fname)){
        	xr_delete(M);
        }
        return M;
    }
	return 0;
}
//------------------------------------------------------------------------------

bool CSceneObject::Load(IReader& F){
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

        // object motions
        if (F.find_chunk(SCENEOBJ_CHUNK_OMOTIONS)){
            m_OMotions.resize(F.r_u32());
            for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++){
                *o_it = xr_new<COMotion>();
                if (!(*o_it)->Load(F)){
                    ELog.Msg(mtError,"SceneObject: '%s' - motions has different version. Load failed.",Name);
                    xr_delete(*o_it);
                    m_OMotions.clear();
                    break;
                }
            }
            if (F.find_chunk(SCENEOBJ_CHUNK_ACTIVE_OMOTION)){
            	string256 buf; F.r_stringZ(buf);
                SetActiveOMotion(FindOMotionByName(buf));
            }
        }

        // object sounds
        if (F.find_chunk(SCENEOBJ_CHUNK_SOUNDS)){
            m_Sounds.resize(F.r_u32());
            for (AStringIt s_it=m_Sounds.begin(); s_it!=m_Sounds.end(); s_it++){
            	string256 buf; F.r_stringZ(buf); *s_it = buf;
            }
            if (F.find_chunk(SCENEOBJ_CHUNK_ACTIVE_SOUND)){
            	string256 buf; F.r_stringZ(buf);
                SetActiveSound(buf);
            }
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

    // object motions
    if (IsOMotionable()){
	    F.open_chunk	(SCENEOBJ_CHUNK_OMOTIONS);
    	F.w_u32		(m_OMotions.size());
	    for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++) (*o_it)->Save(F);
    	F.close_chunk	();

        if (IsOMotionActive()){
	        F.open_chunk	(SCENEOBJ_CHUNK_ACTIVE_OMOTION);
    	    F.w_stringZ		(m_ActiveOMotion->Name());
    		F.close_chunk	();
        }
    }

    if (IsSoundable()){
	    F.open_chunk	(SCENEOBJ_CHUNK_SOUNDS);
    	F.w_u32		(m_Sounds.size());
	    for (AStringIt s_it=m_Sounds.begin(); s_it!=m_Sounds.end(); s_it++)
			F.w_stringZ	(s_it->c_str());
    	F.close_chunk	();

        if (IsOMotionActive()){
	        F.open_chunk	(SCENEOBJ_CHUNK_ACTIVE_SOUND);
    	    F.w_stringZ		(m_ActiveSound.c_str());
    		F.close_chunk	();
        }
    }
}
//----------------------------------------------------

bool CSceneObject::ExportGame(SExportStreams& F)
{
    if (IsDynamic()&&m_Flags.is(flDummy)){
    	R_ASSERT(m_pReference);
    	// export spawn packet
        xrSE_Dummy			dummy("m_dummy");
        strcpy(dummy.s_name_replace,Name);
        dummy.o_Position.set(PPosition);
        dummy.o_Angle.set	(PRotation);
        dummy.s_flags.set	(M_SPAWN_OBJECT_ACTIVE);
        // esAnimated=1<<0,	esModel=1<<1, esParticles=1<<2, esSound=1<<3, esRelativePosition=1<<4
        dummy.s_style		= (IsOMotionable()?(1<<0):0)|
        					 (1<<1)|
                             (IsSoundable()?(1<<3):0);
		// verify path
		// esAnimated
        if (IsOMotionable()){
	        string256 anm_name;	strconcat(anm_name,Name,".anms");
            dummy.s_Animation = xr_strdup(anm_name);
        	strconcat		(anm_name,Builder.m_LevelPath.c_str(),Name,".anms");
	        VerifyPath		(anm_name);
    	    SaveOMotions	(anm_name);
        }
		// esModel
        string256 mdl_name;	strconcat(mdl_name,Name,".ogf");
        dummy.s_Model 		= xr_strdup(mdl_name);
        strconcat			(mdl_name,Builder.m_LevelPath.c_str(),Name,".ogf");
		VerifyPath			(mdl_name);
        if (m_pReference->IsSkeleton()) m_pReference->ExportSkeletonOGF(mdl_name); else m_pReference->ExportObjectOGF(mdl_name);
		// esSound
        if (IsSoundable()){
	        string256 snd_name;	strconcat(snd_name,Name,".wav");
	        dummy.s_Sound		= xr_strdup(m_Sounds.front().c_str());
        }

        NET_Packet Packet;
        dummy.Spawn_Write	(Packet,TRUE);

	    F.spawn.stream.open_chunk	(F.spawn.chunk++);
        F.spawn.stream.w			(Packet.B.data,Packet.B.count);
        F.spawn.stream.close_chunk	();
    	return true;
    }
    return false;
}
//----------------------------------------------------

