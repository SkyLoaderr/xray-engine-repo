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

float CAgentManager::evaluate			(const CEntityAlive *object0, const CEntityAlive *object1) const
{
	ai().ef_storage().m_tpCurrentMember = object0;
	ai().ef_storage().m_tpCurrentEnemy  = object1;
	return								(ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f);
}

void CAgentManager::exchange_enemies	(CMemberOrder &member0, CMemberOrder &member1)
{
	u32								enemy0 = member0.selected_enemy();
	u32								enemy1 = member1.selected_enemy();
	MemorySpace::squad_mask_type	mask0 = mask(member0.object());
	MemorySpace::squad_mask_type	mask1 = mask(member1.object());
	m_enemies[enemy0].m_distribute_mask.set(mask0,FALSE);
	m_enemies[enemy1].m_distribute_mask.set(mask1,FALSE);
	m_enemies[enemy0].m_distribute_mask.set(mask1,TRUE);
	m_enemies[enemy1].m_distribute_mask.set(mask0,TRUE);
	member0.selected_enemy			(enemy1);
	member1.selected_enemy			(enemy0);
//	Msg								("Member %s changed enemy from %s to %s",*member0.object()->cName(),*m_enemies[enemy0].m_object->cName(),*m_enemies[enemy1].m_object->cName());
//	Msg								("Member %s changed enemy from %s to %s",*member1.object()->cName(),*m_enemies[enemy1].m_object->cName(),*m_enemies[enemy0].m_object->cName());
}

void CAgentManager::fill_enemies		()
{
	m_enemies.clear					();
	iterator						I = m_object->members().begin();
	iterator						E = m_object->members().end();
	for ( ; I != E; ++I) {
		(*I).probability			(1.f);
		(*I).object()->fill_enemies	(CEnemyFiller(&m_enemies,mask((*I).object())));
	}
}

void CAgentManager::compute_enemy_danger()
{
	xr_vector<CEnemy>::iterator		I = m_enemies.begin();
	xr_vector<CEnemy>::iterator		E = m_enemies.end();
	for ( ; I != E; ++I) {
		float						best = -1.f;
		xr_vector<CMemberOrder>::const_iterator	i = members().begin();
		xr_vector<CMemberOrder>::const_iterator	e = members().end();
		for ( ; i != e; ++i) {
			float					value = evaluate((*I).m_object,(*i).object());
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
				float					value = evaluate((*i).object(),(*I).m_object);
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

void CAgentManager::permutate_enemies	()
{
	// filling member enemies
	iterator					I = m_members.begin();
	iterator					E = m_members.end();
	for ( ; I != E; ++I) {
		// clear enemies
		(*I).enemies().clear	();
		// setup procesed flag
		(*I).processed			(false);
		// get member squad mask
		MemorySpace::squad_mask_type		member_mask = mask((*I).object());
		// setup if player has enemy
		bool								enemy_selected = false;
		// iterate on enemies
		xr_vector<CEnemy>::const_iterator	i = m_enemies.begin(), b = i;
		xr_vector<CEnemy>::const_iterator	e = m_enemies.end();
		for ( ; i != e; ++i) {
			if ((*i).m_mask.is(member_mask))
				(*I).enemies().push_back	(u32(i - b));
			if ((*i).m_distribute_mask.is(member_mask)) {
				(*I).selected_enemy			(u32(i - b));
				enemy_selected				= true;
			}
		}
		// if there is enemy - all is ok
		if (enemy_selected)
			continue;

		// otherwise temporary make the member processed
		(*I).processed			(true);
	}
	
	// perform permutations
	bool						changed;
	do {
		changed					= false;
		iterator				I = m_members.begin();
		iterator				E = m_members.end();
		for ( ; I != E; ++I) {
			// if member is processed the continue;
			if ((*I).processed())
				continue;

			float				best = (*I).object()->Position().distance_to(m_enemies[(*I).selected_enemy()].m_object->Position());
			bool				found = false;
			xr_vector<u32>::const_iterator	i = (*I).enemies().begin();
			xr_vector<u32>::const_iterator	e = (*I).enemies().end();
			for ( ; i != e; ++i) {
				float			my_distance = (*I).object()->Position().distance_to(m_enemies[*i].m_object->Position());
				if (my_distance < best) {
					// check if we can exchange enemies
					MemorySpace::squad_mask_type	J = m_enemies[*i].m_distribute_mask.get(), K;
					// iterating on members, whose current enemy is the new one
					for ( ; J; J &= J - 1) {
						K			= (J & (J - 1)) ^ J;
						iterator	j = member(K);
						xr_vector<u32>::iterator	ii = std::find((*j).enemies().begin(),(*j).enemies().end(),(*I).selected_enemy());
						// check if member can my current enemy
						if (ii == (*j).enemies().end())
							continue;

						// check if I'm closer to the enemy
						float		member_distance = (*j).object()->Position().distance_to(m_enemies[*i].m_object->Position());
						if (member_distance <= my_distance)
							continue;

						// check if our effectiveness is near the same
						float		my_to_his = evaluate((*I).object(),m_enemies[(*j).selected_enemy()].m_object);
						float		his_to_my = evaluate((*j).object(),m_enemies[(*I).selected_enemy()].m_object);
						if (!fsimilar(my_to_his,(*j).probability()) || !fsimilar(his_to_my,(*I).probability()))
							continue;

						exchange_enemies	(*I,*j);

						found		= true;
						best		= my_distance;
						break;
					}
				}

				if (found)
					break;
			}

			if (!found) {
				(*I).processed	(true);
				continue;
			}

			changed				= true;
		}
	}
	while						(changed);
}

void CAgentManager::assign_enemy_masks	()
{
	xr_vector<CEnemy>::iterator		I = m_enemies.begin();
	xr_vector<CEnemy>::iterator		E = m_enemies.end();
	for ( ; I != E; ++I)
		setup_mask					(*I);
}

void CAgentManager::distribute_enemies	()
{
	fill_enemies					();
	compute_enemy_danger			();
	assign_enemies					();
	permutate_enemies				();
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
