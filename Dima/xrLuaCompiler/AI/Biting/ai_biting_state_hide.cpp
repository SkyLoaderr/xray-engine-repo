#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingHide class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingHide::CBitingHide(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingHide::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);

	// Test
	WRITE_TO_LOG("_ Hide Init _");
}

void CBitingHide::Reset()
{
	inherited::Reset();

	m_tEnemy.obj		= 0;
}

void CBitingHide::Run()
{
	Fvector EnemyPos;
	if (m_tEnemy.obj) EnemyPos = m_tEnemy.obj->Position();
	else EnemyPos = m_tEnemy.position;

	pMonster->Path_GetAwayFromPoint(m_tEnemy.obj, EnemyPos, 3.f, 2000);

	// Установить параметры движения
	pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
}

