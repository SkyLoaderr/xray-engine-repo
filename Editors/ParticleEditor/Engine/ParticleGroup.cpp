//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "psystem.h"
#include "PSLibrary.h"

using namespace PS;

CPGDef::CPGDef()
{
	m_Name[0]		= 0;
    m_Flags.zero	();
}

CPGDef::~CPGDef()
{
}

void CPGDef::SetName(LPCSTR name)
{
    strcpy				(m_Name,name);
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPGDef::Load(IReader& F)
{
	R_ASSERT		(F.find_chunk(PGD_CHUNK_VERSION));
	u16 version		= F.r_u16();

    if (version!=PGD_VERSION){
		Log			("!Unsupported PG version. Load failed.");
    	return FALSE;
    }

	R_ASSERT		(F.find_chunk(PGD_CHUNK_NAME));
	F.r_stringZ		(m_Name);

	F.r_chunk		(PGD_CHUNK_FLAGS,&m_Flags);

	R_ASSERT		(F.find_chunk(PGD_CHUNK_EFFECTS));
    m_Effects.resize(F.r_u32());
    for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
    	F.r_stringZ		(it->m_EffectName);
    	it->m_Time0 	= F.r_float();
    	it->m_Time1 	= F.r_float();
    	it->m_Flags.set	(F.r_u32());
    }
    
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

	F.w_chunk		(PGD_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

	F.open_chunk	(PGD_CHUNK_EFFECTS);
    F.w_u32			(m_Effects.size());
    for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
    	F.w_stringZ	(it->m_EffectName);
    	F.w_float	(it->m_Time0);
    	F.w_float	(it->m_Time1);
    	F.w_u32		(it->m_Flags.get());
    }
    F.close_chunk	();
}

//------------------------------------------------------------------------------
// Particle Group part
//------------------------------------------------------------------------------
CParticleGroup::CParticleGroup()
{
	m_Flags.zero();
}

void CParticleGroup::OnFrame(u32 dt)
{
	if (m_Def&&m_Flags.is(flPlaying)){
        u32 ct	= m_CurrentTime;
        for (CPGDef::EffectVec::const_iterator e_it=m_Def->m_Effects.begin(); e_it!=m_Def->m_Effects.end(); e_it++){	
            CParticleEffect* E = (CParticleEffect*)children[e_it-m_Def->m_Effects.begin()]; R_ASSERT(E);
            switch (e_it->m_Type){
            case CPGDef::SEffect::etStopEnd:{
                if (E->IsPlaying()){
                    if ((ct<=e_it->m_Time1)&&(ct+dt>=e_it->m_Time1))	
                        E->Stop(e_it->m_Flags.is(CPGDef::SEffect::flDeferredStop));
                }else{
                    if ((ct<=e_it->m_Time0)&&(ct+dt>=e_it->m_Time0))	
                        E->Play();
                }
            }break;
	        case CPGDef::SEffect::etPeriodic:{
            }break;
            }
        }
        m_CurrentTime 	+= dt;
    }
    for (u32 i=0; i<children.size(); i++)
        ((CParticleEffect*)children[i])->OnFrame(dt);
}

void CParticleGroup::UpdateParent(const Fmatrix& m, const Fvector& velocity)
{
    for (u32 i=0; i<children.size(); i++)
        ((CParticleEffect*)children[i])->UpdateParent(m,velocity);
}

BOOL CParticleGroup::Compile(CPGDef* def)
{
	m_Def 						= def;
	// destroy existing
    for (u32 i=0; i<children.size(); i++)	
        xr_delete(children[i]);
    children.clear();
    // create new
    if (m_Def){
        children.resize(m_Def->m_Effects.size());
        for (CPGDef::EffectVec::const_iterator e_it=m_Def->m_Effects.begin(); e_it!=m_Def->m_Effects.end(); e_it++){
            CPEDef* pe_def = ::Render->PSystems.FindPED(e_it->m_EffectName);
            children[e_it-def->m_Effects.begin()]=Device.Models.CreatePE(pe_def);
        }
    }
    return TRUE;
}

void CParticleGroup::Play()
{
	m_CurrentTime   = 0;
	m_Flags.set		(flPlaying,TRUE);
}

void CParticleGroup::Stop(bool bFinishPlaying)
{
	m_Flags.set		(flPlaying,FALSE);
    for (u32 i=0; i<children.size(); i++)
        ((CParticleEffect*)children[i])->Stop(bFinishPlaying);
}

