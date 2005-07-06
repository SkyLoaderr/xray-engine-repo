#include "stdafx.h"
#include "control_path_builder_base.h"
#include "../../cover_point.h"
#include "../../cover_manager.h"
#include "../../cover_evaluators.h"
#include "BaseMonster/base_monster.h"
#include "../../detail_path_manager.h"
#include "../../level_location_selector.h"
#include "../../level_path_manager.h"
#include "../../ai_object_location.h"

#define MAX_PATH_DISTANCE		100.f

void CControlPathBuilderBase::prepare_builder() 
{
	m_target_selected.init		();
	m_target_set.init			();

	m_time						= 0;
	m_last_time_target_set		= 0;
	m_distance_to_path_end		= 1.f;
	m_failed					= false;
	m_cover_info.use_covers		= false;

	m_force_rebuild				= false;
	m_target_actual				= false;
	m_wait_path_end				= false;

	m_target_selected.init		();
	m_target_set.init			();

	if (!m_man->path_builder().accessible(m_object->Position())) {
		m_target_found.node			= m_man->path_builder().restrictions().accessible_nearest(m_object->Position(), m_target_found.position);
	} else {
		m_target_found.node			= u32(-1);
		m_target_found.position		= m_object->Position();
	}
}

//////////////////////////////////////////////////////////////////////////
// Set Target Point Routines
//////////////////////////////////////////////////////////////////////////

void CControlPathBuilderBase::set_target_point(const Fvector &position, u32 node)
{
	// �������� ������������
	if (m_target_set.position.similar(position))	m_target_actual = true;
	else											m_target_actual = false;

	// ���������� �������
	m_target_set.set	(position,node);

	// ���������� ���������� ��������� ������������
	m_target_type		= eMoveToTarget;
	select_target		();
}

void CControlPathBuilderBase::set_retreat_from_point(const Fvector &position)
{
	// �������� ������������
	if (m_target_set.position.similar(position))	m_target_actual = true;
	else											m_target_actual = false;

	// ���������� �������
	m_target_set.set	(position,u32(-1));	

	// ���������� ���������� ��������� ������������
	m_target_type		= eRetreatFromTarget;

	select_target		();
}

#define RANDOM_POINT_DISTANCE	20.f

void CControlPathBuilderBase::select_target()
{
	if (m_wait_path_end) return;

	if (m_target_actual && m_failed) {

		m_force_rebuild		= true;
		m_wait_path_end		= true;

		// ���� ���� �������� ��� failed - ������� ������ ��������� �����
		Fvector	pos_random;	
		Fvector dir;		dir.random_dir	();

		pos_random.mad(m_object->Position(), dir, RANDOM_POINT_DISTANCE);

		// ���������� m_target.position
		if (!m_man->path_builder().accessible(pos_random)) {
			m_target_selected.node		= m_man->path_builder().restrictions().accessible_nearest(pos_random, m_target_selected.position);
		} else {
			m_target_selected.node		= u32(-1);
			m_target_selected.position	= pos_random;
		}
	} else {
		m_target_selected.set			(m_target_set.position, m_target_set.node);
	}

	set_level_path_type();
}

