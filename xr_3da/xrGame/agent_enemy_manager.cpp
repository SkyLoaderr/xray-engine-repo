////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_enemy_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent enemy manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_enemy_manager.h"
#include "agent_manager.h"
#include "agent_memory_manager.h"
#include "agent_member_manager.h"
#include "ai_space.h"
#include "ef_storage.h"
#include "ef_pattern.h"
#include "member_order.h"
#include "ai/stalker/ai_stalker.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "sound_memory_manager.h"
#include "hit_memory_manager.h"
#include "enemy_manager.h"
#include "memory_space_impl.h"

struct CEnemyFiller {
	typedef CAgentEnemyManager::ENEMIES ENEMIES;
	ENEMIES			*m_enemies;
	squad_mask_type	m_mask;
	
	IC			CEnemyFiller					(ENEMIES *enemies, squad_mask_type mask)
	{
		m_enemies					= enemies;
		m_mask						= mask;
	}

	IC	void	operator()						(const CEntityAlive *enemy) const
	{
		ENEMIES::iterator			I = std::find(m_enemies->begin(),m_enemies->end(),enemy);
		if (I == m_enemies->end()) {
			m_enemies->push_back	(CMemberEnemy(enemy,m_mask));
			return;
		}

		(*I).m_mask.set				(m_mask,TRUE);
	}
};

struct remove_wounded_predicate {
	IC	bool	operator()	(const CMemberEnemy &enemy) const
	{
		const CAI_Stalker			*stalker = smart_cast<const CAI_Stalker*>(enemy.m_object);
		if (!stalker)
			return					(false);

		if (!stalker->wounded())
			return					(false);

		return						(true);
	}
};

template <typename T>
IC	void CAgentEnemyManager::setup_mask			(xr_vector<T> &objects, CMemberEnemy &enemy, const squad_mask_type &non_combat_members)
{
	xr_vector<T>::iterator			I = std::find(objects.begin(),objects.end(),enemy.m_object->ID());
	if (I != objects.end()) {
		(*I).m_squad_mask.assign	(
			((*I).m_squad_mask.get() & non_combat_members) |
			enemy.m_distribute_mask.get()
		);
	}
}

IC	void CAgentEnemyManager::setup_mask			(CMemberEnemy &enemy, const squad_mask_type &non_combat_members)
{
	setup_mask						(object().memory().visibles(),enemy,non_combat_members);
	setup_mask						(object().memory().sounds(),enemy,non_combat_members);
	setup_mask						(object().memory().hits(),enemy,non_combat_members);
}

float CAgentEnemyManager::evaluate				(const CEntityAlive *object0, const CEntityAlive *object1) const
{
	ai().ef_storage().non_alife().member_item() = 0;
	ai().ef_storage().non_alife().enemy_item() = 0;
	ai().ef_storage().non_alife().member() = object0;
	ai().ef_storage().non_alife().enemy()  = object1;
	return								(ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f);
}

void CAgentEnemyManager::exchange_enemies		(CMemberOrder &member0, CMemberOrder &member1)
{
	u32								enemy0 = member0.selected_enemy();
	u32								enemy1 = member1.selected_enemy();
	squad_mask_type					mask0 = object().member().mask(&member0.object());
	squad_mask_type					mask1 = object().member().mask(&member1.object());
	m_enemies[enemy0].m_distribute_mask.set(mask0,FALSE);
	m_enemies[enemy1].m_distribute_mask.set(mask1,FALSE);
	m_enemies[enemy0].m_distribute_mask.set(mask1,TRUE);
	m_enemies[enemy1].m_distribute_mask.set(mask0,TRUE);
	member0.selected_enemy			(enemy1);
	member1.selected_enemy			(enemy0);
}

