////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_memory_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSoundMemoryManager::CSoundMemoryManager	(CCustomMonster *object, CSoundUserDataVisitor *visitor)
{
	VERIFY						(object);
	m_object					= object;
	VERIFY						(visitor);
	m_visitor					= visitor;
	m_max_sound_count			= 0;
}

IC	const CSoundMemoryManager::SOUNDS &CSoundMemoryManager::objects() const
{
	VERIFY						(m_sounds);
	return						(*m_sounds);
}

IC	void CSoundMemoryManager::priority			(const ESoundTypes &sound_type, u32 priority)
{
	PRIORITIES::const_iterator	I = m_priorities.find(sound_type);
	VERIFY						(m_priorities.end() == I);
	m_priorities.insert			(std::make_pair(sound_type,priority));
}

IC	const MemorySpace::CSoundObject *CSoundMemoryManager::sound		() const
{
	return						(m_selected_sound);
}

IC	void CSoundMemoryManager::set_squad_objects	(SOUNDS *squad_objects)
{
	m_sounds					= squad_objects;
}

IC	void CSoundMemoryManager::set_threshold		(float threshold)
{
	m_sound_threshold			= threshold;
}

IC	void CSoundMemoryManager::restore_threshold	()
{
	m_sound_threshold			= m_min_sound_threshold;
}
