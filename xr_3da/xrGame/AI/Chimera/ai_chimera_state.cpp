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
#include "..\\rat\\ai_rat.h"

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

		//!- проверить необходимо ли устанавливать специфич. параметры (kinda StandUp)
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
	SetLowPriority();			// удиноразовая утановка приоритета
}


void CChimeraRest::Reset()
{
	IState::Reset();

	m_dwReplanTime		= 0;
	m_dwLastPlanTime	= 0;

	m_tAction			= ACTION_STAND;
	
}

void CChimeraRest::Run()
{
	// проверить нужно ли провести перепланировку
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();

	// FSM 2-го уровня
	switch (m_tAction) {
		case ACTION_WALK:
			pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,m_cfWalkSpeed,m_cfWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfWalkTurningSpeed,m_cfWalkTurnRSpeed,m_cfWalkMinAngle);
			break;
		case ACTION_STAND:
			pMonster->Motion.m_tParams.SetParams(eMotionStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_LIE:
			pMonster->Motion.m_tParams.SetParams(eMotionLieIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_TURN:
			pMonster->Motion.m_tParams.SetParams(eMotionStandTurnLeft,0,m_cfStandTurnRSpeed, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
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

		// Построить путь обхода точек графа
		pMonster->vfUpdateDetourPoint();	
		pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;
		pMonster->m_tPathType = ePathTypeStraight;
		
		pMonster->vfChoosePointAndBuildPath(0,0, false, 0);

		dwMinRand = 3000;
		dwMaxRand = 5000;

	} else if (rand_val < 60) {	
		m_tAction = ACTION_STAND;

		dwMinRand = 3000;
		dwMaxRand = 5000;

	} else if (rand_val < 70) {	
		m_tAction = ACTION_LIE;
		// проверить лежит уже?
		if (pMonster->m_tAnim != eMotionLieIdle) {
			pMonster->Motion.m_tSeq.Add(eMotionLieDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tSeq.Switch();
		}

		dwMinRand = 5000;
		dwMaxRand = 7000;

	} else  {	
		m_tAction = ACTION_TURN;
		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI_DIV_2);

		dwMinRand = 800;
		dwMaxRand = 900;

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

	pEnemy				= 0;
	m_bAttackRat		= false;

	m_fDistMin			= 0.f;	
	m_fDistMax			= 0.f;

}

void CChimeraAttack::Init()
{
	IState::Init();

	// Получить врага
	VisionElem ve;
	if (!pMonster->GetEnemyFromMem(ve, pMonster->Position())) R_ASSERT(false);
	pEnemy = ve.obj;

	// Определение класса врага
	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pEnemy);
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

	// Test
	Msg("_ Attack Init _");
}

void CChimeraAttack::Run()
{
	// Выбор состояния
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	if (bAttackMelee && (pEnemy->Position().distance_to(pMonster->Position()) < m_fDistMax)) 
		m_tAction = ACTION_ATTACK_MELEE;
	else 
		m_tAction = ((pEnemy->Position().distance_to(pMonster->Position()) > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);
	
	// Выполнение состояния
	switch (m_tAction) {
		case ACTION_RUN:		// бежать на врага
			pMonster->AI_Path.DestNode = pEnemy->AI_NodeID;
			pMonster->m_tPathType = ePathTypeStraight;
			pMonster->vfChoosePointAndBuildPath(0,&pEnemy->Position(), true, 0);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfRunAttackSpeed,m_cfRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfRunAttackTurnSpeed,m_cfRunAttackTurnRSpeed,m_cfRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
			if (m_bAttackRat)

				pMonster->Motion.m_tParams.SetParams(eMotionAttackRat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			else 
				pMonster->Motion.m_tParams.SetParams(eMotionAttack,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);			
				
			pMonster->Motion.m_tTurn.Clear();
			break;
	}
}

bool CChimeraAttack::CheckCompletion() 
{
	// если враг убит
	if (!pEnemy || !pEnemy->g_Alive()) return true;
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

	// Получить инфо о трупе
	VisionElem ve;
	if (!pMonster->GetCorpseFromMem(ve, pMonster->Position())) R_ASSERT(false);
	pCorpse = ve.obj;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pCorpse);
	m_fDistToCorpse = ((tpRat)? 1.0f : 1.5f);

	SetInertia(20000);
	// Test
	Msg("_ Eat Init _");

}

void CChimeraEat::Run()
{
	bool bStartEating = (m_tAction == ACTION_RUN);

	// Выбор состояния
	if (pCorpse->Position().distance_to(pMonster->Position()) < m_fDistToCorpse) 
		m_tAction = ACTION_EAT;
	else 
		m_tAction = ACTION_RUN;

	bStartEating = bStartEating && (m_tAction == ACTION_EAT);
	if (bStartEating) {	// если монстр подбежал к трупу, необходимо отыграть проверку трупа и лечь
		pMonster->Motion.m_tSeq.Add(eMotionCheckCorpse,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tSeq.Add(eMotionLieDownEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tSeq.Switch();
	}

	// Выполнение состояния
	switch (m_tAction) {
		case ACTION_RUN:
			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->m_tPathType = ePathTypeStraight;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), false, 0);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfRunAttackSpeed,m_cfRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfRunAttackTurnSpeed,m_cfRunAttackTurnRSpeed,m_cfRunAttackMinAngle);

			break;
		case ACTION_EAT:
			pMonster->Motion.m_tParams.SetParams(eMotionEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();

			// съесть часть
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
	// если труп съеден || монстр достаточно сыт
	if ((pCorpse->m_fFood <= 0.f) || (!IsInertia())) return true;
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

	if (!pMonster->GetEnemyFromMem(m_tEnemy, pMonster->Position())) R_ASSERT(false);
	pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
	pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
	pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->m_tPathType = ePathTypeStraight;

	SetInertia(30000);

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
	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0);

	// Установить параметры движения
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfWalkSpeed,m_cfWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfWalkTurningSpeed,m_cfWalkTurnRSpeed,m_cfWalkMinAngle);
}

bool CChimeraHide::CheckCompletion()
{	
	// если большая дистанция || враг забыт
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

	if (!pMonster->GetEnemyFromMem(m_tEnemy,pMonster->Position())) R_ASSERT(false);
	pMonster->m_tPathType = ePathTypeStraight;

	SetInertia(10000);
	// Test
	Msg("_ Detour Init _");

}

void CChimeraDetour::Run()
{
	Msg("--- DETOUR ---");

	VisionElem tempEnemy;
	if (pMonster->GetEnemyFromMem(tempEnemy, pMonster->Position())) {
		m_tEnemy = tempEnemy;
		SetInertia(10000);
	}

	pMonster->vfUpdateDetourPoint();
	pMonster->AI_Path.DestNode		= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;
	
	pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.obj->Position().distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
	pMonster->m_tSelectorCover.m_fOptEnemyDistance = 15;
	pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.obj->Position().distance_to(pMonster->Position()) + 3.f;
	
	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, false, 0);

	// Установить параметры движения
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfWalkSpeed,m_cfWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfWalkTurningSpeed,m_cfWalkTurnRSpeed,m_cfWalkMinAngle);

	SetNextThink(1000);
}

