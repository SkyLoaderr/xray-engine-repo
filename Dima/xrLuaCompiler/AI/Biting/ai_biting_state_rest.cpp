#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

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

	m_tAction			= ACTION_WALK;

	pMonster->SetMemoryTimeDef();

}

void CBitingRest::Init()
{
	IState::Init();

	// если есть путь - дойти до конца (последствия преследования врага)
	if (!pMonster->AI_Path.TravelPath.empty()) {
		m_bFollowPath = true;
	} else m_bFollowPath = false;
}


void CBitingRest::Replanning()
{
	u32		dwMinRand, dwMaxRand;
	u8		day_time = Level().GetDayTime();

	if ((day_time >= pMonster->m_dwDayTimeBegin) && (day_time <= pMonster->m_dwDayTimeEnd)) {  // день?

		bool bNormalSatiety = (pMonster->GetSatiety() > pMonster->m_fMinSatiety) && (pMonster->GetSatiety() < pMonster->m_fMaxSatiety); 
		if (bNormalSatiety) {		// отдых
			WRITE_TO_LOG("ACTION_SATIETY_GOOD");
			m_tAction = ACTION_SATIETY_GOOD;

			dwMinRand = pMonster->m_timeLieIdleMin;	 dwMaxRand = pMonster->m_timeLieIdleMax;

		} else {					// бродит, ищет еду
			WRITE_TO_LOG("ACTION_WALK");
			m_tAction = ACTION_WALK;

			// Построить путь обхода точек графа, поиск пищи
			pMonster->AI_Path.TravelPath.clear();
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

			dwMinRand = pMonster->m_timeFreeWalkMin;  dwMaxRand = pMonster->m_timeFreeWalkMax;
		}
	} else { // ночь

		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // спокоен, спит
			// спать
			WRITE_TO_LOG("ACTION_SLEEP");
			m_tAction = ACTION_SLEEP;

			dwMinRand = pMonster->m_timeSleepMin; dwMaxRand = pMonster->m_timeSleepMax; 

		} else {
			// бродить (настороженно), часто осматриваться по сторонам
			WRITE_TO_LOG("ACTION_WALK_CIRCUMSPECTION");

			m_tAction = ACTION_WALK_CIRCUMSPECTION;

			pMonster->AI_Path.TravelPath.clear();
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

			dwMinRand = pMonster->m_timeFreeWalkMin; dwMaxRand = pMonster->m_timeFreeWalkMax;
		}
	}

	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
}


void CBitingRest::Run()
{
	if (m_bFollowPath) {
		if ((pMonster->AI_Path.TravelPath.size() - 1) <= pMonster->AI_Path.TravelStart) m_bFollowPath = false;
	}

	if (m_bFollowPath) {
		m_tAction = ACTION_WALK_PATH_END;
	} else {
		// проверить нужно ли провести перепланировку
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPlanTime, m_dwReplanTime);
			Replanning();
		DO_IN_TIME_INTERVAL_END();
	}

	// FSM 2-го уровня
	switch (m_tAction) {
		case ACTION_WALK:		// обход точек графа
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			break;
		case ACTION_SATIETY_GOOD:     // стоять, ничего не делать
			pMonster->MotionMan.m_tAction = ACT_REST;
			break;
		case ACTION_SLEEP:		// лежать
			pMonster->MotionMan.m_tAction = ACT_SLEEP;
			break;
		case ACTION_WALK_CIRCUMSPECTION:		// повернуться на 90 град.
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			break;
		case ACTION_WALK_PATH_END:
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			break;
	}
}


TTime CBitingRest::UnlockState(TTime cur_time)
{
	return cur_time;
}

