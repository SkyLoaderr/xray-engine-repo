//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "particles/papi.h"
#include "particles/general.h"

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
	m_Flags.set	(flSprite,TRUE);
}
void CPGDef::pFrame(BOOL random_frame, u32 frame_count, u32 tex_width, u32 tex_height, u32 frame_width, u32 frame_height)
{
	m_Flags.set			(flFramed,TRUE);
	m_Flags.set			(flRandomFrame,random_frame);
	m_Frame.Set			(frame_count, tex_width, tex_height, frame_width, frame_height);
}
void CPGDef::pAnimate(float speed, BOOL random_playback)
{
	m_Frame.m_fSpeed	= speed;
	m_Flags.set			(flAnimated,TRUE);
	m_Flags.set			(flRandomPlayback,random_playback);
}
void CPGDef::pFrameInitExecute(ParticleGroup *group)
{
	for(int i = 0; i < group->p_count; i++){
		Particle &m = group->list[i];
        if (m_Flags.is(Particle::BIRTH)){
            if (m_Flags.is(flRandomFrame))
                m.frame	= Random.randI(m_Frame.m_iFrameCount);
            if (m_Flags.is(flAnimated)&&m_Flags.is(flRandomPlayback)&&Random.randI(2))	
                m.flags.set(Particle::ANIMATE_CCW,TRUE);
        }
    }
}
void CPGDef::pAnimateExecute(ParticleGroup *group)
{
	float speedFac = m_Frame.m_fSpeed * Device.fTimeDelta;
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
BOOL CPGDef::Load(CStream& F)
{
	R_ASSERT		(F.FindChunk(PGD_CHUNK_VERSION));
	u16 version		= F.Rword();

    if (version!=PGD_VERSION)
    	return FALSE;

	R_ASSERT		(F.FindChunk(PGD_CHUNK_NAME));
	F.RstringZ		(m_Name);

	R_ASSERT		(F.FindChunk(PGD_CHUNK_GROUPDATA));
    m_MaxParticles	= F.Rdword();
    
    R_ASSERT		(F.FindChunk(PGD_CHUNK_ACTIONLIST));
    m_ActionCount	= F.Rdword();
    m_ActionList	= xr_alloc<PAPI::PAHeader>(m_ActionCount);
    F.Read			(m_ActionList,m_ActionCount*sizeof(PAPI::PAHeader));

	F.ReadChunk		(PGD_CHUNK_FLAGS,&m_Flags);

    string256		buf;
    if (m_Flags.is(flSprite)){
        R_ASSERT	(F.FindChunk(PGD_CHUNK_SPRITE));
        F.RstringZ	(buf); m_ShaderName = xr_strdup(buf);
        F.RstringZ	(buf); m_TextureName= xr_strdup(buf);
    }

    if (m_Flags.is(flFramed)){
        R_ASSERT	(F.FindChunk(PGD_CHUNK_FRAME));
        F.Read		(&m_Frame,sizeof(SFrame));
    }

#ifdef _EDITOR    
	F.FindChunk		(PGD_CHUNK_SOURCETEXT);
    F.RstringZ		(m_SourceText);
#endif

    return TRUE;
}

void CPGDef::Save(CFS_Base& F)
{
	F.open_chunk	(PGD_CHUNK_VERSION);
	F.Wword			(PGD_VERSION);
    F.close_chunk	();

	F.open_chunk	(PGD_CHUNK_NAME);
    F.WstringZ		(m_Name);
    F.close_chunk	();

	F.open_chunk	(PGD_CHUNK_GROUPDATA);
    F.Wdword		(m_MaxParticles);
    F.close_chunk	();
    
	F.open_chunk	(PGD_CHUNK_ACTIONLIST);
    F.Wdword		(m_ActionCount);
    F.write			(m_ActionList,m_ActionCount*sizeof(PAPI::PAHeader));
    F.close_chunk	();

	F.write_chunk	(PGD_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

    if (m_Flags.is(flSprite)){
        F.open_chunk	(PGD_CHUNK_SPRITE);
        F.WstringZ		(m_ShaderName);
        F.WstringZ		(m_TextureName);
        F.close_chunk	();
    }

    if (m_Flags.is(flFramed)){
        F.open_chunk	(PGD_CHUNK_FRAME);
        F.write			(&m_Frame,sizeof(SFrame));
        F.close_chunk	();
    }

#ifdef _EDITOR    
	F.open_chunk	(PGD_CHUNK_SOURCETEXT);
    F.WstringZ		(m_SourceText.c_str());
	F.close_chunk	();
#endif
}
//------------------------------------------------------------------------------
// class CParticleGroup
//------------------------------------------------------------------------------
CParticleGroup::CParticleGroup()
{
	m_HandleGroup 		= pGenParticleGroups(1, 1);
    m_HandleActionList	= pGenActionLists();
    m_Shader			= 0;
    m_bPlaying			= TRUE;//FALSE;
    m_Def				= 0;
}
CParticleGroup::~CParticleGroup()
{
	OnDeviceDestroy		();
	pDeleteParticleGroups(m_HandleGroup);
	pDeleteActionLists	(m_HandleActionList);
}

void CParticleGroup::OnDeviceCreate()
{
	if (m_Def&&m_Def->m_ShaderName&&m_Def->m_TextureName)
    	m_Shader 		= Device.Shader.Create(m_Def->m_ShaderName,m_Def->m_TextureName);
}

void CParticleGroup::OnDeviceDestroy()
{
	Device.Shader.Delete(m_Shader);
}   

void CParticleGroup::RefreshShader()
{
	OnDeviceDestroy();
    OnDeviceCreate();
}

void CParticleGroup::UpdateParent(const Fmatrix& m, const Fvector& velocity)
{
    pSetActionListParenting(m_HandleActionList,m,velocity);
}

void CParticleGroup::OnFrame()
{
	if (m_Def&&m_bPlaying){
        pTimeStep			(Device.fTimeDelta);
        pCurrentGroup		(m_HandleGroup);
    
        // execute action list
        pCallActionList		(m_HandleActionList);
        ParticleGroup *pg 	= _GetGroupPtr(m_HandleGroup);
        // our actions
        if (m_Def->m_Flags.is(CPGDef::flFramed))    		  		m_Def->pFrameInitExecute(pg);
        if (m_Def->m_Flags.is(CPGDef::flFramed|CPGDef::flAnimated))	m_Def->pAnimateExecute	(pg);
        //-move action
        for(int i = 0; i < pg->p_count; i++){
            Particle &m 	= pg->list[i];
            if (m.flags.is(Particle::DYING)){}
            if (m.flags.is(Particle::BIRTH))m.flags.set(Particle::BIRTH,FALSE);
        }
    }
}
          
void CParticleGroup::Render()
{
/*	// Get a pointer to the particles in gp memory
	ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);

	if(pg == NULL)		return; // ERROR
	if(pg->p_count < 1)	return;

	Device.SetShader	(m_Shader);
    Device.SetTransform	(D3DTS_WORLD,Fidentity);
    CTLSprite 			m_Sprite;
    for(int i = 0; i < pg->p_count; i++){
        Particle &m = pg->list[i];

        Fvector p;
        Fcolor c;
        p.set(m.pos.x,m.pos.y,m.pos.z);
        c.set(m.color.x,m.color.y,m.color.z,m.alpha);
		if (m_Flags.is(flFramed)){
//        	||m_Flags.test(flAnimated)){
			Fvector2 lt,rb;
        	m_Animation.CalculateTC(m.frame,lt,rb);
			m_Sprite.Render(p,c.get(),m.size.x,m.rot.x,lt,rb);
        }else
			m_Sprite.Render(p,c.get(),m.size.x,m.rot.x);
//		DU::DrawCross(p,m.size.x,m.size.y,m.size.z,m.size.x,m.size.y,m.size.z,c.get(),false);
    }
*/
}

BOOL CParticleGroup::Compile(CPGDef* def)
{
	m_Def 			= def;
    if (m_Def){
    	// reset old particles
        pSetMaxParticlesG(m_HandleGroup,0);
        // set current group for action
        pCurrentGroup	(m_HandleGroup);
        // refresh shader
        RefreshShader	();
        // set max particles
        pSetMaxParticlesG(m_HandleGroup,m_Def->m_MaxParticles);
        // load action list
        // get pointer to specified action list.
        PAPI::PAHeader* pa	= _GetListPtr(m_HandleActionList);
        if(pa == NULL)	return FALSE; // ERROR

        // start append actions
        pNewActionList(m_HandleActionList);
        for (int k=0; k<m_Def->m_ActionCount; k++)
            pAddActionToList	(m_Def->m_ActionList+k);
        // end append action
        pEndActionList();
    }

	return TRUE;
}


