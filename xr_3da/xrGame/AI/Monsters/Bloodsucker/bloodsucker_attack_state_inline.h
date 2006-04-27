#pragma once

#include "bloodsucker_attack_state_hide.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CBloodsuckerStateAttackAbstract CBloodsuckerStateAttack<_Object>

TEMPLATE_SPECIALIZATION
CBloodsuckerStateAttackAbstract::CBloodsuckerStateAttack(_Object *obj) : inherited_attack(obj)
{
	add_state(eStateAttack_Hide,	xr_new<CBloodsuckerStateAttackHide<_Object> > (obj));
}

TEMPLATE_SPECIALIZATION
CBloodsuckerStateAttackAbstract::~CBloodsuckerStateAttack()
{
}

TEMPLATE_SPECIALIZATION
void CBloodsuckerStateAttackAbstract::initialize()
{
	inherited::initialize	();
	m_time_deactivated		= 0;
}

TEMPLATE_SPECIALIZATION
void CBloodsuckerStateAttackAbstract::finalize()
{
	inherited::finalize();
	object->stop_invisible_predator();
}

TEMPLATE_SPECIALIZATION
void CBloodsuckerStateAttackAbstract::critical_finalize()
{
	inherited::critical_finalize();
	object->stop_invisible_predator();
}

TEMPLATE_SPECIALIZATION
void CBloodsuckerStateAttackAbstract::execute()
{
	bool selected = false;
	
	if (check_berserk()) {
		bool b_melee = false; 
		
		if (prev_substate == eStateAttack_Melee) {
			if (!get_state_current()->check_completion())					b_melee = true;
		} else {
			if (get_state(eStateAttack_Melee)->check_start_conditions())	b_melee = true;
		}
		
		select_state(b_melee ? eStateAttack_Melee : eStateAttack_Run);
		selected		= true;
	} else if (check_hiding()) {
		select_state	(eStateAttack_Hide);
		selected		= true;
	} else if (check_home_point()) {
		select_state	(eStateAttack_MoveToHomePoint);
		selected		= true;
	} else if (check_steal_state()) {
		select_state	(eStateAttack_Steal);
		selected		= true;
	} else if (check_camp_state()) {
		select_state	(eStateAttackCamp);
		selected		= true;
	} else if (check_find_enemy_state()) {
		select_state	(eStateAttack_FindEnemy);
		selected		= true;
	} else if (check_run_away_state()) {
		select_state	(eStateAttack_RunAway);
		selected		= true;
	} 

	if (!selected) {
		// определить тип атаки
		bool b_melee = false; 

		if (prev_substate == eStateAttack_Melee) {
			if (!get_state_current()->check_completion()) {
				b_melee = true;
			}
		} else if (get_state(eStateAttack_Melee)->check_start_conditions()) {
			b_melee = true;
		}

		// установить целевое состояние
		if (b_melee) {  
			// check if enemy is behind me for a long time
			// [TODO] make specific state and replace run_away state (to avoid ratation jumps)
			//if (check_behinder()) 
			//	select_state(eStateAttack_RunAway);
			//else 
			select_state(eStateAttack_Melee);
		}
		else select_state(eStateAttack_Run);
	}

	update_invisibility	();
	

	get_state_current()->execute();

	prev_substate = current_substate;


	// Notify squad	
	CMonsterSquad *squad	= monster_squad().get_squad(object);
	if (squad) {
		SMemberGoal			goal;

		goal.type			= MG_AttackEnemy;
		goal.entity			= const_cast<CEntityAlive*>(object->EnemyMan.get_enemy());

		squad->UpdateGoal	(object, goal);
	}
	//////////////////////////////////////////////////////////////////////////

	if (current_substate == eStateAttack_Run)
		object->sound().play(CAI_Bloodsucker::eGrowl, 0, 0, 8000);
}

#define INVIS_ACTIVATE_DELAY 3000

TEMPLATE_SPECIALIZATION
void CBloodsuckerStateAttackAbstract::update_invisibility()
{
	// if just turn to Run state
	if ((current_substate == eStateAttack_Run) && (object->EnemyMan.get_enemy()->Position().distance_to(object->Position()) > 5.f)) {
		if (!object->state_invisible && (m_time_deactivated + INVIS_ACTIVATE_DELAY < time()))
			object->start_invisible_predator();
	}

	// if just turn to Attack_Melee state
	if ((current_substate == eStateAttack_Melee) && (prev_substate != eStateAttack_Melee)) {
		object->stop_invisible_predator();
		m_time_deactivated	= time();
	}
}

TEMPLATE_SPECIALIZATION
bool CBloodsuckerStateAttackAbstract::check_hiding()
{
	if (prev_substate == eStateAttack_Hide) {
		return (!get_state_current()->check_completion());
	}
	
	if ((prev_substate != eStateAttack_Run) && (prev_substate != eStateAttack_Melee)) return false;
	if (object->conditions().health() > 0.5f) return false;
	return true;
}

TEMPLATE_SPECIALIZATION
bool CBloodsuckerStateAttackAbstract::check_berserk()
{
	if (!object->berserk_always && (object->time_berserk_start + 20000 < time())) return false;
	
	//object->predator_stop				();
	//object->stop_invisible_predator		();
	return true;
}



#undef TEMPLATE_SPECIALIZATION
#undef CBloodsuckerStateAttackAbstract

