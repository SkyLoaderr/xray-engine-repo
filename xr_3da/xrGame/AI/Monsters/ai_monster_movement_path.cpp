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
	m_target_desired.init		();
	m_target_selected.init		();
	m_target_required.init		();

	m_time						= 0;
	m_last_time_target_set		= 0;
	m_distance_to_path_end		= 1.f;
	m_path_end					= false;
	m_failed					= false;
	m_cover_info.use_covers		= false;

	m_force_rebuild				= false;
	m_target_actual				= false;
}

//////////////////////////////////////////////////////////////////////////
// Set Target Point Routines
//////////////////////////////////////////////////////////////////////////

void CMonsterMovement::set_target_point(const Fvector &position, u32 node)
{
	// �������� ������������
	if (m_target_required.position.similar(position))	m_target_actual = true;
	else												m_target_actual = false;
	
	// ���������� �������
	m_target_required.set	(position,node);

	// ���������� ���������� ��������� ������������
	m_target_type			= eMoveToTarget;
	set_path_type			(MovementManager::ePathTypeLevelPath);

	select_target_desired	();
}

void CMonsterMovement::set_retreat_from_point(const Fvector &position)
{
	// �������� ������������
	if (m_target_required.position.similar(position))	m_target_actual = true;
	else												m_target_actual = false;

	// ���������� �������
	m_target_required.set	(position,u32(-1));	

	// ���������� ���������� ��������� ������������
	m_target_type			= eRetreatFromTarget;
	set_path_type			(MovementManager::ePathTypeLevelPath);
}

#define RANDOM_POINT_DISTANCE	20.f

void CMonsterMovement::select_target_desired()
{
	if (m_target_actual && m_failed) {
		
		Msg("Target Actual & Failed :: select random");
		
		m_force_rebuild		= true;
		
		// ���� ���� �������� ��� failed - ������� ������ ��������� �����
		Fvector	pos_random;	
		Fvector dir;		dir.random_dir	();

		pos_random.mad(object().Position(), dir, RANDOM_POINT_DISTANCE);

		// ���������� m_target.position
		if (!accessible(pos_random)) {
			m_target_desired.node		= restrictions().accessible_nearest(pos_random, m_target_desired.position);	
		} else {
			m_target_desired.node		= u32(-1);
			m_target_desired.position	= pos_random;
		}
	} else {
		m_target_desired.set			(m_target_required.position, m_target_required.node);
		m_force_rebuild					= false;
	}
}

//
//void CMonsterMovement::set_target_point(const Fvector &position, u32 node)
//{
//	// I. ���������� m_target_desired
//	
//	bool same_params =	m_target_desired.position.similar(m_target_request.position) && 
//						(m_target_desired.node == m_target_request.node) &&
//						position.similar(m_target_request.position) &&
//						(node == m_target_request.node);
//	
//	// ������ ���������?
//	if (failed() && same_params) {
//		
//		m_force_rebuild = true;
//
//		// ���� ���� �������� ��� failed - ������� ������ ��������� �����
//		Fvector	pos_random	= position;	
//		Fvector dir;		dir.random_dir	();
//
//		pos_random.mad( object().Position(), dir, RANDOM_POINT_DISTANCE);
//
//		// ���������� m_target.position
//		if (!accessible(pos_random)) {
//			m_target_desired.node		= restrictions().accessible_nearest(pos_random, m_target_desired.position);	
//		} else {
//			m_target_desired.node		= u32(-1);
//			m_target_desired.position	= pos_random;
//		}
//	} else {
//		m_target_desired.position	= position;
//		m_target_desired.node		= node;
//		m_force_rebuild				= false;
//	}
//
//	// II. ��������� ��������� ������� � ���� 		
//	m_target_request.set(position,node);	
//	
//	// III. ���������� ���������� ��������� ������������
//	m_target_type		= eMoveToTarget;
//	set_path_type		(MovementManager::ePathTypeLevelPath);
//}
//
//void CMonsterMovement::set_retreat_from_point(const Fvector &position)
//{
//	// I. ���������� m_target_desired
//	bool same_params =	m_target_desired.position.similar(m_target_request.position) && 
//						position.similar(m_target_request.position);
//		
//	// ������ ���������?
//	if (failed() && same_params) {
//		m_force_rebuild = true;
//
//		// ���� ���� �������� ��� failed - ������� ������ ��������� �����
//		Fvector dir; dir.random_dir	();
//		m_target_desired.position.mad( object().Position(), dir, RANDOM_POINT_DISTANCE);
//	} else {
//		m_target_desired.position	= position;
//		m_force_rebuild				= false;
//	}
//	
//	m_target_desired.node	= u32(-1);
//
//	// II. ��������� ��������� ������� � ���� 		
//	m_target_request.set(position,u32(-1));	
//	
//	// III. ���������� ���������� ��������� ������������
//	m_target_type			= eRetreatFromTarget;
//	set_path_type			(MovementManager::ePathTypeLevelPath);
//}

