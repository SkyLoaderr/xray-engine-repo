////////////////////////////////////////////////////////////////////////////
//	Module 		: hit_memory_manager_inline.h
//	Created 	: 25.12.2003
//  Modified 	: 25.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Hit memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const xr_vector<CHitObject> &CHitMemoryManager::hit_objects() const
{
	return				(*m_hits);
}

IC	void CHitMemoryManager::add_hit_object	(CEntityAlive *entity_alive)
{
	add_hit_object		(0,Fvector().set(0,0,1),entity_alive,0);
}

IC	const CHitObject *CHitMemoryManager::hit() const
{
	return				(m_selected_hit);
}

IC	void CHitMemoryManager::set_squad_objects	(xr_vector<CHitObject> *squad_objects)
{
	m_hits				= squad_objects;
}
