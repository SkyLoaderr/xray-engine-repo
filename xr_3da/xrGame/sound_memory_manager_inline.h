////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_memory_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CSoundMemoryManager::update_sound_threshold			()
{
	VERIFY		(!fis_zero(m_decrease_factor));
	// t = max(t*f^((tc - tl)/tq),min_threshold)
	m_sound_threshold		= _max(
		0*m_sound_threshold*
			exp(
				float(Level().timeServer() - m_last_sound_time)/
				float(m_sound_decrease_quant)*
				log(m_decrease_factor)
				),
		m_min_sound_threshold
	);
	VERIFY		(_valid(m_sound_threshold));
}

IC	const xr_vector<MemorySpace::CSoundObject>	&CSoundMemoryManager::sound_objects() const
{
	return								(*m_sounds);
}

IC	void CSoundMemoryManager::set_sound_type_priority			(ESoundTypes sound_type, u32 priority)
{
	xr_map<ESoundTypes,u32>::const_iterator	I = m_priorities.find(sound_type);
	VERIFY							(m_priorities.end() == I);
	m_priorities.insert				(std::make_pair(sound_type,priority));
}

IC	const MemorySpace::CSoundObject *CSoundMemoryManager::sound		() const
{
	return				(m_selected_sound);
}

IC	u32	 CSoundMemoryManager::get_priority				(const MemorySpace::CSoundObject &sound) const
{
	u32					priority = u32(-1);
	xr_map<ESoundTypes,u32>::const_iterator	I = m_priorities.begin();
	xr_map<ESoundTypes,u32>::const_iterator	E = m_priorities.end();
	for ( ; I != E; ++I)
		if (((*I).second < priority) && ((*I).first & sound.m_sound_type) == (*I).first)
			priority	= (*I).second;
	return				(priority);
}

IC	void CSoundMemoryManager::set_squad_objects			(xr_vector<MemorySpace::CSoundObject> *squad_objects)
{
	m_sounds			= squad_objects;
}

IC	void CSoundMemoryManager::enable		(const CObject *object, bool enable)
{
	xr_vector<CSoundObject>::iterator	J = std::find(m_sounds->begin(),m_sounds->end(),object_id(object));
	if (J == m_sounds->end())
		return;
	(*J).m_enabled		= enable;
}

IC	void CSoundMemoryManager::set_threshold			(float threshold)
{
	m_sound_threshold	= threshold;
}

IC	void CSoundMemoryManager::restore_threshold		()
{
	m_sound_threshold	= m_min_sound_threshold;
}
