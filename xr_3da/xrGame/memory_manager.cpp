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
#include "ai/stalker/ai_stalker.h"
#include "ai/stalker/ai_stalker_impl.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "memory_space_impl.h"
#include "ai_object_location.h"
#include "level_graph.h"

CMemoryManager::CMemoryManager		(CCustomMonster *monster, CSoundUserDataVisitor *visitor)
{
	VERIFY				(monster);
	m_object			= monster;
	m_stalker			= smart_cast<CAI_Stalker*>(monster);

	m_visual			= xr_new<CVisualMemoryManager>	(monster);
	m_sound				= xr_new<CSoundMemoryManager>	(monster, visitor);
	m_hit				= xr_new<CHitMemoryManager>		(monster);
	m_enemy				= xr_new<CEnemyManager>			(monster);
	m_item				= xr_new<CItemManager>			(monster);
	m_greeting			= xr_new<CGreetingManager>		(monster);
}

CMemoryManager::~CMemoryManager		()
{
	xr_delete			(m_visual);
	xr_delete			(m_sound);
	xr_delete			(m_hit);
	xr_delete			(m_enemy);
	xr_delete			(m_item);
	xr_delete			(m_greeting);
}

void CMemoryManager::Load			(LPCSTR section)
{
	visual().Load		(section);
	sound().Load		(section);
	hit().Load			(section);
	enemy().Load		(section);
	item().Load			(section);
	greeting().Load		(section);
}

void CMemoryManager::reinit			()
{
	visual().reinit		();
	sound().reinit		();
	hit().reinit		();
	enemy().reinit		();
	item().reinit		();
	greeting().reinit	();
}

void CMemoryManager::reload			(LPCSTR section)
{
	visual().reload		(section);
	sound().reload		(section);
	hit().reload		(section);
	enemy().reload		(section);
	item().reload		(section);
	greeting().reload	(section);
}

void CMemoryManager::update			(float time_delta)
{
	visual().update		(time_delta);
	sound().update		();
	hit().update		();
	
	// update enemies and items
	enemy().reset		();
	item().reset		();
	greeting().reset	();

	if (visual().enabled())
		update			(visual().objects());

	update				(sound().objects());
	update				(hit().objects());
	
	enemy().update		();
	item().update		();
	greeting().update	();
}

void CMemoryManager::enable			(const CObject *object, bool enable)
{
	visual().enable		(object,enable);
	sound().enable		(object,enable);
	hit().enable		(object,enable);
}

template <typename T>
void CMemoryManager::update			(const xr_vector<T> &objects)
{
	xr_vector<T>::const_iterator	I = objects.begin();
	xr_vector<T>::const_iterator	E = objects.end();
	for ( ; I != E; ++I) {
		if (!(*I).m_enabled)
			continue;
		
		if (m_stalker && !(*I).m_squad_mask.is(m_stalker->agent_manager().member().mask(m_stalker)))
			continue;
		
		const CEntityAlive			*entity_alive = smart_cast<const CEntityAlive*>((*I).m_object);
		if (entity_alive && enemy().add(entity_alive))
			continue;

		const CAI_Stalker			*stalker = smart_cast<const CAI_Stalker*>((*I).m_object);
		if (stalker && greeting().add(stalker))
			continue;

		item().add					((*I).m_object);
	}
}

const CMemoryInfo	CMemoryManager::memory(const CObject *object) const
{
	CMemoryInfo						result;
	result.m_object					= 0;
	if (!this->object().g_Alive())
		return						(result);

	ALife::_TIME_ID					game_time = 0;
	const CGameObject				*game_object = smart_cast<const CGameObject*>(object);
	VERIFY							(game_object);

	{
		xr_vector<CVisibleObject>::const_iterator	I = std::find(visual().objects().begin(),visual().objects().end(),object_id(object));
		if (visual().objects().end() != I) {
			(CMemoryObject<CGameObject>&)result = (CMemoryObject<CGameObject>&)(*I);
			result.m_visible						= (*I).m_visible;
			result.m_visual_info					= true;
			game_time								= (*I).m_game_time;
			VERIFY									(result.m_object);
		}
	}

	{
		xr_vector<CSoundObject>::const_iterator	I = std::find(sound().objects().begin(),sound().objects().end(),object_id(object));
		if ((sound().objects().end() != I) && (game_time < (*I).m_game_time)) {
			(CMemoryObject<CGameObject>&)result = (CMemoryObject<CGameObject>&)(*I);
			result.m_sound_info						= true;
			game_time								= (*I).m_game_time;
			VERIFY									(result.m_object);
		}
	}
	
	{
		xr_vector<CHitObject>::const_iterator	I = std::find(hit().objects().begin(),hit().objects().end(),object_id(object));
		if ((hit().objects().end() != I) && (game_time < (*I).m_game_time)) {
			(CMemoryObject<CGameObject>&)result = (CMemoryObject<CGameObject>&)(*I);
			result.m_object							= game_object;
			result.m_hit_info						= true;
			VERIFY									(result.m_object);
		}
	}

	return		(result);
}

void CMemoryManager::remove_links	(CObject *object)
{
	visual().remove_links	(object);
	sound().remove_links	(object);
	hit().remove_links		(object);
}
