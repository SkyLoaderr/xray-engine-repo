//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSObject.h"
#include "PSVisual.h"
#include "render.h"

CPSObject::CPSObject(LPCSTR ps_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bAutoRemove	= bAutoRemove;

	// create visual
	m_pVisual		= Render->model_CreatePS(ps_name,&m_Emitter);
	VERIFY			(m_pVisual);

	// registry
	m_pCurSector	= S;
	if (S) m_pCurSector->tempobjAdd(this);
}

//----------------------------------------------------
CPSObject::~CPSObject()
{
}

//----------------------------------------------------
void CPSObject::UpdateSector(IRender_Sector* sect)
{
	if (sect == m_pCurSector)	return;
	if (m_pCurSector) m_pCurSector->tempobjRemove(this);
	m_pCurSector	= sect;
	if (m_pCurSector) m_pCurSector->tempobjAdd(this);
}

void CPSObject::Update(u32 dt)
{
	CPSVisual* V	= (CPSVisual*)m_pVisual;
	V->Update		(dt);
	if (m_Emitter.m_Flags.is(PS_EM_PLAY_ONCE))
	{
		if ((0==V->ParticleCount())&&!m_Emitter.IsPlaying()) Stop();
	}
}

void CPSObject::play_at_pos(const Fvector& pos)
{
	m_Emitter.m_Position.set(pos);
	m_Emitter.Play			();
}

void CPSObject::Stop()
{
	m_iLifeTime		= -1;
	((CPSVisual*)m_pVisual)->Stop();
}
