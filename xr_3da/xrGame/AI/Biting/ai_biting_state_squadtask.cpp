#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

CBitingSquadTask::CBitingSquadTask(CAI_Biting *p)
{
	pMonster = p;
}

void CBitingSquadTask::Init()
{
	inherited::Init();

	// Get task
	CMonsterSquad *pSquad = Level().SquadMan.GetSquad((u8)pMonster->g_Squad());
	GTask &task = pSquad->GetTask(pMonster);

	switch (task.state.command) {
	case SC_EXPLORE:		break;
	case SC_ATTACK:			break;
	case SC_THREATEN:		break;
	case SC_COVER:			break;
	case SC_FOLLOW:			
		// 1. set target
		// 2. Prepare selector	


		break;
	case SC_FEEL_DANGER:	break;
	}


	// Test
	LOG_EX("_ Squad Task Init _");
}

void CBitingSquadTask::Run()
{
	// Get task
	CMonsterSquad *pSquad = Level().SquadMan.GetSquad((u8)pMonster->g_Squad());
	GTask &task = pSquad->GetTask(pMonster);

	switch (task.state.command) {
	case SC_EXPLORE:		break;
	case SC_ATTACK:			break;
	case SC_THREATEN:		break;
	case SC_COVER:			break;
	case SC_FOLLOW:			
		// Exec path selector
		pMonster->Path_ApproachPoint(0, task.target.pos, 2000);
		pMonster->CLevelLocationSelector::set_evaluator(pMonster->m_tSelectorApproach);

		LOG_EX("Squad Task	::	Evaluate selector... ");

		break;
	case SC_FEEL_DANGER:	break;
	}



	pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

}

