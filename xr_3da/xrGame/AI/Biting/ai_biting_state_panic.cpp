#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "../bloodsucker/ai_bloodsucker.h"
#include "../../actor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingPanic::CBitingPanic(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}

void CBitingPanic::Init()
{
	LOG_EX("PANIC:: Init");
	inherited::Init();

	// Получить врага
	m_tEnemy		= pMonster->m_tEnemy;

	m_tAction		= ACTION_RUN;
}

void CBitingPanic::Run()
{
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	switch (m_tAction) {
		
		/**************/
		case ACTION_RUN:
		/**************/
		
			LOG_EX("PANIC:: Run away");
			pMonster->MotionMan.m_tAction	= ACT_RUN;

			pMonster->MoveAwayFromTarget			(m_tEnemy.position);
			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);

			// если не видел врага > 15 сек 
			if (m_tEnemy.time + 15000 < m_dwCurrentTime) {
				m_tAction = ACTION_FACE_BACK_SCARED;
				pMonster->CMovementManager::m_body.target.yaw = angle_normalize(pMonster->CMovementManager::m_body.target.yaw + PI);
			}

			break;
		
		/***************************/
		case ACTION_FACE_BACK_SCARED:
		/***************************/

			LOG_EX("PANIC:: Face Back scared!");
			pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;

			// если враг виден
			if (m_tEnemy.time + 1000 > m_dwCurrentTime) m_tAction = ACTION_RUN;

			break;
	}
	
	pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundPanic, 0,0,pMonster->_sd->m_dwAttackSndDelay);

	if (pMonster->ability_invisibility()) {
		CAI_Bloodsucker *pBS =	dynamic_cast<CAI_Bloodsucker *>(pMonster);
		CActor			*pA  =  dynamic_cast<CActor*>(Level().CurrentEntity());

		if (pBS && pA && (pA->Position().distance_to(pBS->Position()) < pBS->m_fEffectDist)) {
			if (pBS->GetPower() > pBS->m_fPowerThreshold) {
				if (pBS->CMonsterInvisibility::Switch(false)) {
					pBS->ChangePower(pBS->m_ftrPowerDown);
					pBS->ActivateEffector(pBS->CMonsterInvisibility::GetInvisibleInterval() / 1000.f);
				}
			}
		}
	}

}

bool CBitingPanic::CheckCompletion()
{
	if (!m_tEnemy.obj) return true;
	if (pMonster->Position().distance_to(m_tEnemy.obj->Position()) > 30.f) return true;

	return false;
}

