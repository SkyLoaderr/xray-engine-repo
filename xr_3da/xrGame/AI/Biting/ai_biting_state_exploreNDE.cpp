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

void CBitingExploreNDE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
}

void CBitingExploreNDE::Init()
{
	// Test
	WRITE_TO_LOG("_ ExploreNDE Init _");

	inherited::Init();

	R_ASSERT(pMonster->IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->GetSound(se,bDangerous);			// возвращает самый опасный звук
	m_tEnemy.obj = dynamic_cast<CEntity *>(se.who);
	m_tEnemy.position = se.position;

	if (m_tEnemy.obj) pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;

	// проиграть анимацию испуга
	SetInertia(6000);
}

void CBitingExploreNDE::Run()
{
	pMonster->vfChoosePointAndBuildPath(0, &m_tEnemy.position, false, 0, 2000);

	// Установить параметры движения
	pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

}



