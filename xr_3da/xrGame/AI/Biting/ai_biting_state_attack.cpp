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

	SetPriority(PRIORITY_HIGH);
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
	LOG_EX("attack init");


	IState::Init();

	// ѕолучить врага
	m_tEnemy = pMonster->m_tEnemy;

	// ќпределение класса врага
	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(m_tEnemy.obj);
	if (tpRat) m_bAttackRat = true;
	else m_bAttackRat = false;

	// ”становка дистанции аттаки
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

	temp_pos.set		(0.f,0.f,1.f);
	m_dwTimeWalkingPath  = 0;

	time_start_walk_away = 0;


	once_flag_1 = once_flag_1 = false;

	ThreatenTimeStarted = 0;
	
	RebuildPathInterval = 0;
	bEnableBackAttack	= true;

	m_tSubAction = ACTION_WALK_END_PATH;
	
	bAngrySubStateActive = false;

}

#define TIME_WALK_PATH 5000

void CBitingAttack::Run()
{
	// ≈сли враг изменилс€, инициализировать состо€ние
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	// for attack stops needed
	if (!m_bAttackRat) {
		m_fDistMin = pMonster->m_fCurMinAttackDist;
		m_fDistMax = pMonster->_sd->m_fMaxAttackDist - (pMonster->_sd->m_fMinAttackDist - pMonster->m_fCurMinAttackDist);
	}

	// ¬ыбор состо€ни€
	bool bAttackMelee = (ACTION_ATTACK_MELEE == m_tAction);

	dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());

	if (bAttackMelee && (dist < m_fDistMax)) m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// если враг не виден на прот€жении 1 сек - бежать к нему
	if (ACTION_ATTACK_MELEE == m_tAction && (m_tEnemy.time + 1000 < m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}

	// проверить на возможность прыжка
	CJumping *pJumping = dynamic_cast<CJumping *>(pMonster);
	if (pJumping) pJumping->Check(pMonster->Position(),m_tEnemy.obj->Position(),m_tEnemy.obj);
	
	if (pMonster->m_PhysicMovementControl.JumpState()) {
		pMonster->enable_movement(false);
		return;
	}

	// проверить на возможность подкрадывани€
	if (!m_bAttackRat) {
		if ((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) != FLAG_ENEMY_DOESNT_SEE_ME) bEnemyDoesntSeeMe = false;
		if (((pMonster->flagsEnemy & FLAG_ENEMY_GO_FARTHER_FAST) == FLAG_ENEMY_GO_FARTHER_FAST) && (m_dwStateStartedTime + 4000 < m_dwCurrentTime)) bEnemyDoesntSeeMe = false;
		if ((ACTION_RUN == m_tAction) && bEnemyDoesntSeeMe) m_tAction = ACTION_STEAL;
	}

	// проверить на возможность пугани€
	if (CheckThreaten()) m_tAction = ACTION_THREATEN;

	// ѕроверить, достижим ли противник
	if (pMonster->ObjectNotReachable(m_tEnemy.obj) && (m_tAction != ACTION_ATTACK_MELEE)) {
		
		if (!bAngrySubStateActive) {
			m_tSubAction = ACTION_WALK_END_PATH;
			bAngrySubStateActive = true;
		}
		
		SubActionStartTime	= m_dwCurrentTime;
		WalkAngrySubState();

	} else bAngrySubStateActive = false;


	
	if (m_tAction != ACTION_ATTACK_MELEE) bEnableBackAttack = true;

	// ¬ыполнение состо€ни€
	switch (m_tAction) {	
		case ACTION_RUN:		 // бежать на врага
			
			DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,500);
				pMonster->MoveToTarget(m_tEnemy.obj, pMonster->eVelocityParameterRunNormal | pMonster->eVelocityParameterStand, pMonster->eVelocityParameterRunNormal);
			DO_IN_TIME_INTERVAL_END();

			pMonster->MotionMan.m_tAction = ACT_RUN;
			break;
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
			pMonster->enable_movement(false);			
			bCanThreaten			= false;

			// если враг крыса под монстром подпрыгнуть и убить
//			if (m_bAttackRat) {
//				if (dist < 0.6f) {
//					if (!m_dwSuperMeleeStarted)	m_dwSuperMeleeStarted = m_dwCurrentTime;
//					if (m_dwSuperMeleeStarted + 600 < m_dwCurrentTime) {
//						// прыгнуть
//						pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT_JUMP);
//						m_dwSuperMeleeStarted = 0;
//					}
//				} else m_dwSuperMeleeStarted = 0;
//			}

			// —мотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
				pMonster->FaceTarget(m_tEnemy.obj);
			DO_IN_TIME_INTERVAL_END();
			
			if (CanAttackFromBack() && bEnableBackAttack) {
				pMonster->MotionMan.SetSpecParams(ASP_BACK_ATTACK);
				bEnableBackAttack = false;
			}

			if (m_bAttackRat) pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT);
			pMonster->MotionMan.m_tAction = ACT_ATTACK;


			break;
		case ACTION_STEAL:
			if (dist < (m_fDistMax + 2.f)) bEnemyDoesntSeeMe = false;
			pMonster->MoveToTarget(m_tEnemy.obj, pMonster->eVelocityParamsSteal, pMonster->eVelocityParameterSteal);
			pMonster->MotionMan.m_tAction = ACT_STEAL;
			break;
		
		case ACTION_THREATEN: 
			pMonster->enable_movement(false);

			// —мотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
				pMonster->FaceTarget(m_tEnemy.obj);
			DO_IN_TIME_INTERVAL_END();

			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			pMonster->MotionMan.SetSpecParams(ASP_THREATEN);
			break;

		case ACTION_WALK_ANGRY_AROUND:

			pMonster->enable_movement(false);
			pMonster->LookPosition(m_tEnemy.obj->Position(), PI_DIV_2);
			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			break;
	}

	pMonster->SetSound(SND_TYPE_ATTACK, pMonster->_sd->m_dwAttackSndDelay);

