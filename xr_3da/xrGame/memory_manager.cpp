////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "sound_memory_manager.h"
#include "hit_memory_manager.h"
#include "enemy_manager.h"
#include "item_manager.h"
#include "greeting_manager.h"
#include "danger_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/stalker/ai_stalker_impl.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "memory_space_impl.h"
#include "ai_object_location.h"
#include "level_navigation_graph.h"
#include "profiler.h"

#define STEALTH_MODE

#ifdef STEALTH_MODE
	const bool update_value = false;
#else
	const bool update_value = true;
#endif // STEALTH_MODE


CMemoryManager::CMemoryManager		(CCustomMonster *monster, CSound_UserDataVisitor *visitor)
{
	VERIFY				(monster);
	m_object			= monster;
	m_stalker			= smart_cast<CAI_Stalker*>(monster);

	m_visual			= xr_new<CVisualMemoryManager>	(monster, m_stalker);
	m_sound				= xr_new<CSoundMemoryManager>	(monster, m_stalker, visitor);
	m_hit				= xr_new<CHitMemoryManager>		(monster, m_stalker);
	m_enemy				= xr_new<CEnemyManager>			(monster);
	m_item				= xr_new<CItemManager>			(monster);
	m_greeting			= xr_new<CGreetingManager>		(monster);
	m_danger			= xr_new<CDangerManager>		(monster);
}

CMemoryManager::~CMemoryManager		()
{
	xr_delete			(m_visual);
	xr_delete			(m_sound);
	xr_delete			(m_hit);
	xr_delete			(m_enemy);
	xr_delete			(m_item);
	xr_delete			(m_greeting);
	xr_delete			(m_danger);
}

void CMemoryManager::Load			(LPCSTR section)
{
	visual().Load		(section);
	sound().Load		(section);
	hit().Load			(section);
	enemy().Load		(section);
	item().Load			(section);
	greeting().Load		(section);
	danger().Load		(section);
}

void CMemoryManager::reinit			()
{
	visual().reinit		();
	sound().reinit		();
	hit().reinit		();
	enemy().reinit		();
	item().reinit		();
	greeting().reinit	();
	danger().reinit		();
}

void CMemoryManager::reload			(LPCSTR section)
{
	visual().reload		(section);
	sound().reload		(section);
	hit().reload		(section);
	enemy().reload		(section);
	item().reload		(section);
	greeting().reload	(section);
	danger().reload		(section);
}

void CMemoryManager::update			(float time_delta)
{
	START_PROFILE("AI/Memory Manager/update")

	visual().update		(time_delta);
	sound().update		();
	hit().update		();
	
	// update enemies and items
	enemy().reset		();
	item().reset		();
	greeting().reset	();

	if (visual().enabled())
		update			(visual().objects(),true);

	update				(sound().objects(),update_value);
	update				(hit().objects(),update_value);
	
	enemy().update		();
	item().update		();
	greeting().update	();
	danger().update		();
	
	STOP_PROFILE
}

void CMemoryManager::enable			(const CObject *object, bool enable)
{
	visual().enable		(object,enable);
	sound().enable		(object,enable);
	hit().enable		(object,enable);
}

template <typename T>
void CMemoryManager::update			(const xr_vector<T> &objects, bool add_enemies)
{
	xr_vector<T>::const_iterator	I = objects.begin();
	xr_vector<T>::const_iterator	E = objects.end();
	for ( ; I != E; ++I) {
		if (!(*I).m_enabled)
			continue;
		
		if (m_stalker && !(*I).m_squad_mask.test(m_stalker->agent_manager().member().mask(m_stalker)))
			continue;

		danger().add				(*I);
		
		if (add_enemies) {
			const CEntityAlive		*entity_alive = smart_cast<const CEntityAlive*>((*I).m_object);
			if (entity_alive && enemy().add(entity_alive))
				continue;
		}

		const CAI_Stalker			*stalker = smart_cast<const CAI_Stalker*>((*I).m_object);
		if (m_stalker && stalker && greeting().add(stalker))
			continue;

		if ((*I).m_object)
			item().add				((*I).m_object);
	}
}

CMemoryInfo CMemoryManager::memory(const CObject *object) const
{
	CMemoryInfo						result;
	if (!this->object().g_Alive())
		return						(result);

	u32								level_time = 0;
	const CGameObject				*game_object = smart_cast<const CGameObject*>(object);
	VERIFY							(game_object);

	{
		xr_vector<CVisibleObject>::const_iterator	I = std::find(visual().objects().begin(),visual().objects().end(),object_id(object));
		if (visual().objects().end() != I) {
			(CMemoryObject<CGameObject>&)result	= (CMemoryObject<CGameObject>&)(*I);
			result.m_visible					= (*I).m_visible;
			result.m_visual_info				= true;
			level_time							= (*I).m_level_time;
			VERIFY								(result.m_object);
		}
	}

	{
		xr_vector<CSoundObject>::const_iterator	I = std::find(sound().objects().begin(),sound().objects().end(),object_id(object));
		if ((sound().objects().end() != I) && (level_time < (*I).m_level_time)) {
			(CMemoryObject<CGameObject>&)result = (CMemoryObject<CGameObject>&)(*I);
			result.m_sound_info						= true;
			level_time								= (*I).m_level_time;
			VERIFY									(result.m_object);
		}
	}
	
	{
		xr_vector<CHitObject>::const_iterator	I = std::find(hit().objects().begin(),hit().objects().end(),object_id(object));
		if ((hit().objects().end() != I) && (level_time < (*I).m_level_time)) {
			(CMemoryObject<CGameObject>&)result = (CMemoryObject<CGameObject>&)(*I);
			result.m_object							= game_object;
			result.m_hit_info						= true;
			VERIFY									(result.m_object);
		}
	}

	return		(result);
}

u32 CMemoryManager::memory_time(const CObject *object) const
{
	u32					result = 0;
	if (!this->object().g_Alive())
		return			(0);

	const CGameObject	*game_object = smart_cast<const CGameObject*>(object);
	VERIFY				(game_object);

	{
		xr_vector<CVisibleObject>::const_iterator	I = std::find(visual().objects().begin(),visual().objects().end(),object_id(object));
		if (visual().objects().end() != I)
			result		= (*I).m_level_time;
	}

	{
		xr_vector<CSoundObject>::const_iterator	I = std::find(sound().objects().begin(),sound().objects().end(),object_id(object));
		if ((sound().objects().end() != I) && (result < (*I).m_level_time))
			result		= (*I).m_level_time;
	}
	
	{
		xr_vector<CHitObject>::const_iterator	I = std::find(hit().objects().begin(),hit().objects().end(),object_id(object));
		if ((hit().objects().end() != I) && (result < (*I).m_level_time))
			result		= (*I).m_level_time;
	}

	return				(result);
}

void CMemoryManager::remove_links	(CObject *object)
{
	if (m_object->g_Alive()) {
		visual().remove_links	(object);
		sound().remove_links	(object);
		hit().remove_links		(object);
	}

	danger().remove_links		(object);
	enemy().remove_links		(object);
	item().remove_links			(object);
}

void CMemoryManager::on_restrictions_change	()
{
	if (!m_object->g_Alive())
		return;

//	danger().on_restrictions_change	();
//	enemy().on_restrictions_change	();
	item().on_restrictions_change	();
}
