#pragma once

#include "state_custom_action.h"
#include "state_move_to_point.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterSquadRestAbstract CStateMonsterSquadRest<_Object>

#define MIN_TIME_IDLE	2000
#define MAX_TIME_IDLE	5000

#define LEADER_RADIUS			20.f
#define FIND_POINT_ATTEMPTS		5


TEMPLATE_SPECIALIZATION
CStateMonsterSquadRestAbstract::CStateMonsterSquadRest(_Object *obj) : inherited(obj)
{
	add_state	(eStateIdle,				xr_new<CStateMonsterCustomAction<_Object> >	(obj));
	add_state	(eStateWalkAroundLeader,	xr_new<CStateMonsterMoveToPoint<_Object> >	(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterSquadRestAbstract::~CStateMonsterSquadRest	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterSquadRestAbstract::reselect_state()
{
	select_state(Random.randI(2) ? eStateIdle : eStateWalkAroundLeader);
}


TEMPLATE_SPECIALIZATION
void CStateMonsterSquadRestAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateIdle) {
		SStateDataAction data;
		data.action			= ACT_REST;
		data.sound_type		= MonsterSpace::eMonsterSoundIdle;
		data.sound_delay	= object->get_sd()->m_dwIdleSndDelay;
		data.time_out		= Random.randI(MIN_TIME_IDLE,MAX_TIME_IDLE);
		
		state->fill_data_with(&data, sizeof(SStateDataAction));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			object->HDebug->M_Add(0,"Squad :: Sleep", D3DCOLOR_XRGB(255,0,0));
		}
#endif

		return;
	}

	if (current_substate == eStateWalkAroundLeader) {
		SStateDataMoveToPoint data;
		CMonsterSquad	*squad = monster_squad().get_squad(object);
		
		if (!object->GetNodeInRadius(squad->GetLeader()->level_vertex_id(), 8.f, LEADER_RADIUS, FIND_POINT_ATTEMPTS, data.vertex)) {
			data.point			= ai().level_graph().vertex_position(data.vertex);
		} else {
			data.point			= random_position(squad->GetLeader()->Position(), LEADER_RADIUS);
			data.vertex			= u32(-1);
		}

		data.action.action	= ACT_WALK_FWD;
		data.accelerated	= true;
		data.braking		= false;
		data.accel_type 	= eAT_Calm;
		data.completion_dist= 2.f;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			object->HDebug->M_Add(0,"Squad :: Move Around Leader", D3DCOLOR_XRGB(255,0,0));
		}
#endif
		return;
	}
}


