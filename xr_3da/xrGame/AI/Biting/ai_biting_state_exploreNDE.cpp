#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreNDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreNDE::CBitingExploreNDE(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetLowPriority();
}


void CBitingExploreNDE::Init()
{
	inherited::Init();

	bool bTemp;
	pMonster->GetSound(m_tSound, bTemp);

	flag_once_1 = false;
	flag_once_2 = false;

	m_tAction = ACTION_LOOK_DESTINATION;
}

void CBitingExploreNDE::Run()
{
	// ���� ����� ����, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();

	switch (m_tAction) {
	case ACTION_LOOK_DESTINATION:			// ����������� � ������� �����
		DO_ONCE_BEGIN(flag_once_1);
			pMonster->enable_movement(false);
			pMonster->LookPosition(m_tSound.position);
		DO_ONCE_END();
	
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;

		if (m_dwStateStartedTime + 3000 < m_dwCurrentTime) m_tAction = ACTION_GOTO_SOUND_SOURCE;
		break;
	case ACTION_GOTO_SOUND_SOURCE:			// ���� � ���������
		pMonster->Path_ApproachPoint(0, m_tSound.position, 2000);

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		
		// ���� ������ ����� �� ����� ����, ������� � ���������� �������
		if ( !pMonster->CDetailPathManager::path().empty() && 
			((pMonster->CDetailPathManager::path().size() - 1 ) <= pMonster->CDetailPathManager::curr_travel_point_index())) m_tAction = ACTION_LOOK_AROUND; 

		break;
	case ACTION_LOOK_AROUND:
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		break;
	}

	pMonster->SetSound(SND_TYPE_IDLE, pMonster->_sd->m_dwIdleSndDelay);
}

