#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "../rat/ai_rat.h"
#include "../bloodsucker/ai_bloodsucker.h"
#include "../../actor.h"
#include "../ai_monster_jump.h"
#include "../ai_monster_group.h"


/////////////////////////////////////////////////////////////////////////
// State Attack Flags 

#define AF_ENEMY_DOESNT_SEE_ME			(1 << 0)
#define AF_ENEMY_GO_FARTHER_FAST		(1 << 1)
#define AF_REMEMBER_HIT_FROM_THIS_ENEMY	(1 << 2)
#define AF_THIS_IS_THE_ONLY_ENEMY		(1 << 3)
#define AF_ENEMY_IS_NOT_REACHABLE		(1 << 4)
#define AF_CAN_ATTACK_FROM_BACK			(1 << 5)
#define AF_SEE_ENEMY					(1 << 6)

#define AF_GOOD_MOVEMENT				(1 << 7)
#define AF_NEED_REBUILD_PATH			(1 << 8)

#define AF_ATTACK_RAT					(1 << 9)
#define AF_NEW_ENEMY					(1 << 10)

#define AF_LOW_MORALE					(1 << 11)

#define AF_HAS_JUMP_ABILITY				(1 << 12)
#define AF_HAS_INVISIBILITY_ABILITY		(1 << 13)

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingAttack::CBitingAttack(CAI_Biting *p,  bool bVisibility)  
{
	pMonster = p;
	m_bInvisibility	= bVisibility;

	SetPriority(PRIORITY_HIGH);
}


void CBitingAttack::Init()
{
	LOG_EX("attack init");
	IState::Init();

	// �������� �����
	m_tEnemy = pMonster->m_tEnemy;

	UpdateInitFlags();

	// ��������� ��������� ������
	if (init_flags.is(AF_ATTACK_RAT)) {
		m_fDistMin = 0.7f;				// todo: make as ltx parameters
		m_fDistMax = 2.8f;				// todo: make as ltx parameters
	}
	pMonster->AS_Start();				// ��������� ����-�����

	
	m_dwFaceEnemyLastTime		= 0; 

	b_in_threaten				= false;
	ThreatenTimeStarted			= 0;
	ThreatenMinDelay			= 0;
	LastTimeRebuild				= 0;
	bEnableBackAttack			= true;

	m_tAction					= ACTION_RUN;
	m_tPrevAction				= ACTION_RUN;
}

#define TIME_WALK_PATH						5000
#define REAL_DIST_THROUGH_TRACE_THRESHOLD	6.0f
#define PREDICT_POSITION_MIN_THRESHOLD		6.0f		// ��� ��������� ��� ������� 	
#define BUILD_FULL_PATH_MAX_DIST			10.0f		// ���� ��������� �� �����, ��� ������� ����� �������� ������ ����
#define BUILD_HALF_PATH_DIST				5.f			// ��������� �� ������� ����

#define THREATEN_DISTANCE					3.0f

