////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_state.cpp
//	Created 	: 27.05.2003
//  Modified 	: 27.05.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"
#include "..\\rat\\ai_rat.h"

using namespace AI_Biting;


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBitingMotion::Init()
{
	m_tSeq.Init();
}

void CBitingMotion::SetFrameParams(CAI_Biting *pData) 
{
	if (!m_tSeq.Active()) {

		m_tParams.ApplyData(pData);
		m_tTurn.CheckTurning(pData);

		//!- ��������� ���������� �� ������������� ��������. ��������� (kinda StandUp)
		if ((pData->m_tAnimPrevFrame == eMotionLieIdle) && (pData->m_tAnim != eMotionLieIdle)){
			m_tSeq.Add(eMotionStandUp,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			m_tSeq.Switch();
			m_tSeq.ApplyData(pData);
		}

		//!---
	} else {
		m_tSeq.ApplyData(pData);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingRest::CBitingRest(CAI_Biting *p)  
{
	pMonster = p;
	Reset();

	SetLowPriority();			// ������������ �������� ����������
}


void CBitingRest::Reset()
{
	IState::Reset();

	m_dwReplanTime		= 0;
	m_dwLastPlanTime	= 0;

	m_tAction			= ACTION_STAND;

	pMonster->SetMemoryTimeDef();
	
}

void CBitingRest::Init()
{
	IState::Init();

	// ���� ���� ���� - ����� �� ����� (����������� ������������� �����)
	if (!pMonster->AI_Path.TravelPath.empty()) {
		m_bFollowPath = true;
	} else m_bFollowPath = false;
}


void CBitingRest::Run()
{
	
	if (m_bFollowPath) {
		if ((pMonster->AI_Path.TravelPath.size() - 1) <= pMonster->AI_Path.TravelStart) m_bFollowPath = false;
	}
	
	if (m_bFollowPath) {
		m_tAction = ACTION_WALK_GRAPH_END;
	} else {
		// ��������� ����� �� �������� ��������������
		if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();
	}
	

	// FSM 2-�� ������
	switch (m_tAction) {
		case ACTION_WALK:		// ����� ����� �����
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
			pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);
			break;
		case ACTION_STAND:     // ������, ������ �� ������
			pMonster->Motion.m_tParams.SetParams(eMotionStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_LIE:		// ������
			pMonster->Motion.m_tParams.SetParams(eMotionLieIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_TURN:		// ����������� �� 90 ����.
			pMonster->Motion.m_tParams.SetParams(eMotionStandTurnLeft,0,m_cfBitingStandTurnRSpeed, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_WALK_GRAPH_END:
			pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);
			break;
	}
}

void CBitingRest::Replanning()
{
	// Test
	Msg("_ Rest replanning _");

	m_dwLastPlanTime = m_dwCurrentTime;	
	u32		rand_val = ::Random.randI(100);
	u32		dwMinRand = 0, dwMaxRand = 0;

	if (rand_val < 50) {	
		m_tAction = ACTION_WALK;
		// ��������� ���� ������ ����� �����
		pMonster->vfUpdateDetourPoint();	
		pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

		dwMinRand = 3000;
		dwMaxRand = 5000;

	} else if (rand_val < 60) {	
		m_tAction = ACTION_STAND;

		dwMinRand = 3000;
		dwMaxRand = 5000;

	} else if (rand_val < 70) {	
		m_tAction = ACTION_LIE;
		// ��������� ����� ���?
		if (pMonster->m_tAnim != eMotionLieIdle) {
			pMonster->Motion.m_tSeq.Add(eMotionLieDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tSeq.Switch();
		}

		dwMinRand = 5000;
		dwMaxRand = 7000;

	} else  {	
		m_tAction = ACTION_TURN;
		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI_DIV_2);

		dwMinRand = 1000;
		dwMaxRand = 1100;
	}
	
	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
	SetNextThink(dwMinRand);
}


TTime CBitingRest::UnlockState(TTime cur_time)
{
	TTime dt = inherited::UnlockState(cur_time);

	m_dwReplanTime		+= dt;
	m_dwLastPlanTime	+= dt;

	return dt;
}

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
	
	nStartStop = 0;
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
		m_fDistMin = 0.7f;
		m_fDistMax = 2.8f;
	} else {
		m_fDistMin = 2.4f;
		m_fDistMax = 3.8f;
	}
	
	pMonster->SetMemoryTimeDef();

	// Test
	Msg("_ Attack Init _");
}

void CBitingAttack::Run()
{
	// ���� ���� ���������, ���������������� ���������
	if (!pMonster->m_tEnemy.obj) R_ASSERT("Enemy undefined!!!");
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) {
		Reset();
		Init();
	} else m_tEnemy = pMonster->m_tEnemy;

	// ����� ���������
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	if (bAttackMelee && (m_tEnemy.obj->Position().distance_to(pMonster->Position()) < m_fDistMax)) 
		m_tAction = ACTION_ATTACK_MELEE;
	else 
		m_tAction = ((m_tEnemy.obj->Position().distance_to(pMonster->Position()) > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// ���������� ������� �����-������ 
	if (bAttackMelee && m_tAction == ACTION_RUN) {
		nStartStop++;
	}
	
	// �������� ��� ���������� ����
	u32 delay;

	// ���� ���� �� ����� - ������ � ����
	if (!m_bAttackRat) {
		if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.time != m_dwCurrentTime)) {
			m_tAction = ACTION_RUN;
		}
	}

	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_RUN:		// ������ �� �����
			delay = ((m_bAttackRat)? 0: 300);
			
			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, delay);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:		// ��������� ��������
			// ���� ���� ����� ��� �������� ����������� � �����
			if (m_tEnemy.obj->Position().distance_to(pMonster->Position()) < 0.6f) {
				if (!m_dwSuperMeleeStarted)	m_dwSuperMeleeStarted = m_dwCurrentTime;

				if (m_dwSuperMeleeStarted + 600 < m_dwCurrentTime) {
					// ��������
					pMonster->Motion.m_tSeq.Add(eMotionAttackJump,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
					pMonster->Motion.m_tSeq.Switch();
					m_dwSuperMeleeStarted = 0;
				}
			} else m_dwSuperMeleeStarted = 0;
			
			
			// �������� �� ����� 
			float yaw, pitch;
			if (m_dwFaceEnemyLastTime + m_dwFaceEnemyLastTimeInterval < m_dwCurrentTime) {
				
				m_dwFaceEnemyLastTime = m_dwCurrentTime;
				pMonster->AI_Path.TravelPath.clear();
				
				Fvector EnemyCenter;
				Fvector MyCenter;

				m_tEnemy.obj->Center(EnemyCenter);
				pMonster->Center(MyCenter);

				EnemyCenter.sub(MyCenter);
				EnemyCenter.getHP(yaw,pitch);
				yaw *= -1;
				yaw = angle_normalize(yaw);
			} else yaw = pMonster->r_torso_target.yaw;

//			// set motion params
			if (m_bAttackRat) pMonster->Motion.m_tParams.SetParams(eMotionAttackRat,0,m_cfBitingRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			else pMonster->Motion.m_tParams.SetParams(eMotionAttack,0,m_cfBitingRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			pMonster->Motion.m_tTurn.Set(eMotionFastTurnLeft, eMotionFastTurnLeft, 0, m_cfBitingAttackFastRSpeed,m_cfBitingRunAttackMinAngle);
			break;
	}
}

bool CBitingAttack::CheckCompletion()
{	
//	if (!m_tEnemy.obj) return true;
//	if (!m_tEnemy.obj->g_Alive()) return true;
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingEat::CBitingEat(CAI_Biting *p)  
{
	pMonster = p;
	Reset();
	SetLowPriority();
}


void CBitingEat::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	pCorpse				= 0;

	m_dwLastTimeEat		= 0;
	m_dwEatInterval		= 1000;
}

void CBitingEat::Init()
{
	IState::Init();

	// �������� ���� � �����
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pCorpse);
	m_fDistToCorpse = ((tpRat)? 1.0f : 1.5f);

	// Test
	//Msg("_ Eat Init _");
}

void CBitingEat::Run()
{
	// ���� ����� ����, ����� ���������������� ��������� 
	VisionElem ve;
	if (!pMonster->GetEnemy(ve)) R_ASSERT(false);
	if (pCorpse != ve.obj) {
		Reset();
		Init();
	}
	
	bool bStartEating = (m_tAction == ACTION_RUN);

	// ����� ���������
	if (pCorpse->Position().distance_to(pMonster->Position()) < m_fDistToCorpse) m_tAction = ACTION_EAT;
	else m_tAction = ACTION_RUN;

	bStartEating = bStartEating && (m_tAction == ACTION_EAT);
	if (bStartEating) {	// ���� ������ �������� � �����, ���������� �������� �������� ����� � ����
		pMonster->Motion.m_tSeq.Add(eMotionCheckCorpse,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tSeq.Add(eMotionLieDownEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tSeq.Switch();
	}

	// ���������� ���������
	switch (m_tAction) {
		case ACTION_RUN:	// ������ � �����

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0,2000);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);

			break;
		case ACTION_EAT:
			pMonster->Motion.m_tParams.SetParams(eMotionEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();

			// ������ �����
			if (m_dwLastTimeEat + m_dwEatInterval < m_dwCurrentTime) {
				pCorpse->m_fFood -= pMonster->m_fHitPower/5.f;
				m_dwLastTimeEat = m_dwCurrentTime;
				//Msg("Food = [%f]",pCorpse->m_fFood);
			}
			break;
	}
}

bool CBitingEat::CheckCompletion()
{	
//	if (!pCorpse) return true;
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingHide class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingHide::CBitingHide(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingHide::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);
	pMonster->SaveEnemy();

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);

	// Test
	//Msg("_ Hide Init _");
}

void CBitingHide::Reset()
{
	inherited::Reset();

	m_tEnemy.obj		= 0;
}

void CBitingHide::Run()
{
	Fvector EnemyPos;
	if (m_tEnemy.obj) EnemyPos = m_tEnemy.obj->Position();
	else EnemyPos = m_tEnemy.position;
	
	pMonster->m_tSelectorCover.m_fMaxEnemyDistance = EnemyPos.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
	pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
	pMonster->m_tSelectorCover.m_fMinEnemyDistance = EnemyPos.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0,2000);

	// ���������� ��������� ��������
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);
}

