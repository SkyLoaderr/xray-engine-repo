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

	m_dwTimeWalkingPath  = 0;

	time_start_walk_away = 0;


	once_flag_1 = once_flag_1 = false;

	ThreatenTimeStarted = 0;
	
	RebuildPathInterval = 0;
	bEnableBackAttack	= true;

	m_tSubAction = ACTION_WALK_END_PATH;
	
	bAngrySubStateActive = false;

	search_vertex_id = u32(-1);
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
//	if (pMonster->ObjectNotReachable(m_tEnemy.obj) && (m_tAction != ACTION_ATTACK_MELEE)) {
//		if (!bAngrySubStateActive) {
//			m_tSubAction = ACTION_WALK_END_PATH;
//			bAngrySubStateActive = true;
//		}
//		
//		m_tAction = ACTION_THREATEN2;
//		WalkAngrySubState();
//	} else {
//		bAngrySubStateActive = false;
//		LOG_EX2("Object Not Reachable = [%u]", *"*/ pMonster->ObjectNotReachable(m_tEnemy.obj) /*"*);
//		LOG_EX2("Action != ACTION_ATTACK_MELEE = [%u]", *"*/ (m_tAction != ACTION_ATTACK_MELEE) /*"*);
//	}
	if (pMonster->ObjectNotReachable(m_tEnemy.obj) && (m_tAction != ACTION_ATTACK_MELEE)) {
		
		// Try to find best node nearest to the point
		xr_vector<u32> nodes;
		ai().graph_engine().search( ai().level_graph(), m_tEnemy.obj->level_vertex_id(), m_tEnemy.obj->level_vertex_id(), &nodes, CGraphEngine::CFlooder(m_fDistMax-0.5f));
		if (nodes.empty()) m_tAction = ACTION_WALK_ANGRY_AROUND;
		else {
			u32 nearest_node_id = u32(-1);
			float dist_nearest = flt_max;

			for (u32 i=0; i<nodes.size(); i++) {
				float cur_dist = ai().level_graph().vertex_position(nodes[i]).distance_to(m_tEnemy.obj->Position());
				if (cur_dist < dist_nearest) {
					dist_nearest	= cur_dist;
					nearest_node_id = nodes[i];
				}
			}

			R_ASSERT(nearest_node_id != u32(-1));

			m_tAction = ACTION_WALK_AWAY;
			pMonster->MoveToTarget(ai().level_graph().vertex_position(nearest_node_id), nearest_node_id, pMonster->eVelocityParameterRunNormal | pMonster->eVelocityParameterStand, pMonster->eVelocityParameterRunNormal);
			
		}
	}

	//bool bNeedRebuildPath = false;
	
	if (m_tAction != ACTION_ATTACK_MELEE) bEnableBackAttack = true;

