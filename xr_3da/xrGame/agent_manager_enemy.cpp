////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_enemy.cpp
//	Created 	: 24.05.2004
//  Modified 	: 04.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager enemy distributor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "ef_storage.h"
#include "cover_point.h"

struct CEnemyFiller {
	xr_vector<CAgentManager::CEnemy>	*m_enemies;
	MemorySpace::squad_mask_type		m_mask;
	
	IC			CEnemyFiller		(xr_vector<CAgentManager::CEnemy> *enemies, MemorySpace::squad_mask_type mask)
	{
		m_enemies					= enemies;
		m_mask						= mask;
	}

	IC	void	operator()			(const CEntityAlive *enemy) const
	{
		xr_vector<CAgentManager::CEnemy>::iterator	I = std::find(m_enemies->begin(),m_enemies->end(),enemy);
		if (I == m_enemies->end())
			m_enemies->push_back	(CAgentManager::CEnemy(enemy,m_mask));
		else
			(*I).m_mask.set			(m_mask,TRUE);
	}
};

void CAgentManager::fill_enemies		()
{
	m_enemies.clear					();
	iterator						I = m_object->members().begin();
	iterator						E = m_object->members().end();
	for ( ; I != E; ++I) {
		(*I).probability			(1.f);
		(*I).object()->fill_enemies	(CEnemyFiller(&m_enemies,mask((*I).object())));
	}
	VERIFY							(!m_enemies.empty());
}

void CAgentManager::compute_enemy_danger()
{
	xr_vector<CEnemy>::iterator		I = m_enemies.begin();
	xr_vector<CEnemy>::iterator		E = m_enemies.end();
	for ( ; I != E; ++I) {
		ai().ef_storage().m_tpCurrentMember = (*I).m_object;
		float						best = -1.f;
		xr_vector<CMemberOrder>::const_iterator	i = members().begin();
		xr_vector<CMemberOrder>::const_iterator	e = members().end();
		for ( ; i != e; ++i) {
			ai().ef_storage().m_tpCurrentEnemy = (*i).object();
			float					value = ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f;
			if (value > best)
				best				= value;
		}
		(*I).m_probability			= best;
	}
	std::sort						(m_enemies.begin(),m_enemies.end());
}

void CAgentManager::assign_enemies		()
{
	for (;;) {
		MemorySpace::squad_mask_type	J, K, N = 0;
		float							best = flt_max;
		
		xr_vector<CEnemy>::iterator		I = m_enemies.begin();
		xr_vector<CEnemy>::iterator		E = m_enemies.end();
		for ( ; I != E; ++I) {
			J							= (*I).m_mask.get();
			N							= 0;
			best						= -1.f;
			for ( ; J; J &= J - 1) {
				K						= (J & (J - 1)) ^ J;
				iterator				i = member(K);
				if (!fsimilar((*i).probability(),1.f))
					continue;
				ai().ef_storage().m_tpCurrentMember = (*i).object();
				ai().ef_storage().m_tpCurrentEnemy = (*I).m_object;
				float					value = ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f;
				if (value > best) {
					best				= value;
					N					= K;
				}
			}
			if (N)
				break;
		}
		if (!N)
			break;
		
		(*I).m_distribute_mask.set	(N,TRUE);
		iterator					i = member(N);
		(*i).probability			(best);
		(*I).m_probability			*= 1.f - best; 

		// recovering sort order
		for (u32 i=0, n = m_enemies.size() - 1; i<n; ++i)
			if (m_enemies[i + 1] < m_enemies[i]) {
				CEnemy				temp = m_enemies[i];
				m_enemies[i]		= m_enemies[i + 1];
				m_enemies[i + 1]	= temp;
			}
			else
				break;
	}
}

void CAgentManager::assign_enemy_masks	()
{
	xr_vector<CEnemy>::iterator	I = m_enemies.begin();
	xr_vector<CEnemy>::iterator	E = m_enemies.end();
	for ( ; I != E; ++I)
		setup_mask				(*I);
}

void CAgentManager::distribute_enemies	()
{
	fill_enemies					();
	compute_enemy_danger			();
	assign_enemies					();
	assign_enemy_masks				();
}

bool CAgentManager::suitable_location	(CAI_Stalker *object, CCoverPoint *location) const
{
	const_iterator					I = members().begin();
	const_iterator					E = members().end();
	for ( ; I != E; ++I) {
		if ((*I).object()->ID() == object->ID())
			continue;
		if ((*I).object()->dest_position().distance_to(location->position()) <= 2.f)
			if ((*I).object()->Position().distance_to(location->position()) <= object->Position().distance_to(location->position()))
				return				(false);
	}
	return							(true);
}

void CAgentManager::distribute_locations()
{
}

void CAgentManager::setup_actions		()
{
}
