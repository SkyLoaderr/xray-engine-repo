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

	const CEntityAlive		*self = dynamic_cast<const CEntityAlive*>(this);
	if (self && (self->tfGetRelationType(entity_alive) != ALife::eRelationTypeEnemy))
		return				(false);

	if (!entity_alive->human_being() && !expedient(entity_alive) && (evaluate(entity_alive) >= .8f)) {
		Msg					("Object %s is ignored\n",*entity_alive->cName());
		return				(false);
	}

	return					(true);
}

float CEnemyManager::evaluate				(const CEntityAlive *object) const
{
	ai().ef_storage().m_tpCurrentMember = dynamic_cast<const CEntityAlive *>(this);
	ai().ef_storage().m_tpCurrentEnemy	= dynamic_cast<const CEntityAlive *>(object);
	return								(ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f);
//	float				distance = dynamic_cast<const CEntityAlive *>(this)->Position().distance_to_sqr(object->Position());
//	distance			= !fis_zero(distance) ? distance : EPS_L;
//	return				(1.f/distance);
}

bool CEnemyManager::expedient				(const CEntityAlive *object) const
{
	if (!selected())
		return				(false);

	ai().ef_storage().m_tpCurrentMember		= dynamic_cast<const CEntityAlive *>(this);
	VERIFY									(ai().ef_storage().m_tpCurrentMember);
	ai().ef_storage().m_tpCurrentEnemy		= selected();

	if (ai().ef_storage().m_pfExpediency->dwfGetDiscreteValue())
		return				(true);

	const CHitMemoryManager	*hit_manager	= dynamic_cast<const CHitMemoryManager*>(this);
	VERIFY					(hit_manager);
	if (hit_manager->hit(ai().ef_storage().m_tpCurrentEnemy))
		return				(true);
	return					(false);
}