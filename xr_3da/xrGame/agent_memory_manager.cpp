////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_memory_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_memory_manager.h"

void CAgentMemoryManager::update		()
{
	reset_memory_masks	();
}

void CAgentMemoryManager::remove_links	(CObject *object)
{
}
