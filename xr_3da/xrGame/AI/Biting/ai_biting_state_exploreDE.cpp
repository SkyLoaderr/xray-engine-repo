#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreDE::CBitingExploreDE(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingExploreDE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
	m_tAction = ACTION_LOOK_AROUND;
	m_dwTimeToTurn	= 0;	
}

void CBitingExploreDE::Init()
{
	// Test
	WRITE_TO_LOG("_ ExploreDE Init _");

	inherited::Init();

	R_ASSERT(pMonster->IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->GetSound(se,bDangerous);	// возвращает самый опасный звук
	m_tEnemy.obj = dynamic_cast<CEntity *>(se.who);
	m_tEnemy.position = se.position;
	m_dwSoundTime	  = se.time;

	float	yaw,pitch;
	Fvector dir;
	dir.sub(m_tEnemy.position,pMonster->Position());
	dir.getHP(yaw,pitch);

	pMonster->r_torso_target.yaw = yaw;
	m_dwTimeToTurn = (TTime)(_abs(angle_normalize_signed(yaw - pMonster->r_torso_current.yaw)) / pMonster->m_ftrStandTurnRSpeed * 1000);

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);
}

void CBitingExploreDE::Run()
{
	// определение состояния
	if (m_tAction == ACTION_LOOK_AROUND && (m_dwStateStartedTime + m_dwTimeToTurn < m_dwCurrentTime)) m_tAction = ACTION_HIDE;

	SoundElem se;
	bool bDangerous;
	pMonster->GetSound(se,bDangerous);	// возвращает самый опасный звук

	switch(m_tAction) {
	case ACTION_LOOK_AROUND:
		pMonster->Motion.m_tParams.SetParams(eAnimStandTurnLeft,0,pMonster->m_ftrStandTurnRSpeed, pMonster->r_torso_target.yaw, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
		pMonster->Motion.m_tTurn.Clear();
		break;
	case ACTION_HIDE:
		pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
		pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
		pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

		pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0,2000);

		// Установить параметры движения
		pMonster->Motion.m_tParams.SetParams	(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set			(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
		break;
	}
}

bool CBitingExploreDE::CheckCompletion()
{	
	//	if (!m_tEnemy.obj) return true;
	//	
	//	// проверить, если ли новый звук, завершить состояние
	//	SoundElem se;
	//	bool bDangerous;
	//	pMonster->GetMostDangerousSound(se,bDangerous);	// возвращает самый опасный звук
	//	
	//	if ((m_dwSoundTime + 2000)< se.time) return true;
	//
	return false;
}