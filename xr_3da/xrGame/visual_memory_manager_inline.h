////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const xr_vector<CVisibleObject>	&CVisualMemoryManager::memory_visible_objects() const
{
	return							(*m_objects);
}

IC	const xr_vector<CObject*>		&CVisualMemoryManager::visible_objects() const
{
	return							(m_visible_objects);
}

IC	const u32						CVisualMemoryManager::visible_object_time_last_seen(const CObject *object) const
{
	xr_vector<CVisibleObject>::iterator	I = std::find(m_objects->begin(),m_objects->end(),object_id(object));
	if (I != m_objects->end()) 
		return (I->m_level_time);	
	else 
		return u32(-1);
}

IC	void CVisualMemoryManager::set_squad_objects(xr_vector<CVisibleObject> *squad_objects)
{
	m_objects						= squad_objects;
}

IC	bool CVisualMemoryManager::visible_now	(const CGameObject *game_object) const
{
	xr_vector<CVisibleObject>::const_iterator	I = std::find(memory_visible_objects().begin(),memory_visible_objects().end(),object_id(game_object));
	return							((memory_visible_objects().end() != I) && (*I).m_visible);
}

IC	void CVisualMemoryManager::enable		(const CObject *object, bool enable)
{
	xr_vector<CVisibleObject>::iterator	J = std::find(m_objects->begin(),m_objects->end(),object_id(object));
	if (J == m_objects->end())
		return;
	(*J).m_enabled					= enable;
}

IC	const xr_vector<CNotYetVisibleObject> &CVisualMemoryManager::not_yet_visible_objects() const
{
	return							(m_not_yet_visible_objects);
}

IC	float CVisualMemoryManager::visibility_threshold	() const
{
	return							(m_visibility_threshold);
}

IC	float CVisualMemoryManager::transparency_threshold	() const
{
	return							(m_transparency_threshold);
}

IC	bool CVisualMemoryManager::enabled					() const
{
	return							(m_enabled);
}

IC	void CVisualMemoryManager::enable					(bool value)
{
	m_enabled						= value;
}
