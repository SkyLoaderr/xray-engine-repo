#pragma once

#include "controller_state_control_hit.h"
#include "controller_state_expose.h"

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

	//if (get_state(eStateControlExpose)->check_start_conditions()) {
	//	select_state(eStateControlExpose);
	//	return;
	//}

	select_state(eStateRun);
}


