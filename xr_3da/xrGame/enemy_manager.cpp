////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager.cpp
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "enemy_manager.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "hit_memory_manager.h"
#include "clsid_game.h"
#include "ef_storage.h"
#include "ef_pattern.h"
#include "autosave_manager.h"
#include "ai_object_location.h"
#include "level_graph.h"
#include "level.h"

#define USE_EVALUATOR

bool CEnemyManager::is_useful				(const CEntityAlive *entity_alive) const
{
	return					(m_object->useful(entity_alive));
}

bool CEnemyManager::useful					(const CEntityAlive *entity_alive) const
{
	if (!entity_alive->g_Alive())
		return				(false);

	if ((entity_alive->spatial.type & STYPE_VISIBLEFORAI) != STYPE_VISIBLEFORAI)
		return				(false);

	if ((m_object->ID() == entity_alive->ID()) || ((m_object->tfGetRelationType(entity_alive) != ALife::eRelationTypeEnemy) && (m_object->tfGetRelationType(entity_alive) != ALife::eRelationTypeWorstEnemy)))
		return				(false);

	if (!ai().get_level_graph() || !ai().level_graph().valid_vertex_id(entity_alive->ai_location().level_vertex_id()))
		return				(false);

	if	(
			m_object->human_being() &&
			!entity_alive->human_being() &&
			!expedient(entity_alive) &&
			(evaluate(entity_alive) >= m_ignore_monster_threshold) &&
			(m_object->Position().distance_to(entity_alive->Position()) >= m_max_ignore_distance)
		)
		return				(false);

	return					(true);
}

float CEnemyManager::do_evaluate			(const CEntityAlive *object) const
{
	return					(m_object->evaluate(object));
}

float CEnemyManager::evaluate				(const CEntityAlive *object) const
{
	if (object->CLS_ID == CLSID_OBJECT_ACTOR)
		m_ready_to_save					= false;

	bool				visible = m_object->memory().visual().visible_now(object);
	if (!visible && m_visible_now)
		return			(1000.f);
	
	m_visible_now		= visible;

#ifdef USE_EVALUATOR
	ai().ef_storage().non_alife().member()	= m_object;
	ai().ef_storage().non_alife().enemy()	= object;
	float				distance = m_object->Position().distance_to_sqr(object->Position());
	return				(1000.f*(visible ? 0.f : 1.f) + distance/100.f + ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f);
#else
	float				distance = m_object->Position().distance_to_sqr(object->Position());
	distance			= !fis_zero(distance) ? distance : EPS_L;
	return				(1000.f*(visible ? 0.f : 1.f) + 1.f/distance);
#endif
}

bool CEnemyManager::expedient				(const CEntityAlive *object) const
{
	ai().ef_storage().non_alife().member()		= m_object;
	VERIFY									(ai().ef_storage().non_alife().member());
	ai().ef_storage().non_alife().enemy()		= object;

	if (ai().ef_storage().m_pfExpediency->dwfGetDiscreteValue())
		return				(true);

	if (m_object->memory().hit().hit(ai().ef_storage().non_alife().enemy()))
		return				(true);
	return					(false);
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

void CEnemyManager::update					()
{
	if (!m_ready_to_save)
		Level().autosave_manager().dec_not_ready();

	m_ready_to_save				= true;
	m_visible_now				= false;

	inherited::update			();

	if (selected())
		m_last_enemy_time		= Device.dwTimeGlobal;

	if (!m_ready_to_save)
		Level().autosave_manager().inc_not_ready();
}

void CEnemyManager::set_ready_to_save		()
{
	if (m_ready_to_save)
		return;

	Level().autosave_manager().dec_not_ready();
	m_ready_to_save				= true;
}