void CAgentEnemyManager::fill_enemies			()
{
	m_enemies.clear					();

	{
		CAgentMemberManager::iterator	I = object().member().combat_members().begin();
		CAgentMemberManager::iterator	E = object().member().combat_members().end();
		for ( ; I != E; ++I) {
			(*I)->probability			(1.f);
			(*I)->object().memory().fill_enemies	(CEnemyFiller(&m_enemies,object().member().mask(&(*I)->object())));
		}
	}

	bool								only_wounded_left = true;
	bool								is_any_wounded = false;
	{
		CAgentMemoryManager				&memory = object().memory();
		ENEMIES::iterator				I = enemies().begin();
		ENEMIES::iterator				E = enemies().end();
		for ( ; I != E; ++I) {
			if (only_wounded_left) {
				const CAI_Stalker			*stalker = smart_cast<const CAI_Stalker*>((*I).m_object);
				if (!stalker || !stalker->wounded())
					only_wounded_left	= false;
				else
					is_any_wounded		= true;
			}

			memory.object_information	((*I).m_object,(*I).m_level_time,(*I).m_enemy_position);
		}
	}

	if (!only_wounded_left && is_any_wounded) {
		ENEMIES::iterator				I = std::remove_if(enemies().begin(),enemies().end(),remove_wounded_predicate());
		enemies().erase					(I,enemies().end());
	}
}

void CAgentEnemyManager::compute_enemy_danger	()
{
	ENEMIES::iterator		I = m_enemies.begin();
	ENEMIES::iterator		E = m_enemies.end();
	for ( ; I != E; ++I) {
		float						best = -1.f;
		CAgentMemberManager::const_iterator	i = object().member().combat_members().begin();
		CAgentMemberManager::const_iterator	e = object().member().combat_members().end();
		for ( ; i != e; ++i) {
			float					value = evaluate((*I).m_object,&(*i)->object());
			if (value > best)
				best				= value;
		}
		(*I).m_probability			= best;
	}
	std::sort						(m_enemies.begin(),m_enemies.end());
}

