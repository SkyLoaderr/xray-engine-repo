////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_particles.h"
#include "../objectanimator.h"

CScriptParticlesCustom::CScriptParticlesCustom(LPCSTR caParticlesName):CParticlesObject(caParticlesName,FALSE)
{
	m_animator					= 0;
}
CScriptParticlesCustom::~CScriptParticlesCustom()
{
	xr_delete					(m_animator);
}
void CScriptParticlesCustom::shedule_Update(u32 _dt)
{
	CParticlesObject::shedule_Update(_dt);
	if (m_animator){
		float dt				= float(_dt)/1000.f; 
		Fvector prev_pos		= m_animator->XFORM().c;
		m_animator->Update		(dt);
		Fvector vel;
		vel.sub					(m_animator->XFORM().c,prev_pos).div(dt);
		UpdateParent			(m_animator->XFORM(),vel);
	}
}
void CScriptParticlesCustom::LoadPath(LPCSTR caPathName)
{
	if (!m_animator || (0!=xr_strcmp(m_animator->Name(),caPathName))){
		xr_delete				(m_animator);
		m_animator				= xr_new<CObjectAnimator>();
		m_animator->Load		(caPathName);
	}
}
void CScriptParticlesCustom::StartPath(bool looped)
{
	VERIFY						(m_animator);
	m_animator->Play			(looped);
}
void CScriptParticlesCustom::PausePath(bool val)
{
	VERIFY						(m_animator);
	m_animator->Pause			(val);
}
void CScriptParticlesCustom::StopPath()
{
	VERIFY						(m_animator);
	m_animator->Stop			();
}

CScriptParticles::CScriptParticles(LPCSTR caParticlesName)
{
	m_particles					= xr_new<CScriptParticlesCustom>(caParticlesName);
}

CScriptParticles::~CScriptParticles()
{
	VERIFY						(m_particles);
	// destroy particles
	m_particles->PSI_destroy	();
	m_particles					= 0;
}

void CScriptParticles::Play()
{
	VERIFY						(m_particles);
	m_particles->Play			();
}

void CScriptParticles::PlayAtPos(const Fvector &position)
{
	VERIFY						(m_particles);
	m_particles->play_at_pos	(position);
}

void CScriptParticles::Stop		()
{
	VERIFY						(m_particles);
	m_particles->Stop			(FALSE);
}

void CScriptParticles::StopDeffered()
{
	VERIFY						(m_particles);
	m_particles->Stop			(TRUE);
}

void CScriptParticles::MoveTo	(const Fvector &pos, const Fvector& vel)
{
	VERIFY						(m_particles);
	Fmatrix						XF;
	XF.translate				(pos);
	m_particles->UpdateParent	(XF,vel);
}

bool CScriptParticles::IsPlaying() const
{
	VERIFY						(m_particles);
	return m_particles->IsPlaying();
}

bool CScriptParticles::IsLooped	() const
{
	VERIFY						(m_particles);
	return m_particles->IsLooped();
}

void CScriptParticles::LoadPath(LPCSTR caPathName)
{
	VERIFY						(m_particles);
	m_particles->LoadPath		(caPathName);
}
void CScriptParticles::StartPath(bool looped)
{
	m_particles->StartPath		(looped);
}
void CScriptParticles::StopPath	()
{
	m_particles->StopPath		();
}
void CScriptParticles::PausePath(bool val)
{
	m_particles->PausePath		(val);
}
