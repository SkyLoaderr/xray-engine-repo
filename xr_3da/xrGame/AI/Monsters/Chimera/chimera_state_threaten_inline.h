#pragma once

#include "../states/state_move_to_point.h"
#include "../states/state_look_point.h"
#include "../states/state_custom_action.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimeraThreatenAbstract CStateChimeraThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::CStateChimeraThreaten(_Object *obj) : inherited(obj)
{
	add_state(eStateWalk,		xr_new<CStateMonsterMoveToPoint<_Object> >	(obj));
	add_state(eStateThreaten,	xr_new<CStateMonsterCustomAction<_Object> >	(obj));
	add_state(eStateFaceEnemy,	xr_new<CStateMonsterLookToPoint<_Object> >	(obj));
	add_state(eStateSteal,		xr_new<CStateMonsterMoveToPoint<_Object> >	(obj));
}

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::~CStateChimeraThreaten()
{
}

#define MAX_DIST_TO_ENEMY	15.f 
#define MORALE_THRESHOLD	0.8f

TEMPLATE_SPECIALIZATION
bool CStateChimeraThreatenAbstract::check_start_conditions()
{
	if (object->Position().distance_to(object->EnemyMan.get_enemy_position()) > MAX_DIST_TO_ENEMY) return false;
	if (object->GetEntityMorale() < MORALE_THRESHOLD)	return false;
	if (object->HitMemory.is_hit())						return false;
	if (object->hear_dangerous_sound)					return false;
	
	return true;
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::initialize()
{
	inherited::initialize	();
	object->SetUpperState	();
	m_action				= eActionThreatenFirst;
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::reselect_state()
{
	//select_action();
	//
	//switch (m_action) {
	//	case eActionThreatenSecond:
	//	case eActionThreatenFirst:	
	//		select_state(eStateThreaten);
	//		break;
	//	case eActionSteal:
	//		select_state(eStateSteal);
	//		break;
	//	case eActionWalk:
	//		select_state(eStateWalk);
	//		break;
	//}
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateWalk) {

		SStateDataMoveToPoint	data;
		
		data.point				= object->EnemyMan.get_enemy_position();
		data.vertex				= object->EnemyMan.get_enemy_vertex();
		data.action.action		= ACT_WALK_FWD;
		data.completion_dist	= 5.f;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;
		data.action.time_out	= object->get_attack_rebuild_time();

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));
		return;
	}

	if (current_substate == eStateSteal) {

		SStateDataMoveToPoint	data;

		data.point				= object->EnemyMan.get_enemy_position();
		data.vertex				= object->EnemyMan.get_enemy_vertex();
		data.action.action		= ACT_STEAL;
		data.completion_dist	= 5.f;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;
		data.action.time_out	= object->get_attack_rebuild_time();

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));
		return;
	}

	if (current_substate == eStateThreaten) {
		SStateDataAction data;
		
		data.action			= ACT_STAND_IDLE;
		data.spec_params	= ASP_THREATEN;
		data.time_out		= 5500;
		data.sound_type		= MonsterSpace::eMonsterSoundThreaten;
		data.sound_delay	= object->get_sd()->m_dwAttackSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataAction));
		return;
	}

	if (current_substate == eStateFaceEnemy) {
		SStateDataLookToPoint data;

		data.point				= object->EnemyMan.get_enemy()->Position();
		data.action.action		= ACT_STAND_IDLE;
		data.action.spec_params	= ASP_THREATEN;
		data.action.time_out	= 5500;
		data.face_delay			= 1200;

		state->fill_data_with(&data, sizeof(SStateDataLookToPoint));
		return;
	}
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::finalize()
{
	inherited::finalize		();
	object->SetUpperState	(false);
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::critical_finalize()
{
	inherited::critical_finalize();
	object->SetUpperState	(false);
}

#define STOP_DIST_TO_ENEMY 5.f

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::select_action()
{
	//if (prev_substate == u32(-1)) return;
	//
	//m_action++;

	//float dist = object->Position().distance_to(object->EnemyMan.get_enemy()->Position());
	//if (dist > STOP_DIST_TO_ENEMY) {
	//	
	//}
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimeraThreatenAbstract
