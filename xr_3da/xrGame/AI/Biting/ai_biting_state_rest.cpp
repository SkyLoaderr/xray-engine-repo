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
	IState::Init();

	LOG_EX("-REST_INIT");

	// ���� ���� ���� - ����� �� ����� (����������� ������������� �����)
	if (!pMonster->path_completed()) {
		m_bFollowPath = true;
	} else m_bFollowPath = false;
}


void CBitingRest::Replanning()
{
	u32		dwMinRand, dwMaxRand;
	u8		day_time = Level().GetDayTime();

	if ((day_time >= pMonster->_sd->m_dwDayTimeBegin) && (day_time <= pMonster->_sd->m_dwDayTimeEnd)) {  // ����?

		bool bNormalSatiety = (pMonster->GetSatiety() > pMonster->_sd->m_fMinSatiety) && (pMonster->GetSatiety() < pMonster->_sd->m_fMaxSatiety); 
		if (bNormalSatiety) {		// �����
			m_tAction = ACTION_SATIETY_GOOD;

			dwMinRand = pMonster->_sd->m_timeLieIdleMin;	 dwMaxRand = pMonster->_sd->m_timeLieIdleMax;

		} else {					// ������, ���� ���
			m_tAction = ACTION_WALK;

			// ��������� ���� ������ ����� �����, ����� ����
			u32 vertex_id = ::Random.randI(ai().level_graph().header().vertex_count());

			pMonster->SetPathParams(
				CMovementManager::ePathTypeLevelPath, 
				vertex_id, 
				ai().level_graph().vertex_position(vertex_id),
				pMonster->eVelocityParamsWalk,
				pMonster->eVelocityParameterWalkNormal | pMonster->eVelocityParameterStand
			);						


//			if (!pMonster->CDetailPathManager::valid()) {
//			}
			
			dwMinRand = pMonster->_sd->m_timeFreeWalkMin;  dwMaxRand = pMonster->_sd->m_timeFreeWalkMax;
		}
	} else { // ����

		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // �������, ����
			// �����
			m_tAction = ACTION_SLEEP;

			dwMinRand = pMonster->_sd->m_timeSleepMin; dwMaxRand = pMonster->_sd->m_timeSleepMax; 

		} else {
			// ������� (������������), ����� ������������� �� ��������

			m_tAction = ACTION_WALK_CIRCUMSPECTION;

			u32 vertex_id = ::Random.randI(ai().level_graph().header().vertex_count());

			pMonster->SetPathParams(
				CMovementManager::ePathTypeLevelPath, 
				vertex_id, 
				ai().level_graph().vertex_position(vertex_id),
				pMonster->eVelocityParamsWalk,
				pMonster->eVelocityParameterWalkNormal | pMonster->eVelocityParameterStand
			);						

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
		// ��������� ����� �� �������� ��������������
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPlanTime, m_dwReplanTime);
			Replanning();
		DO_IN_TIME_INTERVAL_END();
	}

	// FSM 2-�� ������
	switch (m_tAction) {
		case ACTION_WALK:		// ����� ����� �����
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			break;
		case ACTION_SATIETY_GOOD:     // ������, ������ �� ������
			pMonster->enable_movement(false);
			pMonster->MotionMan.m_tAction = ACT_REST;
			break;
		case ACTION_SLEEP:		// ������
			pMonster->enable_movement(false);
			pMonster->MotionMan.m_tAction = ACT_SLEEP;
			break;
		case ACTION_WALK_CIRCUMSPECTION:		// ����������� �� 90 ����.
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

