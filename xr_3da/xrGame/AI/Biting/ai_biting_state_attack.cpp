#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "..\\rat\\ai_rat.h"
#include "..\\bloodsucker\\ai_bloodsucker.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingAttack::CBitingAttack(CAI_Biting *p,  bool bVisibility)  
{
	pMonster = p;
	m_bInvisibility	= bVisibility;

	Reset();
	SetHighPriority();
}


void CBitingAttack::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	m_tEnemy.obj		= 0;
	m_bAttackRat		= false;

	m_fDistMin			= 0.f;	
	m_fDistMax			= 0.f;

	m_dwFaceEnemyLastTime	= 0;
	m_dwFaceEnemyLastTimeInterval = 1200;

	m_dwSuperMeleeStarted	= 0;
}

void CBitingAttack::Init()
{
	IState::Init();

	// Получить врага
	m_tEnemy = pMonster->m_tEnemy;

	// Определение класса врага
	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(m_tEnemy.obj);
	if (tpRat) m_bAttackRat = true;
	else m_bAttackRat = false;

	// Установка дистанции аттаки
	if (m_bAttackRat) {
		m_fDistMin = 0.7f;				// todo: make as ltx parameters
		m_fDistMax = 2.8f;				// todo: make as ltx parameters
	} else {
		m_fDistMin = 2.4f;				// todo: make as ltx parameters
		m_fDistMax = 3.8f;				// todo: make as ltx parameters
	}

	pMonster->SetMemoryTimeDef();
	
	flag_once_1	= false;

	bEnemyDoesntSeeMe = ((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) == FLAG_ENEMY_DOESNT_SEE_ME);
	// Test
	WRITE_TO_LOG("_ Attack Init _");
}

void CBitingAttack::Run()
{
	// Если враг изменился, инициализировать состояние
//	if (!pMonster->m_tEnemy.obj) R_ASSERT("Enemy undefined!!!");
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	// Выбор состояния
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	float dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());

	if (bAttackMelee && (dist < m_fDistMax)) m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// если враг не виден на протяжении 1 сек - бежать к нему
	if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.time + 1000 < m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}

	u32 delay;

#pragma todo("Jim to Jim: fix nesting: Bloodsucker in Biting state")
	if (m_bInvisibility) {
		CAI_Bloodsucker *pBS =	dynamic_cast<CAI_Bloodsucker *>(pMonster);
		if (pBS) {
			if ((dist < pBS->m_fInvisibilityDist) && (pBS->GetPower() > pBS->m_fPowerThreshold)) {
				if (pBS->CMonsterInvisibility::Switch(false)) {
					pBS->ChangePower(pBS->m_ftrPowerDown);
					pBS->ActivateEffector(pBS->CMonsterInvisibility::GetInvisibleInterval() / 1000.f);
				}
			}
		}
	}
	
	bool bJumpState	= pMonster->Movement.JumpState();
	if (bJumpState || (!bJumpState && pMonster->CanJump())) m_tAction = ACTION_JUMP;

	if ((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) != FLAG_ENEMY_DOESNT_SEE_ME) bEnemyDoesntSeeMe = false;
	bool bEnemyRunAway = (pMonster->flagsEnemy & FLAG_ENEMY_GO_FARTHER_FAST) == FLAG_ENEMY_GO_FARTHER_FAST;
	if ((m_tAction == ACTION_RUN) && bEnemyDoesntSeeMe && !bEnemyRunAway) m_tAction = ACTION_STEAL;

	// Выполнение состояния
	switch (m_tAction) {	
		case ACTION_RUN:		// бежать на врага
			delay = ((m_bAttackRat)? 0: 300);

			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, delay);

			pMonster->MotionMan.m_tAction = ACT_RUN;

			break;
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
			// если враг крыса под монстром подпрыгнуть и убить
			if (m_bAttackRat) {
				if (dist < 0.6f) {
					if (!m_dwSuperMeleeStarted)	m_dwSuperMeleeStarted = m_dwCurrentTime;
					if (m_dwSuperMeleeStarted + 600 < m_dwCurrentTime) {
						// прыгнуть
						pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT_JUMP);
						m_dwSuperMeleeStarted = 0;
					}
				} else m_dwSuperMeleeStarted = 0;
			}

			// Смотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
				float yaw, pitch;
				Fvector dir;
				yaw = pMonster->r_torso_target.yaw;
				pMonster->AI_Path.TravelPath.clear();
				dir.sub(m_tEnemy.obj->Position(), pMonster->Position());
				dir.getHP(yaw,pitch);
				yaw *= -1;
				yaw = angle_normalize(yaw);
				pMonster->r_torso_target.yaw = yaw;
			DO_IN_TIME_INTERVAL_END();
			
			if (m_bAttackRat) pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT);
			pMonster->MotionMan.m_tAction = ACT_ATTACK;

			break;
		case ACTION_STEAL:
			if (dist < (m_fDistMax + 2.f)) bEnemyDoesntSeeMe = false;

			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, 2000);

			pMonster->MotionMan.m_tAction = ACT_STEAL;
			break;
		case ACTION_JUMP:
			DO_ONCE_BEGIN(flag_once_1);
				pMonster->Jump(m_tEnemy.obj->Position());
			DO_ONCE_END();
			
			pMonster->MotionMan.m_tAction = ACT_JUMP;
			break;
	}
}

