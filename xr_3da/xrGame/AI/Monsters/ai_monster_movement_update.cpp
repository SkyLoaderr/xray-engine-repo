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

	level_location_selector().set_evaluator			(0);
	detail_path_manager().set_path_type				(eDetailPathTypeSmooth);
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

	detail_path_manager().set_try_min_time			(b_try_min_time); 
	detail_path_manager().set_use_dest_orientation	(b_use_dest_orient);
	enable_movement									(b_enable_movement);
	
	Msg("-- Update  :: Time [%u] --", m_object->m_current_update);
	
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
	select_target			();

	// ���������� ���������
	set_selected_target		();

	if (path_completed() && m_target_selected.position.similar(object().Position())) {
		Msg("Postselector :: Path End");
		m_path_end	= true;
	}

	// ��������� ������� ����� 
	m_last_time_target_set	= m_object->m_current_update;
}

// �������� ���������� � ����������� ���� (m_failed)
void CMonsterMovement::check_failure()
{
	m_failed		= false;

	// ���� ����� ����������� ���� completed - failed!
	bool new_path = detail_path_manager().time_path_built() >= m_last_time_target_set;
	if (path_completed() && IsPathEnd(m_distance_to_path_end) && new_path) {
		Msg("Failed :: New Path & PathEnd");
		m_failed	= true;	
	}

	// ���� level_path_manager failed
	if (level_path_manager().failed()) {
		Msg("Failed :: LevelPathManager");
		m_failed	= true;
	}
}
