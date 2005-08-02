#include "stdafx.h"
#include "controller_animation.h"
#include "controller.h"
#include "../../../detail_path_manager.h"
#include "../../../level.h"
#include "../control_direction_base.h"

void CControllerAnimation::reinit()
{
	m_controller			= smart_cast<CController *>(m_object);
	
	load					();
	inherited::reinit		();

	//m_cur_action				= ACT_RUN;
	//m_use_separate			= true;
	//m_current_legs_action		= eLegsStand;
	//m_current_torso_action	= eTorsoIdle;
}

void CControllerAnimation::on_start_control(ControlCom::EControlType type)
{
	switch (type) {
	case ControlCom::eControlAnimation: 
		m_man->subscribe	(this, ControlCom::eventAnimationEnd);	
		m_man->subscribe	(this, ControlCom::eventTorsoAnimationEnd);	
		m_man->subscribe	(this, ControlCom::eventLegsAnimationEnd);

		if (m_controller->m_mental_state == CController::eStateDanger) {
			select_torso_animation	();
			select_legs_animation	();
		} else {
			select_animation		();
		}

		break;
	}
}

void CControllerAnimation::on_stop_control	(ControlCom::EControlType type)
{
	switch (type) {
	case ControlCom::eControlAnimation: 
		m_man->unsubscribe	(this, ControlCom::eventAnimationEnd);	
		m_man->unsubscribe	(this, ControlCom::eventTorsoAnimationEnd);	
		m_man->unsubscribe	(this, ControlCom::eventLegsAnimationEnd);	
		break;
	}
}

void CControllerAnimation::on_event(ControlCom::EEventType type, ControlCom::IEventData *data)
{
	switch (type) {
	case ControlCom::eventAnimationEnd:			select_animation();			break;
	case ControlCom::eventTorsoAnimationEnd:	select_torso_animation();	break;
	case ControlCom::eventLegsAnimationEnd:		select_legs_animation();	break;
	}
}

void CControllerAnimation::update_frame()
{
	if (m_controller->m_mental_state == CController::eStateIdle) {
		inherited::update_frame();
		return;
	}

	static u32 last_time = 0;

	set_direction			();
	
	select_legs_animation	();	
	select_torso_animation	();	
	
	select_velocity			();
}

void CControllerAnimation::load()
{
	CSkeletonAnimated *skeleton = smart_cast<CSkeletonAnimated*>(m_object->Visual());
	
	m_legs[eLegsStand]					= skeleton->ID_Cycle_Safe("new_run_fwd_0");
	m_legs[eLegsRun]					= skeleton->ID_Cycle_Safe("new_run_fwd_0");
	m_legs[eLegsWalk]					= skeleton->ID_Cycle_Safe("new_run_fwd_0");
	m_legs[eLegsBackRun]				= skeleton->ID_Cycle_Safe("new_run_beack_0");
	m_legs[eLegsRunFwdLeft]				= skeleton->ID_Cycle_Safe("stand_fwd_ls");
	m_legs[eLegsRunFwdRight]			= skeleton->ID_Cycle_Safe("stand_fwd_rs");
	m_legs[eLegsRunBkwdLeft]			= skeleton->ID_Cycle_Safe("stand_bwd_ls");
	m_legs[eLegsRunBkwdRight]			= skeleton->ID_Cycle_Safe("stand_bwd_rs");
	m_legs[eLegsStealFwd]				= skeleton->ID_Cycle_Safe("new_walk_steal_0");
	m_legs[eLegsStealBkwd]				= skeleton->ID_Cycle_Safe("new_walk_steal_beack_0");
	
	m_legs[eLegsStandDamaged]			= skeleton->ID_Cycle_Safe("new_run_fwd_0");
	m_legs[eLegsRunDamaged]				= skeleton->ID_Cycle_Safe("new_run_fwd_0");
	m_legs[eLegsWalkDamaged]			= skeleton->ID_Cycle_Safe("new_run_fwd_0");
	m_legs[eLegsBackRunDamaged]			= skeleton->ID_Cycle_Safe("new_run_fwd_0");
	m_legs[eLegsRunStrafeLeftDamaged]	= skeleton->ID_Cycle_Safe("new_run_fwd_0");
	m_legs[eLegsRunStrafeRightDamaged]	= skeleton->ID_Cycle_Safe("new_run_fwd_0");

	m_torso[eTorsoIdle]					= skeleton->ID_Cycle_Safe("new_torso_attack_0");
	m_torso[eTorsoDanger]				= skeleton->ID_Cycle_Safe("new_torso_steal_0");
	m_torso[eTorsoPsyAttack]			= skeleton->ID_Cycle_Safe("new_torso_idle_0");
	m_torso[eTorsoPanic]				= skeleton->ID_Cycle_Safe("new_torso_run_0");

	add_path_rotation					(ACT_RUN, 0,				eLegsRun);
	add_path_rotation					(ACT_RUN, PI,				eLegsBackRun);
	add_path_rotation					(ACT_RUN, PI_DIV_4,			eLegsRunFwdLeft);
	add_path_rotation					(ACT_RUN, -PI_DIV_4,		eLegsRunFwdRight);
	add_path_rotation					(ACT_RUN, (PI - PI_DIV_4),	eLegsRunBkwdLeft);
	add_path_rotation					(ACT_RUN, -(PI - PI_DIV_4),	eLegsRunBkwdRight);

	add_path_rotation					(ACT_STEAL, 0,				eLegsStealFwd);
	add_path_rotation					(ACT_STEAL, PI,				eLegsStealBkwd);

	// 1. link animation with action
	// 2. link animation with velocities and path velocities
	// 3. 
}

