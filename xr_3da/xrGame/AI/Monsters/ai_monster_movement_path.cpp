#include "stdafx.h"
#include "ai_monster_movement.h"
#include "../../cover_point.h"
#include "../../cover_manager.h"
#include "../../cover_evaluators.h"
#include "BaseMonster/base_monster.h"
#include "../../detail_path_manager.h"
#include "../../level_location_selector.h"
#include "../../level_path_manager.h"
#include "../../ai_object_location.h"

#define MAX_PATH_DISTANCE		100.f

void CMonsterMovement::initialize_movement() 
{
	m_target_selected.init		();
	m_target_found.init		();
	m_target_set.init		();

	m_time						= 0;
	m_last_time_target_set		= 0;
	m_distance_to_path_end		= 1.f;
	m_failed					= false;
	m_cover_info.use_covers		= false;

	m_force_rebuild				= false;
	m_target_actual				= false;
	m_wait_path_end				= false;
}

//////////////////////////////////////////////////////////////////////////
// Set Target Point Routines
//////////////////////////////////////////////////////////////////////////

void CMonsterMovement::set_target_point(const Fvector &position, u32 node)
{
	// �������� ������������
	if (m_target_set.position.similar(position))	m_target_actual = true;
	else												m_target_actual = false;
	
	// ���������� �������
	m_target_set.set	(position,node);

	// ���������� ���������� ��������� ������������
	m_target_type		= eMoveToTarget;
	set_path_type		(MovementManager::ePathTypeLevelPath);

	select_target		();
}

void CMonsterMovement::set_retreat_from_point(const Fvector &position)
{
	// �������� ������������
	if (m_target_set.position.similar(position))	m_target_actual = true;
	else												m_target_actual = false;

	// ���������� �������
	m_target_set.set	(position,u32(-1));	

	// ���������� ���������� ��������� ������������
	m_target_type		= eRetreatFromTarget;
	set_path_type		(MovementManager::ePathTypeLevelPath);

	select_target		();
}

#define RANDOM_POINT_DISTANCE	20.f

void CMonsterMovement::select_target()
{
	if (m_wait_path_end) return;
	
	if (m_target_actual && m_failed) {
		
		m_force_rebuild		= true;
		m_wait_path_end		= true;
		
		// ���� ���� �������� ��� failed - ������� ������ ��������� �����
		Fvector	pos_random;	
		Fvector dir;		dir.random_dir	();

		pos_random.mad(object().Position(), dir, RANDOM_POINT_DISTANCE);

		// ���������� m_target.position
		if (!accessible(pos_random)) {
			m_target_selected.node		= restrictions().accessible_nearest(pos_random, m_target_selected.position);	
		} else {
			m_target_selected.node		= u32(-1);
			m_target_selected.position	= pos_random;
		}
	} else {
		m_target_selected.set			(m_target_set.position, m_target_set.node);
	}
}

//////////////////////////////////////////////////////////////////////////
bool CMonsterMovement::target_point_need_update()
{
	if (m_force_rebuild) {
		m_force_rebuild		= false;
		return				true;
	}

	// ���� ���� ��� �� ��������
	if (!IsPathEnd(m_distance_to_path_end)) {
		
		if (m_wait_path_end) return false;
		
		// ���� ����� �������� �� ���� �� �����, �� �������������
		return (m_last_time_target_set + m_time < m_object->m_current_update);
	}
	
	// ����� ����

	m_wait_path_end	= false;

	// ���� ���� ��� �� �������� - �����
	if (!detail().actual() && (detail().time_path_built() < m_last_time_target_set)) return false;
	return true;
}


