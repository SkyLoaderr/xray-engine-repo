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

	// �������� �����
	m_tEnemy = pMonster->m_tEnemy;

	// ����������� ������ �����
	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(m_tEnemy.obj);
	if (tpRat) m_bAttackRat = true;
	else m_bAttackRat = false;

	// ��������� ��������� ������
	if (m_bAttackRat) {
		m_fDistMin = 0.7f;				// todo: make as ltx parameters
		m_fDistMax = 2.8f;				// todo: make as ltx parameters
	} else {
		m_fDistMin = 2.4f;				// todo: make as ltx parameters
		m_fDistMax = 3.8f;				// todo: make as ltx parameters
	}

	pMonster->SetMemoryTimeDef();

	// Test
	WRITE_TO_LOG("_ Attack Init _");
}

void CBitingAttack::Run()
{
	// ���� ���� ���������, ���������������� ���������
//	if (!pMonster->m_tEnemy.obj) R_ASSERT("Enemy undefined!!!");
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	// ����� ���������
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	float dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());

	if (bAttackMelee && (dist < m_fDistMax)) m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// ���� ���� �� ����� �� ���������� 1 ��� - ������ � ����
	if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.time + 1000 < m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}

	u32 delay;

//#pragma todo("Jim to Jim: fix nesting: Bloodsucker in Biting state")
//	if (m_bInvisibility) {
//		CAI_Bloodsucker *pBS =	dynamic_cast<CAI_Bloodsucker *>(pMonster);
//		if (pBS) {
//			if ((dist < pBS->m_fInvisibilityDist) && (pBS->GetPower() > pBS->m_fPowerThreshold)) {
//				if (pBS->CMonsterInvisibility::Switch(false)) {
//					pBS->ChangePower(pBS->m_ftrPowerDown);
//					pBS->ActivateEffector(pBS->CMonsterInvisibility::GetInvisibleInterval() / 1000.f);
//				}
//			}
//		}
//	}

	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_RUN:		// ������ �� �����
			delay = ((m_bAttackRat)? 0: 300);

			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, delay);

			pMonster->MotionMan.m_tAction = ACT_RUN;

			break;
		case ACTION_ATTACK_MELEE:		// ��������� ��������
			// ���� ���� ����� ��� �������� ����������� � �����
			if (m_bAttackRat) {
				if (dist < 0.6f) {
					if (!m_dwSuperMeleeStarted)	m_dwSuperMeleeStarted = m_dwCurrentTime;
					if (m_dwSuperMeleeStarted + 600 < m_dwCurrentTime) {
						// ��������
						pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT_JUMP);
						m_dwSuperMeleeStarted = 0;
					}
				} else m_dwSuperMeleeStarted = 0;
			}

			// �������� �� ����� 
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
	}
}

