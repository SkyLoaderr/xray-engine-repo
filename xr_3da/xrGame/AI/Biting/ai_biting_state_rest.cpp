#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "..\\rat\\ai_rat.h"

#include "..\\..\\PhysicsShell.h"
#include "..\\..\\phcapture.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingRest::CBitingRest(CAI_Biting *p)  
{
	pMonster = p;
	Reset();

	SetLowPriority();			
}


void CBitingRest::Reset()
{
	IState::Reset();

	m_dwReplanTime		= 0;
	m_dwLastPlanTime	= 0;

	m_tAction			= ACTION_STAND;

	pMonster->SetMemoryTimeDef();

}

void CBitingRest::Init()
{
	IState::Init();

	// если есть путь - дойти до конца (последстви€ преследовани€ врага)
	if (!pMonster->AI_Path.TravelPath.empty()) {
		m_bFollowPath = true;
	} else m_bFollowPath = false;
}


void CBitingRest::Replanning()
{
	m_dwLastPlanTime = m_dwCurrentTime;	
	u32		rand_val = ::Random.randI(100);
	u32		cur_val;
	u32		dwMinRand, dwMaxRand;

	if (rand_val < (cur_val = pMonster->m_dwProbRestWalkFree)) {	
		m_tAction = ACTION_WALK;
		// ѕостроить путь обхода точек графа
		pMonster->AI_Path.TravelPath.clear();
		pMonster->vfUpdateDetourPoint();	
		pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

		dwMinRand = pMonster->m_timeFreeWalkMin;
		dwMaxRand = pMonster->m_timeFreeWalkMax;

	} else if (rand_val < (cur_val = cur_val + pMonster->m_dwProbRestStandIdle)) {	
		m_tAction = ACTION_STAND;

		dwMinRand = pMonster->m_timeStandIdleMin;
		dwMaxRand = pMonster->m_timeStandIdleMax;

	} else if (rand_val < (cur_val = cur_val + pMonster->m_dwProbRestLieIdle)) {	
		m_tAction = ACTION_LIE;
		// проверить лежит уже?
		if (pMonster->m_tAnim != eAnimLieIdle) {
			pMonster->Motion.m_tSeq.Add(eAnimStandLieDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
			pMonster->Motion.m_tSeq.Switch();
		}

		dwMinRand = pMonster->m_timeLieIdleMin;
		dwMaxRand = pMonster->m_timeLieIdleMax;

	} else  {	
		m_tAction = ACTION_TURN;
		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI_DIV_2);

		dwMinRand = 1000;
		dwMaxRand = 1100;

	}

	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
}


void CBitingRest::Run()
{

	if (m_bFollowPath) {
		if ((pMonster->AI_Path.TravelPath.size() - 1) <= pMonster->AI_Path.TravelStart) m_bFollowPath = false;
	}

	if (m_bFollowPath) {
		m_tAction = ACTION_WALK_GRAPH_END;
	} else {
		// проверить нужно ли провести перепланировку
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPlanTime, m_dwReplanTime);
		Replanning();
		DO_IN_TIME_INTERVAL_END();
	}

	// FSM 2-го уровн€
	switch (m_tAction) {
		case ACTION_WALK:		// обход точек графа
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			break;
		case ACTION_STAND:     // сто€ть, ничего не делать
			pMonster->Motion.m_tParams.SetParams(eAnimStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_LIE:		// лежать
			pMonster->Motion.m_tParams.SetParams(eAnimLieIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_TURN:		// повернутьс€ на 90 град.
			pMonster->Motion.m_tParams.SetParams(eAnimStandTurnLeft,0,pMonster->m_ftrStandTurnRSpeed, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_WALK_GRAPH_END:
			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			break;
	}
}


TTime CBitingRest::UnlockState(TTime cur_time)
{
	TTime dt = inherited::UnlockState(cur_time);

	m_dwReplanTime		+= dt;
	m_dwLastPlanTime	+= dt;

	return dt;
}

