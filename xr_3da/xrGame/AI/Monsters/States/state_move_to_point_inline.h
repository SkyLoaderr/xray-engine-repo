#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterMoveToPointAbstract CStateMonsterMoveToPoint<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointAbstract::initialize()
{
	inherited::initialize();
	object->CMonsterMovement::initialize_movement();	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointAbstract::execute()
{
	object->set_action									(data.action.action);
	object->MotionMan.SetSpecParams						(data.action.spec_params);

	object->CMonsterMovement::set_target_point			(data.point,data.vertex);
	object->CMonsterMovement::set_generic_parameters	();

	if (data.accelerated) {
		object->MotionMan.accel_activate	(EAccelType(data.accel_type));
		object->MotionMan.accel_set_braking (data.braking);
	}

	if (data.action.sound_type != u32(-1)) {
		object->set_state_sound(data.action.sound_type, data.action.sound_delay == u32(-1));
	}
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterMoveToPointAbstract::check_completion()
{	
	if (data.action.time_out !=0) {
		if (time_state_started + data.action.time_out < object->m_current_update) return true;
	} 
	
	if (object->Position().distance_to_xz(data.point) < data.completion_dist) return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////
// CStateMonsterMoveToPointEx with path rebuild options
//////////////////////////////////////////////////////////////////////////

#define CStateMonsterMoveToPointExAbstract CStateMonsterMoveToPointEx<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointExAbstract::initialize()
{
	inherited::initialize();
	object->CMonsterMovement::initialize_movement();	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointExAbstract::execute()
{
	object->set_action									(data.action.action);
	object->MotionMan.SetSpecParams						(data.action.spec_params);

	object->CMonsterMovement::set_target_point			(data.point,data.vertex);
	object->CMonsterMovement::set_rebuild_time			(data.time_to_rebuild);
	object->CMonsterMovement::set_distance_to_end		(2.5f);
	object->CMonsterMovement::set_use_covers			();
	object->CMonsterMovement::set_cover_params			(5.f, 30.f, 1.f, 30.f);

	if (data.accelerated) {
		object->MotionMan.accel_activate	(EAccelType(data.accel_type));
		object->MotionMan.accel_set_braking (data.braking);
	}

	if (data.action.sound_type != u32(-1)) {
		object->set_state_sound(data.action.sound_type, data.action.sound_delay == u32(-1));
	}
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterMoveToPointExAbstract::check_completion()
{	
	if (data.action.time_out !=0) {
		if (time_state_started + data.action.time_out < object->m_current_update) return true;
	} 

	if (object->Position().distance_to_xz(data.point) < data.completion_dist) return true;
	return false;
}