bool CChimeraDetour::CheckCompletion()
{	
	// если большая дистанция || враг забыт
	if (!IsInertia()) return true;
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

	if (!pMonster->GetEnemyFromMem(m_tEnemy,pMonster->Position())) R_ASSERT(false);
	pMonster->m_tPathType = ePathTypeStraight;

	SetInertia(30000);

	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	// Test
	Msg("_ Panic Init _");

}

void CChimeraPanic::Run()
{
	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, false, 0);

	pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfRunAttackSpeed,m_cfRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfRunAttackTurnSpeed,m_cfRunAttackTurnRSpeed,m_cfRunAttackMinAngle);
}

bool CChimeraPanic::CheckCompletion()
{	
	// если большая дистанция || враг забыт
	if (!IsInertia()) return true;
	return false;
}

//---------------------------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Chimera state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Chimera::SetState(IState *pS, bool bSkipInertiaCheck)
{
	if (CurrentState != pS) {
		// проверка инерций
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
	//-- проверка специфических анимаций --
	if (Motion.m_tSeq.Started) {
		Motion.m_tSeq.Playing = true;
		Motion.m_tSeq.Started = false;
		Motion.m_tSeq.Finished = false;
		// блокировка состояния
		CurrentState->LockState();
		FORCE_ANIMATION_SELECT();
	} 

	if (!Motion.m_tSeq.Playing) {
		if (m_tAnimPrevFrame != m_tAnim) {
			FORCE_ANIMATION_SELECT();
		}	
	}
	//--------------------------------------

	// Сохранение предыдущей анимации
	m_tAnimPrevFrame = m_tAnim;
}

