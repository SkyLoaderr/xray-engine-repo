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
	object->animation().legs().make_inactual();
}

const CAnimationPair *CStalkerAnimationManager::assign_legs_animation	()
{
	EBodyState				l_tBodyState = body_state();
	
	// moving
	float					yaw, pitch;
	object().sight().GetDirectionAngles	(yaw,pitch);
	yaw						= angle_normalize_signed(-yaw);

	if ((object().movement().speed() < EPS_L) || (eMovementTypeStand == object().movement().movement_type())) {
		// standing
		if (angle_difference(object().movement().body_orientation().current.yaw,object().movement().body_orientation().target.yaw) <= EPS_L)
			return			(&m_part_animations.A[l_tBodyState].m_in_place->A[0]);
		else
			if (left_angle(-object().movement().body_orientation().target.yaw,-object().movement().body_orientation().current.yaw))
				return		(&m_part_animations.A[l_tBodyState].m_in_place->A[(object().movement().mental_state() == eMentalStateFree) ? 3 : 1]);
			else
				return		(&m_part_animations.A[l_tBodyState].m_in_place->A[(object().movement().mental_state() == eMentalStateFree) ? 4 : 2]);
	}

	float					fAnimationSwitchFactor = 1.f;
	if	(((eMovementDirectionForward == m_desirable_direction) && (eMovementDirectionBack == m_movement_direction))	||	((eMovementDirectionBack == m_desirable_direction) && (eMovementDirectionForward == m_movement_direction)))
		fAnimationSwitchFactor = .0f;

	if	(((eMovementDirectionRight == m_desirable_direction) && (eMovementDirectionLeft == m_movement_direction))	||	((eMovementDirectionLeft == m_desirable_direction) && (eMovementDirectionRight == m_movement_direction)))
		fAnimationSwitchFactor = .0f;

	if ((object().movement().body_state() != eBodyStateStand) && (eMentalStateDanger != object().movement().mental_state()))
		object().movement().set_mental_state	(eMentalStateDanger);

	if (eMentalStateDanger != object().movement().mental_state()) {
		if (angle_difference(object().movement().body_orientation().current.yaw,yaw) <= PI_DIV_6)
			return			(&m_part_animations.A[l_tBodyState].m_movement.A[object().movement().movement_type()].A[eMovementDirectionForward].A[object().movement().mental_state()]);
		fAnimationSwitchFactor	= .0f;
	}

	bool	forward_direction = false;
	bool	left = left_angle(-yaw,-object().head_orientation().current.yaw);

	if (left) {
		if (angle_difference(yaw,object().head_orientation().current.yaw) <= 2*PI_DIV_3)
			forward_direction = true;
	}
	else
		if (angle_difference(yaw,object().head_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;

	if (forward_direction) {
		// moving forward
		if (eMovementDirectionForward == m_movement_direction)
			m_direction_start_time	= Device.dwTimeGlobal;
		else
			if (eMovementDirectionForward != m_desirable_direction) {
				m_direction_start_time	= Device.dwTimeGlobal;
				m_desirable_direction	= eMovementDirectionForward;
			}
			else
				if ((Device.dwTimeGlobal - m_direction_start_time) > m_animation_switch_interval*fAnimationSwitchFactor) {
					m_direction_start_time	= Device.dwTimeGlobal;
					m_movement_direction	= eMovementDirectionForward;
				}
	}
	else {
		bool	back = false;
		if (left) {
			if (angle_difference(object().head_orientation().current.yaw,yaw) > 2*PI_DIV_3)
				back = true;
		}
		else {
			if (angle_difference(object().head_orientation().current.yaw,yaw) > PI_DIV_3)
				back = true;
		}
		if (!back)
			// moving left|right
			if (left) {
				// moving right, looking left
				if (eMovementDirectionRight == m_movement_direction)
					m_direction_start_time	= Device.dwTimeGlobal;
				else
					if (eMovementDirectionRight != m_desirable_direction) {
						m_direction_start_time	= Device.dwTimeGlobal;
						m_desirable_direction	= eMovementDirectionRight;
					}
					else
						if ((Device.dwTimeGlobal - m_direction_start_time) > m_animation_switch_interval*fAnimationSwitchFactor) {
							m_direction_start_time	= Device.dwTimeGlobal;
							m_movement_direction	= eMovementDirectionRight;
						}
			}
			else {
				// moving left, looking right
				if (eMovementDirectionLeft == m_movement_direction)
					m_direction_start_time	= Device.dwTimeGlobal;
				else
					if (eMovementDirectionLeft != m_desirable_direction) {
						m_direction_start_time	= Device.dwTimeGlobal;
						m_desirable_direction	= eMovementDirectionLeft;
					}
					else
						if ((Device.dwTimeGlobal - m_direction_start_time) > m_animation_switch_interval*fAnimationSwitchFactor) {
							m_direction_start_time	= Device.dwTimeGlobal;
							m_movement_direction	= eMovementDirectionLeft;
						}
			}
		else {
			// moving back
			if (eMovementDirectionBack == m_movement_direction)
				m_direction_start_time	= Device.dwTimeGlobal;
			else
				if (eMovementDirectionBack != m_desirable_direction) {
					m_direction_start_time	= Device.dwTimeGlobal;
					m_desirable_direction	= eMovementDirectionBack;
				}
				else
					if ((Device.dwTimeGlobal - m_direction_start_time) > m_animation_switch_interval*fAnimationSwitchFactor) {
						m_direction_start_time	= Device.dwTimeGlobal;
						m_movement_direction	= eMovementDirectionBack;
					}
		}
	}

	MonsterSpace::SBoneRotation		body_orientation = object().movement().body_orientation();
	body_orientation.target.yaw		= angle_normalize_signed(yaw + faTurnAngles[m_movement_direction]);
	object().movement().set_body_orientation	(body_orientation);
	
	forward_direction	= false;
	left				= left_angle(-yaw,-object().movement().body_orientation().current.yaw);

	if (left) {
		if (angle_difference(yaw,object().movement().body_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;
	}
	else
		if (angle_difference(yaw,object().movement().body_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;

	bool				back = false;

	if (left) {
		if (angle_difference(object().movement().body_orientation().current.yaw,yaw) > 2*PI_DIV_3)
			back = true;
	}
	else {
		if (angle_difference(object().movement().body_orientation().current.yaw,yaw) > PI_DIV_3)
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

	object().adjust_speed_to_animation	(direction);
	return							(&m_part_animations.A[l_tBodyState].m_movement.A[object().movement().movement_type()].A[direction].A[0]);
}
