////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager.cpp
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "enemy_manager.h"
#include "ef_storage.h"

bool CEnemyManager::useful					(const CEntityAlive *entity_alive) const
{
	if (!entity_alive->g_Alive())
		return				(false);

	return					(true);
}

float CEnemyManager::evaluate				(const CEntityAlive *object) const
{
	ai().ef_storage().m_tpCurrentMember = dynamic_cast<const CEntityAlive *>(this);
	ai().ef_storage().m_tpCurrentEnemy	= dynamic_cast<const CEntityAlive *>(this);
	return								(ai().ef_storage().m_pfVictoryProbability->ffGetValue());
}