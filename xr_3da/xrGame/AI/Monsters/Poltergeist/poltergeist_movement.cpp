#include "stdafx.h"
#include "poltergeist.h"
#include "../../../detail_path_manager.h"

void CPoltergeist::move_along_path(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta)
{
	if (!m_hidden) {
		inherited::move_along_path(movement_control, dest_position, time_delta);
		return;
	}

	dest_position		= m_current_position;

	// Если нет движения по пути
	if (!CMovementManager::enabled() || 
		CMovementManager::path_completed() || 
		detail_path_manager().path().empty() ||
		detail_path_manager().completed(m_current_position,true) || 
		(detail_path_manager().curr_travel_point_index() >= detail_path_manager().path().size() - 1) ||
		fis_zero(CMovementManager::old_desirable_speed())
		)
	{
		m_speed			= 0.f;
		dest_position	= CalculateRealPosition();
		return;
	}

	if (time_delta < EPS) {
		dest_position	= CalculateRealPosition();
		return;
	}

	// Вычислить пройденную дистанцию, определить целевую позицию на маршруте, 
	//			 изменить detail_path_manager().curr_travel_point_index()

	float				desirable_speed		=	old_desirable_speed();				// желаемая скорость объекта
	float				dist				=	desirable_speed * time_delta;		// пройденное расстояние в соостветствие с желаемой скоростью 
	float				desirable_dist		=	dist;

	// определить целевую точку
	Fvector				target;

	u32 prev_cur_point_index = detail_path_manager().curr_travel_point_index();

	// обновить detail_path_manager().curr_travel_point_index() в соответствие с текущей позицией
	while (detail_path_manager().curr_travel_point_index() < detail_path_manager().path().size() - 2) {

		float pos_dist_to_cur_point			= dest_position.distance_to(detail_path_manager().path()[detail_path_manager().curr_travel_point_index()].position);
		float pos_dist_to_next_point		= dest_position.distance_to(detail_path_manager().path()[detail_path_manager().curr_travel_point_index()+1].position);
		float cur_point_dist_to_next_point	= detail_path_manager().path()[detail_path_manager().curr_travel_point_index()].position.distance_to(detail_path_manager().path()[detail_path_manager().curr_travel_point_index()+1].position);

		if ((pos_dist_to_cur_point > cur_point_dist_to_next_point) && (pos_dist_to_cur_point > pos_dist_to_next_point)) {
			++detail_path_manager().m_current_travel_point;			
		} else break;
	}

	target.set			(detail_path_manager().path()[detail_path_manager().curr_travel_point_index() + 1].position);
	// определить направление к целевой точке
	Fvector				dir_to_target;
	dir_to_target.sub	(target, dest_position);

	// дистанция до целевой точки
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
		detail_path_manager().m_current_travel_point = detail_path_manager().path().size() - 1;
		m_speed			= 0.f;
		dest_position	= CalculateRealPosition();
		return;
	}

	// установить позицию
	Fvector				motion;
	motion.mul			(dir_to_target, dist / dist_to_target);
	dest_position.add	(motion);

	// установить скорость
	float	real_motion	= motion.magnitude() + desirable_dist - dist;
	float	real_speed	= real_motion / time_delta;

	m_speed				= 0.5f * desirable_speed + 0.5f * real_speed;

	// Обновить позицию
	m_current_position	= dest_position;
	Position()			= CalculateRealPosition();
	dest_position		= Position();
}

Fvector CPoltergeist::CalculateRealPosition()
{
	Fvector ret_val = m_current_position;
	ret_val.y += m_height;
	return (ret_val);
}
