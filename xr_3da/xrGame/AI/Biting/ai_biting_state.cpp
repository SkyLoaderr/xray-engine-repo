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

void CBitingRest::Run()
{
	// ��������� ����� �� �������� ��������������
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();

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

		dwMinRand = 1100;
		dwMaxRand = 1200;

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

	m_dwFaceEnemyLastTime = 0;
	m_dwFaceEnemyLastTimeInterval = 1200;
	
	nStartStop = nDoDamage = 0;
}

void CBitingAttack::Init()
{
	IState::Init();

	// �������� �����
	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);
	pMonster->SaveEnemy();

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

	// Test
	Msg("_ Attack Init _");
}

void CBitingAttack::Run()
{
	// ���� ���� ���������, ���������������� ���������
	VisionElem ve;
	if (!pMonster->GetEnemy(ve)) R_ASSERT(false);
	if (m_tEnemy.obj != ve.obj) {
		Reset();
		Init();
	} else m_tEnemy = ve;

	// ����� ���������
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	if (bAttackMelee && (m_tEnemy.position.distance_to(pMonster->Position()) < m_fDistMax)) 
		m_tAction = ACTION_ATTACK_MELEE;
	else 
		m_tAction = ((m_tEnemy.position.distance_to(pMonster->Position()) > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// ���������� ������� �����-������
	if (bAttackMelee && m_tAction == ACTION_RUN) {
		nStartStop++;
	}
	if (nStartStop > nDoDamage*2) {
		if (nDoDamage != 0) {
			nStartStop = nDoDamage = 0;
			m_fDistMin -= 0.3f;
		} else nDoDamage = 1;
	}
	
	// �������� ��� ���������� ����
	u32 delay;

	// ���� ���� �� ����� - ������ � ����
	if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.position != m_tEnemy.obj->Position())) {
		m_tAction = ACTION_RUN;
	}

	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_RUN:		// ������ �� �����
			delay = ((m_bAttackRat)? 0: 300);
			pMonster->AI_Path.DestNode = m_tEnemy.node_id;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.position, false, 0, delay);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:		// ��������� ��������
			// ���� ���� ��� �������� ����������� � �����
			float yaw, pitch;

			if (m_dwFaceEnemyLastTime + m_dwFaceEnemyLastTimeInterval < m_dwCurrentTime) {
				// �������� �� ����� 
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

			if (m_bAttackRat) pMonster->Motion.m_tParams.SetParams(eMotionAttackRat,0,m_cfBitingRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			else pMonster->Motion.m_tParams.SetParams(eMotionAttack,0,m_cfBitingRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);			
			pMonster->Motion.m_tTurn.Set(eMotionFastTurnLeft, eMotionFastTurnLeft, 0, m_cfBitingAttackFastRSpeed,m_cfBitingRunAttackMinAngle);

			if (pMonster->AttackMelee(m_tEnemy.obj,false)) {
				//pMonster->DoDamage(pEnemy);
				nDoDamage++; 
			}

			break;
	}
}

bool CBitingAttack::CheckCompletion() 
{
//// ���� ���� ����
//	if (!pEnemy || !pEnemy->g_Alive()) return true;
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
	Msg("_ Eat Init _");
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
				Msg("Food = [%f]",pCorpse->m_fFood);
			}
			break;
	}
}

bool CBitingEat::CheckCompletion() 
{
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

	SetInertia(30000);
	pMonster->SetMemoryTime(30000);

	// Test
	Msg("_ Hide Init _");
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

	pMonster->SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	Msg(" DETOUR init!");
}

void CBitingDetour::Run()
{
	Msg("--- DETOUR ---");

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

bool CBitingDetour::CheckCompletion()
{	
	return false;
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

	m_tEnemy.obj = 0;
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

	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,2000);

	pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);
}

bool CBitingPanic::CheckCompletion()
{	
	return false;
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

	// ��������� �������� ������
	float yaw = angle_normalize(pMonster->r_torso_target.yaw + PI_DIV_2);

	pMonster->Motion.m_tSeq.Add(eMotionFastTurnLeft,0,m_cfBitingScaredRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
	pMonster->Motion.m_tSeq.Add(eMotionScared,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tSeq.Switch();

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);
}

void CBitingExploreDNE::Run()
{
	// ������ �����
	VisionElem tempEnemy;

	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,1000);

	pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);
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

	pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI_DIV_2);

	// ��������� �������� ������
	SetInertia(20000);
	pMonster->SetMemoryTime(20000);
}

void CBitingExploreDE::Run()
{
	// ����������� ���������
	if (m_tAction == ACTION_LOOK_AROUND && (m_dwStateStartedTime + 2000 < m_dwCurrentTime)) m_tAction = ACTION_HIDE;
	
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
		if (AI_Path.TravelPath.empty() || ((AI_Path.TravelPath.size() - 1) < AI_Path.TravelStart)) {
			if (m_tAnim == eMotionWalkFwd || m_tAnim == eMotionRun) {
				m_tAnim = eMotionStandIdle;
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

