#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"
#include "../../game_graph.h"

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
	if (!pMonster->path_completed()) {
		m_bFollowPath = true;
	} else m_bFollowPath = false;
}


void CBitingRest::Replanning()
{
	u32		dwMinRand, dwMaxRand;
	u8		day_time = Level().GetDayTime();

	if ((day_time >= pMonster->_sd->m_dwDayTimeBegin) && (day_time <= pMonster->_sd->m_dwDayTimeEnd)) {  // день?

		bool bNormalSatiety = (pMonster->GetSatiety() > pMonster->_sd->m_fMinSatiety) && (pMonster->GetSatiety() < pMonster->_sd->m_fMaxSatiety); 
		if (bNormalSatiety) {		// отдых
			LOG_EX("ACTION_SATIETY_GOOD");
			m_tAction = ACTION_SATIETY_GOOD;

			dwMinRand = pMonster->_sd->m_timeLieIdleMin;	 dwMaxRand = pMonster->_sd->m_timeLieIdleMax;

		} else {					// бродит, ищет еду
			LOG_EX("ACTION_WALK");
			m_tAction = ACTION_WALK;

			// Построить путь обхода точек графа, поиск пищи
			pMonster->enable_movement(false);
			pMonster->vfUpdateDetourPoint();	
			pMonster->set_level_dest_vertex(ai().game_graph().vertex(pMonster->m_tNextGP)->level_vertex_id());

			dwMinRand = pMonster->_sd->m_timeFreeWalkMin;  dwMaxRand = pMonster->_sd->m_timeFreeWalkMax;
		}
	} else { // ночь

		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // спокоен, спит
			// спать
			LOG_EX("ACTION_SLEEP");
			m_tAction = ACTION_SLEEP;

			dwMinRand = pMonster->_sd->m_timeSleepMin; dwMaxRand = pMonster->_sd->m_timeSleepMax; 

		} else {
			// бродить (настороженно), часто осматриваться по сторонам
			LOG_EX("ACTION_WALK_CIRCUMSPECTION");

			m_tAction = ACTION_WALK_CIRCUMSPECTION;

			pMonster->enable_movement(false);
			pMonster->vfUpdateDetourPoint();	
			pMonster->set_level_dest_vertex	(ai().game_graph().vertex(pMonster->m_tNextGP)->level_vertex_id());

			dwMinRand = pMonster->_sd->m_timeFreeWalkMin; dwMaxRand = pMonster->_sd->m_timeFreeWalkMax;
		}
	}

	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
}


void CBitingRest::Run()
{
	if (m_bFollowPath) {
		if ((pMonster->CDetailPathManager::path().size() - 1) <= pMonster->CDetailPathManager::curr_travel_point_index()) m_bFollowPath = false;
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

	pMonster->SetSound(SND_TYPE_IDLE, pMonster->_sd->m_dwIdleSndDelay);
}


TTime CBitingRest::UnlockState(TTime cur_time)
{
	return cur_time;
}

