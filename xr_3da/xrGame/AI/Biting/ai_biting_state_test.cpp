#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingTest class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingTest::CBitingTest(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}

void CBitingTest::Init()
{
	inherited::Init();

	last_time	= 0;
	vertex_id	= 0;
	start_vertex_id = pMonster->level_vertex_id();
}

void CBitingTest::Run()
{
	bool bNeedRebuild = false;
	pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;

	if (pMonster->CMovementManager::path_completed() || (!pMonster->CDetailPathManager::path().empty() && (pMonster->CDetailPathManager::path().size() < pMonster->CDetailPathManager::curr_travel_point_index() + 5 ))) {
		bNeedRebuild = true;
	} else {
		DO_IN_TIME_INTERVAL_BEGIN(last_time, 500);
			bNeedRebuild = true;
		DO_IN_TIME_INTERVAL_END();
	}
	
	
	if (bNeedRebuild) {
		vertex_id = dynamic_cast<CEntity*>(Level().CurrentEntity())->level_vertex_id();
	}

	pMonster->SetPathParams(
		CMovementManager::ePathTypeLevelPath, 
		vertex_id, 
		ai().level_graph().vertex_position(vertex_id),
		pMonster->eMovementParameterAnyType,
		pMonster->eMovementParameterRunFree | pMonster->eMovementParameterWalkFree
	);
}
