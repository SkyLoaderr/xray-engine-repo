////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_memory_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_memory_manager.h"
#include "agent_manager.h"
#include "agent_member_manager.h"

void CAgentMemoryManager::update		()
{
	reset_memory_masks	();
}

void CAgentMemoryManager::remove_links	(CObject *object)
{
}

template <typename T>
IC	void CAgentMemoryManager::reset_memory_masks					(T &objects)
{
	typename T::iterator	I = objects.begin();
	typename T::iterator	E = objects.end();
	for ( ; I != E; ++I)
		if (object().member().combat_mask() & (*I).m_squad_mask.get())
			(*I).m_squad_mask.assign((*I).m_squad_mask.get() | object().member().combat_mask());
}

void CAgentMemoryManager::reset_memory_masks						()
{
	reset_memory_masks		(visibles());
	reset_memory_masks		(sounds());
	reset_memory_masks		(hits());
}

template <typename T>
IC	void CAgentMemoryManager::update_memory_masks	(const squad_mask_type &mask, T &objects)
{
	typename T::iterator	I = objects.begin();
	typename T::iterator	E = objects.end();
	for ( ; I != E; ++I) {
		squad_mask_type		m = (*I).m_squad_mask.get();
		update_memory_mask	(mask,m);
		(*I).m_squad_mask.assign(m);
	}
}

void CAgentMemoryManager::update_memory_masks		(const squad_mask_type &mask)
{
	update_memory_masks		(mask,visibles());
	update_memory_masks		(mask,sounds());
	update_memory_masks		(mask,hits());
}
