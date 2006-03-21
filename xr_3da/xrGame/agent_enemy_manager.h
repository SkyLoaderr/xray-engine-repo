////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_enemy_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent enemy manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "member_enemy.h"

class CAgentManager;
class CMemberOrder;

class CAgentEnemyManager {
public:
	typedef xr_vector<CMemberEnemy>				ENEMIES;
	typedef MemorySpace::squad_mask_type		squad_mask_type;

private:
	CAgentManager			*m_object;
	ENEMIES					m_enemies;

protected:
	template <typename T>
	IC		void			setup_mask			(xr_vector<T> &objects, CMemberEnemy &enemy, const squad_mask_type &non_combat_members);
	IC		void			setup_mask			(CMemberEnemy &enemy, const squad_mask_type &non_combat_members);
			void			fill_enemies		();
			void			compute_enemy_danger();
			void			assign_enemies		();
			void			permutate_enemies	();
			void			assign_enemy_masks	();
			float			evaluate			(const CEntityAlive *object0, const CEntityAlive *object1) const;
			void			exchange_enemies	(CMemberOrder &member0, CMemberOrder &member1);
	IC		CAgentManager	&object				() const;

public:
	IC						CAgentEnemyManager	(CAgentManager *object);
			void			update				();
			void			distribute_enemies	();
	IC		ENEMIES			&enemies			();
			void			remove_links		(CObject *object);
};

#include "agent_enemy_manager_inline.h"