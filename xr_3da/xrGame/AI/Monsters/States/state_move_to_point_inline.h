#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterMoveToPointAbstract CStateMonsterMoveToPoint<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointAbstract::execute()
{
	if (data.vertex != u32(-1)) object->MoveToTarget(data.point, data.vertex);
	else object->MoveToTarget(data.point);

	object->MotionMan.m_tAction				= data.action.action;
	object->MotionMan.SetSpecParams			(data.action.spec_params);
	
	if (data.accelerated) {
		object->MotionMan.accel_activate	(EAccelType(data.accel_type));
		object->MotionMan.accel_set_braking (data.braking);
	}

	if (data.action.sound_type != u32(-1)) {
		if (data.action.sound_delay != u32(-1))
			object->CSoundPlayer::play(data.action.sound_type, 0,0,data.action.sound_delay);
		else 
			object->CSoundPlayer::play(data.action.sound_type);
	}
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterMoveToPointAbstract::check_completion()
{	
	if (data.action.time_out !=0) {
		if (time_state_started + data.action.time_out < object->m_current_update) return true;
	} 
	
	if (object->Position().distance_to(data.point) < data.completion_dist) return true;
	return false;
}

