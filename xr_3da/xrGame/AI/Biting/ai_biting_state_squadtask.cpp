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
	case SC_FOLLOW:			break;
	case SC_FEEL_DANGER:	break;
	}

	pMonster->Path_ApproachPoint(0, task.target.pos, 2000);
	pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

}