void CBitingAttack::Run()
{
	
	// ���� ���� ���������, ���������������� ���������
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	
	UpdateFrameFlags();
	flags.or(frame_flags, init_flags.get());

	
	// �������� ����������� � ������������ ��������� �� �����
	if (!flags.is(AF_ATTACK_RAT)) {
		m_fDistMin = pMonster->m_fCurMinAttackDist;
		m_fDistMax = pMonster->_sd->m_fMaxAttackDist - (pMonster->_sd->m_fMinAttackDist - pMonster->m_fCurMinAttackDist);
	}

	// ���������� ���������� �� ����������
	dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());
	if (dist < REAL_DIST_THROUGH_TRACE_THRESHOLD) dist = pMonster->GetRealDistToEnemy();
	
	// ���������� ���������� ������ ���������
	bool b_attack_melee		= false;
	
	if ((m_tPrevAction == ACTION_ATTACK_MELEE) && (dist < m_fDistMax)) m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	if (m_tAction == ACTION_ATTACK_MELEE) b_attack_melee = true; 
	
	
	// ��������� �� ����������� ������� � �������������
	if (CheckThreaten()) m_tAction	= ACTION_THREATEN;
	else if (CheckSteal()) m_tAction = ACTION_STEAL;
	
	
	// ���� ���� �� ����� �� ���������� 1 ��� - ������ � ����
	if (!flags.is(AF_SEE_ENEMY) && (m_tEnemy.time + 1000 < m_dwCurrentTime))	m_tAction = ACTION_RUN;
	
	// ���������, �������� �� ���������
	if (flags.is(AF_ENEMY_IS_NOT_REACHABLE) && !b_attack_melee) m_tAction = ACTION_ENEMY_POSITION_APPROACH;
	
	// ��������� �� ����������� ������
	if (flags.is(AF_HAS_JUMP_ABILITY)) pJumping->Check(pMonster->Position(),m_tEnemy.obj->Position(),m_tEnemy.obj);
	
	// ������������ ��������� ����������
	if (!b_attack_melee) bEnableBackAttack = true;
	
	bool bNeedRebuild = false;

	// ���������� ���������
	switch (m_tAction) {	
		
		// ************
		case ACTION_RUN:		 // ������ �� �����
		// ************	
				
			LOG_EX("ATTACK: RUN");
			pMonster->MotionMan.m_tAction	= ACT_RUN;
			pMonster->CMonsterMovement::set_try_min_time(false);

			DO_IN_TIME_INTERVAL_BEGIN(LastTimeRebuild,100 + 50.f * dist);
				bNeedRebuild = true; 
			DO_IN_TIME_INTERVAL_END();
			if (IS_NEED_REBUILD()) bNeedRebuild = true;
			
			
			if (bNeedRebuild) {
				// �������� �������, ����������� ����. ������.
				CMonsterSquad	*pSquad = Level().SquadMan.GetSquad((u8)pMonster->g_Squad());
				TTime			squad_ai_last_updated;
				Fvector			target = pSquad->GetTargetPoint(pMonster, squad_ai_last_updated);
				ESquadAttackAlg alg_type = pSquad->GetAlgType();
				
				bool squad_target_selected = false;
				
				if (alg_type == SAA_DEVIATION) {
					if (squad_ai_last_updated !=0 ) {		// ����� �������
						pMonster->Path_ApproachPoint(target);
						pMonster->SetSelectorPathParams();
						squad_target_selected = true;
						pMonster->set_use_dest_orientation	(false);
					}
				} else {
					pMonster->set_dest_direction		(target);
					pMonster->set_use_dest_orientation	(true);

					pMonster->MoveToTarget(m_tEnemy.obj);
					squad_target_selected = true;
				}
				
				if (!squad_target_selected) {
					pMonster->set_use_dest_orientation	(false);
					pMonster->MoveToTarget(m_tEnemy.obj);

				}
			}
			
			if (!b_silent_run) pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->_sd->m_dwAttackSndDelay);

			break;

		// *********************
		case ACTION_ATTACK_MELEE:		// ��������� ��������
		// *********************

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
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, 1200);
				pMonster->FaceTarget(m_tEnemy.obj);
			DO_IN_TIME_INTERVAL_END();
			
			if (flags.is(AF_CAN_ATTACK_FROM_BACK)) {
				pMonster->MotionMan.SetSpecParams(ASP_BACK_ATTACK);
				bEnableBackAttack = false;
			}

			if (flags.is(AF_ATTACK_RAT)) pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT);

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->_sd->m_dwAttackSndDelay);

			break;

		// ****************
		case ACTION_STEAL:
		// ****************


			LOG_EX("ATTACK: STEAL");
			pMonster->MotionMan.m_tAction = ACT_STEAL;
			pMonster->MoveToTarget(m_tEnemy.obj);
			pMonster->set_use_dest_orientation	(false);
			break;
		
		
		// ******************
		case ACTION_THREATEN: 
		// ******************

			LOG_EX("ATTACK: THREATEN");
			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			pMonster->enable_movement(false);

			// �������� �� ����� 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, 1200);
				pMonster->FaceTarget(m_tEnemy.obj);
			DO_IN_TIME_INTERVAL_END();

			pMonster->MotionMan.SetSpecParams(ASP_THREATEN);

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundThreaten);
			
			break;

		// **********************************
		case ACTION_ENEMY_POSITION_APPROACH:
		// **********************************
			pMonster->MotionMan.m_tAction = ACT_RUN;

			pMonster->Path_ApproachPoint(m_tEnemy.obj->Position());
			pMonster->SetSelectorPathParams();
			pMonster->set_use_dest_orientation	(false);
			
			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->_sd->m_dwAttackSndDelay);

			break;
	}


	// ��������� ����������� �����������
	if (flags.is(AF_HAS_INVISIBILITY_ABILITY) && (ACTION_THREATEN != m_tAction)) {
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
	

	init_flags.set(AF_NEW_ENEMY,FALSE);
	m_tPrevAction	= m_tAction; 
	
}


