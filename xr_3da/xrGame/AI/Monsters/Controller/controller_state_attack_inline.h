#pragma once

#include "controller_state_control_hit.h"
#include "controller_state_expose.h"
#include "../states/state_look_point.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateControllerAttackAbstract CStateControllerAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateControllerAttackAbstract::CStateControllerAttack(_Object *obj, state_ptr state_run, state_ptr state_melee) : inherited(obj)
{
	add_state	(eStateRun,				state_run);
	add_state	(eStateMelee,			state_melee);
	add_state	(eStateControlAttack,	xr_new<CStateControlAttack<_Object> >(obj));
	add_state	(eStateControlExpose,	xr_new<CStateControlExpose<_Object> >(obj));
	add_state	(eStateFaceEnemy,		xr_new<CStateMonsterLookToPoint<_Object> >(obj));
	//add_state	(eStateTakeCover,		xr_new<>)	
}

TEMPLATE_SPECIALIZATION
CStateControllerAttackAbstract::~CStateControllerAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::reselect_state()
{
	if (get_state(eStateMelee)->check_start_conditions()) {
		select_state(eStateMelee);
		return;
	}
	
	if (get_state(eStateControlAttack)->check_start_conditions()) {
		select_state(eStateControlAttack);
		return;
	}

	select_state(eStateFaceEnemy);

	//if (get_state(eStateControlExpose)->check_start_conditions()) {
	//	select_state(eStateControlExpose);
	//	return;
	//}

	//select_state(eStateRun);
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateFaceEnemy) {
		SStateDataLookToPoint data;

		data.point				= object->EnemyMan.get_enemy_position();
		data.action.action		= ACT_STAND_IDLE;

		state->fill_data_with(&data, sizeof(SStateDataLookToPoint));
		return;
	}
}

