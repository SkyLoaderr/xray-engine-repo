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

	// �������� �����
	m_tEnemy = pMonster->m_tEnemy;

	// ����������� ������ �����
	const CAI_Rat	*tpRat = dynamic_cast<const CAI_Rat *>(m_tEnemy.obj);
	if (tpRat) m_bAttackRat = true;
	else m_bAttackRat = false;

	// ��������� ��������� ������
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
#define REAL_DIST_THROUGH_TRACE_THRESHOLD	6.0f

void CBitingAttack::Run()
{
	// ���� ���� ���������, ���������������� ���������
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	// for attack stops needed
	if (!m_bAttackRat) {
		m_fDistMin = pMonster->m_fCurMinAttackDist;
		m_fDistMax = pMonster->_sd->m_fMaxAttackDist - (pMonster->_sd->m_fMinAttackDist - pMonster->m_fCurMinAttackDist);
	}

	// ����� ���������
	bool bAttackMelee = (ACTION_ATTACK_MELEE == m_tAction);

	// ���������� ���������� �� ����������
	dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());
	if (dist < REAL_DIST_THROUGH_TRACE_THRESHOLD) dist = pMonster->GetRealDistToEnemy();

	if (bAttackMelee && (dist < m_fDistMax)) m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// ���� ���� �� ����� �� ���������� 1 ��� - ������ � ����
	if (ACTION_ATTACK_MELEE == m_tAction && (m_tEnemy.time + 1000 < m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}

	// ��������� �� ����������� ������
	CJumping *pJumping = dynamic_cast<CJumping *>(pMonster);
	if (pJumping) pJumping->Check(pMonster->Position(),m_tEnemy.obj->Position(),m_tEnemy.obj);
	
	if (pMonster->m_PhysicMovementControl.JumpState()) {
		pMonster->enable_movement(false);
		return;
	}

	// ��������� �� ����������� �������������
//	if (!m_bAttackRat) {
//		if ((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) != FLAG_ENEMY_DOESNT_SEE_ME) bEnemyDoesntSeeMe = false;
//		if (((pMonster->flagsEnemy & FLAG_ENEMY_GO_FARTHER_FAST) == FLAG_ENEMY_GO_FARTHER_FAST) && (m_dwStateStartedTime + 4000 < m_dwCurrentTime)) bEnemyDoesntSeeMe = false;
//		if ((ACTION_RUN == m_tAction) && bEnemyDoesntSeeMe) m_tAction = ACTION_STEAL;
//	}

	// ��������� �� ����������� �������
	if (CheckThreaten()) m_tAction = ACTION_THREATEN;

	// ���������, �������� �� ���������
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
			pMonster->MoveToTarget(ai().level_graph().vertex_position(nearest_node_id), nearest_node_id);
			
		}
	}

	if (m_tAction != ACTION_ATTACK_MELEE) bEnableBackAttack = true;

//	// -------------------------------------------------------------------------------------------
//	// ���� ���� ������� �� ����
//	if (m_tEnemy.time != m_dwCurrentTime) {
//		if (!bSearchEnemy) {	// ������������� ��������� ������ ���� ������� �� ����������
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

	bool bNeedRebuild = false;

	float max_build_dist = 15.f;

	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_RUN:		 // ������ �� �����
			LOG_EX("ATTACK: RUN");
			 pMonster->MotionMan.m_tAction = ACT_RUN;

			DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,500 + 50.f * dist);
				bNeedRebuild = true; 
			DO_IN_TIME_INTERVAL_END();
			
			if (IS_NEED_REBUILD()) bNeedRebuild = true;
			if (bNeedRebuild) {
				
				if (dist < max_build_dist) {
					CMonsterSquad	*pSquad = Level().SquadMan.GetSquad((u8)pMonster->g_Squad());
					TTime			last_updated;
					Fvector			target = pSquad->GetTargetPoint(pMonster, last_updated);

					if (last_updated !=0 ) {
						pMonster->Path_ApproachPoint(0, target);
						pMonster->m_tSelectorApproach->m_tEnemyPosition = target;
						pMonster->m_tSelectorApproach->m_tEnemy			= 0;

						pMonster->SetPathParams(pMonster->level_vertex_id(), pMonster->Position()); 
					} else {
						pMonster->MoveToTarget(m_tEnemy.obj);
					}
				}
				else {
					// find_nearest_point_to_point
					Fvector pos;
					Fvector dir;
					dir.sub(m_tEnemy.obj->Position(), pMonster->Position()); 
					dir.normalize();
					pos.mad(pMonster->Position(), dir, 15.f);
					pMonster->Path_ApproachPoint(0, pos);
					pMonster->SetPathParams(pMonster->level_vertex_id(), pMonster->Position()); 
				}
			}

			break;
		case ACTION_ATTACK_MELEE:		// ��������� ��������
			LOG_EX("ATTACK: ATTACK_MELEE");
			pMonster->MotionMan.m_tAction = ACT_ATTACK;
			pMonster->enable_movement(false);			
			bCanThreaten			= false;

			// ���� ���� ����� ��� �������� ����������� � �����
