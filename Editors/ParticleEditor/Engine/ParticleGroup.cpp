//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "particles/papi.h"
#include "particles/general.h"

using namespace PAPI;

CParticleGroup::CParticleGroup()
{
	m_HandleGroup 		= pGenParticleGroups(1, 100);
    m_HandleActionList	= pGenActionLists();
    m_Shader			= 0;
    m_ShaderName		= 0;
    m_TextureName		= 0;
    strcpy(m_Name,"unknown");
    m_Animation.InitDefault();
}
CParticleGroup::~CParticleGroup()
{
	OnDeviceDestroy		();
	pDeleteParticleGroups(m_HandleGroup);
	pDeleteActionLists	(m_HandleActionList);
}

void CParticleGroup::SetName(LPCSTR name)
{
    strcpy				(m_Name,name);
}

void CParticleGroup::OnDeviceCreate()
{
	if (m_ShaderName&&m_TextureName)
    	m_Shader 		= Device.Shader.Create(m_ShaderName,m_TextureName);
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

void CParticleGroup::pSprite(LPCSTR sh_name, LPCSTR tex_name)
{
	OnDeviceDestroy		();
    xr_free(m_ShaderName);	m_ShaderName	= xr_strdup(sh_name);
    xr_free(m_TextureName);	m_TextureName	= xr_strdup(tex_name);
	OnDeviceCreate		();
	m_Flags.set			(flSprite,TRUE);
}

// action
void CParticleGroup::pFrame(BOOL random_frame, u32 frame_count, u32 tex_width, u32 tex_height, u32 frame_width, u32 frame_height)
{
	m_Flags.set			(flFramed,TRUE);
	m_Flags.set			(flRandomFrame,random_frame);
	m_Animation.Set		(frame_count, tex_width, tex_height, frame_width, frame_height);
}
void CParticleGroup::pAnimate(float speed, BOOL random_playback)
{
	m_Animation.m_fSpeed= speed;
	m_Flags.set			(flAnimated,TRUE);
	m_Flags.set			(flRandomPlayback,random_playback);
}
void CParticleGroup::pFrameInitExecute(ParticleGroup *group)
{
	for(int i = 0; i < group->p_count; i++){
		Particle &m = group->list[i];
        if (m_Flags.is(Particle::BIRTH)){
            if (m_Flags.is(flRandomFrame))
                m.frame	= Random.randI(m_Animation.m_iFrameCount);
            if (m_Flags.is(flAnimated)&&m_Flags.is(flRandomPlayback)&&Random.randI(2))	
                m.flags.set(Particle::ANIMATE_CCW,TRUE);
        }
    }
}
void CParticleGroup::pAnimateExecute(ParticleGroup *group)
{
	float speedFac = m_Animation.m_fSpeed * Device.fTimeDelta;
	for(int i = 0; i < group->p_count; i++){
		Particle &m = group->list[i];
		m.frame						+= ((m.flags.is(Particle::ANIMATE_CCW))?-1.f:1.f)*speedFac;
		if (m.frame>m_Animation.m_iFrameCount)	m.frame-=m_Animation.m_iFrameCount;
		if (m.frame<0.f)						m.frame+=m_Animation.m_iFrameCount;
	}
}

void CParticleGroup::UpdateParent(const Fmatrix& m, const Fvector& velocity)
{
    pSetActionListParenting(m_HandleActionList,m,velocity);
}

void CParticleGroup::OnFrame()
{
    pTimeStep			(Device.fTimeDelta);
	pCurrentGroup		(m_HandleGroup);
    
    // execute action list
	pCallActionList		(m_HandleActionList);
	ParticleGroup *pg 	= _GetGroupPtr(m_HandleGroup);
    // our actions
    if (m_Flags.is(flFramed))    						pFrameInitExecute	(pg);
    if (m_Flags.is(flFramed)&&m_Flags.is(flAnimated))   pAnimateExecute		(pg);
    //-move action
	for(int i = 0; i < pg->p_count; i++){
        Particle &m 	= pg->list[i];
        if (m.flags.is(Particle::DYING)){}
        if (m.flags.is(Particle::BIRTH))m.flags.set(Particle::BIRTH,FALSE);
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

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CParticleGroup::Load(CStream& F)
{
	pCurrentGroup	(m_HandleGroup);

	string256		buf;
	R_ASSERT		(F.FindChunk(PG_CHUNK_VERSION));
	u16 version		= F.Rword();

    if (version!=PG_VERSION)
    	return FALSE;

	R_ASSERT		(F.FindChunk(PG_CHUNK_NAME));
	F.RstringZ		(buf); SetName(buf);

	R_ASSERT		(F.FindChunk(PG_CHUNK_GROUPDATA));
    u32 max_part	= F.Rdword();
    pSetMaxParticlesG(m_HandleGroup,max_part);
    
    CStream* O		= F.OpenChunk(PG_CHUNK_ACTIONLIST);	R_ASSERT(O);
	if (!LoadActionList(*O))
    	return FALSE;
    O->Close		();

	F.ReadChunk		(PG_CHUNK_FLAGS,&m_Flags);

    if (m_Flags.is(flSprite)){
        R_ASSERT	(F.FindChunk(PG_CHUNK_SPRITE));
        F.RstringZ	(buf); m_ShaderName = xr_strdup(buf);
        F.RstringZ	(buf); m_TextureName= xr_strdup(buf);
	    RefreshShader();
    }

    if (m_Flags.is(flFramed)){
        R_ASSERT	(F.FindChunk(PG_CHUNK_FRAME));
        F.Read		(&m_Animation,sizeof(SAnimation));
    }

	F.FindChunk		(PG_CHUNK_SOURCETEXT);
    F.RstringZ		(m_SourceText);

    return TRUE;
}

BOOL CParticleGroup::LoadActionList(CStream& F)
{
	// get pointer to specified action list.
	PAPI::PAHeader *pa	= _GetListPtr(m_HandleActionList);

	if(pa == NULL)
		return FALSE; // ERROR

	R_ASSERT(F.FindChunk(AL_CHUNK_VERSION));
	u16 version		= F.Rword();

	if (version!=ACTION_LIST_VERSION) 
    	return FALSE;
	
	// load actions
	R_ASSERT(F.FindChunk(AL_CHUNK_ACTIONS));
	u32 a_count			=	F.Length()/sizeof(PAPI::PAHeader);

	// start append actions
	pNewActionList(m_HandleActionList);

	PAPI::PAHeader S;
	for (u32 k=0; k<a_count; k++){
		F.Read				(&S,sizeof(PAPI::PAHeader));
		pAddActionToList	(&S);
	}
	// end append action
	pEndActionList();

	return TRUE;
}


