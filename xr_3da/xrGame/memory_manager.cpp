////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "memory_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "agent_manager.h"

CMemoryManager::CMemoryManager		()
{
	init							();
}

CMemoryManager::~CMemoryManager		()
{
}

void CMemoryManager::init			()
{
}

void CMemoryManager::Load			(LPCSTR section)
{
	CVisualMemoryManager::Load		(section);
	CSoundMemoryManager::Load		(section);
	CHitMemoryManager::Load			(section);
	CEnemyManager::Load				(section);
	CItemManager::Load				(section);
	m_object						= smart_cast<CAI_Stalker*>(this);
	m_self							= smart_cast<CEntityAlive*>(this);
	VERIFY							(m_self);
}

void CMemoryManager::reinit			()
{
	CVisualMemoryManager::reinit	();
	CSoundMemoryManager::reinit		();
	CHitMemoryManager::reinit		();
	CEnemyManager::reinit			();
	CItemManager::reinit			();
}

void CMemoryManager::reload			(LPCSTR section)
{
	CVisualMemoryManager::reload	(section);
	CSoundMemoryManager::reload		(section);
	CHitMemoryManager::reload		(section);
	CEnemyManager::reload			(section);
	CItemManager::reload			(section);
}

void CMemoryManager::update			(float time_delta)
{
	CVisualMemoryManager::update	(time_delta);
	CSoundMemoryManager::update		();
	CHitMemoryManager::update		();
	
	// update enemies and items
	CEnemyManager::reset			();
	CItemManager::reset				();

	if (CVisualMemoryManager::enabled())
		update						(memory_visible_objects());

	update							(sound_objects());
	update							(hit_objects());
	
	CEnemyManager::update			();
	CItemManager::update			();
}

template <typename T>
void CMemoryManager::update			(const xr_vector<T> &objects)
{
	xr_vector<T>::const_iterator	I = objects.begin();
	xr_vector<T>::const_iterator	E = objects.end();
	for ( ; I != E; ++I) {
		if (!(*I).m_enabled)
			continue;
		
		if (m_object && !(*I).m_squad_mask.is(m_object->agent_manager().mask(m_object)))
			continue;
		
		const CEntityAlive			*entity_alive = smart_cast<const CEntityAlive*>((*I).m_object);
		if (!entity_alive || !CEnemyManager::add(entity_alive))
			CItemManager::add		((*I).m_object);
	}
}

const CMemoryInfo	CMemoryManager::memory(const CObject *object) const
{
	CMemoryInfo						result;
	result.m_object					= 0;
	if (!m_self->g_Alive())
		return						(result);

	ALife::_TIME_ID					game_time = 0;
	const CGameObject				*game_object = smart_cast<const CGameObject*>(object);
	VERIFY							(game_object);

	{
		xr_vector<CVisibleObject>::const_iterator	I = std::find(memory_visible_objects().begin(),memory_visible_objects().end(),object_id(object));
		if (memory_visible_objects().end() != I) {
			(CMemoryObject<CGameObject>&)result = (CMemoryObject<CGameObject>&)(*I);
			result.m_visible						= (*I).m_visible;
			result.m_visual_info					= true;
			game_time								= (*I).m_game_time;
			VERIFY									(result.m_object);
		}
	}

	{
		xr_vector<CSoundObject>::const_iterator	I = std::find(sound_objects().begin(),sound_objects().end(),object_id(object));
		if ((sound_objects().end() != I) && (game_time < (*I).m_game_time)) {
			(CMemoryObject<CGameObject>&)result = (CMemoryObject<CGameObject>&)(*I);
			result.m_sound_info						= true;
			game_time								= (*I).m_game_time;
			VERIFY									(result.m_object);
		}
	}
	
	{
		xr_vector<CHitObject>::const_iterator	I = std::find(hit_objects().begin(),hit_objects().end(),object_id(object));
		if ((hit_objects().end() != I) && (game_time < (*I).m_game_time)) {
			(CMemoryObject<CGameObject>&)result = (CMemoryObject<CGameObject>&)(*I);
			result.m_object							= game_object;
			result.m_hit_info						= true;
			VERIFY									(result.m_object);
		}
	}

	return		(result);
}