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
	return								(*m_objects);
}

IC	const xr_vector<CObject*>		&CVisualMemoryManager::visible_objects() const
{
	return								(m_visible_objects);
}

IC	void CVisualMemoryManager::set_squad_objects(xr_vector<CVisibleObject> *squad_objects)
{
	m_objects						= squad_objects;
}
