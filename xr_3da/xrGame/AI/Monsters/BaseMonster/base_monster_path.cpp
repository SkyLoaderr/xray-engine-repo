////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base_monster.h"
#include "../../../graph_engine.h"
#include "../../../game_graph.h"
#include "../../../game_level_cross_table.h"
#include "../corpse_cover.h"
#include "../../../cover_manager.h"
#include "../../../cover_point.h"

// ������ ������ ����� ��-������� ���������� ��� ���� (e.g. �������� � ��������� ������ � �.�.)
void CBaseMonster::LookPosition(Fvector to_point, float angular_speed)
{
	// ��-��������� ������ �������� m_body.target.yaw
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());	
	
	// �������� ������ ����������� � ��������� ����� � ��� ������� ����
	float		yaw,p;
	dir.getHP(yaw,p);

	// ���������� ������� ����
	m_body.target.yaw = angle_normalize(-yaw);
}

void CBaseMonster::on_travel_point_change()
{
	if (IsMovingOnPath()) {
		u32 cur_point_velocity_index = CDetailPathManager::path()[curr_travel_point_index()].velocity;		
		if ((cur_point_velocity_index == eVelocityParameterStand) && !fis_zero(m_velocity_linear.current) && !b_velocity_reset) {
			stop_linear		();
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

// Get Point From Node in Radius
bool CBaseMonster::GetNodeInRadius(u32 src_node, float min_radius, float max_radius, u32 attempts, u32 &dest_node)
{
	Fvector vertex_position = ai().level_graph().vertex_position(src_node);

	for (u32 i=0; i<attempts; i++) {
		
		Fvector dir;
		dir.random_dir	();
		dir.normalize	();

		Fvector new_pos;
		new_pos.mad(vertex_position, dir, Random.randF(min_radius, max_radius));

		dest_node = ai().level_graph().check_position_in_direction(src_node, vertex_position, new_pos);
		if (dest_node != u32(-1) && accessible(dest_node)) return (true);
	}
	return false;
}
