#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDNE class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreDNE::CBitingExploreDNE(CAI_Biting *p)
{
	pMonster = p;
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
	StartPosition			= SavedPosition;			// сохранить стартовую позицию
	LastPosition.set		(0.f,0.f,0.f);			
	m_fRunAwayDist			= 50.f;
	m_dwLastPosSavedTime	= 0;
	m_dwStayLastTimeCheck	= m_dwCurrentTime;

	m_tAction				= ACTION_RUN_AWAY;

	SetInertia				(20000);
}

void CBitingExploreDNE::Run()
{
	// если новый звук, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();

//	// нивидимость
//	if (pMonster->GetPower() > pMonster->m_fPowerThreshold) {
//		if (pMonster->CMonsterInvisibility::Switch(false)) pMonster->ChangePower(pMonster->m_ftrPowerDown);
//	}
	
	switch (m_tAction) {
	case ACTION_RUN_AWAY: // убегать на N метров от звука
		Msg(" DNE : [RUN AWAY]");

		pMonster->Path_GetAwayFromPoint (0, m_tSound.position, m_fRunAwayDist, 2000);
		pMonster->MotionMan.m_tAction = ACT_RUN;

		// каждую минуту сохранять текущую позицию, а затем развернуться и посмотреть в последнюю позицию
		// т.е. развернуться назад
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPosSavedTime, 1000);
			SavedPosition = pMonster->Position();
		DO_IN_TIME_INTERVAL_END();

		// проверить условие перехода в след. состояние (достаточное расстояние || стоим на месте около 2 сек)
		if (pMonster->Position().distance_to(StartPosition) > m_fRunAwayDist) m_tAction = ACTION_LOOK_BACK_POSITION;
		
		DO_IN_TIME_INTERVAL_BEGIN(m_dwStayLastTimeCheck, 2000);
			if (pMonster->Position().similar(LastPosition)) m_tAction = ACTION_LOOK_BACK_POSITION;
			LastPosition = pMonster->Position();
		DO_IN_TIME_INTERVAL_END();

		break;
	case ACTION_LOOK_BACK_POSITION:			// повернуться в сторону звука
		Msg("DNE : [LOOK_BACK_POSITION]");
		DO_ONCE_BEGIN(flag_once_1);
			pMonster->AI_Path.TravelPath.clear();
			pMonster->LookPosition(SavedPosition);
		DO_ONCE_END();

		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		
		// если уже повернулся, перейти в след. состояние
		if (getAI().bfTooSmallAngle(pMonster->r_torso_current.yaw, pMonster->r_torso_target.yaw, PI_DIV_6/6)) m_tAction = ACTION_LOOK_AROUND;
		break;

	case ACTION_LOOK_AROUND:
		Msg("DNE : [LOOK_AROUND]");
		
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		break;
	}
}
