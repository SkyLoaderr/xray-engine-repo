////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_physic.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager : physic character movement
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "PHMovementControl.h"
#include "detail_path_manager.h"
#include "level.h"
#include "custommonster.h"

#define DISTANCE_PHISICS_ENABLE_CHARACTERS 2.f


void CMovementManager::move_along_path	(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta)
{
	Fvector				motion;
	dest_position		= object().Position();

	float				precision = 0.5f;
	
	
	// ���� ��� �������� �� ����
	if (	!enabled() || 
			path_completed() || 
			detail_path_manager().path().empty() ||
			detail_path_manager().completed(dest_position,true) || 
			(detail_path_manager().curr_travel_point_index() >= detail_path_manager().path().size() - 1) ||
			fis_zero(old_desirable_speed())
		)
	{
		m_speed			= 0.f;

		// ������ ������������� ������� � ������������ � ������� ��������� 
		if(!movement_control->JumpState())
		{
			Fvector velocity={0.f,0.f,0.f};
			movement_control->SetVelocity		(velocity);
		}
		if(movement_control->IsCharacterEnabled()) {
			movement_control->Calculate(detail_path_manager().path(),0.f,detail_path_manager().m_current_travel_point,precision);
			movement_control->GetPosition(dest_position);
		}

		// �������� �� ���
		if (!fsimilar(0.f,movement_control->gcontact_HealthLost)) {
			Fvector		d;
			d.set		(0,1,0);
			object().Hit(movement_control->gcontact_HealthLost,d,m_object,movement_control->ContactBone(),dest_position,0);
		}
		return;
	}



	if (time_delta < EPS) return;

	//#pragma todo("Dima to Kostia : Please change this piece of code to support paths with multiple desired velocities")
	
	
	// ��������� ���������� ���������, ���������� ������� ������� �� ��������, 
	//			 �������� detail_path_manager().m_current_travel_point
	
	float				desirable_speed		=	old_desirable_speed();				// �������� �������� �������
	float				dist				=	desirable_speed * time_delta;		// ���������� ���������� � ������������� � �������� ��������� 
	float				desirable_dist		=	dist;

	// ���������� ������� �����
	Fvector				target;
	
	u32 prev_cur_point_index = detail_path_manager().curr_travel_point_index();

	// �������� detail_path_manager().m_current_travel_point � ������������ � ������� ��������
	while (detail_path_manager().m_current_travel_point < detail_path_manager().path().size() - 2) {

		float pos_dist_to_cur_point			= dest_position.distance_to(detail_path_manager().path()[detail_path_manager().m_current_travel_point].position);
		float pos_dist_to_next_point		= dest_position.distance_to(detail_path_manager().path()[detail_path_manager().m_current_travel_point+1].position);
		float cur_point_dist_to_next_point	= detail_path_manager().path()[detail_path_manager().m_current_travel_point].position.distance_to(detail_path_manager().path()[detail_path_manager().m_current_travel_point+1].position);
		
		if ((pos_dist_to_cur_point > cur_point_dist_to_next_point) && (pos_dist_to_cur_point > pos_dist_to_next_point)) {
			++detail_path_manager().m_current_travel_point;			
		} else break;
	}

	target.set			(detail_path_manager().path()[detail_path_manager().curr_travel_point_index() + 1].position);
	// ���������� ����������� � ������� �����
	Fvector				dir_to_target;
	dir_to_target.sub	(target, dest_position);

	// ��������� �� ������� �����
	float				dist_to_target = dir_to_target.magnitude();
	
	while (dist > dist_to_target) {
		dest_position.set	(target);

		if (detail_path_manager().curr_travel_point_index() + 1 >= detail_path_manager().path().size())	break;
		else {
			dist			-= dist_to_target;
			++detail_path_manager().m_current_travel_point;
			if ((detail_path_manager().curr_travel_point_index()+1) >= detail_path_manager().path().size())
				break;
			target.set			(detail_path_manager().path()[detail_path_manager().curr_travel_point_index() + 1].position);
			dir_to_target.sub	(target, dest_position);
			dist_to_target		= dir_to_target.magnitude();
		}
	}
	
	if (prev_cur_point_index != detail_path_manager().curr_travel_point_index()) on_travel_point_change();

	if (dist_to_target < EPS_L) {
#pragma todo("Dima to ? : is this correct?")
		detail_path_manager().m_current_travel_point = detail_path_manager().path().size() - 1;
		m_speed			= 0.f;
		return;
	}

	// ������ ������������� ����� �������
	Device.Statistic.Physics.Begin	();

	// �������� ���. ������� � �������
	object().setEnabled(false);
	Level().ObjectSpace.GetNearest		(dest_position,DISTANCE_PHISICS_ENABLE_CHARACTERS + (movement_control->IsCharacterEnabled() ? 0.5f : 0.f)); 
	xr_vector<CObject*> &tpNearestList	= Level().ObjectSpace.q_nearest; 
	object().setEnabled(true);

	// ���������� �������
	motion.mul			(dir_to_target, dist / dist_to_target);
	dest_position.add	(motion);
	
	Fvector velocity					=	dir_to_target;
	velocity.normalize_safe();							  //��� �� �������, mdir - �� ����������
	velocity.mul						(desirable_speed);//*1.25f
	if(!movement_control->JumpState())
				movement_control->SetVelocity		(velocity);

	if ((tpNearestList.empty())) {  // ��� ���. ��������
		
		if(!movement_control->TryPosition(dest_position)) {
			movement_control->Calculate		(detail_path_manager().path(),desirable_speed,detail_path_manager().m_current_travel_point,precision);
			movement_control->GetPosition	(dest_position);
			// �������� �� ���
			if (!fsimilar(0.f,movement_control->gcontact_HealthLost)) object().Hit	(movement_control->gcontact_HealthLost,dir_to_target,m_object,movement_control->ContactBone(),dest_position,0);

		} else {
			movement_control->b_exect_position	=	true;
			//Fvector velocity					=	mdir;
			//velocity.mul						(desirable_speed);//*1.25f
			//movement_control->SetVelocity		(velocity);
		}
	} else { // ���� ���. �������
		movement_control->Calculate				(detail_path_manager().path(), desirable_speed, detail_path_manager().m_current_travel_point, precision);
		movement_control->GetPosition			(dest_position);
		
		// �������� �� ���
		if (!fsimilar(0.f,movement_control->gcontact_HealthLost)) object().Hit	(movement_control->gcontact_HealthLost,dir_to_target,m_object,movement_control->ContactBone(),dest_position,0);
	}
	
	// ���������� ��������
	float	real_motion	= motion.magnitude() + desirable_dist - dist;
	float	real_speed	= real_motion / time_delta;
	
	m_speed				= 0.5f * desirable_speed + 0.5f * real_speed;
	
	Device.Statistic.Physics.End	();

}