//			if (m_bAttackRat) {
//				if (dist < 0.6f) {
//					if (!m_dwSuperMeleeStarted)	m_dwSuperMeleeStarted = m_dwCurrentTime;
//					if (m_dwSuperMeleeStarted + 600 < m_dwCurrentTime) {
//						// ��������
//						pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT_JUMP);
//						m_dwSuperMeleeStarted = 0;
//					}
//				} else m_dwSuperMeleeStarted = 0;
//			}

			// �������� �� ����� 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
				pMonster->FaceTarget(m_tEnemy.obj);
			DO_IN_TIME_INTERVAL_END();
			
			if (CanAttackFromBack() && bEnableBackAttack) {
				pMonster->MotionMan.SetSpecParams(ASP_BACK_ATTACK);
				bEnableBackAttack = false;
			}

			if (m_bAttackRat) pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT);

			break;
		case ACTION_STEAL:
			LOG_EX("ATTACK: STEAL");
			pMonster->MotionMan.m_tAction = ACT_STEAL;
			if (dist < (m_fDistMax + 2.f)) bEnemyDoesntSeeMe = false;
			pMonster->MoveToTarget(m_tEnemy.obj);
			break;
		
		case ACTION_THREATEN: 
			LOG_EX("ATTACK: THREATEN");
			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			pMonster->enable_movement(false);

			// �������� �� ����� 
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
	}

	pMonster->SetSound(SND_TYPE_ATTACK, pMonster->_sd->m_dwAttackSndDelay);

#pragma todo("Jim to Jim: fix nesting: Bloodsucker in Biting state")
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
}

#define THREATEN_TIME  2300

// ���������� �������:
// ������ - ���������, ���������� - ���������� ��� ������
// �� ���������� N ��� �� ��� �������� ���� ��������
// ���� ����� �� �����, �����
// ���� ��������� ATTACK_MELEE ��� �� ����
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
//	// 1. ����� �� ����� ����
//	// 2. �������� �� ����
//	// 3. ������
//	// 4. ��������� ���� �� ������
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
//		// �������� �� ����� 
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
	// ��������� ���� ���� ��������� �����
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
		pMonster->MotionMan.m_tAction = ACT_RUN;

		DO_IN_TIME_INTERVAL_BEGIN(RebuildPathInterval,1000);
			pMonster->MoveToTarget(ai().level_graph().vertex_position(search_vertex_id),search_vertex_id);
		DO_IN_TIME_INTERVAL_END();
						
		
		if (!pMonster->IsMovingOnPath()) {
			m_tAction = ACTION_SEARCH_ENEMY;
		}

		break;
	case ACTION_SEARCH_ENEMY:
		// search_vertex_id - �������� �������� ����
		LOG_EX("ATTACK: SEARCH_ENEMY");
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

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

			pMonster->MoveToTarget(ai().level_graph().vertex_position(vertex_id),vertex_id);
		}

		break;
	}
}



