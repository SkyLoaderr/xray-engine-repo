//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSObject.h"
#include "PSVisual.h"
#include "fstaticrender.h"

CPSObject::CPSObject(LPCSTR ps_name, CSector* S, BOOL bAutoRemove)
{
	m_bAutoRemove	= bAutoRemove;

	// create visual
	m_pVisual		= Render.Models.CreatePS(ps_name,&m_Emitter);

	// registry
	m_pCurSector	= S;
	if (S) m_pCurSector->tempobjAdd(this);
}
//----------------------------------------------------

CPSObject::~CPSObject()
{
}
//----------------------------------------------------

void CPSObject::UpdateSector(CSector* sect){
	if (m_pCurSector) m_pCurSector->tempobjRemove(this);
	m_pCurSector	=sect;
	m_pCurSector->tempobjAdd(this);
}

void CPSObject::Update(DWORD dt){
	CPSVisual* V	= (CPSVisual*)m_pVisual;
	V->Update(dt);
	if (m_Emitter.m_dwFlag&PS_EM_PLAY_ONCE){
		if ((0==V->ParticleCount())&&!m_Emitter.IsPlaying()) Stop();
	}
}

void CPSObject::PlayAtPos(const Fvector& pos){
	m_Emitter.m_Position.set(pos);
	m_Emitter.Play();
}

void CPSObject::Stop(){
	m_iLifeTime		= -1;
	((CPSVisual*)m_pVisual)->Stop();
}