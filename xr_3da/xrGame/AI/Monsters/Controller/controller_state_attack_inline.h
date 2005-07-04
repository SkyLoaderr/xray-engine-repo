#pragma once

#include "controller_state_control_hit.h"
#include "../states/state_look_point.h"
#include "controller_state_attack_hide.h"
#include "controller_state_attack_moveout.h"
#include "../states/state_test_look_actor.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateControllerAttackAbstract CStateControllerAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateControllerAttackAbstract::CStateControllerAttack(_Object *obj, state_ptr state_run, state_ptr state_melee) : inherited(obj)
{
	add_state	(eStateRun,					state_run);
	add_state	(eStateMelee,				state_melee);
	add_state	(eStateControlAttack,		xr_new<CStateControlAttack<_Object> >(obj));
	add_state	(eStateFaceEnemy,			xr_new<CStateMonsterLookToPoint<_Object> >(obj));
	add_state	(eStateTakeCover,			xr_new<CStateControlHide<_Object> >(obj));
	add_state	(eStateMoveOutFromCover,	xr_new<CStateControlMoveOut<_Object> >(obj));
	add_state	(eStateCustom,				xr_new<CStateTest1<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateControllerAttackAbstract::~CStateControllerAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::reselect_state()
{
	//if (get_state(eStateMelee)->check_start_conditions()) {
	//	select_state(eStateMelee);
	//	return;
	//}

	//if (get_state(eStateControlAttack)->check_start_conditions()) {
	//	select_state(eStateControlAttack);
	//	return;
	//}
	//
	//if (prev_substate == eStateFaceEnemy) {
	//	if (get_state(eStateControlAttack)->check_start_conditions()) {
	//		select_state(eStateControlAttack);
	//		return;
	//	}
	//}

	//if (prev_substate == eStateMoveOutFromCover) {
	//	if (get_state(eStateFaceEnemy)->check_start_conditions()) {
	//		select_state(eStateFaceEnemy);
	//		return;
	//	}
	//}
	//
	//if (prev_substate == eStateTakeCover) {
	//	if (get_state(eStateMoveOutFromCover)->check_start_conditions()) {
	//		select_state(eStateMoveOutFromCover);
	//		return;
	//	}
	//}

	//if (get_state(eStateTakeCover)->check_start_conditions()) {
	//	select_state(eStateTakeCover);
	//	return;
	//}
	//
	//select_state(eStateRun);

	
	if (get_state(eStateTakeCover)->check_start_conditions()) {
		select_state(eStateTakeCover);
		return;
	}

	select_state(eStateCustom);
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
	
		object->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,object->db().m_dwAttackSndDelay);
		return;
	}
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::check_force_state() 
{
	if (current_substate == eStateRun) {
		float dist_to_enemy = object->Position().distance_to(object->EnemyMan.get_enemy_position());
		if (dist_to_enemy > 10.f) {
			get_state_current()->critical_finalize();
			current_substate = u32(-1);
		}
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateControllerAttackAbstract