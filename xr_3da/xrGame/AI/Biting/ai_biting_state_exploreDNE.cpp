#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "..\\bloodsucker\\ai_bloodsucker.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDNE class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreDNE::CBitingExploreDNE(CAI_Biting *p, bool bVisibility)
{
	pMonster = p;
	m_bInvisibility	= bVisibility;

	Reset();
	SetHighPriority();
}


void CBitingExploreDNE::Init()
{
	IState::Init();	
	bool bTemp;
	pMonster->GetSound(m_tSound, bTemp);

	flag_once_1				= false;
	SavedPosition			= pMonster->Position();
	StartPosition			= SavedPosition;			// ��������� ��������� �������
	LastPosition.set		(0.f,0.f,0.f);			
	m_fRunAwayDist			= 50.f;
	m_dwLastPosSavedTime	= 0;
	m_dwStayLastTimeCheck	= m_dwCurrentTime;

	m_tAction				= ACTION_RUN_AWAY;

	SetInertia				(20000);
}

void CBitingExploreDNE::Run()
{
	// ���� ����� ����, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();

	// �����������
#pragma todo("Jim to Jim: fix nesting: Bloodsucker in Biting state")
	if (m_bInvisibility) {
		CAI_Bloodsucker *pBS =	dynamic_cast<CAI_Bloodsucker *>(pMonster);	
		if (pBS->GetPower() > pBS->m_fPowerThreshold) {
			if (pBS->CMonsterInvisibility::Switch(false)) {
				pBS->ChangePower(pBS->m_ftrPowerDown);
				pBS->ActivateEffector(pBS->CMonsterInvisibility::GetInvisibleInterval() / 1000.f);
			}
		}
	}
	
	switch (m_tAction) {
	case ACTION_RUN_AWAY: // ������� �� N ������ �� �����
		WRITE_TO_LOG(" DNE : [RUN AWAY]");

		pMonster->Path_GetAwayFromPoint (0, m_tSound.position, m_fRunAwayDist, 2000);
		pMonster->MotionMan.m_tAction = ACT_RUN;

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
		WRITE_TO_LOG("DNE : [LOOK_BACK_POSITION]");
		DO_ONCE_BEGIN(flag_once_1);
			pMonster->AI_Path.TravelPath.clear();
			pMonster->LookPosition(SavedPosition);
		DO_ONCE_END();

		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		
		// ���� ��� ����������, ������� � ����. ���������
		if (getAI().bfTooSmallAngle(pMonster->r_torso_current.yaw, pMonster->r_torso_target.yaw, PI_DIV_6/6)) m_tAction = ACTION_LOOK_AROUND;
		break;

	case ACTION_LOOK_AROUND:
		WRITE_TO_LOG("DNE : [LOOK_AROUND]");
		
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		break;
	}
	
	pMonster->SetSound(pMonster->SND_TYPE_IDLE, pMonster->m_dwIdleSndDelay, false);
}
