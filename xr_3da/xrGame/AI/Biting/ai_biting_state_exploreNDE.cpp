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
			pMonster->AI_Path.TravelPath.clear();
			pMonster->LookPosition(m_tSound.position);
		DO_ONCE_END();
	
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;

		if (m_dwStateStartedTime + 3000 < m_dwCurrentTime) m_tAction = ACTION_GOTO_SOUND_SOURCE;
		break;
	case ACTION_GOTO_SOUND_SOURCE:			// ���� � ���������
 
		DO_ONCE_BEGIN(flag_once_2);
			pMonster->vfInitSelector(pMonster->m_tSelectorHearSnd, true);
			pMonster->m_tSelectorHearSnd.m_tEnemyPosition		= m_tSound.position;
		DO_ONCE_END();

		pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorHearSnd,0, true, 0, 300);
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		break;
	}
}

