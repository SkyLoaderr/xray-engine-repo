////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera_state.cpp
//	Created 	: 27.05.2003
//  Modified 	: 27.05.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_chimera.h"
#include "ai_chimera_state.h"

using namespace AI_Chimera;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraMotion implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CChimeraMotion::Init()
{
	m_tSeq.Init();
}

void CChimeraMotion::SetFrameParams(CAI_Chimera *pData) 
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


//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATE MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraRest::CChimeraRest(CAI_Chimera *p)  
{
	pMonster = p;
	Reset();

	SetLowPriority();			// ������������ �������� ����������
}


void CChimeraRest::Reset()
{
	IState::Reset();

	m_dwReplanTime		= 0;
	m_dwLastPlanTime	= 0;

	m_tAction			= ACTION_STAND;

	pMonster->SetMemoryTimeDef();
	
}

void CChimeraRest::Init()
{
	IState::Init();

	// ���� ���� ���� - ����� �� ����� (����������� ������������� �����)
	if (!pMonster->AI_Path.TravelPath.empty()) {
		m_bFollowPath = true;
	} else m_bFollowPath = false;
}


void CChimeraRest::Run()
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

			pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,m_cfChimeraWalkSpeed,m_cfChimeraWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfChimeraWalkTurningSpeed,m_cfChimeraWalkTurnRSpeed,m_cfChimeraWalkMinAngle);
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
			pMonster->Motion.m_tParams.SetParams(eMotionStandTurnLeft,0,m_cfChimeraStandTurnRSpeed, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_WALK_GRAPH_END:
			pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,m_cfChimeraWalkSpeed,m_cfChimeraWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfChimeraWalkTurningSpeed,m_cfChimeraWalkTurnRSpeed,m_cfChimeraWalkMinAngle);
			break;
	}
}

void CChimeraRest::Replanning()
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


TTime CChimeraRest::UnlockState(TTime cur_time)
{
	TTime dt = inherited::UnlockState(cur_time);

	m_dwReplanTime		+= dt;
	m_dwLastPlanTime	+= dt;

	return dt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraAttack::CChimeraAttack(CAI_Chimera *p)  
{
	pMonster = p;
	Reset();
	SetHighPriority();
}


void CChimeraAttack::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	m_tEnemy.obj		= 0;

	m_fDistMin			= 0.f;	
	m_fDistMax			= 0.f;

	m_dwFaceEnemyLastTime	= 0;
	m_dwFaceEnemyLastTimeInterval = 1200;
	
	nStartStop = nDoDamage	= 0;
}

void CChimeraAttack::Init()
{
	IState::Init();

	// �������� �����
	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);
	pMonster->SaveEnemy();

	m_fDistMin = 2.4f;
	m_fDistMax = 4.8f;
	
	pMonster->SetMemoryTime(3000);

	// Test
	Msg("_ Attack Init _");
}

void CChimeraAttack::Run()
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

	if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.time != m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}

	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_RUN:		// ������ �� �����
			//delay = ((m_tEnemy.position.distance_to(pMonster->Position()) < (m_fDistMax+4))? 0:300);
			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, 300);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfChimeraRunAttackSpeed,m_cfChimeraRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfChimeraRunAttackTurnSpeed,m_cfChimeraRunAttackTurnRSpeed,m_cfChimeraRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:		// ��������� ��������
			
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

			pMonster->Motion.m_tParams.SetParams(eMotionAttack,0,m_cfChimeraRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);			
			pMonster->Motion.m_tTurn.Set(eMotionFastTurnLeft, eMotionFastTurnLeft, 0, m_cfChimeraAttackFastRSpeed,m_cfChimeraRunAttackMinAngle);

			break;
	}
}

bool CChimeraAttack::CheckCompletion() 
{
//// ���� ���� ����
//	if (!pEnemy || !pEnemy->g_Alive()) return true;
	return false;
}	


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraEat::CChimeraEat(CAI_Chimera *p)  
{
	pMonster = p;
	Reset();
	SetLowPriority();
}


void CChimeraEat::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	pCorpse				= 0;

	m_dwLastTimeEat		= 0;
	m_dwEatInterval		= 1000;
}

void CChimeraEat::Init()
{
	IState::Init();

	// �������� ���� � �����
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	m_fDistToCorpse = 1.5f;

	// Test
	Msg("_ Eat Init _");
}

void CChimeraEat::Run()
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

			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfChimeraRunAttackSpeed,m_cfChimeraRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfChimeraRunAttackTurnSpeed,m_cfChimeraRunAttackTurnRSpeed,m_cfChimeraRunAttackMinAngle);

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

bool CChimeraEat::CheckCompletion() 
{
	return false;
}	



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraHide class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraHide::CChimeraHide(CAI_Chimera *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CChimeraHide::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);
	pMonster->SaveEnemy();

	SetInertia(30000);
	pMonster->SetMemoryTime(30000);

	// Test
	Msg("_ Hide Init _");
}

void CChimeraHide::Reset()
{
	inherited::Reset();

	m_tEnemy.obj		= 0;
}

