#include "stdafx.h"
#include "ai_monster_movement.h"
#include "../../cover_point.h"
#include "../../cover_manager.h"
#include "../../cover_evaluators.h"
#include "BaseMonster/base_monster.h"
#include "../../detail_path_manager.h"
#include "../../level_location_selector.h"
#include "../../level_path_manager.h"

#define MAX_COVER_DISTANCE		50.f
#define MAX_SELECTOR_DISTANCE	10.f
#define MAX_PATH_DISTANCE		100.f


//////////////////////////////////////////////////////////////////////////
// ������������ �� ����������
// ���� �� ������ ������� m_intermediate.node != u32(-1) - ���� �������
void CMonsterMovement::get_intermediate() 
{
	m_intermediate.node		= m_target.node;

	Fvector	dir;
	if (m_target_type == eMoveToTarget) {
		dir.sub						(m_target.position, Position());
		dir.normalize_safe			();
		m_intermediate.position		= m_target.position;
	} else if (m_target_type == eRetreatFromTarget){
		VERIFY(m_intermediate.node == u32(-1));
		
		dir.sub						(Position(), m_target.position);
		dir.normalize_safe			();
		m_intermediate.position.mad	(Position(), dir, MAX_PATH_DISTANCE - 1.f);
	}
	
	float dist = Position().distance_to(m_intermediate.position);		

	// ������������ �� ����������
	if (dist > MAX_PATH_DISTANCE) {
		m_intermediate.position.mad	(Position(), dir, MAX_PATH_DISTANCE);
		m_intermediate.node			= u32(-1);
	} else {
		// ���� ������ ����, �� �������
		if (m_intermediate.node != u32(-1) && accessible(m_intermediate.node)) {
			return;
		}
	}

	// ���� � ������ ��������� ?
	if (position_in_direction(m_intermediate.position, m_intermediate.node)) {
		m_intermediate.position.y = ai().level_graph().vertex_plane_y(m_intermediate.node);
		Fvector pos = m_intermediate.position;
		fix_position(pos, m_intermediate.node, m_intermediate.position);
		return;
	}

	// ���������� ���������?
	if (m_cover_info.use_covers) {
		if (dist > MAX_COVER_DISTANCE) {
			m_intermediate.position.mad	(Position(), dir, MAX_COVER_DISTANCE);
			m_intermediate.node			= u32(-1);
		}

		// ���� � ������ ��������� ?
		if (position_in_direction(m_intermediate.position, m_intermediate.node)) {
			m_intermediate.position.y = ai().level_graph().vertex_plane_y(m_intermediate.node);
			Fvector pos = m_intermediate.position;
			fix_position(pos, m_intermediate.node, m_intermediate.position);
			return;
		}
	}

	// ������������ �� ����������
	if (dist > MAX_SELECTOR_DISTANCE) {
		m_intermediate.position.mad	(Position(), dir, MAX_SELECTOR_DISTANCE);
		m_intermediate.node			= u32(-1);
	}

	// ���� � ������ ��������� ?
	if (position_in_direction(m_intermediate.position, m_intermediate.node)) {
		m_intermediate.position.y = ai().level_graph().vertex_plane_y(m_intermediate.node);
		Fvector pos = m_intermediate.position;
		fix_position(pos, m_intermediate.node, m_intermediate.position);
		return;
	}
}

bool CMonsterMovement::position_in_direction(const Fvector &target, u32 &node) 
{
	// ���� � ������ ���������?
	CRestrictedObject::add_border(Position(), target);
	node = ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),target);
	CRestrictedObject::remove_border();
	if (ai().level_graph().valid_vertex_id(node) && accessible(node)) {
		return true;
	}

	// ������ ���� �� ������� �������
	if (ai().level_graph().valid_vertex_position(target) && accessible(target)) {
		node = ai().level_graph().vertex_id(target);
		if (ai().level_graph().valid_vertex_id(node) && accessible(node)) {
			return true;
		}
	}

	node = u32(-1);
	return false;
}

