////////////////////////////////////////////////////////////////////////////
//	Module 		: selector_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Selector manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "selector_manager.h"
#include "memory_manager.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"

CSelectorManager::CSelectorManager		()
{
	Init						();
}

CSelectorManager::~CSelectorManager		()
{
	remove_all					();
}

void CSelectorManager::Init				()
{
}

void CSelectorManager::reinit			()
{
	remove_all					();
}

void CSelectorManager::remove			(LPCSTR selector_name)
{
	xr_map<LPCSTR,CAbstractVertexEvaluator*,pred_str>::iterator	I = m_selectors.find(selector_name);
	VERIFY						(m_selectors.end() != I);
	xr_delete					((*I).second);
	m_selectors.erase			(I);
}

void CSelectorManager::remove_all		()
{
	while (!m_selectors.empty())
		remove					(m_selectors.begin()->first);
}

void CSelectorManager::init_selector	(CAbstractVertexEvaluator &S)
{
	const CMemoryManager	*memory_manager = dynamic_cast<const CMemoryManager*>(this);
	VERIFY					(memory_manager);

	const CEntityAlive		*entity_alive = dynamic_cast<const CEntityAlive*>(this);
	VERIFY					(entity_alive);

	if (memory_manager->hit()) {
		S.m_hit_direction	= memory_manager->hit()->m_direction;
		S.m_hit_time		= memory_manager->hit()->m_level_time;
	}
	else {
		S.m_hit_direction	= Fvector().set(0,0,0);
		S.m_hit_time		= 0;
	}

	S.m_dwCurTime			= Level().timeServer();
	S.m_tMe					= dynamic_cast<CEntity*>(this);
	S.m_tpMyNode			= memory_manager->level_vertex();
	S.m_tMyPosition			= memory_manager->Position();
	
	if (memory_manager->enemy()) {
		S.m_tEnemy			= memory_manager->enemy();
		S.m_tEnemyPosition	= memory_manager->enemy()->Position();
		S.m_dwEnemyNode		= memory_manager->enemy()->level_vertex_id();
		S.m_tpEnemyNode		= memory_manager->enemy()->level_vertex();
	}
	else {
		S.m_tEnemy			= 0;
		S.m_dwEnemyNode		= u32(-1);
		S.m_tpEnemyNode		= 0;
	}
	
	S.m_taMembers			= &(Level().seniority_holder().team(entity_alive->g_Team()).squad(entity_alive->g_Squad()).group(entity_alive->g_Group()).members());
	S.m_dwStartNode			= memory_manager->level_vertex_id();
	S.m_tStartPosition		= memory_manager->Position();
}