bool CBitingHide::CheckCompletion()
{	
//	if (!m_tEnemy.obj) return true;
//	if (m_tEnemy.position.distance_to(pMonster->Position()) > 30) return true;
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingDetour class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingDetour::CBitingDetour(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingDetour::Reset()
{
	inherited::Reset();
	m_tEnemy.obj		= 0;
}

void CBitingDetour::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);
	pMonster->SaveEnemy();

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	//Msg(" DETOUR init!");
}

void CBitingDetour::Run()
{
	//Msg("--- DETOUR ---");

	VisionElem tempEnemy;
	if (pMonster->GetEnemy(tempEnemy)) m_tEnemy = tempEnemy;

	pMonster->vfUpdateDetourPoint();
	pMonster->AI_Path.DestNode		= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;
	
	pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.obj->Position().distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
	pMonster->m_tSelectorCover.m_fOptEnemyDistance = 15;
	pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.obj->Position().distance_to(pMonster->Position()) + 3.f;
	

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0, 2000);

	// ���������� ��������� ��������
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingPanic::CBitingPanic(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBitingPanic::Reset()
{
	inherited::Reset();

	m_tEnemy.obj	= 0;

	cur_pos.set		(0.f,0.f,0.f);
	prev_pos		= cur_pos;
	bFacedOpenArea	= false;
	m_dwStayTime	= 0;

}

void CBitingPanic::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);
	pMonster->SaveEnemy();

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	// Test
	Msg("_ Panic Init _");
}

