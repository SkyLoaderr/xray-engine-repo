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

float faTurnAngles	[] = {
	0.f,
	M_PI,
	PI_DIV_6,
	0.f,
};

void CStalkerAnimationManager::legs_play_callback		(CBlend *blend)
{
	CAI_Stalker				*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY					(object);
	object->animation().legs().reset();
}

const CAnimationPair *CStalkerAnimationManager::assign_legs_animation	()
{
	EBodyState				l_tBodyState = body_state();
	
	// moving
	float					yaw, pitch;
	object()->GetDirectionAngles	(yaw,pitch);
	yaw						= angle_normalize_signed(-yaw);

	if ((object()->speed() < EPS_L) || (eMovementTypeStand == object()->movement_type())) {
		// standing
		if (angle_difference(object()->body_orientation().current.yaw,object()->body_orientation().target.yaw) <= EPS_L)
			return			(&m_part_animations.A[l_tBodyState].m_in_place->A[0]);
		else
			return			(&m_part_animations.A[l_tBodyState].m_in_place->A[left_angle(-object()->body_orientation().target.yaw,-object()->body_orientation().current.yaw) ? 1 : 2]);
	}

	float					fAnimationSwitchFactor = 1.f;
	if	(((eMovementDirectionForward == m_desirable_direction) && (eMovementDirectionBack == m_movement_direction))	||	((eMovementDirectionBack == m_desirable_direction) && (eMovementDirectionForward == m_movement_direction)))
		fAnimationSwitchFactor = .0f;

	if	(((eMovementDirectionRight == m_desirable_direction) && (eMovementDirectionLeft == m_movement_direction))	||	((eMovementDirectionLeft == m_desirable_direction) && (eMovementDirectionRight == m_movement_direction)))
		fAnimationSwitchFactor = .0f;

	if ((object()->body_state() != eBodyStateStand) && (eMentalStateDanger != object()->mental_state()))
		object()->set_mental_state	(eMentalStateDanger);

	if (eMentalStateDanger != object()->mental_state()) {
		if (angle_difference(object()->body_orientation().current.yaw,yaw) <= PI_DIV_6)
			return			(&m_part_animations.A[l_tBodyState].m_movement.A[object()->movement_type()].A[eMovementDirectionForward].A[object()->mental_state()]);
		fAnimationSwitchFactor	= .0f;
	}

	bool	forward_direction = false;
	bool	left = left_angle(-yaw,-object()->head_orientation().current.yaw);

	if (left) {
		if (angle_difference(yaw,object()->head_orientation().current.yaw) <= 2*PI_DIV_3)
			forward_direction = true;
	}
	else
		if (angle_difference(yaw,object()->head_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;

	if (forward_direction) {
		// moving forward
		if (eMovementDirectionForward == m_movement_direction)
			m_direction_start_time	= Level().timeServer();
		else
			if (eMovementDirectionForward != m_desirable_direction) {
				m_direction_start_time	= Level().timeServer();
				m_desirable_direction	= eMovementDirectionForward;
			}
			else
				if ((Level().timeServer() - m_direction_start_time) > m_animation_switch_interval*fAnimationSwitchFactor) {
					m_direction_start_time	= Level().timeServer();
					m_movement_direction	= eMovementDirectionForward;
				}
	}
	else {
		bool	back = false;
		if (left) {
			if (angle_difference(object()->head_orientation().current.yaw,yaw) > 2*PI_DIV_3)
				back = true;
		}
		else {
			if (angle_difference(object()->head_orientation().current.yaw,yaw) > PI_DIV_3)
				back = true;
		}
		if (!back)
			// moving left|right
			if (left) {
				// moving right, looking left
				if (eMovementDirectionRight == m_movement_direction)
					m_direction_start_time	= Level().timeServer();
				else
					if (eMovementDirectionRight != m_desirable_direction) {
						m_direction_start_time	= Level().timeServer();
						m_desirable_direction	= eMovementDirectionRight;
					}
					else
						if ((Level().timeServer() - m_direction_start_time) > m_animation_switch_interval*fAnimationSwitchFactor) {
							m_direction_start_time	= Level().timeServer();
							m_movement_direction	= eMovementDirectionRight;
						}
			}
			else {
				// moving left, looking right
				if (eMovementDirectionLeft == m_movement_direction)
					m_direction_start_time	= Level().timeServer();
				else
					if (eMovementDirectionLeft != m_desirable_direction) {
						m_direction_start_time	= Level().timeServer();
						m_desirable_direction	= eMovementDirectionLeft;
					}
					else
						if ((Level().timeServer() - m_direction_start_time) > m_animation_switch_interval*fAnimationSwitchFactor) {
							m_direction_start_time	= Level().timeServer();
							m_movement_direction	= eMovementDirectionLeft;
						}
			}
		else {
			// moving back
			if (eMovementDirectionBack == m_movement_direction)
				m_direction_start_time	= Level().timeServer();
			else
				if (eMovementDirectionBack != m_desirable_direction) {
					m_direction_start_time	= Level().timeServer();
					m_desirable_direction	= eMovementDirectionBack;
				}
				else
					if ((Level().timeServer() - m_direction_start_time) > m_animation_switch_interval*fAnimationSwitchFactor) {
						m_direction_start_time	= Level().timeServer();
						m_movement_direction	= eMovementDirectionBack;
					}
		}
	}

	MonsterSpace::SBoneRotation		body_orientation = object()->body_orientation();
	body_orientation.target.yaw		= angle_normalize_signed(yaw + faTurnAngles[m_movement_direction]);
	object()->set_body_orientation	(body_orientation);
	
	forward_direction	= false;
	left				= left_angle(-yaw,-object()->body_orientation().current.yaw);

	if (left) {
		if (angle_difference(yaw,object()->body_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;
	}
	else
		if (angle_difference(yaw,object()->body_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;

	bool				back = false;

	if (left) {
		if (angle_difference(object()->body_orientation().current.yaw,yaw) > 2*PI_DIV_3)
			back = true;
	}
	else {
		if (angle_difference(object()->body_orientation().current.yaw,yaw) > PI_DIV_3)
			back = true;
	}
	
	EMovementDirection				direction;
	if (left)
		if (forward_direction)
			direction				= eMovementDirectionForward;
		else
			if (back)
				direction			= eMovementDirectionBack;
			else
				direction			= eMovementDirectionRight;
	else
		if (forward_direction)
			direction				= eMovementDirectionForward;
		else
			if (back)
				direction			= eMovementDirectionBack;
			else
				direction			= eMovementDirectionLeft;

	object()->adjust_speed_to_animation	(direction);
	return							(&m_part_animations.A[l_tBodyState].m_movement.A[object()->movement_type()].A[direction].A[0]);
}
