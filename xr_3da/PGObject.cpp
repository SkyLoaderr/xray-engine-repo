//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PGObject.h"
#include "ParticleGroup.h"
#include "render.h"

CPGObject::CPGObject(LPCSTR ps_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bAutoRemove	= bAutoRemove;

	// create visual
	m_pVisual		= Render->model_CreatePG(ps_name);
	VERIFY			(m_pVisual);
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual); R_ASSERT(V);
	
	if (bAutoRemove&&V->GetDefinition()->m_Flags.is(PS::CPGDef::dfTimeLimit)){
		m_iLifeTime	= V->GetDefinition()->m_TimeLimit;
	}else{
		R_ASSERT3	(!m_bAutoRemove,"Can't set auto-remove flag for looped particle system.",ps_name);
	}

	// registry
	m_pCurSector	= S;
	if (S) m_pCurSector->tempAdd(this);

	// sheduled
	shedule_Min			= 20;
	shedule_Max			= 100;
}

//----------------------------------------------------
CPGObject::~CPGObject()
{
	if (m_pCurSector) m_pCurSector->tempRemove	(this);
}

LPCSTR CPGObject::dbg_ref_name()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual); R_ASSERT(V);
	return V->GetDefinition()->m_Name;
	
}
//----------------------------------------------------
void CPGObject::Play()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual); R_ASSERT(V);
	V->Play			();
}

void CPGObject::Stop()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual); R_ASSERT(V);
	V->Stop			();
}

void CPGObject::play_at_pos(const Fvector& pos)
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual); R_ASSERT(V);
	Fmatrix m; m.identity(); m.c.set(pos); 
	Fvector vel={0,0,0};
	V->UpdateParent	(m,vel);
	V->Play			();
}

void CPGObject::UpdateSector(IRender_Sector* sect)
{
	if (sect == m_pCurSector)	return;
	if (m_pCurSector) m_pCurSector->tempRemove	(this);
	m_pCurSector	= sect;
	if (m_pCurSector) m_pCurSector->tempAdd		(this);
}

void CPGObject::Update(u32 dt)
{
	inherited::Update(dt);
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual); R_ASSERT(V);
	V->OnFrame			(dt);
}

static const Fvector zero_vel	= {0.f,0.f,0.f};
void CPGObject::SetTransform(const Fmatrix& m)
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual);	R_ASSERT(V);
	V->UpdateParent		(m,zero_vel);
}

void CPGObject::UpdateParent(const Fmatrix& m, const Fvector& vel)
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual);	R_ASSERT(V);
	V->UpdateParent		(m,vel);
}

Fvector& CPGObject::Position()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual);	R_ASSERT(V);
	return V->vis.sphere.P;
}