void CBitingPanic::Run()
{
	cur_pos = pMonster->Position();

	// implementation of 'face the most open area'
	if (!bFacedOpenArea && cur_pos.similar(prev_pos) && (m_dwStayTime != 0) && (m_dwStayTime + 300 < m_dwCurrentTime) && (m_dwStateStartedTime + 3000 < m_dwCurrentTime)) {
		bFacedOpenArea	= true;
		pMonster->AI_Path.TravelPath.clear();

		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI);

		pMonster->Motion.m_tSeq.Add(eMotionFastTurnLeft,0,m_cfBitingScaredRSpeed * 2.0f,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);		
		pMonster->Motion.m_tSeq.Switch();
	} 

	if (!cur_pos.similar(prev_pos)) {
		bFacedOpenArea = false;
		m_dwStayTime = 0;
	} else if (m_dwStayTime == 0) m_dwStayTime = m_dwCurrentTime;


	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,2000);

	if (!bFacedOpenArea) {
		pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);
	} else {
		// try to rebuild path
		if (pMonster->AI_Path.TravelPath.size() > 5) {
			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);
		} else {
			pMonster->Motion.m_tParams.SetParams(eMotionStandDamaged,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
		}
	}
	
	prev_pos = cur_pos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDNE class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreDNE::CBitingExploreDNE(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBitingExploreDNE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;

	cur_pos.set		(0.f,0.f,0.f);
	prev_pos		= cur_pos;
	bFacedOpenArea	= false;
	m_dwStayTime	= 0;

}

