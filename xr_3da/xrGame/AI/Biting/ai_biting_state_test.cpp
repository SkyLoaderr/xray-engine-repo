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
		DO_IN_TIME_INTERVAL_BEGIN(last_time, 1000);
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
		pMonster->eVelocityParamsRun,
		pMonster->eVelocityParameterWalkNormal | pMonster->eVelocityParameterRunNormal
	);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CBitingSearchEnemy::CBitingSearchEnemy(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}
void CBitingSearchEnemy::Init()
{
	inherited::Init();

	// ѕолучить врага
	m_tEnemy = pMonster->m_tEnemy;

	Fvector v;
	v.sub(ai().level_graph().vertex_position(m_tEnemy.node_id),pMonster->Position());
	v.normalize();
	Fvector next_pos;
	next_pos.mad(ai().level_graph().vertex_position(m_tEnemy.node_id),v,5.f);

	search_vertex_id = ai().level_graph().check_position_in_direction(m_tEnemy.node_id, m_tEnemy.position, next_pos);
	if (search_vertex_id == u32(-1)) search_vertex_id = m_tEnemy.node_id;

	pMonster->HDebug->M_Add(0,"INIT !!! SEARCH ENEMY ",D3DCOLOR_XRGB(255,0,0));
	pMonster->HDebug->L_Add(ai().level_graph().vertex_position(search_vertex_id),D3DCOLOR_XRGB(255,0,0));
	pMonster->HDebug->L_Add(next_pos,D3DCOLOR_XRGB(0,0,255));

	pMonster->MoveToTarget(ai().level_graph().vertex_position(search_vertex_id),search_vertex_id, pMonster->eVelocityParameterRunNormal | pMonster->eVelocityParameterStand, pMonster->eVelocityParameterRunNormal);
	m_tAction = ACTION_SEARCH_ENEMY_INIT;

	RebuildPathInterval = 0;
}


void CBitingSearchEnemy::Run()
{
	// ≈сли враг изменилс€, инициализировать состо€ние
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;
	
	//pMonster->HDebug->M_Add(0,"SEARCH ENEMY",D3DCOLOR_XRGB(255,0,128));

	bool bNeedRebuildPath = false;

	switch (m_tAction) {
	case ACTION_SEARCH_ENEMY_INIT: 
		LOG_EX("ATTACK: SEARCH_ENEMY_INIT");

		pMonster->MotionMan.m_tAction = ACT_RUN;


		if (!pMonster->IsMovingOnPath()) {
			m_tAction = ACTION_SEARCH_ENEMY;
			RebuildPathInterval = 0;
		} else break;

	case ACTION_SEARCH_ENEMY:
		// search_vertex_id - содержит исходную ноду
		LOG_EX("ATTACK: SEARCH_ENEMY");

		//if (pMonster->IsMovingOnPath()) bNeedRebuildPath = true;

		DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,3000);
			bNeedRebuildPath = true;
		DO_IN_TIME_INTERVAL_END();

		if (!pMonster->MotionStats->is_good_motion(5) || !pMonster->IsMovingOnPath()) {
			bNeedRebuildPath = true;
		}

		if (bNeedRebuildPath) {
			xr_vector<u32> nodes;
			ai().graph_engine().search( ai().level_graph(), search_vertex_id, search_vertex_id, &nodes, CGraphEngine::CFlooder(10.f));

			u32 vertex_id = nodes[::Random.randI(nodes.size())];

			pMonster->MoveToTarget(
				ai().level_graph().vertex_position(vertex_id),
				vertex_id,
				pMonster->eVelocityParamsWalk,
				pMonster->eVelocityParameterWalkNormal | pMonster->eVelocityParameterStand			
				);
		}

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		break;
	}
}

void CBitingSearchEnemy::Done()
{
	pMonster->HDebug->M_Clear();
	pMonster->HDebug->L_Clear();
}