//	// -------------------------------------------------------------------------------------------
//	// если враг потер€н из вида
//	if (m_tEnemy.time != m_dwCurrentTime) {
//		if (!bSearchEnemy) {	// инициализаци€ состо€ни€ только если потер€л на рассто€нии
//			if (m_tEnemy.position.distance_to(pMonster->Position()) > 5.0f) {
//				m_tAction = ACTION_SEARCH_ENEMY_INIT;
//
//				Fvector v;
//				v.sub(ai().level_graph().vertex_position(m_tEnemy.node_id),pMonster->Position());
//				v.normalize();
//				Fvector next_pos;
//				next_pos.mad(ai().level_graph().vertex_position(m_tEnemy.node_id),v,5.f);
//				pMonster->HDebug->L_Add(next_pos,D3DCOLOR_XRGB(0,0,255));
//
//				search_vertex_id = ai().level_graph().check_position_in_direction(m_tEnemy.node_id, m_tEnemy.position, next_pos);
//				if (search_vertex_id == u32(-1)) search_vertex_id = m_tEnemy.node_id;
//
//				pMonster->HDebug->M_Add(0,"INIT !!! SEARCH ENEMY ",D3DCOLOR_XRGB(255,0,0));
//				pMonster->HDebug->L_Add(ai().level_graph().vertex_position(search_vertex_id),D3DCOLOR_XRGB(255,0,0));
//			} 
//		} else {				// continue
//			m_tAction = ACTION_SEARCH_ENEMY;
//			pMonster->HDebug->M_Add(0,"SEARCH ENEMY",D3DCOLOR_XRGB(255,0,128));
//		}
//	} else { 
//		bSearchEnemy = false;
//		pMonster->HDebug->M_Clear();
//		pMonster->HDebug->L_Clear();
//	}
//	
//	if ((m_tAction == ACTION_SEARCH_ENEMY) || (m_tAction == ACTION_SEARCH_ENEMY_INIT)) {
//		SearchEnemy();
//	}
//	// -------------------------------------------------------------------------------------------


	// ¬ыполнение состо€ни€
	switch (m_tAction) {	
		case ACTION_RUN:		 // бежать на врага
			LOG_EX("ATTACK: RUN");

			DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,500);
				pMonster->MoveToTarget(m_tEnemy.obj, pMonster->eVelocityParameterRunNormal | pMonster->eVelocityParameterStand, pMonster->eVelocityParameterRunNormal);
			DO_IN_TIME_INTERVAL_END();

			pMonster->MotionMan.m_tAction = ACT_RUN;
			break;
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
			LOG_EX("ATTACK: ATTACK_MELEE");
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
			LOG_EX("ATTACK: STEAL");
			if (dist < (m_fDistMax + 2.f)) bEnemyDoesntSeeMe = false;
			pMonster->MoveToTarget(m_tEnemy.obj, pMonster->eVelocityParamsSteal, pMonster->eVelocityParameterSteal);
			pMonster->MotionMan.m_tAction = ACT_STEAL;
			break;
		
		case ACTION_THREATEN: 
			LOG_EX("ATTACK: THREATEN");
			pMonster->enable_movement(false);

			// —мотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
				pMonster->FaceTarget(m_tEnemy.obj);
			DO_IN_TIME_INTERVAL_END();

			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			pMonster->MotionMan.SetSpecParams(ASP_THREATEN);
			break;


//		case ACTION_WALK_AWAY:
//			pMonster->MotionMan.m_tAction = ACT_RUN;
//			if (pMonster->MotionStats->is_good_motion(3)) break;
//	
//		case ACTION_WALK_ANGRY_AROUND:
//			LOG_EX("ATTACK: WALK_ANGRY_AROUND");
//			
//			DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,3000);
//				bNeedRebuildPath = true;
//			DO_IN_TIME_INTERVAL_END();
//
//			if (!pMonster->MotionStats->is_good_motion(5) || !pMonster->IsMovingOnPath()) {
//				bNeedRebuildPath = true;
//			}
//
//			if (bNeedRebuildPath) {
//				xr_vector<u32> nodes;
//				ai().graph_engine().search( ai().level_graph(), m_tEnemy.obj->level_vertex_id(), m_tEnemy.obj->level_vertex_id(), &nodes, CGraphEngine::CFlooder(10.f));
//
//				u32 vertex_id = nodes[::Random.randI(nodes.size())];
//
//				pMonster->MoveToTarget(
//					ai().level_graph().vertex_position(vertex_id),
//					vertex_id,
//					pMonster->eVelocityParamsWalk,
//					pMonster->eVelocityParameterWalkNormal | pMonster->eVelocityParameterStand			
//				);
//			}
//
//			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
//			break;

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
//	// 1. дойти до конца пути
//	// 2. смотреть на цель
//	// 3. пугать
//	// 4. построить путь не далеко
//	// 5. goto 1
//	
//	switch (m_tSubAction) {
//	case ACTION_WALK_END_PATH:
//	
//		LOG_EX("ATTACK: WALK_END_PATH");
//		pMonster->HDebug->M_Add(0,"STATE0: WALK_END_PATH",D3DCOLOR_XRGB(255,0,128));
//
//		if (pMonster->IsMovingOnPath()) {
//			DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,500);
//				pMonster->MoveToTarget(m_tEnemy.obj, pMonster->eVelocityParamsWalk, pMonster->eVelocityParameterWalkNormal | pMonster->eVelocityParameterStand);
//			DO_IN_TIME_INTERVAL_END();
//
//			pMonster->MotionMan.m_tAction = ACT_RUN;
//		} else {
//			m_tSubAction = ACTION_FACE_ENEMY;
//		}
//
//		if (!pMonster->MotionStats->is_good_motion(3)) {
//			m_tSubAction = ACTION_FACE_ENEMY;
//		}
//			
//		break;
//	case ACTION_FACE_ENEMY:
//		LOG_EX("ATTACK: FACE_ENEMY");
//		pMonster->HDebug->M_Add(0,"STATE1: FACE_ENEMY",D3DCOLOR_XRGB(255,0,128));		
//		pMonster->enable_movement(false);
//
//		// —мотреть на врага 
//		DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
//			pMonster->FaceTarget(m_tEnemy.obj);
//		DO_IN_TIME_INTERVAL_END();
//
//		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
//			
//		if (angle_difference(pMonster->m_body.target.yaw,pMonster->m_body.current.yaw) < PI_DIV_6/6) {
//			ThreatenTimeStarted = m_dwCurrentTime;
//			m_tSubAction = ACTION_THREATEN2;
//		}
//
//		break;
//	case ACTION_THREATEN2:
//		LOG_EX("ATTACK: THREATEN");
//		pMonster->HDebug->M_Add(0,"STATE2: THREATEN",D3DCOLOR_XRGB(255,0,128));		
//		pMonster->enable_movement(false);
//			
//		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
//		pMonster->MotionMan.SetSpecParams(ASP_THREATEN);
//
//		if (ThreatenTimeStarted + 2000 < m_dwCurrentTime) {
//			
//			xr_vector<u32> nodes;
//			ai().graph_engine().search( ai().level_graph(), m_tEnemy.obj->level_vertex_id(), m_tEnemy.obj->level_vertex_id(), &nodes, CGraphEngine::CFlooder(10.f));
//			
//			u32 vertex_id = nodes[::Random.randI(nodes.size())];
//
//			pMonster->MoveToTarget(
//				ai().level_graph().vertex_position(vertex_id),
//				vertex_id,
//				pMonster->eVelocityParamsWalk,
//				pMonster->eVelocityParameterWalkNormal | pMonster->eVelocityParameterStand			
//			);
//
//			m_tSubAction		= ACTION_WALK_AWAY;
//		}
//
//		break;
//
//	case ACTION_WALK_AWAY:
//		LOG_EX("ATTACK: WALK_AWAY");
//		pMonster->HDebug->M_Add(0,"STATE3: WALK_AWAY",D3DCOLOR_XRGB(255,0,128));		
//
//		if (pMonster->IsMovingOnPath()) pMonster->MotionMan.m_tAction = ACT_RUN;
//		else m_tSubAction = ACTION_FACE_ENEMY;
//
//		if (!pMonster->MotionStats->is_good_motion(3)) {
//			m_tSubAction = ACTION_FACE_ENEMY;
//		}
//
//		break;
//	}

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

