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


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingRest::CBitingRest(CAI_Biting *p)  
{
	pMonster = p;
	Reset();

	SetLowPriority();			// удиноразовая утановка приоритета
}


void CBitingRest::Reset()
{
	IState::Reset();

	m_dwReplanTime		= 0;
	m_dwLastPlanTime	= 0;

	m_tAction			= ACTION_STAND;
	
}

void CBitingRest::Run()
{
	// проверить нужно ли провести перепланировку
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();

	// FSM 2-го уровня
	switch (m_tAction) {
		case ACTION_WALK:
			// Построить путь обхода точек графа
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;
			
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0);

			pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);
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

	pEnemy				= 0;
	m_bAttackRat		= false;

	m_fDistMin			= 0.f;	
	m_fDistMax			= 0.f;

}

void CBitingAttack::Init()
{
	IState::Init();

	// Получить врага
	VisionElem ve;
	if (!pMonster->GetEnemyFromMem(ve,pMonster->Position())) R_ASSERT(false);
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

void CBitingAttack::Run()
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
			pMonster->vfChoosePointAndBuildPath(0,&pEnemy->Position(), false, 0);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);

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

bool CBitingAttack::CheckCompletion() 
{
	// если враг убит
	if (!pEnemy || !pEnemy->g_Alive()) return true;
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

	// Получить инфо о трупе
	VisionElem ve;
	if (!pMonster->GetCorpseFromMem(ve,pMonster->Position())) R_ASSERT(false);
	pCorpse = ve.obj;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pCorpse);
	m_fDistToCorpse = ((tpRat)? 1.0f : 1.5f);

	SetInertia(20000);
	// Test
	Msg("_ Eat Init _");

}

void CBitingEat::Run()
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

			if (pMonster->m_tPathState != ePathStateBuilt) {
				pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
				pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0);
			}
			
			pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);

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

bool CBitingEat::CheckCompletion() 
{
	// если труп съеден || монстр достаточно сыт
	if (pCorpse && (pCorpse->m_fFood <= 0.f) || (!IsInertia())) {
		Msg("--Eating complete! -- ");
		return true;
	}
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

	if (!pMonster->GetEnemyFromMem(m_tEnemy,pMonster->Position())) R_ASSERT(false);
	pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
	pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
	pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->m_tPathType = ePathTypeStraight;

	SetInertia(30000);

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
	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0);

	// Установить параметры движения
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);
}

bool CBitingHide::CheckCompletion()
{	
	// если большая дистанция || враг забыт
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

	if (!pMonster->GetEnemyFromMem(m_tEnemy,pMonster->Position())) R_ASSERT(false);
	pMonster->m_tPathType = ePathTypeStraight;

	SetInertia(10000);
	// Test
	Msg("_ Detour Init _");

}

void CBitingDetour::Run()
{
	Msg("--- DETOUR ---");

	VisionElem tempEnemy;
	if (pMonster->GetEnemyFromMem(tempEnemy,pMonster->Position())) {
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
	pMonster->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfBitingWalkSpeed,m_cfBitingWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfBitingWalkTurningSpeed,m_cfBitingWalkTurnRSpeed,m_cfBitingWalkMinAngle);

	SetNextThink(1000);
}

bool CBitingDetour::CheckCompletion()
{	
	// если большая дистанция || враг забыт
	if (!IsInertia()) return true;
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

	if (!pMonster->GetEnemyFromMem(m_tEnemy,pMonster->Position())) R_ASSERT(false);
	pMonster->m_tPathType = ePathTypeStraight;

	SetInertia(30000);

	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	// Test
	Msg("_ Panic Init _");

}

void CBitingPanic::Run()
{
	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, false, 0);

	pMonster->Motion.m_tParams.SetParams(eMotionRun,m_cfBitingRunAttackSpeed,m_cfBitingRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfBitingRunAttackTurnSpeed,m_cfBitingRunAttackTurnRSpeed,m_cfBitingRunAttackMinAngle);
}

bool CBitingPanic::CheckCompletion()
{	
	// если большая дистанция || враг забыт
	if (!IsInertia()) return true;
	return false;
}

//---------------------------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Biting state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Biting::SetState(IState *pS, bool bSkipInertiaCheck)
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

void CAI_Biting::ControlAnimation()
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