#pragma todo("Jim to Jim: fix nesting: Bloodsucker in Biting state")
	if (m_bInvisibility && ACTION_THREATEN != m_tAction) {
		CAI_Bloodsucker *pBS =	dynamic_cast<CAI_Bloodsucker *>(pMonster);
		CActor			*pA  =  dynamic_cast<CActor*>(Level().CurrentEntity());

		bool			bActorIsEnemy = (dynamic_cast<CActor*>(m_tEnemy.obj) != 0);	// set !=0 to work

		if (pBS && pA && bActorIsEnemy && (pA->Position().distance_to(pBS->Position()) < pBS->m_fEffectDist)) {
			if ((dist < pBS->m_fInvisibilityDist) && (pBS->GetPower() > pBS->m_fPowerThreshold)) {
				if (pBS->CMonsterInvisibility::Switch(false)) {
					pBS->ChangePower(pBS->m_ftrPowerDown);
					pBS->ActivateEffector(pBS->CMonsterInvisibility::GetInvisibleInterval() / 1000.f);
				}
			}
		}
	}
}


void CBitingAttack::Done()
{
	inherited::Done();

	pMonster->AS_Stop();
}

#define THREATEN_TIME  2300

// –еализаци€ пугани€:
// мораль - маленька€, рассто€ние - рассто€ние дл€ аттаки
// на прот€жении N мин не был атакован этим монстром
// враг стоит не бежит, видит
// если состо€ни€ ATTACK_MELEE ещЄ не было
bool CBitingAttack::CheckThreaten()
{
	if (pMonster->GetEntityMorale() > 0.8f) {
		return false;
	}

	if (((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) == FLAG_ENEMY_DOESNT_SEE_ME) || 
		((pMonster->flagsEnemy & FLAG_ENEMY_GO_FARTHER_FAST) == FLAG_ENEMY_GO_FARTHER_FAST)) {
		return false;
	}

	if ((dist < m_fDistMin) || (dist > m_fDistMax)) {
		return false;
	}
	if (pMonster->IsDangerousEnemy(m_tEnemy.obj)) {
		return false;
	}

	if (!bCanThreaten) {
		return false;
	}

	if (ThreatenTimeStarted == 0) ThreatenTimeStarted = m_dwCurrentTime;
	if (ThreatenTimeStarted + THREATEN_TIME < m_dwCurrentTime) return false;

	return true;
}