void CBitingAttack::Done()
{
	inherited::Done();

	pMonster->AS_Stop();
	pMonster->set_use_dest_orientation	(false);
}

#define THREATEN_TIME  5300

// ���������� �������:
// ������ - ���������, ���������� - ���������� ��� ������
// �� ���������� N ��� �� ��� �������� ���� ��������
// ���� ����� �� �����, �����
// ���� ��������� ATTACK_MELEE ��� �� ����
bool CBitingAttack::CheckStartThreaten()
{
	if (b_in_threaten) return false;

	// �������� ������
	if (!flags.is(AF_LOW_MORALE) || flags.is(AF_ENEMY_DOESNT_SEE_ME) || flags.is(AF_ENEMY_GO_FARTHER_FAST) || flags.is(AF_REMEMBER_HIT_FROM_THIS_ENEMY)) {
		return false;
	}

	// �������� ���������
	if ((dist < m_fDistMax) || (dist > THREATEN_DISTANCE)) {
		return false;
	}

	// �������� ����
	float h,p;
	Fvector().sub(m_tEnemy.obj->Position(),pMonster->Position()).getHP(h,p);
	if (angle_difference(angle_normalize(-pMonster->CMovementManager::m_body.current.yaw),h) > PI / 15) {
		return false;
	}

	// �������� �������� ����� ���������� �������
	if (ThreatenTimeStarted + ThreatenMinDelay > m_dwCurrentTime) {
		return false;
	}

	return true;
}

bool CBitingAttack::CheckEndThreaten()
{
	if (!b_in_threaten) return false;
	if (ThreatenTimeStarted + THREATEN_TIME < m_dwCurrentTime) return true;

	float h,p;
	Fvector().sub(m_tEnemy.obj->Position(),pMonster->Position()).getHP(h,p);
	if (angle_difference(angle_normalize(-pMonster->CMovementManager::m_body.current.yaw),h) > PI_DIV_6) return true;
	
	// �������� ������
	if (!flags.is(AF_LOW_MORALE) || flags.is(AF_ENEMY_DOESNT_SEE_ME) || flags.is(AF_ENEMY_GO_FARTHER_FAST) || flags.is(AF_REMEMBER_HIT_FROM_THIS_ENEMY)) {
		return true;
	}

	// �������� ���������
	if ((dist < m_fDistMin) || (dist > THREATEN_DISTANCE)) {
		return true;
	}

	return false;
}

bool CBitingAttack::CheckThreaten()
{
	if (b_in_threaten) {
		if (CheckEndThreaten()) {
			b_in_threaten			= false;
			ThreatenTimeStarted		= 0;
			ThreatenMinDelay		= 0;
		}
	} else {
		if (CheckStartThreaten()) {
			b_in_threaten			= true;
			ThreatenTimeStarted		= m_dwCurrentTime;
			ThreatenMinDelay		= Random.randI(5000,10000);
		}
	}
	return b_in_threaten;
}



#define STEAL_MAX_ANGLE		PI_DIV_3