void CChimeraHide::Run()
{
	Fvector EnemyPos;
	if (m_tEnemy.obj) EnemyPos = m_tEnemy.obj->Position();
	else EnemyPos = m_tEnemy.position;
	
	pMonster->m_tSelectorCover.m_fMaxEnemyDistance = EnemyPos.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
	pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
	pMonster->m_tSelectorCover.m_fMinEnemyDistance = EnemyPos.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0,2000);

	// ���������� ��������� ��������
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfChimeraWalkSpeed,m_cfChimeraWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfChimeraWalkTurningSpeed,m_cfChimeraWalkTurnRSpeed,m_cfChimeraWalkMinAngle);
}

bool CChimeraHide::CheckCompletion()
{	
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraDetour class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraDetour::CChimeraDetour(CAI_Chimera *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CChimeraDetour::Reset()
{
	inherited::Reset();
	m_tEnemy.obj		= 0;
}

void CChimeraDetour::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);
	pMonster->SaveEnemy();

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	Msg(" DETOUR init!");
}

void CChimeraDetour::Run()
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
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfChimeraWalkSpeed,m_cfChimeraWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfChimeraWalkTurningSpeed,m_cfChimeraWalkTurnRSpeed,m_cfChimeraWalkMinAngle);

}

bool CChimeraDetour::CheckCompletion()
{	
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraPanic::CChimeraPanic(CAI_Chimera *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CChimeraPanic::Reset()
{
	inherited::Reset();

	m_tEnemy.obj = 0;
}

void CChimeraPanic::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);
	pMonster->SaveEnemy();

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	// Test
	Msg("_ Panic Init _");

}

void CChimeraPanic::Run()
{

	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,2000);

	pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfChimeraRunAttackSpeed,m_cfChimeraRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfChimeraRunAttackTurnSpeed,m_cfChimeraRunAttackTurnRSpeed,m_cfChimeraRunAttackMinAngle);
}

bool CChimeraPanic::CheckCompletion()
{	
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraExploreDNE class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraExploreDNE::CChimeraExploreDNE(CAI_Chimera *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CChimeraExploreDNE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
}

void CChimeraExploreDNE::Init()
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

	pMonster->Motion.m_tSeq.Add(eMotionFastTurnLeft,0,m_cfChimeraScaredRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
	pMonster->Motion.m_tSeq.Add(eMotionScared,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tSeq.Switch();

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);
}

void CChimeraExploreDNE::Run()
{
	// ������ �����
	VisionElem tempEnemy;

	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,1000);

	pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfChimeraRunAttackSpeed,m_cfChimeraRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfChimeraRunAttackTurnSpeed,m_cfChimeraRunAttackTurnRSpeed,m_cfChimeraRunAttackMinAngle);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraExploreDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraExploreDE::CChimeraExploreDE(CAI_Chimera *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CChimeraExploreDE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
	m_tAction = ACTION_LOOK_AROUND;
}

void CChimeraExploreDE::Init()
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

void CChimeraExploreDE::Run()
{
	// ����������� ���������
	if (m_tAction == ACTION_LOOK_AROUND && (m_dwStateStartedTime + 2000 < m_dwCurrentTime)) m_tAction = ACTION_HIDE;
	
	switch(m_tAction) {
	case ACTION_LOOK_AROUND:
		pMonster->Motion.m_tParams.SetParams(eMotionStandTurnLeft,0,m_cfChimeraStandTurnRSpeed, pMonster->r_torso_target.yaw, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
		pMonster->Motion.m_tTurn.Clear();
		break;
	case ACTION_HIDE:
		pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
		pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
		pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

		pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0,2000);

		// ���������� ��������� ��������
		pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfChimeraWalkSpeed,m_cfChimeraWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfChimeraWalkTurningSpeed,m_cfChimeraWalkTurnRSpeed,m_cfChimeraWalkMinAngle);
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraExploreNDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraExploreNDE::CChimeraExploreNDE(CAI_Chimera *p)
{
	pMonster = p;
	Reset();
	SetLowPriority();
}

void CChimeraExploreNDE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
}

void CChimeraExploreNDE::Init()
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

void CChimeraExploreNDE::Run()
{
	pMonster->vfChoosePointAndBuildPath(0, &m_tEnemy.position, false, 0, 2000);

	// ���������� ��������� ��������
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfChimeraWalkSpeed,m_cfChimeraWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfChimeraWalkTurningSpeed,m_cfChimeraWalkTurnRSpeed,m_cfChimeraWalkMinAngle);
}



//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Chimera state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Chimera::SetState(IState *pS, bool bSkipInertiaCheck)
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

void CAI_Chimera::ControlAnimation()
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
			if (m_tAnim == eMotionWalkFwd || m_tAnim == eMotionRun) {
				m_tAnim = eMotionStandIdle;
			}
		}

		// ���� ����� �� ����� � �������� ������...
		int i = ps_Size();		
		if (i > 1) {
			CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
			if (tCurrentPosition.vPosition.similar(tPreviousPosition.vPosition)) {
				if (m_tAnim == eMotionWalkFwd || m_tAnim == eMotionRun) {
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