//////////////////////////////////////////////////////////////////////////
// ���� �� ������ ������� m_target_found.node != u32(-1) - ���� �������
void CMonsterMovement::find_target()
{
	m_target_found.node	= m_target_selected.node;

	// I. ���������� �� ���� ����������
	
	// ������� ������� ������� � ������������ � ������������ ��������, ��������� ����������� ��������
	Fvector	dir;
	if (m_target_type == eMoveToTarget) {

		dir.sub						(m_target_selected.position, object().Position());
		//VERIFY						(!fis_zero(dir.square_magnitude())); //todo
		dir.normalize_safe			();
		m_target_found.position	= m_target_selected.position;

	} else if (m_target_type == eRetreatFromTarget){
		
		if (m_target_found.node == u32(-1)) {
			dir.sub							(object().Position(), m_target_selected.position);
			dir.normalize_safe				();
			m_target_found.position.mad		(object().Position(), dir, MAX_PATH_DISTANCE - 1.f);
		} else {
			m_target_found.position	= m_target_selected.position;
		}
	}

	// ���������� ���������� �� ��������� �����
	float dist = object().Position().distance_to(m_target_found.position);		

	// ������������ �� ����������
	if (dist > MAX_PATH_DISTANCE) {
		m_target_found.position.mad	(object().Position(), dir, MAX_PATH_DISTANCE);
		m_target_found.node			= u32(-1);
	} else {
		// ���� ������ ����...
		if ((m_target_found.node != u32(-1)) && accessible(m_target_found.node)) {
			
			// ���� ��������� ���� � �������, �� �������
			if (valid_destination(m_target_found.position, m_target_found.node)) return;
			else m_target_found.node = u32(-1);
		}
	}

	// ��������� ������� �� accessible
	if (!accessible(m_target_found.position)) {
		m_target_found.node = restrictions().accessible_nearest(Fvector().set(m_target_found.position),m_target_found.position);
		// �� �������� - �����
		return;
	}
	
	//VERIFY	(m_target_found.node == u32(-1));
	
	// II. ������� �������, ���� ����
	
	// ���� � ������ ���������?
	restrictions().add_border		(object().Position(), m_target_found.position);
	m_target_found.node			= ai().level_graph().check_position_in_direction(object().ai_location().level_vertex_id(),object().Position(),m_target_found.position);
	restrictions().remove_border	();
	
	if (ai().level_graph().valid_vertex_id(m_target_found.node) && accessible(m_target_found.node)) {
		// ������������� �������
		m_target_found.position.y = ai().level_graph().vertex_plane_y(m_target_found.node);
		fix_position(Fvector().set(m_target_found.position), m_target_found.node, m_target_found.position);
		return;
	}

	// ������ ���� �� ������� �������
	if (ai().level_graph().valid_vertex_position(m_target_found.position)) {
		m_target_found.node = ai().level_graph().vertex_id(m_target_found.position);
		if (ai().level_graph().valid_vertex_id(m_target_found.node) && accessible(m_target_found.node)) {
			// ������������� �������
			m_target_found.position.y = ai().level_graph().vertex_plane_y(m_target_found.node);
			fix_position(Fvector().set(m_target_found.position), m_target_found.node, m_target_found.position);
			return;
		}
	}

	// ���� �� �������. �� ��������� ����� ����� ������������ ���� ������, ���� ���������
	m_target_found.node = u32(-1);
}

//////////////////////////////////////////////////////////////////////////
// � ������������ � m_target_found ������������� ���������
void CMonsterMovement::set_found_target()
{
	// ���������� direction
	detail().set_use_dest_orientation	(b_use_dest_orient);
	if (b_use_dest_orient) {
		detail().set_dest_direction	(m_dest_dir);
	}

	// �������� ���� - ������������� ���������
	if (m_target_found.node != u32(-1)) {
		detail().set_dest_position		(m_target_found.position);
		set_level_dest_vertex						(m_target_found.node);
		return;
	}

	// ������� � ������� �������
	if (m_cover_info.use_covers) {
		m_cover_approach->setup	(m_target_found.position, m_cover_info.min_dist, m_cover_info.max_dist, m_cover_info.deviation);
		CCoverPoint	*point = ai().cover_manager().best_cover(object().Position(),m_cover_info.radius,*m_cover_approach);
		// ����� �����?	
		if (point) {
			m_target_found.node						= point->m_level_vertex_id;
			m_target_found.position					= point->m_position;	
			
			detail().set_dest_position		(m_target_found.position);
			set_level_dest_vertex						(m_target_found.node);
			return;
		}
	}

	// ������� � ������� ���������
	level_selector().set_evaluator			(m_selector_approach);
	level_selector().set_query_interval	(0);
	InitSelector									(*m_selector_approach, m_target_found.position);
	use_selector_path								(true);		// ������������ ��� ��������� ���������: true - ������������ ���� ��������� ����������, false - �������� ������� ������� ����, ���� ������ BuildLevelPath
}

//////////////////////////////////////////////////////////////////////////
// Utilities
//////////////////////////////////////////////////////////////////////////
void CMonsterMovement::fix_position(const Fvector &pos, u32 node, Fvector &res_pos)
{
	VERIFY(accessible(node));

	if (!accessible(pos)) {
		u32	level_vertex_id = restrictions().accessible_nearest(pos,res_pos);
		VERIFY	(level_vertex_id == node);
	}
}

bool CMonsterMovement::valid_destination(const Fvector &pos, u32 node) 
{
	return (
		ai().level_graph().valid_vertex_id(node) &&
		ai().level_graph().valid_vertex_position(pos) && 
		ai().level_graph().inside(node, pos)
	);
}