bool CBitingAttack::CheckSteal()
{
	if (!flags.is(AF_ATTACK_RAT) &&  flags.is(AF_THIS_IS_THE_ONLY_ENEMY) &&  flags.is(AF_ENEMY_DOESNT_SEE_ME) && !flags.is(AF_ENEMY_GO_FARTHER_FAST)) {
		b_silent_run = true;
		
		// ��������� ���������� �� ����
		float path_angle = 0.f;
		if (pMonster->IsMovingOnPath() && (pMonster->CDetailPathManager::curr_travel_point_index() < pMonster->CDetailPathManager::path().size()-3)) {
			const xr_vector<CDetailPathManager::STravelPathPoint> &path = pMonster->CDetailPathManager::path();

			float prev_yaw, prev_h;
			pMonster->CDetailPathManager::direction().getHP(prev_yaw,prev_h);
			for (u32 i=pMonster->CDetailPathManager::curr_travel_point_index()+1; i<path.size()-1;i++) {
				float h,p;
				Fvector().sub(path[i+1].position, path[i].position).getHP(h,p);

				path_angle += angle_difference(prev_yaw,h);
				prev_yaw = h;

				if (path_angle > STEAL_MAX_ANGLE) break;
			}
		}

		if ((path_angle < STEAL_MAX_ANGLE) && 	(dist > (m_fDistMax + 2.f) && (dist < 10.f))) return true;
	} else b_silent_run = false;
	
	return false;
}

Fvector CBitingAttack::RandomPos(Fvector pos, float R)
{
	Fvector local;

	local = pos;
	local.x += ::Random.randF(-R,R);
	local.z += ::Random.randF(-R,R);

	return local;
}


bool CBitingAttack::CanAttackFromBack()
{
	if (!bEnableBackAttack) return false;

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



//////////////////////////////////////////////////////////////////////////
// New State
//////////////////////////////////////////////////////////////////////////

void CBitingAttack::UpdateInitFlags() 
{
	init_flags.zero();

	const CAI_Rat *tpRat = dynamic_cast<const CAI_Rat *>(m_tEnemy.obj);
	if (tpRat) init_flags.or(AF_ATTACK_RAT);

	// ���������� �����������
	pJumping = dynamic_cast<CJumping *>(pMonster);
	if (pJumping)			init_flags.or(AF_HAS_JUMP_ABILITY);
	if (m_bInvisibility)	init_flags.or(AF_HAS_INVISIBILITY_ABILITY);

	init_flags.or(AF_NEW_ENEMY);
}

void CBitingAttack::UpdateFrameFlags() 
{
	frame_flags.zero();

	if ((pMonster->flagsEnemy & FLAG_ENEMY_DOESNT_SEE_ME) == FLAG_ENEMY_DOESNT_SEE_ME)		
		frame_flags.or(AF_ENEMY_DOESNT_SEE_ME);
	
	if ((pMonster->flagsEnemy & FLAG_ENEMY_GO_FARTHER_FAST) == FLAG_ENEMY_GO_FARTHER_FAST) 	
		frame_flags.or(AF_ENEMY_GO_FARTHER_FAST);
	
	if (pMonster->IsDangerousEnemy(m_tEnemy.obj))											
		frame_flags.or(AF_REMEMBER_HIT_FROM_THIS_ENEMY);
	
	if (pMonster->GetEnemyNumber()==1)		
		frame_flags.or(AF_THIS_IS_THE_ONLY_ENEMY);
	
	if (pMonster->ObjectNotReachable(m_tEnemy.obj))
		frame_flags.or(AF_ENEMY_IS_NOT_REACHABLE);

	// ����� �������� �� ����
	if (pMonster->IsMovingOnPath())			frame_flags.or(AF_GOOD_MOVEMENT);
	if (IS_NEED_REBUILD())					frame_flags.or(AF_NEED_REBUILD_PATH);

	if (pMonster->GetEntityMorale() < 0.8f) frame_flags.or(AF_LOW_MORALE);

	if (CanAttackFromBack())				frame_flags.or(AF_CAN_ATTACK_FROM_BACK);

	if (m_tEnemy.time == m_dwCurrentTime)	frame_flags.or(AF_SEE_ENEMY);

}