void CBitingAttack::SearchEnemy()
{
	bool bNeedRebuildPath = false;

	switch (m_tAction) {
	case ACTION_SEARCH_ENEMY_INIT: 

		LOG_EX("ATTACK: SEARCH_ENEMY_INIT");

		DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,1000);
			pMonster->MoveToTarget(ai().level_graph().vertex_position(search_vertex_id),search_vertex_id, pMonster->eVelocityParameterRunNormal | pMonster->eVelocityParameterStand, pMonster->eVelocityParameterRunNormal);
		DO_IN_TIME_INTERVAL_END();
						
		pMonster->MotionMan.m_tAction = ACT_RUN;

		if (!pMonster->IsMovingOnPath()) {
			m_tAction = ACTION_SEARCH_ENEMY;
		}

		break;
	case ACTION_SEARCH_ENEMY:
		// search_vertex_id - содержит исходную ноду
		LOG_EX("ATTACK: SEARCH_ENEMY");

		//if (pMonster->IsMovingOnPath()) bNeedRebuildPath = true;
		
		DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,3000);
			bNeedRebuildPath = true;
		DO_IN_TIME_INTERVAL_END();

		if (!pMonster->MotionStats->is_good_motion(5) || !pMonster->IsMovingOnPath()) {
			bNeedRebuildPath = true;
		}

		if (bNeedRebuildPath) {
			xr_vector<u32> nodes;
			ai().graph_engine().search( ai().level_graph(), search_vertex_id, search_vertex_id, &nodes, CGraphEngine::CFlooder(10.f));

			u32 vertex_id = nodes[::Random.randI(nodes.size())];

			pMonster->MoveToTarget(
				ai().level_graph().vertex_position(vertex_id),
				vertex_id,
				pMonster->eVelocityParamsWalk,
				pMonster->eVelocityParameterWalkNormal | pMonster->eVelocityParameterStand			
			);
		}

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		break;
	}
}



