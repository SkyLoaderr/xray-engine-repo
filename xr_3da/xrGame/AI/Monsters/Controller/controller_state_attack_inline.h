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

//TEMPLATE_SPECIALIZATION
//void CStateControllerAttackAbstract::execute()
//{
//	float dist, m_fDistMin, m_fDistMax;
//	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);
//
//	// определить тип атаки
//	bool b_melee = false; 
//	if ((prev_substate == eStateMelee) && (dist < m_fDistMax)) b_melee = true;
//	else if (dist < m_fDistMin) b_melee = true;
//
//	// установить целевое состояние
//	if (b_melee)	select_state(eStateMelee);
//	else			select_state(eStateRun);
//
//	get_state_current()->execute();
//
//	prev_substate = current_substate;
//}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::reselect_state()
{
	if (get_state(eStateMelee)->check_start_conditions()) {
		select_state(eStateMelee);
		return;
	}
	
	if (get_state(eStateControlExpose)->check_start_conditions()) {
		select_state(eStateControlExpose);
		return;
	}

	select_state(eStateRun);
}


