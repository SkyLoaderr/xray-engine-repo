//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "psystem.h"
#include "PSLibrary.h"
#include "render.h"

using namespace PS;

CPGDef::CPGDef()
{
	m_Name[0]		= 0;
    m_Flags.zero	();
    m_fTimeLimit	= 0.f;
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
    	it->m_Time0 	= F.r_float();//float(F.r_u32())/1000.f;
    	it->m_Time1 	= F.r_float();//float(F.r_u32())/1000.f;
    	it->m_Flags.set	(F.r_u32());
    }
    
    if (F.find_chunk(PGD_CHUNK_TIME_LIMIT)){
   		m_fTimeLimit= F.r_float();
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

    F.open_chunk	(PGD_CHUNK_TIME_LIMIT);
   	F.w_float		(m_fTimeLimit);
    F.close_chunk	();
}

//------------------------------------------------------------------------------
// Particle Group part
//------------------------------------------------------------------------------
CParticleGroup::CParticleGroup()
{
	m_RT_Flags.zero			();
	m_InitialPosition.set	(0,0,0);
}

void CParticleGroup::OnFrame(u32 u_dt)
{
	if (m_Def&&m_RT_Flags.is(flRT_Playing)){
        float ct	= m_CurrentTime;
        float f_dt	= float(u_dt)/1000.f;
        for (CPGDef::EffectVec::const_iterator e_it=m_Def->m_Effects.begin(); e_it!=m_Def->m_Effects.end(); e_it++){	
            CParticleEffect* E = (CParticleEffect*)children[e_it-m_Def->m_Effects.begin()]; R_ASSERT(E);
            if (e_it->m_Flags.is(CPGDef::SEffect::flEnabled)){
                switch (e_it->m_Type){
                case CPGDef::SEffect::etStopEnd:{
                    if (E->IsPlaying()){
                        if ((ct<=e_it->m_Time1)&&(ct+f_dt>=e_it->m_Time1))	
                            E->Stop(e_it->m_Flags.is(CPGDef::SEffect::flDeferredStop));
                    }else{
                        if ((ct<=e_it->m_Time0)&&(ct+f_dt>=e_it->m_Time0))	
                            E->Play();
                    }
                }break;
                }
            }
        }
        m_CurrentTime 	+= f_dt;
        if (m_CurrentTime>m_Def->m_fTimeLimit)
            if (!m_RT_Flags.is(flRT_DefferedStop)) Stop(true);
        bool bPlaying = false;
        Fbox box; box.invalidate();
        for (u32 i=0; i<children.size(); i++){
            CParticleEffect* E	= (CParticleEffect*)children[i];
            E->OnFrame		(u_dt);
            if (E->IsPlaying()){ 
            	bPlaying	= true;
                if (E->vis.box.is_valid())
                	box.merge(E->vis.box);
            }
        }
        if (m_RT_Flags.is(flRT_DefferedStop)&&!bPlaying){
            m_RT_Flags.set		(flRT_Playing|flRT_DefferedStop,FALSE);
        }
        if (box.is_valid()){
        	vis.box.set			(box);
			vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
		}
	} else {
		vis.box.set			(m_InitialPosition,m_InitialPosition);
		vis.box.grow		(EPS_L);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}
}

void CParticleGroup::UpdateParent(const Fmatrix& m, const Fvector& velocity)
{
	m_InitialPosition		= m.c;
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
        for (CPGDef::EffectVec::const_iterator e_it=m_Def->m_Effects.begin(); e_it!=m_Def->m_Effects.end(); e_it++)
			children[e_it-def->m_Effects.begin()]	= ::Render->model_CreatePE(e_it->m_EffectName);
    }
    return TRUE;
}

void CParticleGroup::Play()
{
	m_CurrentTime   = 0;
	m_RT_Flags.set 	(flRT_Playing,TRUE);
}

void CParticleGroup::Stop(bool bFinishPlaying)
{
	if (bFinishPlaying){
		m_RT_Flags.set	(flRT_DefferedStop,TRUE);
    }else{
    	m_RT_Flags.set	(flRT_Playing,FALSE);
    }
    for (u32 i=0; i<children.size(); i++)
        ((CParticleEffect*)children[i])->Stop(bFinishPlaying);
}

