////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager.cpp
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "enemy_manager.h"
#include "ef_storage.h"
#include "hit_memory_manager.h"

bool CEnemyManager::useful					(const CEntityAlive *entity_alive) const
{
	if (!entity_alive->g_Alive())
		return				(false);

	if ((entity_alive->spatial.type & STYPE_VISIBLEFORAI) != STYPE_VISIBLEFORAI)
		return				(false);

	if (m_self_entity_alive && (m_self_entity_alive->tfGetRelationType(entity_alive) != ALife::eRelationTypeEnemy))
		return				(false);

	if (m_self_entity_alive->human_being() && !entity_alive->human_being() && !expedient(entity_alive) && (evaluate(entity_alive) >= m_ignore_monster_threshold))
		return				(false);

	return					(true);
}

float CEnemyManager::evaluate				(const CEntityAlive *object) const
{
	ai().ef_storage().m_tpCurrentMember = m_self_entity_alive;
	ai().ef_storage().m_tpCurrentEnemy	= object;
	return								(ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f);
//	float				distance = dynamic_cast<const CEntityAlive *>(this)->Position().distance_to_sqr(object->Position());
//	distance			= !fis_zero(distance) ? distance : EPS_L;
//	return				(1.f/distance);
}

bool CEnemyManager::expedient				(const CEntityAlive *object) const
{
	ai().ef_storage().m_tpCurrentMember		= m_self_entity_alive;
	VERIFY									(ai().ef_storage().m_tpCurrentMember);
	ai().ef_storage().m_tpCurrentEnemy		= object;

	if (ai().ef_storage().m_pfExpediency->dwfGetDiscreteValue())
		return				(true);

	VERIFY					(m_self_hit_manager);
	if (m_self_hit_manager->hit(ai().ef_storage().m_tpCurrentEnemy))
		return				(true);
	return					(false);
}

void CEnemyManager::Load					(LPCSTR section)
{
	m_self_entity_alive		= dynamic_cast<CEntityAlive*>(this);
	m_self_hit_manager		= dynamic_cast<CHitMemoryManager*>(this);
}

void CEnemyManager::reload					(LPCSTR section)
{
	m_ignore_monster_threshold	= 1.f;
	if (pSettings->line_exist(section,"ignore_monster_threshold"))
		m_ignore_monster_threshold	= pSettings->r_float(section,"ignore_monster_threshold");
}
