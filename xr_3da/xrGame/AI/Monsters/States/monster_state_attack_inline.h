#pragma once

#include "monster_state_attack_melee.h"
#include "monster_state_attack_run.h"
#include "monster_state_attack_run_attack.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackAbstract CStateMonsterAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(_Object *obj) : inherited(obj)
{
	add_state	(eStateRun,			xr_new<CStateMonsterAttackRun<_Object> >(obj));
	add_state	(eStateMelee,		xr_new<CStateMonsterAttackMelee<_Object> >(obj));
	add_state	(eStateRunAttack,	xr_new<CStateMonsterAttackRunAttack<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(_Object *obj, state_ptr state_run, state_ptr state_melee) : inherited(obj)
{
	add_state	(eStateRun,			state_run);
	add_state	(eStateMelee,		state_melee);
	add_state	(eStateRunAttack,	xr_new<CStateMonsterAttackRunAttack<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::~CStateMonsterAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::initialize()
{
	inherited::initialize				();

	object->MeleeChecker.init_attack	();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::execute()
{
	bool selected = false;
	
	if (prev_substate == eStateRun) {
		if (get_state(eStateRunAttack)->check_start_conditions()) {
			select_state	(eStateRunAttack);
			selected		= true;
		}
	} else if (prev_substate == eStateRunAttack) {
		if (!get_state(eStateRunAttack)->check_completion()) {
			select_state	(eStateRunAttack);
			selected		= true;
		}
	}
	
	if (!selected) {
		// определить тип атаки
		bool b_melee = false; 
		if ((prev_substate == eStateMelee) && !object->MeleeChecker.should_stop_melee(object->EnemyMan.get_enemy())) b_melee = true;
		else if (object->MeleeChecker.can_start_melee(object->EnemyMan.get_enemy())) b_melee = true;

		// установить целевое состояние
		if (b_melee)	select_state(eStateMelee);
		else			select_state(eStateRun);
	}

	get_state_current()->execute();
	
	prev_substate = current_substate;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterAttackAbstract
