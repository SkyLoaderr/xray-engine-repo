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

#define DISTANCE_PHISICS_ENABLE_CHARACTERS 2.f


void CMovementManager::move_along_path	(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta)
{
	Fvector				motion;
	dest_position		= Position();

	float				precision = 0.5f;
	
	
	// ≈сли нет движени€ по пути
	if (	!enabled() || 
			path_completed() || 
			CDetailPathManager::path().empty() ||
			CDetailPathManager::completed(dest_position,true) || 
			(CDetailPathManager::curr_travel_point_index() >= CDetailPathManager::path().size() - 1) ||
			fis_zero(old_desirable_speed())
		)
	{
		m_speed			= 0.f;

		// ‘изика устанавливает позицию в соответствии с нулевой скоростью 
		if(!movement_control->JumpState())
		{
			Fvector velocity={0.f,0.f,0.f};
			movement_control->SetVelocity		(velocity);
		}
		if(movement_control->IsCharacterEnabled()) {
			movement_control->Calculate(CDetailPathManager::path(),0.f,CDetailPathManager::m_current_travel_point,precision);
			movement_control->GetPosition(dest_position);
		}

		// проверка на хит
		if (!fsimilar(0.f,movement_control->gcontact_HealthLost)) {
			Fvector		d;
			d.set		(0,1,0);
			Hit			(movement_control->gcontact_HealthLost,d,this,movement_control->ContactBone(),dest_position,0);
		}
		return;
	}



	if (time_delta < EPS) return;

	//#pragma todo("Dima to Kostia : Please change this piece of code to support paths with multiple desired velocities")
	
	
	// ¬ычислить пройденную дистанцию, определить целевую позицию на маршруте, 
	//			 изменить CDetailPathManager::m_current_travel_point
	
	float				desirable_speed		=	old_desirable_speed();				// желаема€ скорость объекта
	float				dist				=	desirable_speed * time_delta;		// пройденное рассто€ние в соостветствие с желаемой скоростью 
	float				desirable_dist		=	dist;

	// определить целевую точку
	Fvector				target;
	
	// обновить CDetailPathManager::m_current_travel_point в соответствие с текущей позицией
	while (CDetailPathManager::m_current_travel_point < CDetailPathManager::path().size() - 2) {

		float pos_dist_to_cur_point			= dest_position.distance_to(CDetailPathManager::path()[CDetailPathManager::m_current_travel_point].position);
		float pos_dist_to_next_point		= dest_position.distance_to(CDetailPathManager::path()[CDetailPathManager::m_current_travel_point+1].position);
		float cur_point_dist_to_next_point	= CDetailPathManager::path()[CDetailPathManager::m_current_travel_point].position.distance_to(CDetailPathManager::path()[CDetailPathManager::m_current_travel_point+1].position);
		
		if ((pos_dist_to_cur_point > cur_point_dist_to_next_point) && (pos_dist_to_cur_point > pos_dist_to_next_point)) {
			++CDetailPathManager::m_current_travel_point;			
		} else break;
	}

	target.set			(CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].position);
	// определить направление к целевой точке
	Fvector				dir_to_target;
	dir_to_target.sub	(target, dest_position);

	// дистанци€ до целевой точки
	float				dist_to_target = dir_to_target.magnitude();
	
	while (dist > dist_to_target) {
		dest_position.set	(target);

		if (CDetailPathManager::curr_travel_point_index() + 1 >= CDetailPathManager::path().size())	break;
		else {
			dist			-= dist_to_target;
			++CDetailPathManager::m_current_travel_point;
			if ((CDetailPathManager::curr_travel_point_index()+1) >= CDetailPathManager::path().size())
				break;
			target.set			(CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].position);
			dir_to_target.sub	(target, dest_position);
			dist_to_target		= dir_to_target.magnitude();
		}
	}
	
	if (dist_to_target < EPS_L) {
		CDetailPathManager::m_current_travel_point = CDetailPathManager::path().size() - 1;
		m_speed			= 0.f;
		return;
	}

	// ‘изика устанавливает новую позицию
	Device.Statistic.Physics.Begin	();

	// получить физ. объекты в радиусе
	setEnabled(false);
	Level().ObjectSpace.GetNearest		(dest_position,DISTANCE_PHISICS_ENABLE_CHARACTERS + (movement_control->IsCharacterEnabled() ? 0.5f : 0.f)); 
	xr_vector<CObject*> &tpNearestList	= Level().ObjectSpace.q_nearest; 
	setEnabled(true);

	// установить позицию
	motion.mul			(dir_to_target, dist / dist_to_target);
	dest_position.add	(motion);
	
	Fvector velocity					=	dir_to_target;
	velocity.normalize_safe();							  //как не странно, mdir - не нормирован
	velocity.mul						(desirable_speed);//*1.25f
	if(!movement_control->JumpState())
				movement_control->SetVelocity		(velocity);

	if ((tpNearestList.empty())) {  // нет физ. объектов
		
		if(!movement_control->TryPosition(dest_position)) {
			movement_control->Calculate		(CDetailPathManager::path(),desirable_speed,CDetailPathManager::m_current_travel_point,precision);
			movement_control->GetPosition	(dest_position);
			// проверка на хит
			if (!fsimilar(0.f,movement_control->gcontact_HealthLost)) Hit	(movement_control->gcontact_HealthLost,dir_to_target,this,movement_control->ContactBone(),dest_position,0);

		} else {
			movement_control->b_exect_position	=	true;
			//Fvector velocity					=	mdir;
			//velocity.mul						(desirable_speed);//*1.25f
			//movement_control->SetVelocity		(velocity);
		}
	} else { // есть физ. объекты
		movement_control->Calculate				(CDetailPathManager::path(), desirable_speed, CDetailPathManager::m_current_travel_point, precision);
		movement_control->GetPosition			(dest_position);
		
		// проверка на хит
		if (!fsimilar(0.f,movement_control->gcontact_HealthLost)) Hit	(movement_control->gcontact_HealthLost,dir_to_target,this,movement_control->ContactBone(),dest_position,0);
	}
	
	// установить скорость
	float	real_motion	= motion.magnitude() + desirable_dist - dist;
	float	real_speed	= real_motion / time_delta;
	
	m_speed				= 0.5f * desirable_speed + 0.5f * real_speed;
	
	Device.Statistic.Physics.End	();

}

