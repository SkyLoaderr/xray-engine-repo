////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_legs.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager : legs animations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "sight_manager.h"
#include "stalker_movement_manager.h"
#include "stalker_animation_data.h"

const float right_forward_angle			= PI_DIV_4;
const float left_forward_angle			= PI_DIV_4;
const float standing_turn_angle			= PI_DIV_6;
const float epsilon						= EPS_L;
const u32	direction_switch_interval 	= 500;
const float direction_angles[]			= {
	0.f,		//	eMovementDirectionForward
	PI,			//	eMovementDirectionBackward
	PI_DIV_2,	//	eMovementDirectionLeft
	-PI_DIV_2	//	eMovementDirectionRight
};

void CStalkerAnimationManager::legs_play_callback			(CBlend *blend)
{
	CAI_Stalker				*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY					(object);
	object->animation().legs().make_inactual();
}

IC	float CStalkerAnimationManager::legs_switch_factor		() const
{
	if	(
			(m_target_direction == eMovementDirectionForward) &&
			(m_current_direction == eMovementDirectionBackward)
		)
		return					(0.f);

	if	(
			(m_target_direction == eMovementDirectionBackward) &&
			(m_current_direction == eMovementDirectionForward)
		)
		return					(0.f);

	if	(
			(m_target_direction == eMovementDirectionLeft) &&
			(m_current_direction == eMovementDirectionRight)
		)
		return					(0.f);

	if	(
			(m_target_direction == eMovementDirectionRight) &&
			(m_current_direction == eMovementDirectionLeft)
		)
		return					(0.f);

	return						(1.f);
}

void CStalkerAnimationManager::legs_assign_direction		(float switch_factor, const EMovementDirection &direction)
{
	if (m_current_direction == direction) {
		m_direction_start		= Device.dwTimeGlobal;
		return;
	}

	if (m_target_direction != direction) {
		m_direction_start		= Device.dwTimeGlobal;
		m_target_direction		= direction;
		return;
	}

	VERIFY						(m_direction_start <= Device.dwTimeGlobal);
	if ((Device.dwTimeGlobal - m_direction_start) <= (u32)iFloor(switch_factor*direction_switch_interval))
		return;

	m_direction_start			= Device.dwTimeGlobal;
	m_current_direction			= direction;
}

void CStalkerAnimationManager::legs_process_direction		(float yaw)
{
	float						switch_factor = legs_switch_factor();
	CStalkerMovementManager		&movement = object().movement();
	float						head_current = movement.head_orientation().current.yaw;
	float						left = left_angle(yaw,head_current);
	float						test_angle_forward = right_forward_angle;
	float						test_angle_backward = left_forward_angle;
	if (left) {
		test_angle_forward		= left_forward_angle;
		test_angle_backward		= right_forward_angle;
	}
	test_angle_backward			= PI - test_angle_backward;

	float						difference = angle_difference(yaw,head_current);

	if (difference <= test_angle_forward)
		legs_assign_direction			(switch_factor,eMovementDirectionForward);
	else {
		if (difference > test_angle_backward)
			legs_assign_direction		(switch_factor,eMovementDirectionBackward);
		else
			if (left)
				legs_assign_direction	(switch_factor,eMovementDirectionLeft);
			else
				legs_assign_direction	(switch_factor,eMovementDirectionRight);
	}
	
	movement.m_body.target.yaw	= yaw + direction_angles[m_current_direction];
}

MotionID CStalkerAnimationManager::legs_move_animation		()
{
	CStalkerMovementManager		&movement = object().movement();

	VERIFY						(
		(movement.body_state() == eBodyStateStand) ||
		(movement.mental_state() != eMentalStateFree)
	);

	if (eMentalStateDanger != movement.mental_state()) {
		m_current_speed			= movement.speed(eMovementDirectionForward);

		return					(
			m_data_storage->m_part_animations.A[
				body_state()
			].m_movement.A[
				movement.movement_type()
			].A[
				eMovementDirectionForward
			].A[
				1
			]
		);
	}

	float						yaw,pitch;
	object().sight().GetDirectionAngles(yaw,pitch);

	yaw							= angle_normalize_signed(-yaw);;
	legs_process_direction		(yaw);

	float						body_current = movement.body_orientation().current.yaw;
	bool						left = left_angle(yaw,body_current);
	float						test_angle_forward = right_forward_angle;
	float						test_angle_backward = left_forward_angle;
	if (left) {
		test_angle_forward		= left_forward_angle;
		test_angle_backward		= right_forward_angle;
	}
	test_angle_backward			= PI - test_angle_backward;

	EMovementDirection			speed_direction;
	float						difference = angle_difference(yaw,body_current);

	if (difference <= test_angle_forward)
		speed_direction			= eMovementDirectionForward;
	else {
		if (difference > test_angle_backward)
			speed_direction		= eMovementDirectionBackward;
		else {
			if (left)
				speed_direction	= eMovementDirectionLeft;
			else
				speed_direction	= eMovementDirectionRight;
		}
	}

	m_current_speed				= movement.speed(speed_direction);

	return						(
		m_data_storage->m_part_animations.A[
			body_state()
		].m_movement.A[
			movement.movement_type()
		].A[
			speed_direction
		].A[
			0
		]
	);
}

MotionID CStalkerAnimationManager::legs_no_move_animation	()
{
	m_current_speed				= 0.f;

	const xr_vector<MotionID>	&animation = m_data_storage->m_part_animations.A[body_state()].m_in_place->A;

	CStalkerMovementManager		&movement = object().movement();
	const SBoneRotation			&body_orientation = movement.body_orientation();
	float						current = body_orientation.current.yaw;
	float						target = body_orientation.target.yaw;
	if (angle_difference(target,current) < EPS_L) {

		float					head_current = movement.head_orientation().current.yaw;
		if (angle_difference(current,head_current) > standing_turn_angle)
			movement.m_body.target.yaw	= head_current;

		if (movement.mental_state() == eMentalStateFree)
			return				(animation[1]);

		if (movement.body_state() == eBodyStateCrouch)
			return				(animation[1]);

		return					(animation[0]);
	}

	if (left_angle(current,target)) {
		if (movement.mental_state() == eMentalStateFree)
			return				(animation[4]);

		return					(animation[2]);
	}

	if (movement.mental_state() == eMentalStateFree)
		return					(animation[5]);

	return						(animation[3]);
}

MotionID CStalkerAnimationManager::assign_legs_animation	()
{
	if (standing())
		return					(legs_no_move_animation());

	return						(legs_move_animation());
}
