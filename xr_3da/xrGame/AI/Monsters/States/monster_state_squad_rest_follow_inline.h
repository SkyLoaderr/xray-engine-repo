#pragma once

#include "state_custom_action.h"
#include "state_move_to_point.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterSquadRestFollowAbstract CStateMonsterSquadRestFollow<_Object>


#define STOP_DISTANCE	2.f
#define STAY_DISTANCE	5 * STOP_DISTANCE
#define MIN_TIME_OUT	2000
#define MAX_TIME_OUT	3000

TEMPLATE_SPECIALIZATION
CStateMonsterSquadRestFollowAbstract::CStateMonsterSquadRestFollow(_Object *obj) : inherited(obj)
{
	add_state	(eStateIdle,		xr_new<CStateMonsterCustomAction<_Object> >	(obj));
	add_state	(eStateWalkToPoint,	xr_new<CStateMonsterMoveToPointEx<_Object> >	(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterSquadRestFollowAbstract::~CStateMonsterSquadRestFollow	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterSquadRestFollowAbstract::initialize()
{
	inherited::initialize();
	
	SSquadCommand &command = monster_squad().get_squad(object)->GetCommand(object);
	last_point	 = command.position;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterSquadRestFollowAbstract::reselect_state()
{
	SSquadCommand &command = monster_squad().get_squad(object)->GetCommand(object);
	if (command.position.distance_to(object->Position()) < Random.randF(STOP_DISTANCE, STAY_DISTANCE)) {
		select_state(eStateIdle);
	} else {
		select_state(eStateWalkToPoint);
	}
}

TEMPLATE_SPECIALIZATION 
void CStateMonsterSquadRestFollowAbstract::check_force_state()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterSquadRestFollowAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateIdle) {
		SStateDataAction data;
		data.action			= ACT_REST;
		data.sound_type		= MonsterSpace::eMonsterSoundIdle;
		data.sound_delay	= object->get_sd()->m_dwIdleSndDelay;
		data.time_out		= Random.randI(MIN_TIME_OUT, MAX_TIME_OUT);

		state->fill_data_with(&data, sizeof(SStateDataAction));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			object->HDebug->M_Add(0,"Squad :: Idle", D3DCOLOR_XRGB(255,0,0));
		}
#endif

		return;
	}

	if (current_substate == eStateWalkToPoint) {
		SStateDataMoveToPointEx data;

		Fvector dest_pos = monster_squad().get_squad(object)->GetCommand(object).position;
		if (!object->accessible(dest_pos)) {
			data.vertex		= object->accessible_nearest(dest_pos, data.point);
		} else {
			data.point		= dest_pos;
			data.vertex		= u32(-1);
		}

		data.action.action		= ACT_WALK_FWD;
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type 		= eAT_Calm;
		data.completion_dist	= STOP_DISTANCE;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;
		data.time_to_rebuild	= u32(-1);

		state->fill_data_with(&data, sizeof(SStateDataMoveToPointEx));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			object->HDebug->M_Add(0,"Squad :: Follow Leader", D3DCOLOR_XRGB(255,0,0));
		}
#endif
		return;
	}
}