//////////////////////////////////////////////////////////////////////////
bool CMonsterMovement::target_point_need_update()
{
	if (m_force_rebuild) return true;

	// ���� ���� ��� �� ��������
	if (!IsPathEnd(m_distance_to_path_end)) {
		// ���� ����� �������� �� ���� �� �����, �� �������������
		return (m_last_time_target_set + m_time < m_object->m_current_update);
	}
	
	// ���� ���� ��� �� �������� - �����
	if (!detail_path_manager().actual() && (detail_path_manager().time_path_built() < m_last_time_target_set)) return false;
	return true;
}


//////////////////////////////////////////////////////////////////////////
// ���� �� ������ ������� m_target_selected.node != u32(-1) - ���� �������
void CMonsterMovement::select_target()
{
	Msg("Select Target Point :: M_Target_Selected");
	
	m_target_selected.node	= m_target_desired.node;

	// I. ���������� �� ���� ����������
	
	// ������� ������� ������� � ������������ � ������������ ��������, ��������� ����������� ��������
	Fvector	dir;
	if (m_target_type == eMoveToTarget) {

		dir.sub						(m_target_desired.position, object().Position());
		VERIFY						(!fis_zero(dir.square_magnitude()));
		dir.normalize				();
		m_target_selected.position	= m_target_desired.position;

	} else if (m_target_type == eRetreatFromTarget){
		VERIFY(m_target_selected.node == u32(-1));

		dir.sub							(object().Position(), m_target_desired.position);
		dir.normalize_safe				();
		m_target_selected.position.mad	(object().Position(), dir, MAX_PATH_DISTANCE - 1.f);
	}

	// ���������� ���������� �� ��������� �����
	float dist = object().Position().distance_to(m_target_selected.position);		

	// ������������ �� ����������
	if (dist > MAX_PATH_DISTANCE) {
		m_target_selected.position.mad	(object().Position(), dir, MAX_PATH_DISTANCE);
		m_target_selected.node			= u32(-1);
	} else {
		// ���� ������ ����...
		if ((m_target_selected.node != u32(-1)) && accessible(m_target_selected.node)) {
			
			// ���� ��������� ���� � �������, �� �������
			if (valid_destination(m_target_selected.position, m_target_selected.node)) return;
			else m_target_selected.node = u32(-1);
		}
	}

	// ��������� ������� �� accessible
	if (!accessible(m_target_selected.position)) {
		m_target_selected.node = restrictions().accessible_nearest(Fvector().set(m_target_selected.position),m_target_selected.position);
		// �� �������� - �����
		return;
	}
	
	VERIFY	(m_target_selected.node == u32(-1));
	
	// II. ������� �������, ���� ����
	
	// ���� � ������ ���������?
	restrictions().add_border		(object().Position(), m_target_selected.position);
	m_target_selected.node			= ai().level_graph().check_position_in_direction(object().ai_location().level_vertex_id(),object().Position(),m_target_selected.position);
	restrictions().remove_border	();
	
	if (ai().level_graph().valid_vertex_id(m_target_selected.node) && accessible(m_target_selected.node)) {
		// ������������� �������
		m_target_selected.position.y = ai().level_graph().vertex_plane_y(m_target_selected.node);
		fix_position(Fvector().set(m_target_selected.position), m_target_selected.node, m_target_selected.position);
		return;
	}

	// ������ ���� �� ������� �������
	if (ai().level_graph().valid_vertex_position(m_target_selected.position)) {
		m_target_selected.node = ai().level_graph().vertex_id(m_target_selected.position);
		if (ai().level_graph().valid_vertex_id(m_target_selected.node) && accessible(m_target_selected.node)) {
			// ������������� �������
			m_target_selected.position.y = ai().level_graph().vertex_plane_y(m_target_selected.node);
			fix_position(Fvector().set(m_target_selected.position), m_target_selected.node, m_target_selected.position);
			return;
		}
	}

	// ���� �� �������. �� ��������� ����� ����� ������������ ���� ������, ���� ���������
	VERIFY(m_target_selected.node == u32(-1));
}

//////////////////////////////////////////////////////////////////////////
// � ������������ � m_target_selected ������������� ���������
void CMonsterMovement::set_selected_target()
{
	// �������� ���� - ������������� ���������
	if (m_target_selected.node != u32(-1)) {
		detail_path_manager().set_dest_position		(m_target_selected.position);
		set_level_dest_vertex						(m_target_selected.node);
		return;
	}

	// ������� � ������� �������
	if (m_cover_info.use_covers) {
		m_cover_approach->setup	(m_target_selected.position, m_cover_info.min_dist, m_cover_info.max_dist, m_cover_info.deviation);
		CCoverPoint	*point = ai().cover_manager().best_cover(object().Position(),m_cover_info.radius,*m_cover_approach);
		// ����� �����?	
		if (point) {
			m_target_selected.node						= point->m_level_vertex_id;
			m_target_selected.position					= point->m_position;	
			
			detail_path_manager().set_dest_position		(m_target_selected.position);
			set_level_dest_vertex						(m_target_selected.node);
			return;
		}
	}

	// ������� � ������� ���������
	level_location_selector().set_evaluator			(m_selector_approach);
	level_location_selector().set_query_interval	(0);
	InitSelector									(*m_selector_approach, m_target_selected.position);
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

