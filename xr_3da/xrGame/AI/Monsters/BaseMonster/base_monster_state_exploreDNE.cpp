#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterExploreDNE class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBaseMonsterExploreDNE::CBaseMonsterExploreDNE(CBaseMonster *p)
{
	pMonster = p;
	
	SetPriority(PRIORITY_NORMAL);
}


void CBaseMonsterExploreDNE::Init()
{
	IState::Init();	
	bool bTemp;
	pMonster->SoundMemory.GetSound(m_tSound, bTemp);

	flag_once_1				= false;
	SavedPosition			= pMonster->Position();
	StartPosition			= SavedPosition;			// ��������� ��������� �������
	LastPosition.set		(0.f,0.f,0.f);			
	m_fRunAwayDist			= 50.f;
	m_dwLastPosSavedTime	= 0;
	m_dwStayLastTimeCheck	= m_dwCurrentTime;

	m_tAction				= ACTION_RUN_AWAY;
}

void CBaseMonsterExploreDNE::Run()
{
	// ���� ����� ����, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->SoundMemory.GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();

	switch (m_tAction) {
	case ACTION_RUN_AWAY: // ������� �� N ������ �� �����
		
		pMonster->MotionMan.m_tAction = ACT_RUN;
		pMonster->MoveAwayFromTarget (m_tSound.position);

		// ������ ������ ��������� ������� �������, � ����� ������������ � ���������� � ��������� �������
		// �.�. ������������ �����
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPosSavedTime, 1000);
			SavedPosition = pMonster->Position();
		DO_IN_TIME_INTERVAL_END();

		// ��������� ������� �������� � ����. ��������� (����������� ���������� || ����� �� ����� ����� 2 ���)
		if (pMonster->Position().distance_to(StartPosition) > m_fRunAwayDist) m_tAction = ACTION_LOOK_BACK_POSITION;
		
		DO_IN_TIME_INTERVAL_BEGIN(m_dwStayLastTimeCheck, 2000);
			if (pMonster->Position().similar(LastPosition)) m_tAction = ACTION_LOOK_BACK_POSITION;
			LastPosition = pMonster->Position();
		DO_IN_TIME_INTERVAL_END();

		break;
	case ACTION_LOOK_BACK_POSITION:			// ����������� � ������� �����
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;

		DO_ONCE_BEGIN(flag_once_1);
			pMonster->LookPosition(SavedPosition);
		DO_ONCE_END();

		// ���� ��� ����������, ������� � ����. ���������
		if (angle_difference(pMonster->CMovementManager::m_body.current.yaw, pMonster->CMovementManager::m_body.target.yaw) <= PI_DIV_6/6) m_tAction = ACTION_LOOK_AROUND;
		break;

	case ACTION_LOOK_AROUND:
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		break;
	}
	
	pMonster->State_PlaySound(MonsterSpace::eMonsterSoundIdle, pMonster->get_sd()->m_dwIdleSndDelay);

}
