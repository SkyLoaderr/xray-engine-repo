#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "..\\rat\\ai_rat.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingAttack::CBitingAttack(CAI_Biting *p)  
{
	pMonster = p;
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
		m_fDistMin = 0.7f;
		m_fDistMax = 2.8f;
	} else {
		m_fDistMin = 2.4f;
		m_fDistMax = 3.8f;
	}

	pMonster->SetMemoryTimeDef();

	// Test
	WRITE_TO_LOG("_ Attack Init _");
}

void CBitingAttack::Run()
{
	if (!pMonster->m_tEnemy.obj) R_ASSERT("Enemy undefined!!!");

	// Если враг изменился, инициализировать состояние
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	// Выбор состояния
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);
	float dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());

	if (bAttackMelee && (dist < m_fDistMax)) 
		m_tAction = ACTION_ATTACK_MELEE;
	else 
		m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// задержка для построения пути
	u32 delay;

	// если враг не виден - бежать к нему
	if (!m_bAttackRat) {
		if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.time != m_dwCurrentTime)) {
			m_tAction = ACTION_RUN;
		}
	}

	// Выполнение состояния
	switch (m_tAction) {	
		case ACTION_RUN:		// бежать на врага
			delay = ((m_bAttackRat)? 0: 300);

			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, delay);

			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimRunTurnLeft,eAnimRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
			// если враг крыса под монстром подпрыгнуть и убить
			if (dist < 0.6f) {
				if (!m_dwSuperMeleeStarted)	m_dwSuperMeleeStarted = m_dwCurrentTime;

				if (m_dwSuperMeleeStarted + 600 < m_dwCurrentTime) {
					// прыгнуть
					pMonster->Motion.m_tSeq.Add(eAnimAttackJump,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
					pMonster->Motion.m_tSeq.Switch();
					m_dwSuperMeleeStarted = 0;
				}
			} else m_dwSuperMeleeStarted = 0;


			// Смотреть на врага 
			float yaw, pitch;
			yaw = pMonster->r_torso_target.yaw;

			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);

			pMonster->AI_Path.TravelPath.clear();

			Fvector dir;
			dir.sub(m_tEnemy.obj->Position(), pMonster->Position());
			dir.getHP(yaw,pitch);
			yaw *= -1;
			yaw = angle_normalize(yaw);

			DO_IN_TIME_INTERVAL_END();

			// set motion params
			if (m_bAttackRat) pMonster->Motion.m_tParams.SetParams(eAnimAttackRat,0,pMonster->m_ftrRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			else pMonster->Motion.m_tParams.SetParams(eAnimAttack,0,pMonster->m_ftrRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			pMonster->Motion.m_tTurn.Set(eAnimFastTurn, eAnimFastTurn, 0, pMonster->m_ftrAttackFastRSpeed,pMonster->m_ftrRunAttackMinAngle);
			break;
	}
}
