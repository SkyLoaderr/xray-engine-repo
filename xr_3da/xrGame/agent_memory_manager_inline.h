////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_memory_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CAgentMemoryManager::CAgentMemoryManager						(CAgentManager *object)
{
	VERIFY						(object);
	m_object					= object;
}

IC	void CAgentMemoryManager::set_squad_objects						(VISIBLES *objects)
{
	m_visible_objects			= objects;
}

IC	void CAgentMemoryManager::set_squad_objects						(SOUNDS *objects)
{
	m_sound_objects				= objects;
}

IC	void CAgentMemoryManager::set_squad_objects						(HITS *objects)
{
	m_hit_objects				= objects;
}

IC	CAgentMemoryManager::VISIBLES &CAgentMemoryManager::visibles	() const
{
	VERIFY						(m_visible_objects);
	return						(*m_visible_objects);
}

IC	CAgentMemoryManager::SOUNDS &CAgentMemoryManager::sounds		() const
{
	VERIFY						(m_sound_objects);
	return						(*m_sound_objects);
}

IC	CAgentMemoryManager::HITS &CAgentMemoryManager::hits			() const
{
	VERIFY						(m_hit_objects);
	return						(*m_hit_objects);
}

template <typename T>
IC	void CAgentMemoryManager::reset_memory_masks					(xr_vector<T> &objects)
{
	VISIBLES::iterator			I = m_visible_objects->begin();
	VISIBLES::iterator			E = m_visible_objects->end();
	for ( ; I != E; ++I)
		(*I).m_squad_mask.one	();
}

IC	void CAgentMemoryManager::reset_memory_masks					()
{
	reset_memory_masks			(*m_visible_objects);
	reset_memory_masks			(*m_sound_objects);
	reset_memory_masks			(*m_hit_objects);
}
