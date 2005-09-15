#pragma once

#include "state_move_to_point.h"
#include "state_look_point.h"
#include "monster_state_attack_camp_stealout.h"
#include "../../../cover_point.h"
#include "../monster_cover_manager.h"


#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackCampAbstract CStateMonsterAttackCamp<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackCampAbstract::CStateMonsterAttackCamp(_Object *obj) : inherited(obj) 
{
	add_state	(eStateAttackCamp_Hide,		xr_new<CStateMonsterMoveToPointEx<_Object> >	(obj));
	add_state	(eStateAttackCamp_Camp,		xr_new<CStateMonsterLookToPoint<_Object> >		(obj));
	add_state	(eStateAttackCamp_StealOut,	xr_new<CStateMonsterAttackCampStealOut<_Object> >(obj));
}


TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackCampAbstract::check_completion()
{
	if (current_substate == eStateAttackCamp_Camp) {
		return get_state_current()->check_completion();
	}

	if (current_substate == eStateAttackCamp_StealOut) {
		return get_state_current()->check_completion();
	}
	
	if (object->EnemyMan.get_enemy()->Position().distance_to(object->Position()) < 5.f) return true;

	return false;
}

#define MIN_DISTANCE_TO_ENEMY 20.f

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackCampAbstract::check_start_conditions()
{
	// check enemy
	if (!object->EnemyMan.get_enemy()) return false;
	
	// check distance to enemy
	if (object->Position().distance_to(object->EnemyMan.get_enemy_position()) < MIN_DISTANCE_TO_ENEMY) return false;
		
	// check if enemy see me
	//if (EnemyMan.get_flags().is(FLAG_ENEMY_DOESNT_SEE_ME)) return false;

	// try to get cover
	CCoverPoint *point = object->CoverMan->find_cover(object->EnemyMan.get_enemy_position(), 10.f, 30.f);
	if (!point) return false;

	m_target_node = point->level_vertex_id();
	
	return true;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackCampAbstract::reselect_state()
{
	if (current_substate == u32(-1)) {
		select_state(eStateAttackCamp_Hide);
		return;
	}

	if (prev_substate == eStateAttackCamp_Hide) {
		select_state(eStateAttackCamp_Camp);
		return;
	}

	if (prev_substate == eStateAttackCamp_Camp) {
		if (get_state(eStateAttackCamp_StealOut)->check_start_conditions())
			select_state(eStateAttackCamp_StealOut);
		else 
			select_state(eStateAttackCamp_Hide);
		return;
	}

	if (prev_substate == eStateAttackCamp_StealOut) {
		select_state(eStateAttackCamp_Camp);
		return;
	}
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackCampAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateAttackCamp_Hide) {
		SStateDataMoveToPointEx data;

		data.vertex				= m_target_node;
		data.point				= ai().level_graph().vertex_position(data.vertex);
		data.action.action		= ACT_RUN;
		data.action.time_out	= 0;		// do not use time out
		data.completion_dist	= 0.f;		// get exactly to the point
		data.time_to_rebuild	= 0;		// do not rebuild
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type 		= eAT_Aggressive;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->db().m_dwIdleSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPointEx));
		return;
	}
	
	if (current_substate == eStateAttackCamp_Camp) {

		SStateDataLookToPoint	data;
		
		object->CoverMan->less_cover_direction(data.point);
		
		data.action.action		= ACT_RUN;
		data.action.time_out	= 4000;		// do not use time out
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->db().m_dwIdleSndDelay;
		data.face_delay			= 0;

		state->fill_data_with(&data, sizeof(SStateDataLookToPoint));
		return;
	}
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackCampAbstract::check_force_state()
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterAttackCampAbstract

