#include "stdafx.h"
#include "ai_monster_movement.h"
#include "BaseMonster/base_monster.h"
#include "critical_action_info.h"
#include "../../detail_path_manager.h"
#include "../../level_location_selector.h"
#include "../../level_path_manager.h"

//////////////////////////////////////////////////////////////////////////
// Init Movement
void CMonsterMovement::Update_Initialize()
{
	if (m_object->CriticalActionInfo->is_path_locked()) return;

	level_selector().set_evaluator			(0);
	detail().set_path_type				(eDetailPathTypeSmooth);
	b_try_min_time									= true;
	b_enable_movement								= true;
	set_path_targeted								(false);
	set_use_dest_orient								(false);

	MotionStats->update								();
}


//////////////////////////////////////////////////////////////////////////
// Update Movement
void CMonsterMovement::Update_Execute()
{	
	if (m_object->CriticalActionInfo->is_path_locked()) return;

	detail().set_try_min_time			(b_try_min_time); 
	enable_movement									(b_enable_movement);
	
	// �������� / ���������� ������� �������
	update_target_point								();
	
	// ������� ����
	update_path										();

	// ��������� �� ����
	check_failure									();
}


//////////////////////////////////////////////////////////////////////////
// Finalize Movement
void CMonsterMovement::Update_Finalize()
{
	set_desirable_speed						(m_object->m_fCurSpeed);
}


//////////////////////////////////////////////////////////////////////////
// update path with new scheme method
void CMonsterMovement::update_target_point() 
{
	if (!enabled() || !b_enable_movement)					return;
	if (path_type() != MovementManager::ePathTypeLevelPath) return;
	
	// ��������� �������, ����� ���� ������� �� �����
	if (!target_point_need_update())						return; 

	// ������� ���� � ������� � ������������ � ��������� ����� � ��������
	find_target			();

	// ���������� ���������
	set_found_target	();

	// ��������� ������� ����� 
	m_last_time_target_set	= m_object->m_current_update;
}

// �������� ���������� � ����������� ���� (m_failed)
void CMonsterMovement::check_failure()
{
	m_failed		= false;

	//bool new_path = detail().time_path_built() >= m_last_time_target_set;
	if (IsPathEnd(m_distance_to_path_end) && actual() && !m_wait_path_end && (m_target_selected.position.similar(m_target_set.position))) {
		m_failed	= true;	
	}

	// ���� level_path_manager failed
	if (level_path().failed()) {
		m_failed	= true;
	}
}