void CControllerAnimation::add_path_rotation(EAction action, float angle, ELegsActionType type)
{
	SPathRotations	rot;
	rot.angle		= angle;
	rot.legs_motion	= type;

	PATH_ROTATIONS_MAP_IT map_it = m_path_rotations.find(action);
	if (map_it == m_path_rotations.end()) {
		PATH_ROTATIONS_VEC vec;
		vec.push_back(rot);
		m_path_rotations.insert(mk_pair(action, vec));
	} else {
		map_it->second.push_back(rot);
	}
}

void CControllerAnimation::select_velocity() 
{
	if (m_tAction == ACT_RUN)
		m_man->path_builder().set_desirable_speed(4.f);
	else if (m_tAction == ACT_STEAL)
		m_man->path_builder().set_desirable_speed(1.5f);
	else 
		m_man->path_builder().set_desirable_speed(0.f);
}

void CControllerAnimation::set_direction()
{
	if (!m_man->path_builder().is_moving_on_path()) return;
	
	float cur_yaw = Fvector().sub(m_controller->m_look_point, m_object->Position()).getH();
	cur_yaw = angle_normalize(-cur_yaw);

	float target_yaw = m_man->path_builder().detail().direction().getH();
	target_yaw = angle_normalize(-target_yaw);

	SPathRotations path_rot = get_path_rotation(cur_yaw);	

	m_object->dir().set_heading(angle_normalize(target_yaw + path_rot.angle));
	m_object->dir().set_heading_speed(PI);
}

void CControllerAnimation::select_torso_animation()
{
	// select from mental state	
	// start new animation
	SControlAnimationData		*ctrl_data = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	if (!ctrl_data) return;

	if (ctrl_data->torso.motion != m_torso[eTorsoDanger])
		ctrl_data->torso.actual	= false;

	ctrl_data->torso.motion	= m_torso[eTorsoDanger];
}

void CControllerAnimation::select_legs_animation()
{
	// select from action
	ELegsActionType legs_action = eLegsUndefined;

	if (m_man->path_builder().is_moving_on_path()) {
		// if we are moving, get yaw from path
		float cur_yaw, target_yaw;
		m_man->direction().get_heading(cur_yaw, target_yaw);

		SPathRotations	path_rot = get_path_rotation(cur_yaw);
		legs_action		= path_rot.legs_motion;
	
	} else {
		// else select standing animation
		legs_action		= eLegsStand;
	}
	
	// start new animation
	SControlAnimationData		*ctrl_data = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	if (!ctrl_data) return;
	
	if (ctrl_data->legs.motion != m_legs[legs_action])
		ctrl_data->legs.actual	= false;

	ctrl_data->legs.motion	= m_legs[legs_action];
}

CControllerAnimation::SPathRotations CControllerAnimation::get_path_rotation(float cur_yaw)
{
	EAction action = m_tAction;
	if ((action != ACT_RUN) && (action != ACT_STEAL)) action = ACT_RUN;
	
	float target_yaw = m_man->path_builder().detail().direction().getH();
	target_yaw = angle_normalize(-target_yaw);

	float	diff	= angle_difference(cur_yaw,target_yaw);
	if (from_right(target_yaw, cur_yaw)) diff = -diff;

	diff = angle_normalize(diff);

	PATH_ROTATIONS_VEC_IT it_best = m_path_rotations[m_tAction].begin();
	float best_diff = flt_max;
	for (PATH_ROTATIONS_VEC_IT it = m_path_rotations[m_tAction].begin(); it != m_path_rotations[m_tAction].end(); it++) {
		float angle_diff = angle_normalize(it->angle);

		float cur_diff = angle_difference(angle_diff, diff);
		if (cur_diff < best_diff) {
			best_diff	= cur_diff;
			it_best		= it;
		}
	}

	return (*it_best);
}
