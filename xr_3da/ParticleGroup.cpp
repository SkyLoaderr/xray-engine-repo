//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "psystem.h"

using namespace PAPI;
using namespace PS;

//------------------------------------------------------------------------------
// class CParticleGroupDef
//------------------------------------------------------------------------------
CPGDef::CPGDef()
{
    strcpy				(m_Name,"unknown");
    m_ShaderName		= 0;
    m_TextureName		= 0;
    m_Frame.InitDefault	();
    m_ActionCount		= 0;
    m_ActionList		= 0;
    m_MaxParticles		= 0;
	m_CachedShader		= 0;
	m_TimeLimit			= 0;
}
CPGDef::~CPGDef()
{
    xr_free				(m_ShaderName);
    xr_free				(m_TextureName);
    xr_free				(m_ActionList);
}
void CPGDef::SetName(LPCSTR name)
{
    strcpy				(m_Name,name);
}
void CPGDef::pSprite(string64& sh_name, string64& tex_name)
{
    xr_free(m_ShaderName);	m_ShaderName		= xr_strdup(sh_name);
    xr_free(m_TextureName);	m_TextureName	= xr_strdup(tex_name);
	m_Flags.set	(dfSprite,TRUE);
}
void CPGDef::pFrame(BOOL random_frame, u32 frame_count, u32 tex_width, u32 tex_height, u32 frame_width, u32 frame_height)
{
	m_Flags.set			(dfFramed,TRUE);
	m_Flags.set			(dfRandomFrame,random_frame);
	m_Frame.Set			(frame_count, (float)tex_width, (float)tex_height, (float)frame_width, (float)frame_height);
}
void CPGDef::pAnimate(float speed, BOOL random_playback)
{
	m_Frame.m_fSpeed	= speed;
	m_Flags.set			(dfAnimated,TRUE);
	m_Flags.set			(dfRandomPlayback,random_playback);
}
void CPGDef::pTimeLimit(float time_limit)
{
	m_Flags.set			(dfTimeLimit,TRUE);
	m_TimeLimit			= iFloor(time_limit*1000.f);
}
void CPGDef::pFrameInitExecute(ParticleGroup *group)
{
	for(int i = 0; i < group->p_count; i++){
		Particle &m = group->list[i];
        if (m.flags.is(Particle::BIRTH)){
            if (m_Flags.is(dfRandomFrame))
                m.frame	= (float)Random.randI(m_Frame.m_iFrameCount);
            if (m_Flags.is(dfAnimated)&&m_Flags.is(dfRandomPlayback)&&Random.randI(2))
                m.flags.set(Particle::ANIMATE_CCW,TRUE);
        }
    }
}
void CPGDef::pAnimateExecute(ParticleGroup *group, float dt)
{
	float speedFac = m_Frame.m_fSpeed * dt;
	for(int i = 0; i < group->p_count; i++){
		Particle &m = group->list[i];
		m.frame						+= ((m.flags.is(Particle::ANIMATE_CCW))?-1.f:1.f)*speedFac;
		if (m.frame>m_Frame.m_iFrameCount)	m.frame-=m_Frame.m_iFrameCount;
		if (m.frame<0.f)					m.frame+=m_Frame.m_iFrameCount;
	}
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPGDef::Load(IReader& F)
{
	R_ASSERT		(F.find_chunk(PGD_CHUNK_VERSION));
	u16 version		= F.r_u16();

    if (version!=PGD_VERSION)
    	return FALSE;

	R_ASSERT		(F.find_chunk(PGD_CHUNK_NAME));
	F.r_stringZ		(m_Name);

	R_ASSERT		(F.find_chunk(PGD_CHUNK_GROUPDATA));
    m_MaxParticles	= F.r_u32();

    R_ASSERT		(F.find_chunk(PGD_CHUNK_ACTIONLIST));
    m_ActionCount	= F.r_u32();
    m_ActionList	= xr_alloc<PAPI::PAHeader>(m_ActionCount);
    F.r				(m_ActionList,m_ActionCount*sizeof(PAPI::PAHeader));

	F.r_chunk		(PGD_CHUNK_FLAGS,&m_Flags);

    string256		buf;
    if (m_Flags.is(dfSprite)){
        R_ASSERT	(F.find_chunk(PGD_CHUNK_SPRITE));
        F.r_stringZ	(buf); m_ShaderName = xr_strdup(buf);
        F.r_stringZ	(buf); m_TextureName= xr_strdup(buf);
    }

    if (m_Flags.is(dfFramed)){
        R_ASSERT	(F.find_chunk(PGD_CHUNK_FRAME));
        F.r			(&m_Frame,sizeof(SFrame));
    }

    if (m_Flags.is(dfTimeLimit)){
        R_ASSERT	(F.find_chunk(PGD_CHUNK_TIMELIMIT));
        m_TimeLimit	= F.r_u32();
    }
    
#ifdef _PARTICLE_EDITOR
	F.find_chunk		(PGD_CHUNK_SOURCETEXT);
    F.r_stringZ		(m_SourceText);
#endif

    return TRUE;
}

void CPGDef::Save(IWriter& F)
{
	F.open_chunk	(PGD_CHUNK_VERSION);
	F.w_u16			(PGD_VERSION);
    F.close_chunk	();

	F.open_chunk	(PGD_CHUNK_NAME);
    F.w_stringZ		(m_Name);
    F.close_chunk	();

	F.open_chunk	(PGD_CHUNK_GROUPDATA);
    F.w_u32			(m_MaxParticles);
    F.close_chunk	();

	F.open_chunk	(PGD_CHUNK_ACTIONLIST);
    F.w_u32			(m_ActionCount);
    F.w				(m_ActionList,m_ActionCount*sizeof(PAPI::PAHeader));
    F.close_chunk	();

	F.w_chunk		(PGD_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

    if (m_Flags.is(dfSprite)){
        F.open_chunk	(PGD_CHUNK_SPRITE);
        F.w_stringZ		(m_ShaderName);
        F.w_stringZ		(m_TextureName);
        F.close_chunk	();
    }

    if (m_Flags.is(dfFramed)){
        F.open_chunk	(PGD_CHUNK_FRAME);
        F.w				(&m_Frame,sizeof(SFrame));
        F.close_chunk	();
    }

    if (m_Flags.is(dfTimeLimit)){
        F.open_chunk	(PGD_CHUNK_TIMELIMIT);
        F.w_u32			(m_TimeLimit);
        F.close_chunk	();
    }
#ifdef _PARTICLE_EDITOR
	F.open_chunk	(PGD_CHUNK_SOURCETEXT);
    F.w_stringZ		(m_SourceText.c_str());
	F.close_chunk	();
#endif
}
//------------------------------------------------------------------------------
// class CParticleGroup
//------------------------------------------------------------------------------
CParticleGroup::CParticleGroup():IVisual()
{
	m_HandleGroup 		= pGenParticleGroups(1, 1);
    m_HandleActionList	= pGenActionLists();
    m_Flags.zero		();
    m_Def				= 0;
    m_ElapsedLimit		= 0;
	m_MemDT				= 0;
}
CParticleGroup::~CParticleGroup()
{
	OnDeviceDestroy		();
	pDeleteParticleGroups(m_HandleGroup);
	pDeleteActionLists	(m_HandleActionList);
}

void CParticleGroup::OnDeviceCreate()
{
	hGeom				= Device.Shader.CreateGeom	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
}

void CParticleGroup::OnDeviceDestroy()
{
	Device.Shader.DeleteGeom(hGeom);
}

void CParticleGroup::Play()
{
	m_Flags.set			(flPlaying,TRUE);
   	pStartPlaying		(m_HandleActionList);
}
void CParticleGroup::Stop(bool bFinishPlaying)
{
	if (bFinishPlaying){
    	pStopPlaying	(m_HandleActionList);
    }else{
    	m_Flags.set		(flPlaying,FALSE);
		ResetParticles	();
    }
}
void CParticleGroup::RefreshShader()
{
	OnDeviceDestroy();
    OnDeviceCreate();
}

void CParticleGroup::ResetParticles()
{
	pSetMaxParticlesG	(m_HandleGroup,0);
	pSetMaxParticlesG	(m_HandleGroup,m_Def->m_MaxParticles);
}
void CParticleGroup::UpdateParent(const Fmatrix& m, const Fvector& velocity)
{
	pSetActionListParenting(m_HandleActionList,m,velocity);
}

static const u32	uDT_STEP = 33;
static const float	fDT_STEP = float(uDT_STEP)/1000.f;
void CParticleGroup::OnFrame(u32 frame_dt)
{
	if (m_Def&&m_Flags.is(flPlaying)){
		m_MemDT			+= frame_dt;
		for (;m_MemDT>=uDT_STEP; m_MemDT-=uDT_STEP){
    		if (m_Def->m_Flags.is(CPGDef::dfTimeLimit)){ 
        		m_ElapsedLimit 	-= uDT_STEP;
				if (m_ElapsedLimit<0){
            		m_ElapsedLimit 	= m_Def->m_TimeLimit;
                    Stop		(true);
					return;
				}
			}
			pTimeStep			(fDT_STEP);
			pCurrentGroup		(m_HandleGroup);

			// execute action list
			pCallActionList		(m_HandleActionList);
			ParticleGroup *pg 	= _GetGroupPtr(m_HandleGroup);
			// our actions
			if (m_Def->m_Flags.is(CPGDef::dfFramed))    		  		m_Def->pFrameInitExecute(pg);
			if (m_Def->m_Flags.is(CPGDef::dfFramed|CPGDef::dfAnimated))	m_Def->pAnimateExecute	(pg,fDT_STEP);
			//-move action
			vis.box.invalidate	();
			float p_size = 0.f;
			for(int i = 0; i < pg->p_count; i++){
				Particle &m 	= pg->list[i]; 
				if (m.flags.is(Particle::DYING)){}
				if (m.flags.is(Particle::BIRTH))m.flags.set(Particle::BIRTH,FALSE);
				vis.box.modify((Fvector&)m.pos);
				if (m.size.x>p_size) p_size = m.size.x;
			}
			vis.box.grow		(p_size);
			vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
		}
	}
}

BOOL CParticleGroup::Compile(CPGDef* def)
{
	m_Def 						= def;
    if (m_Def){
        // set current group for action
        pCurrentGroup			(m_HandleGroup);
        // refresh shader
        RefreshShader			();
        // reset particles
        ResetParticles			();
        // load action list
        // get pointer to specified action list.
        PAPI::PAHeader* pa		= _GetListPtr(m_HandleActionList);
        if(pa == NULL)	
			return FALSE; // ERROR

        // start append actions
        pNewActionList			(m_HandleActionList);
        for (int k=0; k<m_Def->m_ActionCount; k++)
            pAddActionToList	(m_Def->m_ActionList+k);
        // end append action
        pEndActionList();
        // time limit
		if (m_Def->m_Flags.is(CPGDef::dfTimeLimit))
			m_ElapsedLimit 		= m_Def->m_TimeLimit;
    }
	hShader 					= def?def->m_CachedShader:0;
	return TRUE;
}