void CAgentEnemyManager::assign_enemies			()
{
	for (;;) {
		squad_mask_type	J, K, N = 0;
		float							best = flt_max;
		
		ENEMIES::iterator				I = m_enemies.begin();
		ENEMIES::iterator				E = m_enemies.end();
		for ( ; I != E; ++I) {
			J							= (*I).m_mask.get();
			N							= 0;
			best						= -1.f;
			for ( ; J; J &= J - 1) {
				K						= (J & (J - 1)) ^ J;
				CAgentMemberManager::iterator	i = object().member().member(K);
				if (!fsimilar((*i)->probability(),1.f))
					continue;

				float					value = evaluate(&(*i)->object(),(*I).m_object);
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
		
		(*I).m_distribute_mask.set		(N,TRUE);
		CAgentMemberManager::iterator	i = object().member().member(N);
		(*i)->probability				(best);
		(*I).m_probability				*= 1.f - best; 

		// recovering sort order
		for (u32 i=0, n = m_enemies.size() - 1; i<n; ++i)
			if (m_enemies[i + 1] < m_enemies[i])
				std::swap				(m_enemies[i],m_enemies[i + 1]);
			else
				break;
	}
}

void CAgentEnemyManager::permutate_enemies		()
{
	// filling member enemies
	CAgentMemberManager::iterator					I = object().member().combat_members().begin();
	CAgentMemberManager::iterator					E = object().member().combat_members().end();
	for ( ; I != E; ++I) {
		// clear enemies
		(*I)->enemies().clear	();
		// setup procesed flag
		(*I)->processed			(false);
		// get member squad mask
		squad_mask_type		member_mask = object().member().mask(&(*I)->object());
		// setup if player has enemy
		bool								enemy_selected = false;
		// iterate on enemies
		ENEMIES::const_iterator	i = m_enemies.begin(), b = i;
		ENEMIES::const_iterator	e = m_enemies.end();
		for ( ; i != e; ++i) {
			if ((*i).m_mask.is(member_mask))
				(*I)->enemies().push_back	(u32(i - b));
			if ((*i).m_distribute_mask.is(member_mask)) {
				(*I)->selected_enemy		(u32(i - b));
				enemy_selected				= true;
			}
		}
		// if there is enemy - all is ok
		if (enemy_selected)
			continue;

		// otherwise temporary make the member processed
		(*I)->processed			(true);
	}
	
	// perform permutations
	bool						changed;
	do {
		changed					= false;
		CAgentMemberManager::iterator				I = object().member().combat_members().begin();
		CAgentMemberManager::iterator				E = object().member().combat_members().end();
		for ( ; I != E; ++I) {
			// if member is processed the continue;
			if ((*I)->processed())
				continue;

			float				best = (*I)->object().Position().distance_to(m_enemies[(*I)->selected_enemy()].m_object->Position());
			bool				found = false;
			xr_vector<u32>::const_iterator	i = (*I)->enemies().begin();
			xr_vector<u32>::const_iterator	e = (*I)->enemies().end();
			for ( ; i != e; ++i) {
				if ((*I)->selected_enemy() == *i)
					continue;
				float			my_distance = (*I)->object().Position().distance_to(m_enemies[*i].m_object->Position());
				if (my_distance < best) {
					// check if we can exchange enemies
					squad_mask_type	J = m_enemies[*i].m_distribute_mask.get(), K;
					// iterating on members, whose current enemy is the new one
					for ( ; J; J &= J - 1) {
						K			= (J & (J - 1)) ^ J;
						CAgentMemberManager::iterator	j = object().member().member(K);
						xr_vector<u32>::iterator	ii = std::find((*j)->enemies().begin(),(*j)->enemies().end(),(*I)->selected_enemy());
						// check if member can my current enemy
						if (ii == (*j)->enemies().end())
							continue;

						// check if I'm closer to the enemy
						float		member_distance = (*j)->object().Position().distance_to(m_enemies[*i].m_object->Position());
						if (member_distance <= my_distance)
							continue;

						// check if our effectiveness is near the same
						float		my_to_his = evaluate(&(*I)->object(),m_enemies[(*j)->selected_enemy()].m_object);
						float		his_to_my = evaluate(&(*j)->object(),m_enemies[(*I)->selected_enemy()].m_object);
						if (!fsimilar(my_to_his,(*j)->probability()) || !fsimilar(his_to_my,(*I)->probability()))
							continue;

						exchange_enemies	(**I,**j);

						found		= true;
						best		= my_distance;
						break;
					}
				}

				if (found)
					break;
			}

			if (!found) {
				(*I)->processed	(true);
				continue;
			}

			changed				= true;
		}
	}
	while						(changed);

	{
		CAgentMemberManager::iterator					I = object().member().combat_members().begin();
		CAgentMemberManager::iterator					E = object().member().combat_members().end();
		for ( ; I != E; ++I) {
			ENEMIES::iterator	i = m_enemies.begin();
			ENEMIES::iterator	e = m_enemies.end();
			for ( ; i != e; ++i)
				if ((*I)->object().memory().visual().visible_now((*i).m_object))
					(*i).m_distribute_mask.assign((*i).m_distribute_mask.get() | object().member().mask(&(*I)->object()));
		}
	}
}

void CAgentEnemyManager::assign_enemy_masks		()
{
	squad_mask_type			non_combat_members = object().member().non_combat_members_mask();

	ENEMIES::iterator		I = m_enemies.begin();
	ENEMIES::iterator		E = m_enemies.end();
	for ( ; I != E; ++I)
		setup_mask			(*I,non_combat_members);
}

void CAgentEnemyManager::distribute_enemies	()
{
	if (!object().member().combat_mask())
		return;

	fill_enemies			();
	compute_enemy_danger	();
	assign_enemies			();
	permutate_enemies		();
	assign_enemy_masks		();
}

void CAgentEnemyManager::remove_links			(CObject *object)
{
}

void CAgentEnemyManager::update					()
{
}