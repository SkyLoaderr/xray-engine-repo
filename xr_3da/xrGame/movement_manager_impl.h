////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_impl.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "mt_config.h"

IC	bool CMovementManager::can_use_distributed_compuations	(u32 option) const
{
	return	(!m_build_at_once && g_mt_config.test(option));
}