Fvector CBitingAttack::RandomPos(Fvector pos, float R)
{
	Fvector local;

	local = pos;
	local.x += ::Random.randF(-R,R);
	local.z += ::Random.randF(-R,R);

	return local;
}

void CBitingAttack::WalkAngrySubState()
{
	
	// 1. дойти до конца пути
	// 2. смотреть на цель
	// 3. пугать
	// 4. построить путь не далеко
	// 5. goto 1
	
	switch (m_tSubAction) {
	case ACTION_WALK_END_PATH:

		if (pMonster->IsMovingOnPath()) {
			DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,500);
				pMonster->MoveToTarget(m_tEnemy.obj, pMonster->eVelocityParameterRunNormal | pMonster->eVelocityParameterStand, pMonster->eVelocityParameterRunNormal);
			DO_IN_TIME_INTERVAL_END();

			pMonster->MotionMan.m_tAction = ACT_RUN;
		} else {
			m_tSubAction = ACTION_FACE_ENEMY;
		}

		if (pMonster->IsObstacle(1000)) {
			m_tSubAction = ACTION_FACE_ENEMY;
		}
			
		break;
	case ACTION_FACE_ENEMY:
		
		pMonster->enable_movement(false);

		// —мотреть на врага 
		DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
			pMonster->FaceTarget(m_tEnemy.obj);
		DO_IN_TIME_INTERVAL_END();

		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			
		if (angle_difference(pMonster->m_body.target.yaw,pMonster->m_body.current.yaw) < PI_DIV_6/6) {
			ThreatenTimeStarted = m_dwCurrentTime;
			m_tSubAction = ACTION_THREATEN2;
		}
		break;
	case ACTION_THREATEN2:
		pMonster->enable_movement(false);
			
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		pMonster->MotionMan.SetSpecParams(ASP_THREATEN);

		if (ThreatenTimeStarted + 2500 < m_dwCurrentTime) {
			
			xr_vector<u32> nodes;
			ai().graph_engine().search( ai().level_graph(), pMonster->level_vertex_id(), pMonster->level_vertex_id(), &nodes, CGraphEngine::CFlooder(10.f));
			
			u32 vertex_id = nodes[::Random.randI(nodes.size())];

			pMonster->MoveToTarget(
				ai().level_graph().vertex_position(vertex_id),
				vertex_id,
				pMonster->eVelocityParameterRunNormal | pMonster->eVelocityParameterStand,
				pMonster->eVelocityParameterRunNormal
			);

			m_tSubAction		= ACTION_WALK_AWAY;
			SubActionStartTime	= m_dwCurrentTime;
		}

		break;

	case ACTION_WALK_AWAY:
		if (pMonster->IsMovingOnPath()) pMonster->MotionMan.m_tAction = ACT_RUN;
		else m_tSubAction = ACTION_FACE_ENEMY;

		if (pMonster->IsObstacle(1000)) {
			m_tSubAction = ACTION_FACE_ENEMY;
		}

		break;
	}
}

bool CBitingAttack::CanAttackFromBack()
{
	// проверить если враг находитс€ сзади
	Fvector dir;
	dir.sub(m_tEnemy.obj->Position(), pMonster->Position());

	float yaw1,p1,yaw2,p2;
	dir.getHP(yaw1,p1);
	pMonster->Direction().getHP(yaw2,p2);
	float dif = angle_difference(yaw1,yaw2);

	if ((PI - PI_DIV_6 < dif) && (dif < PI + PI_DIV_6)) return true;
	
	return false;
}


//void StartBadMotionCheck()
//{
//	TTime time_start = m_dwCurrentTime;
//	bool b = false;
//	Fvector pos;
//	
//
//}




