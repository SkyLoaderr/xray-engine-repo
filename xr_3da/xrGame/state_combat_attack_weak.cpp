////////////////////////////////////////////////////////////////////////////
//	Module 		: state_combat_attack_weak.cpp
//	Created 	: 24.01.2004
//  Modified 	: 24.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Combat state attacking weak enemy
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_combat_attack_weak.h"
#include "ai/stalker/ai_stalker.h"

CStateAttackWeak::CStateAttackWeak	()
{
	Init							();
}

CStateAttackWeak::~CStateAttackWeak	()
{
}

void CStateAttackWeak::Init			()
{
}

void CStateAttackWeak::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateAttackWeak::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateAttackWeak::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateAttackWeak::initialize		()
{
	inherited::initialize			();
	m_object->set_selection_type	(CMovementManager::eSelectionTypeRandomBranching);
}

void CStateAttackWeak::execute			()
{
	VERIFY										(m_object->enemy());
	m_object->CObjectHandler::set_dest_state	(eObjectActionFire1,m_object->best_weapon());
	m_object->CSightManager::update				(eLookTypePathDirection);
	m_object->set_level_dest_vertex				(m_object->enemy()->level_vertex_id());
	m_object->CStalkerMovementManager::update	(0,0,&m_object->enemy()->Position(),0,
		CMovementManager::ePathTypeLevelPath,
		CMovementManager::eDetailPathTypeSmooth,
		eBodyStateStand,
		eMovementTypeWalk,
		eMentalStateDanger
	);
}

void CStateAttackWeak::finalize		()
{
	inherited::finalize				();
}
