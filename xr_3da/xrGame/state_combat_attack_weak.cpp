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
#include "inventory_item.h"
#include "missile.h"

CStateAttackWeak::CStateAttackWeak	(LPCSTR state_name) : inherited(state_name)
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
}

void CStateAttackWeak::execute			()
{
	inherited::execute				();
	
	if (!m_object->enemy())
		return;
	
	CMemoryInfo									mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (m_object->visible(m_object->enemy())) {
		m_object->CObjectHandler::set_dest_state(eObjectActionFire1,m_object->best_weapon());
		m_object->CSightManager::update			(eLookTypeFirePoint,&mem_object.m_object_params.m_position);
	}
	else {
		if (!dynamic_cast<CMissile*>(m_object->best_weapon()))
			m_object->CObjectHandler::set_dest_state(eObjectActionAim1,m_object->best_weapon());
		
		Fvector			direction;
		direction.sub	(mem_object.m_object_params.m_position,m_object->Position());
		m_object->CSightManager::update				(eLookTypeDirection,&direction);
	}

	if (m_object->visible(m_object->enemy()) && (m_object->Position().distance_to(m_object->enemy()->Position()) < 10.f)) {
		m_object->CStalkerMovementManager::update	(
			0,
			0,
			0,
			0,
			CMovementManager::ePathTypeLevelPath,
			CMovementManager::eDetailPathTypeSmooth,
			eBodyStateStand,
			eMovementTypeStand,
			eMentalStateDanger
		);
		return;
	}

	m_object->set_level_dest_vertex				(mem_object.m_object_params.m_level_vertex_id);
	m_object->CStalkerMovementManager::update	(
		0,
		0,
		&mem_object.m_object_params.m_position,
		0,
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
