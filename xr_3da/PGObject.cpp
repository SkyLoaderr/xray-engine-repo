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

	// registry
	m_pCurSector	= S;
	if (S) m_pCurSector->tempAdd(this);
}

//----------------------------------------------------
CPGObject::~CPGObject()
{
}

//----------------------------------------------------
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
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual); R_ASSERT(V);
	V->OnFrame			(dt);
}

void CPGObject::UpdateParent(const Fmatrix& m, const Fvector* vel)
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual);	R_ASSERT(V);
	Fvector zero_vel	= {0.f,0.f,0.f};
	V->UpdateParent		(m,vel?*vel:zero_vel);
}

Fvector& CPGObject::Position()
{
	PS::CParticleGroup* V	= dynamic_cast<PS::CParticleGroup*>(m_pVisual);	R_ASSERT(V);
	return V->vis.sphere.P;
}
