//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PGObject.h"
#include "ParticleEffect.h"
#include "render.h"

CPGObject::CPGObject	(LPCSTR ps_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bAutoRemove			= bAutoRemove;

	// create visual
	renderable.visual		= Render->model_CreatePE(ps_name);
	VERIFY					(renderable.visual);
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	
	if (bAutoRemove&&V->GetDefinition()->m_Flags.is(PS::CPEDef::dfTimeLimit)){
		m_iLifeTime	= V->GetDefinition()->m_TimeLimit;
	}else{
		R_ASSERT3	(!m_bAutoRemove,"Can't set auto-remove flag for looped particle system.",ps_name);
	}

	// spatial
	spatial.type			= 0;
	spatial.sector			= S;

	// sheduled
	shedule.t_min			= 20;
	shedule.t_max			= 50;
	shedule_register		();

	// event
	_ui64toa				(u64(this), rm_event_desc, 16);
	rm_event				= Engine.Event.Handler_Attach	(rm_event_desc,this);
}

//----------------------------------------------------
CPGObject::~CPGObject()
{
	Engine.Event.Handler_Detach	(rm_event,this);
}

LPCSTR CPGObject::dbg_ref_name()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	return V->GetDefinition()->m_Name;
	
}
//----------------------------------------------------
void CPGObject::Play()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	V->Play			();
}

void CPGObject::Stop()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	V->Stop			();
}

void CPGObject::play_at_pos(const Fvector& pos)
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	Fmatrix m; m.identity(); m.c.set(pos); 
	Fvector vel={0,0,0};
	V->UpdateParent	(m,vel);
	V->Play			();
}

void CPGObject::shedule_Update	(u32 dt)
{
	inherited::shedule_Update	(dt);

	// visual
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	V->OnFrame			(dt);

	// spatial
	spatial.center		= V->vis.sphere.P;
	spatial.radius		= V->vis.sphere.R;
	if (0==spatial.type)	{
		spatial.type		= STYPE_RENDERABLE;
		spatial_register	();
	} else {
		spatial_move		();
	}

	// remove???
	if (m_bAutoRemove && m_iLifeTime<=0)
	{
		spatial_unregister	();
		shedule_unregister	();
		Engine.Event.Defer	(rm_event);
	}
}

void CPGObject::OnEvent(EVENT E, u64 P1, u64 P2)
{
	if (rm_event==E)	
	{
		// xr_delete	(this);
	}
}

static const Fvector zero_vel		= {0.f,0.f,0.f};
void CPGObject::SetTransform		(const Fmatrix& m)
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual);	R_ASSERT(V);
	V->UpdateParent		(m,zero_vel);
}

void CPGObject::UpdateParent		(const Fmatrix& m, const Fvector& vel)
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual);	R_ASSERT(V);
	V->UpdateParent		(m,vel);
}

Fvector& CPGObject::Position		()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual);	R_ASSERT(V);
	return V->vis.sphere.P;
}

void CPGObject::renderable_Render	()
{
	::Render->set_Transform			(&Fidentity);
	::Render->add_Visual			(renderable.visual);
}
