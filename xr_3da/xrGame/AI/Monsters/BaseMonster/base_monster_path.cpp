////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base_monster.h"
#include "../../../game_graph.h"
#include "../../../game_level_cross_table.h"
#include "../corpse_cover.h"
#include "../../../cover_manager.h"
#include "../../../cover_point.h"
#include "../../../detail_path_manager.h"
#include "../ai_monster_movement.h"
#include "../../../ai_space.h"

// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)
void CBaseMonster::LookPosition(Fvector to_point, float angular_speed)
{
	// по-умолчанию просто изменить movement().m_body.target.yaw
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());	
	
	// получаем вектор направления к источнику звука и его мировые углы
	float		yaw,p;
	dir.getHP(yaw,p);

	// установить текущий угол
	movement().m_body.target.yaw = angle_normalize(-yaw);
}

void CBaseMonster::on_travel_point_change()
{
	if (movement().IsMovingOnPath()) {
		u32 cur_point_velocity_index = movement().detail().path()[movement().detail().curr_travel_point_index()].velocity;		
		if ((cur_point_velocity_index == eVelocityParameterStand) && !fis_zero(movement().m_velocity_linear.current) && !b_velocity_reset) {
			movement().stop_linear		();
			b_velocity_reset = true;
		}
		if (cur_point_velocity_index != eVelocityParameterStand) b_velocity_reset = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Covers
//////////////////////////////////////////////////////////////////////////

bool CBaseMonster::GetCorpseCover(Fvector &position, u32 &vertex_id) 
{
	m_corpse_cover_evaluator->setup(10.f,50.f);
	CCoverPoint	 *point = ai().cover_manager().best_cover(Position(),30.f,*m_corpse_cover_evaluator);
	if (!point) return false;
	
	position	= point->m_position;
	vertex_id	= point->m_level_vertex_id;
	return true;
}

bool CBaseMonster::GetCoverFromEnemy(const Fvector &enemy_pos, Fvector &position, u32 &vertex_id) 
{
	m_enemy_cover_evaluator->setup(enemy_pos, 30.f,50.f);
	CCoverPoint	 *point = ai().cover_manager().best_cover(Position(),40.f,*m_enemy_cover_evaluator);
	if (!point) return false;

	position	= point->m_position;
	vertex_id	= point->m_level_vertex_id;
	return true;
}

bool CBaseMonster::GetCoverFromPoint(const Fvector &pos, Fvector &position, u32 &vertex_id, float min_dist, float max_dist, float radius) 
{
	m_enemy_cover_evaluator->setup(pos, min_dist,max_dist);
	CCoverPoint	 *point = ai().cover_manager().best_cover(Position(),radius,*m_enemy_cover_evaluator);
	if (!point) return false;

	position	= point->m_position;
	vertex_id	= point->m_level_vertex_id;
	return true;
}

bool CBaseMonster::GetCoverCloseToPoint(const Fvector &dest_pos, float min_dist, float max_dist, float deviation, float radius ,Fvector &position, u32 &vertex_id) 
{
	m_cover_evaluator_close_point->setup(dest_pos,min_dist, max_dist,deviation);
	CCoverPoint	 *point = ai().cover_manager().best_cover(Position(),radius,*m_cover_evaluator_close_point);
	if (!point) return false;

	position	= point->m_position;
	vertex_id	= point->m_level_vertex_id;
	return true;
}
