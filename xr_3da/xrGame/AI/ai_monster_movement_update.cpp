#include "stdafx.h"
#include "ai_monster_movement.h"
#include "biting/ai_biting.h"

//////////////////////////////////////////////////////////////////////////
// Init Movement
void CMonsterMovement::Update_Initialize()
{
	CLevelLocationSelector::set_evaluator			(0);
	CDetailPathManager::set_path_type				(eDetailPathTypeSmooth);
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
#ifdef _DEBUG	
	//update_target_point								();
#endif
	
	CDetailPathManager::set_try_min_time			(b_try_min_time); 
	CDetailPathManager::set_use_dest_orientation	(b_use_dest_orient);
	enable_movement									(b_enable_movement);
	update_path										();
}


//////////////////////////////////////////////////////////////////////////
// Finalize Movement
void CMonsterMovement::Update_Finalize()
{
	set_desirable_speed						(m_object->m_fCurSpeed);
}