void CMonsterMovement::set_parameters()
{
	// �������� ���� - ������������� ���������
	if (m_intermediate.node != u32(-1)) {
		detail_path_manager().set_dest_position		(m_intermediate.position);
		set_level_dest_vertex	(m_intermediate.node);
		return;
	}

	// ������� � ������� �������
	if (m_cover_info.use_covers) {
		m_cover_approach->setup	(m_intermediate.position, m_cover_info.min_dist, m_cover_info.max_dist, m_cover_info.deviation);
		CCoverPoint	*point = ai().cover_manager().best_cover(Position(),m_cover_info.radius,*m_cover_approach);
		if (point) {
			m_intermediate.node		= point->m_level_vertex_id;
			m_intermediate.position	= point->m_position;	
			detail_path_manager().set_dest_position		(m_intermediate.position);
			set_level_dest_vertex	(m_intermediate.node);
			return;
		}
	}

	// ������� � ������� ���������
	level_location_selector().set_evaluator		(m_selector_approach);
	level_location_selector().set_query_interval	(0);
	InitSelector								(*m_selector_approach, m_intermediate.position);
	use_selector_path							(true);		// ������������ ��� ��������� ���������: true - ������������ ���� ��������� ����������, false - �������� ������� ������� ����, ���� ������ BuildLevelPath
}


void CMonsterMovement::initialize_movement() 
{
	m_target.node				= u32(-1);
	m_intermediate.node			= u32(-1);

	m_time						= 0;
	m_last_time_path_update		= 0;
	m_distance_to_path_end		= 1.f;
	m_path_end					= false;
	m_failed					= false;
	m_cover_info.use_covers		= false;

	m_actual					= false;
	m_force_rebuild				= false;
}

//////////////////////////////////////////////////////////////////////////
// Set Target Point Routines
//////////////////////////////////////////////////////////////////////////
#define RANDOM_POINT_DISTANCE	20.f

void CMonsterMovement::set_target_point(const Fvector &position, u32 node)
{
	bool bad_state	= false;
	if (failed())	bad_state = true;

	// ������ ���������?
	if (bad_state) {
		m_force_rebuild = true;

		// ���� ���� �������� ��� failed - ������� ������ ��������� �����
		Fvector	pos_random			= position;	
		Fvector dir; dir.random_dir	();

		pos_random.mad( Position(), dir, RANDOM_POINT_DISTANCE);
		
		// ���������� m_target.position
		if (!accessible(pos_random)) {
			m_target.node		= accessible_nearest(pos_random, m_target.position);	
		} else {
			m_target.node		= u32(-1);
			m_target.position	= pos_random;
		}
	} else {
		m_target.position	= position;
		m_target.node		= node;
		m_force_rebuild		= false;
	}
	
	m_target_type		= eMoveToTarget;

	set_path_type		(MovementManager::ePathTypeLevelPath);
}

void CMonsterMovement::set_retreat_from_point(const Fvector &position)
{
	bool bad_state = false;
	if (failed())	bad_state = true;

	// ������ ���������?
	if (bad_state) {
		m_force_rebuild = true;

		// ���� ���� �������� ��� failed - ������� ������ ��������� �����
		Fvector dir; dir.random_dir	();
		m_target.position.mad( Position(), dir, RANDOM_POINT_DISTANCE);
	} else {
		m_target.position	= position;
		m_force_rebuild		= false;
	}
	
	m_target.node		= u32(-1);
	m_target_type		= eRetreatFromTarget;

	set_path_type		(MovementManager::ePathTypeLevelPath);
}

//////////////////////////////////////////////////////////////////////////
// ������������ �� ����������
// ���� �� ������ ������� m_intermediate.node != u32(-1) - ���� �������
bool CMonsterMovement::check_build_conditions()
{
	if (m_force_rebuild) return true;

	// ��������� �� ���������� ����
	if (!IsPathEnd(m_distance_to_path_end)) {
		m_path_end	= false;
		m_failed	= false;

		// ���� ����� �������� �� ���� �� �����
		if (m_last_time_path_update + m_time > m_object->m_current_update) return false;
		return		true;
	}
	
	if (Position().similar(m_target.position)) {
		m_path_end	= true;
		return false;
	}
	
	if (m_intermediate.position.similar(m_target.position) || detail_path_manager().actual()) {
		m_path_end	= true;
		return		true;
	}

	if (level_path_manager().failed()) {
		m_failed	= true;
		return		false;
	}

	// ���� ���� ��� �� �������� - �����
	if (!detail_path_manager().actual() && (detail_path_manager().time_path_built() < m_last_time_path_update)) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////////////////////////////
void CMonsterMovement::fix_position(const Fvector &pos, u32 node, Fvector &res_pos)
{
	VERIFY(accessible(node));

	if (!accessible(pos)) {
		u32	level_vertex_id = accessible_nearest(pos,res_pos);
		VERIFY	(level_vertex_id == node);
	}
}
