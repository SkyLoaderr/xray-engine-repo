//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticlesObject.h"
#include "../ParticleCustom.h"
#include "../render.h"

CParticlesObject::CParticlesObject	(LPCSTR p_name, BOOL bAutoRemove)
{
	Init					(p_name,0,bAutoRemove);
}

CParticlesObject::CParticlesObject	(LPCSTR p_name, IRender_Sector* S, BOOL bAutoRemove)
{
	Init					(p_name,S,bAutoRemove);
}

void CParticlesObject::Init(LPCSTR p_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bLooped = false;
	m_bAutoRemove			= bAutoRemove;

	// create visual
	renderable.visual		= Render->model_CreateParticles(p_name);
	VERIFY					(renderable.visual);
	IParticleCustom* V		= dynamic_cast<IParticleCustom*>(renderable.visual);  VERIFY(V);
	float time_limit		= V->GetTimeLimit();
	
	if(time_limit > 0.f)
	{
		m_iLifeTime			= iFloor(time_limit*1000.f);
	}
	else
	{
		if(bAutoRemove)
		{
			R_ASSERT3			(!m_bAutoRemove,"Can't set auto-remove flag for looped particle system.",p_name);
		}
		else
		{
			m_iLifeTime = 0;
			m_bLooped = true;
		}
	}



	// spatial
	spatial.type			= 0;
	spatial.sector			= S;
	
	// sheduled
	shedule.t_min			= 20;
	shedule.t_max			= 50;
	shedule_register		();

	dwLastTime				= Device.dwTimeGlobal;
}

//----------------------------------------------------
CParticlesObject::~CParticlesObject()
{
}

LPCSTR CParticlesObject::dbg_ref_name()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	return V->Name();
	
}
//----------------------------------------------------
void CParticlesObject::Play()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->Play			();
}

void CParticlesObject::Stop()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->Stop			();
}

void CParticlesObject::play_at_pos(const Fvector& pos, BOOL xform)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	Fmatrix m; m.identity(); m.c.set(pos); 
	Fvector vel={0,0,0};
	V->UpdateParent	(m,vel,xform);
	V->Play			();
}

void CParticlesObject::shedule_Update	(u32 _dt)
{
	inherited::shedule_Update	(_dt);

	// visual
	u32 dt							= Device.dwTimeGlobal - dwLastTime;
	if (dt)							{
		IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
		V->OnFrame			(dt);
		dwLastTime			= Device.dwTimeGlobal;
	}

	// spatial	(+ workaround occasional bug inside particle-system)
	if (_valid(renderable.visual->vis.sphere))
	{
		spatial.center		= renderable.visual->vis.sphere.P;
		spatial.radius		= renderable.visual->vis.sphere.R;
		if (0==spatial.type)	{
			// First 'valid' update - register
			spatial.type						= STYPE_RENDERABLE;
			spatial_register					();
		} else {
			spatial_move		();
		}
	}

	// Msg	("update(%s): %3.1f,%3.1f,%3.1f,%3.1f",V->GetDefinition()->m_Name,VPUSH(spatial.center),spatial.radius);
}

static const Fvector zero_vel		= {0.f,0.f,0.f};
void CParticlesObject::SetXFORM		(const Fmatrix& m)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->UpdateParent		(m,zero_vel,TRUE);
}

void CParticlesObject::UpdateParent		(const Fmatrix& m, const Fvector& vel)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->UpdateParent		(m,vel,FALSE);
}

Fvector& CParticlesObject::Position		()
{
	return renderable.visual->vis.sphere.P;
}

void CParticlesObject::renderable_Render	()
{
	u32 dt							= Device.dwTimeGlobal - dwLastTime;
	if (dt)							{
		IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
		V->OnFrame			(dt);
		dwLastTime			= Device.dwTimeGlobal;
	}
	::Render->set_Transform			(&Fidentity);
	::Render->add_Visual			(renderable.visual);
}

bool CParticlesObject::IsAutoRemove()
{
	if(m_bAutoRemove) return true;
	else return false;
}
void CParticlesObject::SetAutoRemove(bool auto_remove)
{
	m_bAutoRemove = auto_remove;
}
