#include "stdafx.h"
#include "ai_monster_movement.h"
#include "../cover_point.h"
#include "../cover_manager.h"
#include "../cover_evaluators.h"
#include "biting/ai_biting.h"

//////////////////////////////////////////////////////////////////////////
// Select Target
//////////////////////////////////////////////////////////////////////////

#define MAX_COVER_DISTANCE		50.f
#define MAX_SELECTOR_DISTANCE	10.f
#define MAX_PATH_DISTANCE		100.f

void CMonsterMovement::set_target_point(const Fvector &position, u32 node)
{
	VERIFY(ai().level_graph().valid_vertex_position(position));

	m_target.position	= position;
	m_target.node		= node;
}

//////////////////////////////////////////////////////////////////////////
// ������������ �� ����������
// ���� �� ������ ������� m_intermediate.node != u32(-1) - ���� �������
void CMonsterMovement::get_intermediate(const Fvector &position) 
{
	m_intermediate.node		= u32(-1);
	m_intermediate.position = position;

	Fvector	dir;
	dir.sub		(position, Position());
	float dist	= Position().distance_to(position);

	// ������������ �� ����������
	if (dist > MAX_PATH_DISTANCE) {
		m_intermediate.position.mad	(Position(), dir, MAX_PATH_DISTANCE);
		m_intermediate.node			= u32(-1);
	}

	// ���� � ������ ��������� ?
	if (position_in_direction(m_intermediate.position, m_intermediate.node)) return;

	// ���������� ���������?
	if (m_cover_info.use_covers) {
		if (dist > MAX_COVER_DISTANCE) {
			m_intermediate.position.mad	(Position(), dir, MAX_COVER_DISTANCE);
			m_intermediate.node			= u32(-1);
		}

		// ���� � ������ ��������� ?
		if (position_in_direction(m_intermediate.position, m_intermediate.node)) return;
	}

	// ������������ �� ����������
	if (dist > MAX_SELECTOR_DISTANCE) {
		m_intermediate.position.mad	(Position(), dir, MAX_SELECTOR_DISTANCE);
		m_intermediate.node			= u32(-1);
	}

	// ���� � ������ ��������� ?
	if (position_in_direction(m_intermediate.position, m_intermediate.node)) return;
}

bool CMonsterMovement::position_in_direction(const Fvector &target, u32 &node) 
{
	// ���� � ������ ���������?
	CRestrictedObject::add_border(Position(), target);
	node = ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),target);
	CRestrictedObject::remove_border();
	if (ai().level_graph().valid_vertex_id(node) && accessible(node)) return true;

	// ������ ���� �� ������� �������
	if (ai().level_graph().valid_vertex_position(target) && accessible(target)) {
		node = ai().level_graph().vertex_id(target);
		if (ai().level_graph().valid_vertex_id(node) && accessible(node)) return true;
	}

	node = u32(-1);
	return false;
}

void CMonsterMovement::set_parameters()
{
	// �������� ���� - ������������� ���������
	if (m_intermediate.node != u32(-1)) {
		set_dest_position		(m_intermediate.position);
		set_level_dest_vertex	(m_intermediate.node);
		set_path_type			(MovementManager::ePathTypeLevelPath);
		return;
	}

	// ������� � ������� �������
	if (m_cover_info.use_covers) {
		m_cover_approach->setup	(m_intermediate.position, m_cover_info.min_dist, m_cover_info.max_dist, m_cover_info.deviation);
		CCoverPoint	*point = ai().cover_manager().best_cover(Position(),m_cover_info.radius,*m_cover_approach);
		if (point) {
			m_intermediate.node		= point->m_level_vertex_id;
			m_intermediate.position	= point->m_position;	
			set_dest_position		(m_intermediate.position);
			set_level_dest_vertex	(m_intermediate.node);
			set_path_type			(MovementManager::ePathTypeLevelPath);
			return;
		}
	}

	// ������� � ������� ���������
	CLevelLocationSelector::set_evaluator		(m_selector_approach);
	CLevelLocationSelector::set_query_interval	(0);
	InitSelector								(*m_selector_approach, m_intermediate.position);
	use_selector_path							(true);		// ������������ ��� ��������� ���������: true - ������������ ���� ��������� ����������, false - �������� ������� ������� ����, ���� ������ BuildLevelPath
	set_path_type								(MovementManager::ePathTypeLevelPath);
}

void CMonsterMovement::update_target_point() 
{
	// ���� ����� �������� �� ���� �� ����� - �����
	bool b_path_end = IsPathEnd(m_distance_to_path_end);

	if (!b_path_end && (m_last_time_target_set + m_time < pMonster->m_current_update)) return;
	if (b_path_end && m_intermediate.position.similar(m_target.position, EPS)) {
		m_path_end = true;
		return;
	}
	
	// �������� ������������� ������� � ����
	get_intermediate		(m_target.position);
	// ���������� ���������
	set_parameters			();

	// ��������� �� ����
	if (Position().similar(m_intermediate.position)) {
		m_failed = true;
		return;
	}

	m_last_time_target_set	= pMonster->m_current_update;
	m_path_end				= false;
}

