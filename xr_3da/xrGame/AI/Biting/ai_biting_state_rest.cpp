#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#define DELTA_NEXT_THINK	5000


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingRest::CBitingRest(CAI_Biting *p)  
{
	pMonster = p;
	SetPriority(PRIORITY_LOW);
}

void CBitingRest::Init()
{
	LOG_EX2("%s: rest init", *"*/ pMonster->cName() /*"*);
	IState::Init();

	// ���� ���� ���� - ����� �� ����� (����������� ������������� �����) FIXME
	if (pMonster->IsMovingOnPath()) m_bFollowPath = true;
	else m_bFollowPath = false;

	m_dwLastPlanTime	= 0;
	m_tAction			= ACTION_WALK;

}

void CBitingRest::Replanning()
{
	u8		day_time = Level().GetDayTime();

	if ((day_time >= pMonster->_sd->m_dwDayTimeBegin) && (day_time <= pMonster->_sd->m_dwDayTimeEnd)) {  // ����?

		bool bNormalSatiety = (pMonster->GetSatiety() > pMonster->_sd->m_fMinSatiety) && (pMonster->GetSatiety() < pMonster->_sd->m_fMaxSatiety); 
		if (bNormalSatiety) {		// �����
			m_tAction = ACTION_SATIETY_GOOD;
		} else {					// ������, ���� ���
			m_tAction = ACTION_WALK;
		}
	} else { // ����

		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // �������, ����
			// �����
			m_tAction = ACTION_SLEEP;
		} else {
			// ������� (������������), ����� ������������� �� ��������
			m_tAction = ACTION_WALK_CIRCUMSPECTION;
		}
	}
}


void CBitingRest::Run()
{
	if (m_bFollowPath) 
		if (pMonster->CDetailPathManager::completed(pMonster->Position())) m_bFollowPath = false;

	if (m_bFollowPath) {
		m_tAction = ACTION_WALK_PATH_END;
	} else {
		
		// ��������� ����� �� �������� ��������������
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPlanTime, DELTA_NEXT_THINK);
			Replanning();
		DO_IN_TIME_INTERVAL_END();
	}

	
	pMonster->MotionMan.accel_activate(eAT_Calm);

	switch (m_tAction) {
		case ACTION_WALK:						// ����� ����� �����
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			
			// ������ �� ������ �����
			pMonster->WalkNextGraphPoint();

			break;
		case ACTION_SATIETY_GOOD:				// ������, ������ �� ������
			pMonster->MotionMan.m_tAction = ACT_REST;
			break;
		case ACTION_SLEEP:						// ������
			pMonster->MotionMan.m_tAction = ACT_SLEEP;
			break;
		case ACTION_WALK_CIRCUMSPECTION:		// ����������� �� 90 ����.
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

			// ������ �� ������ �����
			pMonster->WalkNextGraphPoint();
			break;
		case ACTION_WALK_PATH_END:
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			
			break;
	}

	pMonster->State_PlaySound(MonsterSpace::eMonsterSoundIdle, pMonster->_sd->m_dwIdleSndDelay);

}

TTime CBitingRest::UnlockState(TTime cur_time)
{
	return cur_time;
}

