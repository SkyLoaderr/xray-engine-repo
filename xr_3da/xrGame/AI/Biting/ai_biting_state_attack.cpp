#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "../rat/ai_rat.h"
#include "../bloodsucker/ai_bloodsucker.h"
#include "../../actor.h"
#include "../ai_monster_jump.h"

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
		m_fDistMin = pMonster->_sd->m_fMinAttackDist;
		m_fDistMax = pMonster->_sd->m_fMaxAttackDist;
	}
	
	dist = 0;

	pMonster->SetMemoryTimeDef();
	
	flag_once_1	= false;

	bEnemyDoesntSeeMe = ((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) == FLAG_ENEMY_DOESNT_SEE_ME);

	pMonster->AS_Start();

	bCanThreaten	= true;

	// Test
	LOG_EX("_ Attack Init _");
}

void CBitingAttack::Run()
{
	// Если враг изменился, инициализировать состояние
//	if (!pMonster->m_tEnemy.obj) R_ASSERT("m_enemy undefined!!!");
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	// for attack stops needed
	if (!m_bAttackRat) {
		m_fDistMin = pMonster->m_fCurMinAttackDist;
		m_fDistMax = pMonster->_sd->m_fMaxAttackDist - (pMonster->_sd->m_fMinAttackDist - pMonster->m_fCurMinAttackDist);
	}

	// Выбор состояния
	bool bAttackMelee = (ACTION_ATTACK_MELEE == m_tAction);

	dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());

	if (bAttackMelee && (dist < m_fDistMax)) m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// если враг не виден на протяжении 1 сек - бежать к нему
	if (ACTION_ATTACK_MELEE == m_tAction && (m_tEnemy.time + 1000 < m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}

	u32 delay;

	// проверить на возможность прыжка
	CJumping *pJumping = dynamic_cast<CJumping *>(pMonster);
	if (pJumping) pJumping->Check(pMonster->Position(),m_tEnemy.obj->Position(),m_tEnemy.obj);
	
	if (pMonster->m_PhysicMovementControl.JumpState()) {
		pMonster->enable_movement(false);
		return;
	}

	if ((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) != FLAG_ENEMY_DOESNT_SEE_ME) bEnemyDoesntSeeMe = false;
	if (((pMonster->flagsEnemy & FLAG_ENEMY_GO_FARTHER_FAST) == FLAG_ENEMY_GO_FARTHER_FAST) && (m_dwStateStartedTime + 4000 < m_dwCurrentTime)) bEnemyDoesntSeeMe = false;
	if ((ACTION_RUN == m_tAction) && bEnemyDoesntSeeMe) m_tAction = ACTION_STEAL;

	if (CheckThreaten()) m_tAction = ACTION_THREATEN;

#pragma todo("Jim to Jim: fix nesting: Bloodsucker in Biting state")
	if (m_bInvisibility && ACTION_THREATEN != m_tAction) {
		CAI_Bloodsucker *pBS =	dynamic_cast<CAI_Bloodsucker *>(pMonster);
		CActor			*pA  =  dynamic_cast<CActor*>(Level().CurrentEntity());

		if (pBS && pA && (pA->Position().distance_to(pBS->Position()) < pBS->m_fEffectDist)) {
			if ((dist < pBS->m_fInvisibilityDist) && (pBS->GetPower() > pBS->m_fPowerThreshold)) {
				if (pBS->CMonsterInvisibility::Switch(false)) {
					pBS->ChangePower(pBS->m_ftrPowerDown);
					pBS->ActivateEffector(pBS->CMonsterInvisibility::GetInvisibleInterval() / 1000.f);
				}
			}
		}
	}


	// Выполнение состояния
	switch (m_tAction) {	
		case ACTION_RUN:		 // бежать на врага
			delay = ((m_bAttackRat)? 0: 300);

			pMonster->set_level_dest_vertex(m_tEnemy.obj->level_vertex_id());
			pMonster->set_dest_position(m_tEnemy.obj->Position());

			pMonster->MotionMan.m_tAction = ACT_RUN;
			break;
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
			pMonster->enable_movement(false);			
			
			bCanThreaten	= false;

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
				yaw = pMonster->m_body.target.yaw;
				dir.sub(m_tEnemy.obj->Position(), pMonster->Position());
				dir.getHP(yaw,pitch);
				yaw *= -1;
				yaw = angle_normalize(yaw);
				pMonster->m_body.target.yaw = yaw;
			DO_IN_TIME_INTERVAL_END();
			
			if (m_bAttackRat) pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT);
			pMonster->MotionMan.m_tAction = ACT_ATTACK;

			break;
		case ACTION_STEAL:
			if (dist < (m_fDistMax + 2.f)) bEnemyDoesntSeeMe = false;

			pMonster->set_level_dest_vertex(m_tEnemy.obj->level_vertex_id());
			pMonster->set_dest_position(m_tEnemy.obj->Position());

			pMonster->MotionMan.m_tAction = ACT_STEAL;
			break;
		case ACTION_THREATEN: 
			// Смотреть на врага 
			LOG_EX("ACTION THREATEN!!!");
			pMonster->enable_movement(false);

			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
				float yaw, pitch;
				Fvector dir;
				yaw = pMonster->m_body.target.yaw;
				dir.sub(m_tEnemy.obj->Position(), pMonster->Position());
				dir.getHP(yaw,pitch);
				yaw *= -1;
				yaw = angle_normalize(yaw);
				pMonster->m_body.target.yaw = yaw;
			DO_IN_TIME_INTERVAL_END();

			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			pMonster->MotionMan.SetSpecParams(ASP_THREATEN);

			break;
	}

	pMonster->SetSound(SND_TYPE_ATTACK, pMonster->_sd->m_dwAttackSndDelay);
}

void CBitingAttack::Done()
{
	inherited::Done();

	pMonster->AS_Stop();
}


// Реализация пугания:
// мораль - маленькая, расстояние - расстояние для аттаки
// на протяжении N мин не был атакован этим монстром
// враг стоит не бежит, видит
// если состояния ATTACK_MELEE ещё не было
bool CBitingAttack::CheckThreaten()
{
//	if (pMonster->GetEntityMorale() > 0.8f) {
//		LOG_EX("Try Threaten:: Entity Morale > 0.8");
//		return false;
//	}

	if (((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) == FLAG_ENEMY_DOESNT_SEE_ME) || 
		((pMonster->flagsEnemy & FLAG_ENEMY_GO_FARTHER_FAST) == FLAG_ENEMY_GO_FARTHER_FAST)) {
		LOG_EX("Try Threaten:: враг бежит || не видит");
		return false;
	}

	if ((dist < m_fDistMin) || (dist > m_fDistMax)) {
		LOG_EX("Try Threaten:: дистанция не для атаки");
		return false;
	}
	if (pMonster->IsDangerousEnemy(m_tEnemy.obj)) {
		LOG_EX("Try Threaten:: враг опасный");
		return false;
	}

	if (!bCanThreaten) {
		LOG_EX("Try Threaten:: не могу атаковать");	
		return false;
	}

	return true;
}