void CBitingExploreDNE::Init()
{
	// Test
	Msg("_ ExploreDNE Init _");

	inherited::Init();

	R_ASSERT(pMonster->IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->GetMostDangerousSound(se,bDangerous);	// ���������� ����� ������� ����
	m_tEnemy.obj = dynamic_cast<CEntity *>(se.who);
	m_tEnemy.position = se.Position;
	m_tEnemy.time = se.time;

	Fvector dir; 
	dir.sub(m_tEnemy.position,pMonster->Position());
	float yaw,pitch;
	dir.getHP(yaw,pitch);
	
	// ��������� �������� ������

	pMonster->Motion.m_tSeq.Add(eMotionFastTurnLeft,0,m_cfBitingScaredRSpeed * 2.0f,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
	pMonster->Motion.m_tSeq.Add(eMotionScared,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tSeq.Switch();

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);
}

void CBitingExploreDNE::Run()
{
	// ������ �����
	cur_pos = pMonster->Position();

	// implementation of 'face the most open area'
	if (!bFacedOpenArea && cur_pos.similar(prev_pos) && (m_dwStayTime != 0) && (m_dwStayTime + 300 < m_dwCurrentTime) && (m_dwStateStartedTime + 3000 < m_dwCurrentTime)) {
		bFacedOpenArea	= true;
		pMonster->AI_Path.TravelPath.clear();

		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI);

		pMonster->Motion.m_tSeq.Add(eMotionFastTurnLeft,0,m_cfBitingScaredRSpeed * 2.0f,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);		
		pMonster->Motion.m_tSeq.Switch();
	} 

	if (!cur_pos.similar(prev_pos)) {
		bFacedOpenArea = false;
		m_dwStayTime = 0;
	} else if (m_dwStayTime == 0) m_dwStayTime = m_dwCurrentTime;


	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,2000);

	if (!bFacedOpenArea) {
		pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);
	} else {
		pMonster->Motion.m_tParams.SetParams(eMotionStandDamaged,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Clear();
	}

	prev_pos = cur_pos;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreDE::CBitingExploreDE(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingExploreDE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
	m_tAction = ACTION_LOOK_AROUND;
	m_dwTimeToTurn	= 0;	
}

void CBitingExploreDE::Init()
{
	// Test
	Msg("_ ExploreDE Init _");

	inherited::Init();

	R_ASSERT(pMonster->IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->GetMostDangerousSound(se,bDangerous);	// ���������� ����� ������� ����
	m_tEnemy.obj = dynamic_cast<CEntity *>(se.who);
	m_tEnemy.position = se.Position;
	m_dwSoundTime	  = se.time;

	float	yaw,pitch;
	Fvector dir;
	dir.sub(m_tEnemy.position,pMonster->Position());
	dir.getHP(yaw,pitch);

	pMonster->r_torso_target.yaw = yaw;
	m_dwTimeToTurn = (TTime)(_abs(angle_normalize_signed(yaw - pMonster->r_torso_current.yaw)) / m_cfBitingStandTurnRSpeed * 1000);

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);
}

void CBitingExploreDE::Run()
{
	// ����������� ���������
	if (m_tAction == ACTION_LOOK_AROUND && (m_dwStateStartedTime + m_dwTimeToTurn < m_dwCurrentTime)) m_tAction = ACTION_HIDE;

	SoundElem se;
	bool bDangerous;
	pMonster->GetMostDangerousSound(se,bDangerous);	// ���������� ����� ������� ����
	
	switch(m_tAction) {
	case ACTION_LOOK_AROUND:
		pMonster->Motion.m_tParams.SetParams(eMotionStandTurnLeft,0,m_cfBitingStandTurnRSpeed, pMonster->r_torso_target.yaw, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
		pMonster->Motion.m_tTurn.Clear();
		break;
	case ACTION_HIDE:
		pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
		pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
		pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

		pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0,2000);

		// ���������� ��������� ��������
		pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);
		break;
	}
}

