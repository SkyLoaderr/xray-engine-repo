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
#include "level_navigation_graph.h"
#include "level.h"
#include "script_game_object.h"
#include "ai_space.h"
#include "profiler.h"

#define USE_EVALUATOR

bool CEnemyManager::is_useful				(const CEntityAlive *entity_alive) const
{
	return					(m_object->useful(this,entity_alive));
}

bool CEnemyManager::useful					(const CEntityAlive *entity_alive) const
{
	if (!entity_alive->g_Alive())
		return				(false);

	if ((entity_alive->spatial.type & STYPE_VISIBLEFORAI) != STYPE_VISIBLEFORAI)
		return				(false);

	if ((m_object->ID() == entity_alive->ID()) || !m_object->is_relation_enemy(entity_alive))
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

//	if (entity_alive->CLS_ID == CLSID_OBJECT_ACTOR)
//		return				(false);

	return					(m_useful_callback ? m_useful_callback(m_object->lua_game_object(),entity_alive->lua_game_object()) : true);
}

float CEnemyManager::do_evaluate			(const CEntityAlive *object) const
{
	return					(m_object->evaluate(this,object));
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
	ai().ef_storage().non_alife().member_item() = 0;
	ai().ef_storage().non_alife().enemy_item() = 0;
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
	m_ignore_monster_threshold	= READ_IF_EXISTS(pSettings,r_float,section,"ignore_monster_threshold",1.f);
	m_max_ignore_distance		= READ_IF_EXISTS(pSettings,r_float,section,"max_ignore_distance",0.f);
	m_visible_now				= false;
	m_last_enemy_time			= 0;
	m_last_enemy				= 0;
	m_useful_callback.clear		();
	VERIFY						(m_ready_to_save);
}

void CEnemyManager::update					()
{
	START_PROFILE("AI/Memory Manager/enemies/update")

	if (!m_ready_to_save) {
//		Msg						("%6d %s DEcreased enemy counter for player (%d -> %d)",Device.dwTimeGlobal,*m_object->cName(),Level().autosave_manager().not_ready_count(),Level().autosave_manager().not_ready_count()-1);
		Level().autosave_manager().dec_not_ready();
	}

	m_ready_to_save				= true;
	m_visible_now				= false;

	inherited::update			();

	if (selected()) {
		m_last_enemy_time		= Device.dwTimeGlobal;
		m_last_enemy			= selected();
	}

	if (!m_ready_to_save) {
//		Msg						("%6d %s INcreased enemy counter for player (%d -> %d)",Device.dwTimeGlobal,*m_object->cName(),Level().autosave_manager().not_ready_count(),Level().autosave_manager().not_ready_count()+1);
		Level().autosave_manager().inc_not_ready();
	}
	STOP_PROFILE
}

void CEnemyManager::set_ready_to_save		()
{
	if (m_ready_to_save)
		return;

//	Msg							("%6d %s DEcreased enemy counter for player (%d -> %d)",Device.dwTimeGlobal,*m_object->cName(),Level().autosave_manager().not_ready_count(),Level().autosave_manager().not_ready_count()-1);
	Level().autosave_manager().dec_not_ready();
	m_ready_to_save				= true;
}

void CEnemyManager::remove_links			(CObject *object)
{
	if (!m_last_enemy)
		return;

	// since we use no members in CEntityAlive during search,
	// we just use the pinter itself, we can just statically cast object
	OBJECTS::iterator			I = std::find(m_objects.begin(),m_objects.end(),(CEntityAlive*)object);
	if (I != m_objects.end())
		m_objects.erase			(I);

	if (m_last_enemy->ID() == object->ID())
		m_last_enemy			= 0;

	if (m_selected && (m_selected->ID() == object->ID()))
		m_selected				= 0;
}

void CEnemyManager::ignore_monster_threshold			(const float &ignore_monster_threshold)
{
	m_ignore_monster_threshold	= ignore_monster_threshold;
}

void CEnemyManager::restore_ignore_monster_threshold	()
{
	m_ignore_monster_threshold	= READ_IF_EXISTS(pSettings,r_float,*m_object->cNameSect(),"ignore_monster_threshold",1.f);
}

float CEnemyManager::ignore_monster_threshold			() const
{
	return						(m_ignore_monster_threshold);
}

void CEnemyManager::max_ignore_monster_distance			(const float &max_ignore_monster_distance)
{
	m_max_ignore_distance		= max_ignore_monster_distance;
}

void CEnemyManager::restore_max_ignore_monster_distance	()
{
	m_max_ignore_distance		= READ_IF_EXISTS(pSettings,r_float,*m_object->cNameSect(),"max_ignore_distance",0.f);
}

float CEnemyManager::max_ignore_monster_distance		() const
{
	return						(m_max_ignore_distance);
}
