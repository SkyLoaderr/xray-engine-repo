#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "../rat/ai_rat.h"
#include "../bloodsucker/ai_bloodsucker.h"
#include "../../actor.h"
#include "../ai_monster_jump.h"
#include "../ai_monster_group.h"

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
	const CAI_Rat	*tpRat = dynamic_cast<const CAI_Rat *>(m_tEnemy.obj);
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
	
	EnemySavedPos = m_tEnemy.obj->Position();
	
}

#define TIME_WALK_PATH						5000
#define REAL_DIST_THROUGH_TRACE_THRESHOLD	6.0f
#define PREDICT_POSITION_MIN_THRESHOLD		6.0f		// мин дистанци€ при которой 	
#define BUILD_FULL_PATH_MAX_DIST			10.0f		// макс дистанци€ до врага, при которой будет строитс€ полный путь
#define BUILD_HALF_PATH_DIST				5.f			// дистанци€ не полного пути

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

	// определить рассто€ние до противника
	dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());
	if (dist < REAL_DIST_THROUGH_TRACE_THRESHOLD) dist = pMonster->GetRealDistToEnemy();

	if (bAttackMelee && (dist < m_fDistMax)) m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// если враг не виден на прот€жении 1 сек - бежать к нему
	if (ACTION_ATTACK_MELEE == m_tAction && (m_tEnemy.time + 1000 < m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}

	// проверить на возможность пугани€
	if (CheckThreaten()) m_tAction = ACTION_THREATEN;

	// проверить на возможность прыжка
	CJumping *pJumping = dynamic_cast<CJumping *>(pMonster);
	if (pJumping && (m_tAction != ACTION_THREATEN)) pJumping->Check(pMonster->Position(),m_tEnemy.obj->Position(),m_tEnemy.obj);
	
	if (pMonster->m_PhysicMovementControl.JumpState()) {
		pMonster->enable_movement(false);
		return;
	}

	// проверить на возможность подкрадывани€
	if (!m_bAttackRat && (pMonster->GetEnemyNumber()==1)) {
		if ((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) != FLAG_ENEMY_DOESNT_SEE_ME) bEnemyDoesntSeeMe = false;
		if (((pMonster->flagsEnemy & FLAG_ENEMY_GO_FARTHER_FAST) == FLAG_ENEMY_GO_FARTHER_FAST) && (m_dwStateStartedTime + 4000 < m_dwCurrentTime)) bEnemyDoesntSeeMe = false;
		if ((ACTION_RUN == m_tAction) && bEnemyDoesntSeeMe) m_tAction = ACTION_STEAL;
	}

	// ѕроверить, достижим ли противник
	if (pMonster->ObjectNotReachable(m_tEnemy.obj) && (m_tAction != ACTION_ATTACK_MELEE)) m_tAction = ACTION_ENEMY_POSITION_APPROACH;

	if (m_tAction != ACTION_ATTACK_MELEE) bEnableBackAttack = true;

	bool bNeedRebuild = false;

	// ¬ыполнение состо€ни€
	switch (m_tAction) {	
		
		// ************
		case ACTION_RUN:		 // бежать на врага
		// ************	
				
			LOG_EX("ATTACK: RUN");
			pMonster->MotionMan.m_tAction	= ACT_RUN;
			pMonster->b_try_min_time		= false;

			DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,200 + 100.f * dist);
				bNeedRebuild = true; 
			DO_IN_TIME_INTERVAL_END();
			if (IS_NEED_REBUILD()) bNeedRebuild = true;
			
			
			if (bNeedRebuild) {
				// ѕолучить позицию, определЄнную груп. интелл.
				CMonsterSquad	*pSquad = Level().SquadMan.GetSquad((u8)pMonster->g_Squad());
				TTime			squad_ai_last_updated;
				Fvector			target = pSquad->GetTargetPoint(pMonster, squad_ai_last_updated);
				ESquadAttackAlg alg_type = pSquad->GetAlgType();
				
				bool squad_target_selected = false;
				
				if (alg_type == SAA_DEVIATION) {
					if (squad_ai_last_updated !=0 ) {		// нова€ позици€
						pMonster->Path_ApproachPoint(target);
						pMonster->SetPathParams(pMonster->level_vertex_id(), pMonster->Position()); 
						squad_target_selected = true;
						pMonster->set_use_dest_orientation	(false);
					}
				} else {
					if (dist < BUILD_FULL_PATH_MAX_DIST) {
						pMonster->set_dest_direction		(target);
						pMonster->set_use_dest_orientation	(true);
						

						pMonster->MoveToTarget(m_tEnemy.obj);
						squad_target_selected = true;
					}
				}
				
				if (!squad_target_selected) {
					
					pMonster->set_use_dest_orientation	(false);

					// ¬раг далеко? —троить полный путь?
					if (dist < BUILD_FULL_PATH_MAX_DIST) {
						// squad_ai выбрал оптимальную позицию, соответствующую позиции врага	
						
						// использовать ли предсказание позиции
						Fvector dir;
						dir.sub(m_tEnemy.obj->Position(),EnemySavedPos);

						if ((dist > PREDICT_POSITION_MIN_THRESHOLD) && (dir.square_magnitude() > 1)) {
							Fvector target_point;
							target_point.add(m_tEnemy.obj->Position(),dir);
							pMonster->Path_ApproachPoint(target_point);
							pMonster->SetPathParams(pMonster->level_vertex_id(), pMonster->Position()); 
						} else {
							pMonster->MoveToTarget(m_tEnemy.obj);
						}
					} else { // построить неполный путь на рассто€ние BUILD_HALF_PATH_DIST
						Fvector pos;
						Fvector dir;
						dir.sub(m_tEnemy.obj->Position(), pMonster->Position()); 
						dir.normalize();
						pos.mad(pMonster->Position(), dir, BUILD_HALF_PATH_DIST);
						pMonster->Path_ApproachPoint(pos);
						pMonster->SetPathParams(pMonster->level_vertex_id(), pMonster->Position()); 
					}
				}
			}

			if (bNeedRebuild) EnemySavedPos = m_tEnemy.obj->Position();

			break;

		// *********************
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
		// *********************

			LOG_EX("ATTACK: ATTACK_MELEE");
			pMonster->MotionMan.m_tAction = ACT_ATTACK;
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

			break;


		// ****************
		case ACTION_STEAL:
		// ****************


			LOG_EX("ATTACK: STEAL");
			pMonster->MotionMan.m_tAction = ACT_STEAL;
			if (dist < (m_fDistMax + 2.f)) bEnemyDoesntSeeMe = false;
			pMonster->MoveToTarget(m_tEnemy.obj);
			pMonster->set_use_dest_orientation	(false);
			break;
		
		
		// ******************
		case ACTION_THREATEN: 
		// ******************

			LOG_EX("ATTACK: THREATEN");
			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			pMonster->enable_movement(false);

			// —мотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
				pMonster->FaceTarget(m_tEnemy.obj);
			DO_IN_TIME_INTERVAL_END();

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

		// **********************************
		case ACTION_ENEMY_POSITION_APPROACH:
		// **********************************
			pMonster->MotionMan.m_tAction = ACT_RUN;

			DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,500 + 50.f * dist);
				bNeedRebuild = true; 
			DO_IN_TIME_INTERVAL_END();
			if (IS_NEED_REBUILD()) bNeedRebuild = true;
			
			if (bNeedRebuild) {
				pMonster->Path_ApproachPoint(m_tEnemy.obj->Position());
				pMonster->SetPathParams(pMonster->level_vertex_id(), pMonster->Position()); 
				pMonster->set_use_dest_orientation	(false);
			}

			break;
	}


	pMonster->SetSound(SND_TYPE_ATTACK, pMonster->_sd->m_dwAttackSndDelay);

	if (m_bInvisibility && ACTION_THREATEN != m_tAction) {
		CAI_Bloodsucker *pBS =	dynamic_cast<CAI_Bloodsucker *>(pMonster);
		CActor			*pA  =  dynamic_cast<CActor*>(Level().CurrentEntity());

		bool			bActorIsEnemy = (dynamic_cast<const CActor*>(m_tEnemy.obj) != 0);	// set !=0 to work

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

	pMonster->set_use_dest_orientation	(false);
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


