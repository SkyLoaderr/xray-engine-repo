#include "stdafx.h"
#include "ai_monster_movement.h"


CMonsterMovement::CMonsterMovement()
{
}

CMonsterMovement::~CMonsterMovement()
{
}

void CMonsterMovement::Init()
{
	b_try_min_time = false;
}

//////////////////////////////////////////////////////////////////////////
// Init Movement
void CMonsterMovement::Frame_Init()
{
	enable_movement							(true);
	CLevelLocationSelector::set_evaluator	(0);
	CDetailPathManager::set_path_type		(eDetailPathTypeSmooth);
	b_try_min_time							= true;		
}


//////////////////////////////////////////////////////////////////////////
// Update Movement
void CMonsterMovement::Frame_Update()
{
	CDetailPathManager::set_try_min_time	(b_try_min_time); 
	update_path								();
}


//////////////////////////////////////////////////////////////////////////
// Finalize Movement
void CMonsterMovement::Frame_Finalize()
{
	SetVelocity								();
	set_desirable_speed						(m_fCurSpeed);
}

