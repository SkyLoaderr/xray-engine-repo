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

#include "level.h"
#include "actor.h"
#include "ai/stalker/ai_stalker.h"
#include "map_location.h"

bool CEnemyManager::useful					(const CEntityAlive *entity_alive) const
{
	if (!entity_alive->g_Alive())
		return				(false);

	if ((entity_alive->spatial.type & STYPE_VISIBLEFORAI) != STYPE_VISIBLEFORAI)
		return				(false);

	if (m_self_entity_alive && ((m_self_entity_alive->ID() == entity_alive->ID()) || (m_self_entity_alive->tfGetRelationType(entity_alive) != ALife::eRelationTypeEnemy)))
		return				(false);

	if (!ai().get_level_graph() || !ai().level_graph().valid_vertex_id(entity_alive->level_vertex_id()))
		return				(false);

	if	(
			m_self_entity_alive->human_being() &&
			!entity_alive->human_being() &&
			!expedient(entity_alive) &&
			(evaluate(entity_alive) >= m_ignore_monster_threshold) &&
			(m_self_entity_alive->Position().distance_to(entity_alive->Position()) >= m_max_ignore_distance)
		)
		return				(false);

	return					(true);
}

float CEnemyManager::evaluate				(const CEntityAlive *object) const
{
	ai().ef_storage().m_tpCurrentMember = m_self_entity_alive;
	ai().ef_storage().m_tpCurrentEnemy	= object;
	return								(ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f);
//	float				distance = smart_cast<const CEntityAlive *>(this)->Position().distance_to_sqr(object->Position());
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
	m_self_entity_alive		= smart_cast<CEntityAlive*>(this);
	m_self_hit_manager		= smart_cast<CHitMemoryManager*>(this);
}

void CEnemyManager::reload					(LPCSTR section)
{
	m_ignore_monster_threshold	= 1.f;
	m_max_ignore_distance		= 0.f;

	if (pSettings->line_exist(section,"ignore_monster_threshold"))
		m_ignore_monster_threshold	= pSettings->r_float(section,"ignore_monster_threshold");
	if (pSettings->line_exist(section,"max_ignore_distance"))
		m_max_ignore_distance		= pSettings->r_float(section,"max_ignore_distance");
}


CEnemyManager::CEnemyManager		()
{
	m_actor_enemy = NULL;
}

void CEnemyManager::update					()
{
	inherited::update			();

/*	if(selected())
	{
		if(m_actor_enemy && selected()->ID() != m_actor_enemy->ID())
		{
			Level().RemoveMapLocationByID(m_self_entity_alive->ID());
		}
		else if(!m_actor_enemy)
		{
			m_actor_enemy = smart_cast<const CActor*>(selected());
			if(m_actor_enemy && smart_cast<const CAI_Stalker*>(m_self_entity_alive))
			{
				SMapLocation map_location;
				map_location.attached_to_object = true;
				map_location.object_id = m_self_entity_alive->ID();
				map_location.icon_width = map_location.icon_height = 0;	
				map_location.icon_color = 0xFFFF0000;

				map_location.icon_x = map_location.icon_y = 0;
				map_location.text = "";
				Level().AddMapLocation(map_location);
			}
		}
	}
	else if(!m_actor_enemy)
	{
		Level().RemoveMapLocationByID(m_self_entity_alive->ID());
	}*/
}