//////////////////////////////////////////////////////////////////////////
bool CControlPathBuilderBase::target_point_need_update()
{
	if (m_force_rebuild) {
		m_force_rebuild		= false;
		return				true;
	}

	// ���� ���� ��� �� ��������
	if (!m_man->path_builder().is_path_end(m_distance_to_path_end)) {

		if (m_wait_path_end) return false;

		// ���� ����� �������� �� ���� �� �����, �� �������������
		return (m_last_time_target_set + m_time < Device.dwTimeGlobal);
	}

	// ����� ����

	m_wait_path_end	= false;

	// ���� ���� ��� �� �������� - �����
	if (!m_man->path_builder().detail().actual() && (m_man->path_builder().detail().time_path_built() < m_last_time_target_set)) {
		bool b_actual = m_man->path_builder().detail().actual();
		u32 time = m_man->path_builder().detail().time_path_built();	

		return false;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// ���� �� ������ ������� m_target_found.node != u32(-1) - ���� �������
void CControlPathBuilderBase::find_target()
{
	m_target_found.node	= m_target_selected.node;
	
	// I. ���������� �� ���� ����������
	// ������� ������� ������� � ������������ � ������������ ��������, ��������� ����������� ��������
	Fvector	dir;
	if (m_target_type == eMoveToTarget) {

		dir.sub						(m_target_selected.position, m_object->Position());
		dir.normalize_safe			();
		m_target_found.position	= m_target_selected.position;

	} else if (m_target_type == eRetreatFromTarget){

		if (m_target_found.node == u32(-1)) {
			dir.sub							(m_object->Position(), m_target_selected.position);
			dir.normalize_safe				();
			m_target_found.position.mad		(m_object->Position(), dir, MAX_PATH_DISTANCE - 1.f);
		} else {
			m_target_found.position	= m_target_selected.position;
		}
	}

	// ���������� ���������� �� ��������� �����
	float dist = m_object->Position().distance_to(m_target_found.position);		

	// ������������ �� ����������
	if (dist > MAX_PATH_DISTANCE) {
		m_target_found.position.mad	(m_object->Position(), dir, MAX_PATH_DISTANCE);
		m_target_found.node			= u32(-1);
	} else {
		// ���� ������ ����...
		if ((m_target_found.node != u32(-1)) && m_man->path_builder().accessible(m_target_found.node)) {

			// ���� ��������� ���� � �������, �� �������
			if (m_man->path_builder().valid_destination(m_target_found.position, m_target_found.node)) return;
			else m_target_found.node = u32(-1);
		}
	}

	// ��������� ������� �� accessible
	if (!m_man->path_builder().accessible(m_target_found.position)) {
		m_target_found.node = m_man->path_builder().restrictions().accessible_nearest(Fvector().set(m_target_found.position),m_target_found.position);
		// �� �������� - �����
		return;
	}

	// TODO: find out reason
	//VERIFY(m_target_found.node == u32(-1));

	// II. ������� �������, ���� ����

	// ���� � ������ ���������?
	m_man->path_builder().restrictions().add_border		(m_object->Position(), m_target_found.position);
	m_target_found.node	= ai().level_graph().check_position_in_direction(m_object->ai_location().level_vertex_id(),m_object->Position(),m_target_found.position);
	m_man->path_builder().restrictions().remove_border	();

	if (ai().level_graph().valid_vertex_id(m_target_found.node) && m_man->path_builder().accessible(m_target_found.node)) {
		// ������������� �������
		m_target_found.position.y = ai().level_graph().vertex_plane_y(m_target_found.node);
		m_man->path_builder().fix_position(Fvector().set(m_target_found.position), m_target_found.node, m_target_found.position);
		return;
	}

	// ������ ���� �� ������� �������
	if (ai().level_graph().valid_vertex_position(m_target_found.position)) {
		m_target_found.node = ai().level_graph().vertex_id(m_target_found.position);
		if (ai().level_graph().valid_vertex_id(m_target_found.node) && m_man->path_builder().accessible(m_target_found.node)) {
			// ������������� �������
			m_target_found.position.y = ai().level_graph().vertex_plane_y(m_target_found.node);
			m_man->path_builder().fix_position(Fvector().set(m_target_found.position), m_target_found.node, m_target_found.position);
			return;
		}
	}

	// ������� � ������� �������
	if (m_cover_info.use_covers) {
		m_cover_approach->setup	(m_target_found.position, m_cover_info.min_dist, m_cover_info.max_dist, m_cover_info.deviation);
		CCoverPoint	*point = ai().cover_manager().best_cover(m_object->Position(),m_cover_info.radius,*m_cover_approach);
		// ����� �����?	
		if (point) {
			m_target_found.node				= point->m_level_vertex_id;
			m_target_found.position			= point->m_position;	
			return;
		}
	}

	// ���� �� �������. �� ��������� ����� ����� ������������ ���� ������, ���� ���������
	m_target_found.node = u32(-1);
}