bool CBitingExploreDE::CheckCompletion()
{	
//	if (!m_tEnemy.obj) return true;
//	
//	// ���������, ���� �� ����� ����, ��������� ���������
//	SoundElem se;
//	bool bDangerous;
//	pMonster->GetMostDangerousSound(se,bDangerous);	// ���������� ����� ������� ����
//	
//	if ((m_dwSoundTime + 2000)< se.time) return true;
//
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreNDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreNDE::CBitingExploreNDE(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetLowPriority();
}

void CBitingExploreNDE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
}

void CBitingExploreNDE::Init()
{
	// Test
	Msg("_ ExploreNDE Init _");

	inherited::Init();

	R_ASSERT(pMonster->IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->GetMostDangerousSound(se,bDangerous);	// ���������� ����� ������� ����
	m_tEnemy.obj = dynamic_cast<CEntity *>(se.who);
	m_tEnemy.position = se.Position;

	// ��������� �������� ������
	SetInertia(6000);
}

void CBitingExploreNDE::Run()
{
	pMonster->vfChoosePointAndBuildPath(0, &m_tEnemy.position, false, 0, 2000);

	// ���������� ��������� ��������
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFindEnemy class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CFindEnemy::CFindEnemy(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CFindEnemy::Reset()
{
	inherited::Reset();

	m_dwReplanTime		= 1000;			
	m_dwLastPlanTime	= 0;
	bLookLeft			= true;

}

void CFindEnemy::Init()
{
	// Test
	Msg("_ FindEnemy Init _");
	
	inherited::Init();
	SetInertia(2000);
}

void CFindEnemy::Run()
{
	// ���������� �� ��������
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) {
		m_dwLastPlanTime = m_dwCurrentTime;
		
		if (bLookLeft) 
			pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI_DIV_2);	
		else 
			pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw - PI);

		bLookLeft = !bLookLeft;
	}
	
	pMonster->Motion.m_tParams.SetParams(eMotionFastTurnLeft,0,m_cfBitingAttackFastRSpeed2, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Clear();
}


//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Biting state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Biting::SetState(IState *pS, bool bSkipInertiaCheck)
{
	if (CurrentState != pS) {
		// �������� �������
		if (!bSkipInertiaCheck)
			if (CurrentState->IsInertia()) {
				if (CurrentState->GetPriority() >= pS->GetPriority()) return;
			}

		CurrentState->Reset();
		CurrentState = pS;
		CurrentState->Activate();
	}
}

void CAI_Biting::ControlAnimation()
{
	//-- �������� ������������� �������� --
	if (Motion.m_tSeq.Started) {
		Motion.m_tSeq.Playing = true;
		Motion.m_tSeq.Started = false;
		Motion.m_tSeq.Finished = false;
		// ���������� ���������
		CurrentState->LockState();
		FORCE_ANIMATION_SELECT();
	} 

	if (!Motion.m_tSeq.Playing) {
		
		// ���� ��� ���� � ���� �������� ��������, �� ������ �������� ������
		if (AI_Path.TravelPath.empty() || ((AI_Path.TravelPath.size() - 1) <= AI_Path.TravelStart)) {
			if ((m_tAnim == eMotionWalkFwd) || (m_tAnim == eMotionRun)) {
				m_tAnim = eMotionStandIdle;
			}
		}

		// ���� ����� �� ����� � �������� ������...
		int i = ps_Size();		
		if (i > 1) {
			CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
			if (tCurrentPosition.vPosition.similar(tPreviousPosition.vPosition)) {
				if ((m_tAnim == eMotionWalkFwd) || (m_tAnim == eMotionRun)) {
					m_tAnim = eMotionStandIdle;
				}
			}
		}

		// ���� �������� ����������, ������������� ��������
		if (m_tAnimPrevFrame != m_tAnim) {
			FORCE_ANIMATION_SELECT();
		}	
	}
	//--------------------------------------

	// ���������� ���������� ��������
	m_tAnimPrevFrame = m_tAnim;
}

