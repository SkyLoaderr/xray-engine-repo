#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"
#include "../../game_graph.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingDetour class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingDetour::CBitingDetour(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}

void CBitingDetour::Reset()
{
	inherited::Reset();
	m_tEnemy.obj		= 0;
}

void CBitingDetour::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);
}

void CBitingDetour::Run()
{

	VisionElem tempEnemy;
	if (pMonster->GetEnemy(tempEnemy)) m_tEnemy = tempEnemy;

	pMonster->set_level_dest_vertex	(ai().game_graph().vertex(pMonster->m_tNextGP)->level_vertex_id());
	pMonster->Path_CoverFromPoint	(m_tEnemy.obj,m_tEnemy.obj->Position());

	pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

	pMonster->SetSound(SND_TYPE_IDLE, pMonster->_sd->m_dwIdleSndDelay);
}


