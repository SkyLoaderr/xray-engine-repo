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

IC	void CVisualMemoryManager::set_squad_objects(xr_vector<CVisibleObject> *squad_objects)
{
	m_objects						= squad_objects;
}

IC	bool CVisualMemoryManager::visible_now	(const CGameObject *game_object) const
{
	xr_vector<CObject*>::const_iterator	I = std::find_if(m_visible_objects.begin(),m_visible_objects.end(),CVisibleObjectPredicate(game_object->ID()));
	return							(m_visible_objects.end() != I);
}

IC	void CVisualMemoryManager::enable		(const CObject *object, bool enable)
{
	xr_vector<CVisibleObject>::iterator	J = std::find(m_objects->begin(),m_objects->end(),object_id(object));
	if (J == m_objects->end())
		return;
	(*J).m_enabled					= enable;
}
