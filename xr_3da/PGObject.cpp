//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PGObject.h"
#include "ParticleGroup.h"
#include "render.h"

CPGObject::CPGObject	(LPCSTR ps_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bAutoRemove			= bAutoRemove;

	// create visual
	renderable.visual		= Render->model_CreatePG(ps_name);
	VERIFY					(renderable.visual);
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual); R_ASSERT(V);
	
	if (bAutoRemove&&V->GetDefinition()->m_Flags.is(PS::CPGDef::dfTimeLimit)){
		m_iLifeTime	= V->GetDefinition()->m_TimeLimit;
	}else{
		R_ASSERT3	(!m_bAutoRemove,"Can't set auto-remove flag for looped particle system.",ps_name);
	}

	// registry
	spatial.sector	= S;

	// sheduled
	shedule.t_min			= 20;
	shedule.t_max			= 50;
}

//----------------------------------------------------
CPGObject::~CPGObject()
{
}

LPCSTR CPGObject::dbg_ref_name()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual); R_ASSERT(V);
	return V->GetDefinition()->m_Name;
	
}
//----------------------------------------------------
void CPGObject::Play()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual); R_ASSERT(V);
	V->Play			();
}

void CPGObject::Stop()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual); R_ASSERT(V);
	V->Stop			();
}

void CPGObject::play_at_pos(const Fvector& pos)
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual); R_ASSERT(V);
	Fmatrix m; m.identity(); m.c.set(pos); 
	Fvector vel={0,0,0};
	V->UpdateParent	(m,vel);
	V->Play			();
}

void CPGObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual); R_ASSERT(V);
	V->OnFrame			(dt);
}

static const Fvector zero_vel	= {0.f,0.f,0.f};
void CPGObject::SetTransform(const Fmatrix& m)
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual);	R_ASSERT(V);
	V->UpdateParent		(m,zero_vel);
}

void CPGObject::UpdateParent		(const Fmatrix& m, const Fvector& vel)
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual);	R_ASSERT(V);
	V->UpdateParent		(m,vel);
}

Fvector& CPGObject::Position		()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(renderable.visual);	R_ASSERT(V);
	return V->vis.sphere.P;
}

void CPGObject::renderable_Render	()
{
	::Render->set_Transform			(&Fidentity);
	::Render->add_Visual			(renderable.visual);
}
