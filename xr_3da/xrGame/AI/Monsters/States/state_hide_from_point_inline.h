#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterHideFromPointAbstract CStateMonsterHideFromPoint<_Object>
#define COVER_RESELECT_TIME_QUANT 5000

TEMPLATE_SPECIALIZATION
void CStateMonsterHideFromPointAbstract::initialize()
{
	inherited::initialize();

	last_time_reselect_cover	= 0;
	target.node					= u32(-1);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHideFromPointAbstract::execute()
{
	bool b_reselect_target = false;
	if (last_time_reselect_cover + COVER_RESELECT_TIME_QUANT < object->m_current_update) b_reselect_target = true;
	if (object->IsPathEnd(2.5f)) b_reselect_target = true;

	if (b_reselect_target) {
		if (!object->GetCoverFromPoint(data.point,target.position, target.node, data.cover_min_dist,data.cover_max_dist,data.cover_search_radius)) {
			target.node					= u32(-1);
			last_time_reselect_cover	= object->m_current_update;
		}
	}

	if (target.node != u32(-1)) {
		object->MoveToTarget			(target.position, target.node);
	} else {
		object->MoveAwayFromTarget		(data.point);
	}

	object->MotionMan.m_tAction				= data.action.action;
	object->MotionMan.SetSpecParams			(data.action.spec_params);

	if (data.accelerated) {
		object->MotionMan.accel_activate	(EAccelType(data.accel_type));
		object->MotionMan.accel_set_braking (data.braking);
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

#undef COVER_RESELECT_TIME_QUANT
#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterHideFromPointAbstract
