////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "memory_manager.h"

CMemoryManager::CMemoryManager		()
{
	Init							();
}

CMemoryManager::~CMemoryManager		()
{
}

void CMemoryManager::Init			()
{
}

void CMemoryManager::Load			(LPCSTR section)
{
	CVisualMemoryManager::Load		(section);
	CSoundMemoryManager::Load		(section);
	CHitMemoryManager::Load			(section);
	CEnemyManager::Load				(section);
	CItemManager::Load				(section);
}

void CMemoryManager::reinit	()
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

void CMemoryManager::update			()
{
	CVisualMemoryManager::update	();
	CSoundMemoryManager::update		();
	CHitMemoryManager::update		();
	
	// update enemies and items
	CEnemyManager::reset			();
	CItemManager::reset				();

	update							(memory_visible_objects());
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
		const CEntityAlive			*entity_alive = dynamic_cast<const CEntityAlive*>((*I).m_object);
		if (!entity_alive || !CEnemyManager::add(entity_alive))
			CItemManager::add		((*I).m_object);
	}
}

const CMemoryInfo	CMemoryManager::memory(const CObject *object) const
{
	CMemoryInfo						result;
	result.m_object					= 0;
	ALife::_TIME_ID					game_time = 0;
	const CGameObject				*game_object = dynamic_cast<const CGameObject*>(object);
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
