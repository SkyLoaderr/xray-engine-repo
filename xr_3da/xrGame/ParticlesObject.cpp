//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticlesObject.h"
#include "../ParticleCustom.h"
#include "../render.h"

const Fvector zero_vel		= {0.f,0.f,0.f};

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
	m_bStoppig = false;
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

void CParticlesObject::UpdateSpatial()
{
	// spatial	(+ workaround occasional bug inside particle-system)
	if (_valid(renderable.visual->vis.sphere))
	{
		renderable.xform.transform_tiny	(spatial.center,renderable.visual->vis.sphere.P);
		spatial.radius					= renderable.visual->vis.sphere.R;
		if (0==spatial.type)	{
			// First 'valid' update - register
			spatial.type			= STYPE_RENDERABLE;
			spatial_register		();
		} else {
			spatial_move			();
		}
	}
}

const ref_str CParticlesObject::Name()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	return V->Name();
}

//----------------------------------------------------
void CParticlesObject::Play()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->Play			();
	dwLastTime		= Device.dwTimeGlobal-33ul;
	shedule_Update	(0);

	m_bStoppig = false;
}

void CParticlesObject::play_at_pos(const Fvector& pos, BOOL xform)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	Fmatrix m; m.identity(); m.c.set(pos); 
	V->UpdateParent				(m,zero_vel,xform);
	V->Play						();
	dwLastTime					= Device.dwTimeGlobal-33ul;
	shedule_Update				(0);
	dwFrameDisableSheduleUpdate	= Device.dwFrame;
	m_bStoppig					= false;
}

void CParticlesObject::Stop(BOOL bDefferedStop)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->Stop			(bDefferedStop);

	m_bStoppig = true;
}

void CParticlesObject::shedule_Update	(u32 _dt)
{
	if (Device.dwFrame==dwFrameDisableSheduleUpdate)	return;	//. hack
	inherited::shedule_Update		(_dt);

	// visual
	u32 dt							= Device.dwTimeGlobal - dwLastTime;
	if (dt)							{
		IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
		V->OnFrame			(dt);
		dwLastTime			= Device.dwTimeGlobal;
	}

	UpdateSpatial			();



	//Msg	("update(%s): %3.1f,%3.1f,%3.1f,%3.1f",V->GetDefinition()->m_Name,VPUSH(spatial.center),spatial.radius);

	//const Fmatrix& m = XFORM();
	//Msg("%.3f %.3f %.3f", m.c.x, m.c.y, m.c.z);

}

void CParticlesObject::SetXFORM		(const Fmatrix& m)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->UpdateParent		(m,zero_vel,TRUE);
	renderable.xform.set(m);
	UpdateSpatial		();
}

void CParticlesObject::UpdateParent		(const Fmatrix& m, const Fvector& vel)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); VERIFY(V);
	V->UpdateParent		(m,vel,FALSE);
	UpdateSpatial		();
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
	::Render->set_Transform			(&renderable.xform);
	::Render->add_Visual			(renderable.visual);
}

bool CParticlesObject::IsAutoRemove()
{
	if(m_bAutoRemove) return true;
	else return false;
}
void CParticlesObject::SetAutoRemove(bool auto_remove)
{
	VERIFY(m_bStoppig || !IsLooped());
	m_bAutoRemove = auto_remove;
}

//играются ли партиклы, отличается от PSI_Alive, тем что после
//остановки Stop партиклы могут еще доигрывать анимацию IsPlaying = true
bool CParticlesObject::IsPlaying()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); 
	VERIFY(V);
	return !!V->IsPlaying();
}