#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterHideFromPointAbstract CStateMonsterHideFromPoint<_Object>
#define DIST_TO_PATH_END		1.5f

TEMPLATE_SPECIALIZATION
void CStateMonsterHideFromPointAbstract::initialize()
{
	inherited::initialize();

	object->CMonsterMovement::initialize_movement();	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHideFromPointAbstract::execute()
{
	object->set_action									(data.action.action);
	object->MotionMan.SetSpecParams						(data.action.spec_params);
	
	object->CMonsterMovement::set_retreat_from_point	(data.point);
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
bool CStateMonsterHideFromPointAbstract::check_completion()
{	
	if (data.action.time_out !=0) {
		if (time_state_started + data.action.time_out < object->m_current_update) return true;
	} 

	if (!fis_zero(data.distance)) {
		if (object->Position().distance_to(data.point) > data.distance) return true;
	}
		
	return false;
}

#undef DIST_TO_PATH_END
#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterHideFromPointAbstract
