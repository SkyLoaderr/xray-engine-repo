#include "stdafx.h"
#include "poltergeist.h"
#include "../../../detail_path_manager.h"
#include "../ai_monster_movement.h"

void CPoltergeist::move_along_path(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta)
{
	if (!m_hidden) {
		movement().move_along_path(movement_control, dest_position, time_delta);
		return;
	}

#if 0
	dest_position		= m_current_position;

	// Если нет движения по пути
	if (!movement().enabled() || 
		movement().path_completed() || 
		movement().detail().path().empty() ||
		movement().detail().completed(m_current_position,true) || 
		(movement().detail().curr_travel_point_index() >= movement().detail().path().size() - 1) ||
		fis_zero(movement().old_desirable_speed())
		)
	{
		movement().m_speed	= 0.f;
		dest_position		= CalculateRealPosition();
		return;
	}

	if (time_delta < EPS) {
		dest_position	= CalculateRealPosition();
		return;
	}

	// Вычислить пройденную дистанцию, определить целевую позицию на маршруте, 
	//			 изменить movement().detail().curr_travel_point_index()

	float				desirable_speed		=	movement().old_desirable_speed();				// желаемая скорость объекта
	float				dist				=	desirable_speed * time_delta;		// пройденное расстояние в соостветствие с желаемой скоростью 
	float				desirable_dist		=	dist;

	// определить целевую точку
	Fvector				target;

	u32 prev_cur_point_index = movement().detail().curr_travel_point_index();

	// обновить movement().detail().curr_travel_point_index() в соответствие с текущей позицией
	while (movement().detail().curr_travel_point_index() < movement().detail().path().size() - 2) {

		float pos_dist_to_cur_point			= dest_position.distance_to(movement().detail().path()[movement().detail().curr_travel_point_index()].position);
		float pos_dist_to_next_point		= dest_position.distance_to(movement().detail().path()[movement().detail().curr_travel_point_index()+1].position);
		float cur_point_dist_to_next_point	= movement().detail().path()[movement().detail().curr_travel_point_index()].position.distance_to(movement().detail().path()[movement().detail().curr_travel_point_index()+1].position);

		if ((pos_dist_to_cur_point > cur_point_dist_to_next_point) && (pos_dist_to_cur_point > pos_dist_to_next_point)) {
			++movement().detail().m_current_travel_point;			
		} else break;
	}

	target.set			(movement().detail().path()[movement().detail().curr_travel_point_index() + 1].position);
	// определить направление к целевой точке
	Fvector				dir_to_target;
	dir_to_target.sub	(target, dest_position);

	// дистанция до целевой точки
	float				dist_to_target = dir_to_target.magnitude();

	while (dist > dist_to_target) {
		dest_position.set	(target);

		if (movement().detail().curr_travel_point_index() + 1 >= movement().detail().path().size())	break;
		else {
			dist			-= dist_to_target;
			++movement().detail().m_current_travel_point;
			if ((movement().detail().curr_travel_point_index()+1) >= movement().detail().path().size())
				break;
			target.set			(movement().detail().path()[movement().detail().curr_travel_point_index() + 1].position);
			dir_to_target.sub	(target, dest_position);
			dist_to_target		= dir_to_target.magnitude();
		}
	}

	if (prev_cur_point_index != movement().detail().curr_travel_point_index()) on_travel_point_change();

	if (dist_to_target < EPS_L) {
		movement().detail().m_current_travel_point = movement().detail().path().size() - 1;
		movement().m_speed			= 0.f;
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

	movement().m_speed				= 0.5f * desirable_speed + 0.5f * real_speed;

	// Обновить позицию
	m_current_position	= dest_position;
	Position()			= CalculateRealPosition();
	dest_position		= Position();
#endif
}

Fvector CPoltergeist::CalculateRealPosition()
{
	Fvector ret_val = m_current_position;
	ret_val.y += m_height;
	return (ret_val);
}
