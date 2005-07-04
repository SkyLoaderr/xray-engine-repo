#pragma once

#include "../../../level.h"
#include "state_move_to_point.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterTestStateAbstract CStateMonsterTestState<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterTestStateAbstract::CStateMonsterTestState(_Object *obj) : inherited(obj) 
{
	add_state(eStateCustom,xr_new<CStateMonsterMoveToPointEx<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
void CStateMonsterTestStateAbstract::reselect_state()
{
	select_state(eStateCustom);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterTestStateAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateCustom) {
		SStateDataMoveToPointEx data;

		Fvector dest_pos = Level().CurrentEntity()->Position();
		dest_pos = random_position(dest_pos, 20.f);

		if (!object->control().path_builder().restrictions().accessible(dest_pos)) {
			data.vertex		= object->control().path_builder().restrictions().accessible_nearest(dest_pos, data.point);
		} else {
			data.point		= dest_pos;
			data.vertex		= u32(-1);
		}

		data.action.action		= ACT_RUN;
		data.action.time_out	= 20000;
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type 		= eAT_Calm;
		data.completion_dist	= 3.f;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->db().m_dwIdleSndDelay;
		data.time_to_rebuild	= 0;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPointEx));

		return;
	}

}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterTestStateAbstract
