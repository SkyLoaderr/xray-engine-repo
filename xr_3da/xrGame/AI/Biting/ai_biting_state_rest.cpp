#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingRest::CBitingRest(CAI_Biting *p)  
{
	pMonster = p;
	SetPriority(PRIORITY_LOW);
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
	LOG_EX2("%s: rest init", *"*/ pMonster->cName() /*"*);
	IState::Init();

	// если есть путь - дойти до конца (последствия преследования врага) FIXME
	if (pMonster->IsMovingOnPath()) m_bFollowPath = true;
	else m_bFollowPath = false;
}

void CBitingRest::Replanning()
{
	u32		dwMinRand, dwMaxRand;
	u8		day_time = Level().GetDayTime();

	if ((day_time >= pMonster->_sd->m_dwDayTimeBegin) && (day_time <= pMonster->_sd->m_dwDayTimeEnd)) {  // день?

		bool bNormalSatiety = (pMonster->GetSatiety() > pMonster->_sd->m_fMinSatiety) && (pMonster->GetSatiety() < pMonster->_sd->m_fMaxSatiety); 
		if (bNormalSatiety) {		// отдых
			m_tAction = ACTION_SATIETY_GOOD;

			dwMinRand = pMonster->_sd->m_timeLieIdleMin;	 dwMaxRand = pMonster->_sd->m_timeLieIdleMax;

		} else {					// бродит, ищет еду
			m_tAction = ACTION_WALK;
			
			dwMinRand = pMonster->_sd->m_timeFreeWalkMin;  dwMaxRand = pMonster->_sd->m_timeFreeWalkMax;
		}
	} else { // ночь

		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // спокоен, спит
			// спать
			m_tAction = ACTION_SLEEP;

			dwMinRand = pMonster->_sd->m_timeSleepMin; dwMaxRand = pMonster->_sd->m_timeSleepMax; 

		} else {
			// бродить (настороженно), часто осматриваться по сторонам

			m_tAction = ACTION_WALK_CIRCUMSPECTION;


			dwMinRand = pMonster->_sd->m_timeFreeWalkMin; dwMaxRand = pMonster->_sd->m_timeFreeWalkMax;
		}
	}

	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
}


void CBitingRest::Run()
{
	if (m_bFollowPath) 
		if (pMonster->CDetailPathManager::completed(pMonster->Position())) m_bFollowPath = false;

	if (m_bFollowPath) {
		m_tAction = ACTION_WALK_PATH_END;
	} else {
		// проверить нужно ли провести перепланировку
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPlanTime, m_dwReplanTime);
			Replanning();
		DO_IN_TIME_INTERVAL_END();
	}

	switch (m_tAction) {
		case ACTION_WALK:						// обход точек графа
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

			// ходить по точкам графа
			pMonster->WalkNextGraphPoint();

			break;
		case ACTION_SATIETY_GOOD:				// стоять, ничего не делать
			pMonster->enable_movement(false);
			pMonster->MotionMan.m_tAction = ACT_REST;
			break;
		case ACTION_SLEEP:						// лежать
			pMonster->enable_movement(false);
			pMonster->MotionMan.m_tAction = ACT_SLEEP;
			break;
		case ACTION_WALK_CIRCUMSPECTION:		// повернуться на 90 град.
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

			// ходить по точкам графа
			pMonster->WalkNextGraphPoint();
			break;
		case ACTION_WALK_PATH_END:
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			break;
	}

	pMonster->SetSound(SND_TYPE_IDLE, pMonster->_sd->m_dwIdleSndDelay);
}

TTime CBitingRest::UnlockState(TTime cur_time)
{
	return cur_time;
}

