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


//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// MOTION STATE MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionParams implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMotionParams::SetParams(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m)
{
	mask = m;
	anim = a;
	speed = s;
	r_speed = r_s;
	yaw = y;
	time = t;
}

void CMotionParams::ApplyData(CAI_Biting *pData)
{
	if ((mask & MASK_TIME) == MASK_TIME) {
		if (time < pData->m_dwCurrentUpdate) {
			pData->m_tAnim = DEFAULT_ANIM;
			pData->m_fCurSpeed = 0.f; 
			pData->r_torso_speed = 0.f; 
			return;
		}
	}

	if ((mask & MASK_ANIM) == MASK_ANIM) pData->m_tAnim = anim; 
	if ((mask & MASK_SPEED) == MASK_SPEED) pData->m_fCurSpeed = speed; 
	if ((mask & MASK_R_SPEED) == MASK_R_SPEED) pData->r_torso_speed = r_speed; 
	if ((mask & MASK_YAW) == MASK_YAW) pData->r_torso_target.yaw = yaw;

	pData->r_target = pData->r_torso_target;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionTurn implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMotionTurn::Set(EMotionAnim a_left, EMotionAnim a_right, float s, float r_s, float min_angle)
{

	TurnLeft.SetParams(a_left,s,r_s,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	TurnRight.SetParams(a_right,s,r_s,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);

	fMinAngle = min_angle;
}

bool CMotionTurn::CheckTurning(CAI_Biting *pData)
{
	if (0 == fMinAngle) return false;

	if (NeedToTurn(pData)){
		if (bLeftSide) TurnLeft.ApplyData(pData);
		else TurnRight.ApplyData(pData);
		return true;
	}

	return false;
}

bool CMotionTurn::NeedToTurn(CAI_Biting *pData)
{
	// Если путь построен выполнить SetDirectionLook 
	if (!pData->AI_Path.TravelPath.empty() && (pData->AI_Path.TravelPath.size() > 1)) {
		pData->SetDirectionLook();
	} 

	if (!getAI().bfTooSmallAngle(pData->r_torso_target.yaw,pData->r_torso_current.yaw, fMinAngle)) { // если угол не маленький то...
		if (angle_normalize_signed(pData->r_torso_target.yaw - pData->r_torso_current.yaw) > 0) bLeftSide = false;
		else bLeftSide = true;
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionSequence implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMotionSequence::Init()
{
	States.clear();
	it = 0;
	Playing = Started = Finished = false;
}

void CMotionSequence::Add(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m)
{
	CMotionParams tS;
	tS.SetParams(a,s,r_s,y,t,m);

	States.push_back(tS);
}

void CMotionSequence::Switch()
{
		Started = true;
		if (it == 0) it = States.begin();
		else {
			it++; 
			if (it != States.end()) Started = true;	
			else {
				Finish();
				return;
			}
		}
}

void CMotionSequence::ApplyData(CAI_Biting *pData)
{
	it->ApplyData(pData);
}

void CMotionSequence::Finish()
{
	Init(); Finished = true;
}

void CMotionSequence::Cycle(u32 cur_time)
{
	if (((it->mask & MASK_TIME) == MASK_TIME) && (cur_time > it->time))	Switch();
}

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

//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATE MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// IState implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
IState::IState(CAI_Biting *p) 
{
	pData = p;
}

void IState::Execute(bool bNothingChanged) 
{
	if (m_bLocked) return;

	m_dwCurrentTime = pData->m_dwCurrentUpdate;

	switch (m_tState) {
		case STATE_NOT_ACTIVE:
		case STATE_INIT:
			Init();
		case STATE_RUN:
			if (!CheckCompletion()) {
				if (m_dwNextThink < m_dwCurrentTime) {
					Run();
				}
				break;
			}
		case STATE_DONE:
			Done();
			break;
	}
}

void IState::Init()
{
	m_dwNextThink = m_dwCurrentTime;
	m_tState = STATE_RUN;
}
void IState::Run()
{
	
}

void IState::Done()
{
	Reset();
}

void IState::Reset()
{
	m_dwCurrentTime		= 0;
	m_dwNextThink		= 0;
	m_dwTimeLocked		= 0;
	m_tState			= STATE_NOT_ACTIVE;	

	m_bLocked			= false;
}

void IState::LockState()
{
	m_dwTimeLocked = m_dwCurrentTime;
	m_bLocked		= true;
}

//Info: если в классах-потомках, используются дополнительные поля времени,
//      метод UnlockState() должне быть переопределены 
TTime IState::UnlockState(TTime cur_time)
{
	TTime dt = (m_dwCurrentTime = cur_time) - m_dwTimeLocked;
	
	m_dwNextThink  += dt;
	m_bLocked		= false;

	return dt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CRest::CRest(CAI_Biting *p)  
	: IState(p) 
{
	Reset();
}


void CRest::Reset()
{
	IState::Reset();

	m_dwReplanTime		= 0;
	m_dwLastPlanTime	= 0;

	m_tAction			= ACTION_STAND;
}

void CRest::Run()
{
	// проверить нужно ли провести перепланировку
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();

	// FSM 2-го уровня
	switch (m_tAction) {
		case ACTION_WALK:
			pData->Motion.m_tParams.SetParams(eMotionWalkFwd,m_cfWalkSpeed,m_cfWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfWalkTurningSpeed,m_cfWalkTurnRSpeed,m_cfWalkMinAngle);
			break;
		case ACTION_STAND:
			pData->Motion.m_tParams.SetParams(eMotionStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Clear();
			break;
		case ACTION_LIE:
			pData->Motion.m_tParams.SetParams(eMotionLieIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Clear();
			break;
		case ACTION_TURN:
			pData->Motion.m_tParams.SetParams(eMotionStandTurnLeft,0,m_cfStandTurnRSpeed, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Clear();
			break;
	}
}

void CRest::Replanning()
{
	m_dwLastPlanTime = m_dwCurrentTime;	
	u32		rand_val = ::Random.randI(100);
	u32		dwMinRand = 0, dwMaxRand = 0;

	if (rand_val < 50) {	
		m_tAction = ACTION_WALK;

		// Построить путь обхода точек графа
		pData->vfUpdateDetourPoint();	
		pData->AI_Path.DestNode	= getAI().m_tpaGraph[pData->m_tNextGP].tNodeID;
		pData->m_tPathType = ePathTypeStraight;
		
		pData->vfChoosePointAndBuildPathAtOnce(0,0, false, 0);

		dwMinRand = 3000;
		dwMaxRand = 5000;

	} else if (rand_val < 60) {	
		m_tAction = ACTION_STAND;

		dwMinRand = 3000;
		dwMaxRand = 5000;

	} else if (rand_val < 70) {	
		m_tAction = ACTION_LIE;
		// проверить лежит уже?
		if (pData->m_tAnim != eMotionLieIdle) {
			pData->Motion.m_tSeq.Add(eMotionLieDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tSeq.Switch();
		}

		dwMinRand = 5000;
		dwMaxRand = 7000;

	} else  {	
		m_tAction = ACTION_TURN;
		pData->r_torso_target.yaw = angle_normalize(pData->r_torso_target.yaw + PI_DIV_2);

		dwMinRand = 800;
		dwMaxRand = 900;

	}
	
	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
	SetNextThink(dwMinRand);
}


TTime CRest::UnlockState(TTime cur_time)
{
	TTime dt = inherited::UnlockState(cur_time);

	m_dwReplanTime		+= dt;
	m_dwLastPlanTime	+= dt;

	return dt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CAttack::CAttack(CAI_Biting *p)  
	: IState(p) 
{
	Reset();
}


void CAttack::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	pEnemy				= 0;
	m_bAttackRat		= false;

	m_fDistMin			= 0.f;	
	m_fDistMax			= 0.f;

}

void CAttack::Init()
{
	IState::Init();

	// Получить врага
	VisionElem ve;
	if (!pData->SelectEnemy(ve)) R_ASSERT(false);
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

}

void CAttack::Run()
{
	// Выбор состояния
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	if (bAttackMelee && (pEnemy->Position().distance_to(pData->Position()) < m_fDistMax)) 
		m_tAction = ACTION_ATTACK_MELEE;
	else 
		m_tAction = ((pEnemy->Position().distance_to(pData->Position()) > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);
	
	// Выполнение состояния
	switch (m_tAction) {
		case ACTION_RUN:
			pData->AI_Path.DestNode = pEnemy->AI_NodeID;
			pData->m_tPathType = ePathTypeStraight;
			pData->vfChoosePointAndBuildPathAtOnce(0,&pEnemy->Position(), false, 0);

			pData->Motion.m_tParams.SetParams(eMotionRun,m_cfRunAttackSpeed,m_cfRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfRunAttackTurnSpeed,m_cfRunAttackTurnRSpeed,m_cfRunAttackMinAngle);

			SetNextThink(300);	// чем больше расстояние, тем больше SetNextThink

			break;
		case ACTION_ATTACK_MELEE:
			if (m_bAttackRat)

				pData->Motion.m_tParams.SetParams(eMotionAttackRat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			else 
				pData->Motion.m_tParams.SetParams(eMotionAttack,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);			
				
			pData->Motion.m_tTurn.Clear();
			SetNextThink(400);
			break;
	}
}

bool CAttack::CheckCompletion() 
{
	// если враг убит
	if (!pEnemy || !pEnemy->g_Alive()) return true;
	return false;
}	


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CEat::CEat(CAI_Biting *p)  
	: IState(p) 
{
	Reset();
}


void CEat::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	pCorpse				= 0;
}

void CEat::Init()
{
	IState::Init();

	// Получить инфо о трупе
	VisionElem ve;
	if (!pData->SelectCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pCorpse);
	m_fDistToCorpse = ((tpRat)? 1.0f : 1.5f);
}

void CEat::Run()
{
	bool bStartEating = (m_tAction == ACTION_RUN);

	// Выбор состояния
	if (pCorpse->Position().distance_to(pData->Position()) < m_fDistToCorpse) 
		m_tAction = ACTION_EAT;
	else 
		m_tAction = ACTION_RUN;

	bStartEating = bStartEating && (m_tAction == ACTION_EAT);
	if (bStartEating) {
		pData->Motion.m_tSeq.Add(eMotionCheckCorpse,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pData->Motion.m_tSeq.Add(eMotionLieDownEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pData->Motion.m_tSeq.Switch();
	}

	// Выполнение состояния
	switch (m_tAction) {
		case ACTION_RUN:
			pData->AI_Path.DestNode = pCorpse->AI_NodeID;
			pData->m_tPathType = ePathTypeStraight;
			pData->vfChoosePointAndBuildPathAtOnce(0,&pCorpse->Position(), false, 0);

			pData->Motion.m_tParams.SetParams(eMotionRun,m_cfRunAttackSpeed,m_cfRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, m_cfRunAttackTurnSpeed,m_cfRunAttackTurnRSpeed,m_cfRunAttackMinAngle);

			if (pCorpse->Position().distance_to(pData->Position()) > m_fDistToCorpse*5)
				SetNextThink(300);	// чем больше расстояние, тем больше SetNextThink

			break;
		case ACTION_EAT:
			pData->Motion.m_tParams.SetParams(eMotionEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Clear();

			SetNextThink(400);
			break;
	}
}

bool CEat::CheckCompletion() 
{
	// если труп съеден || монстр достаточно сыт
	return false;
}	



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHide class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CHide::CHide(CAI_Biting *p)
	: inherited(p) 
{
	Reset();
}

void CHide::Init()
{
	inherited::Init();

	if (!pData->SelectEnemy(m_tEnemy)) R_ASSERT(false);
	pData->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pData->Position()) + pData->m_tSelectorCover.m_fSearchRange;
	pData->m_tSelectorCover.m_fOptEnemyDistance = pData->m_tSelectorCover.m_fMaxEnemyDistance;
	pData->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pData->Position()) + 3.f;

	pData->m_tPathType = ePathTypeStraight;
}

void CHide::Reset()
{
	inherited::Reset();

	m_tEnemy.obj		= 0;
}

void CHide::Run()
{
	pData->vfChoosePointAndBuildPath(&pData->m_tSelectorCover, 0, true, 0);

	// Установить параметры движения
	pData->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfWalkSpeed,m_cfWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pData->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfWalkTurningSpeed,m_cfWalkTurnRSpeed,m_cfWalkMinAngle);
}

bool CHide::CheckCompletion()
{	
	// если большая дистанция || враг забыт
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDetour class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CDetour::CDetour(CAI_Biting *p)
		: inherited(p) 
{
	Reset();
}

void CDetour::Reset()
{
	inherited::Reset();
	m_tEnemy.obj		= 0;
}

void CDetour::Init()
{
	inherited::Init();

	if (!pData->SelectEnemy(m_tEnemy)) R_ASSERT(false);

	pData->m_tPathType = ePathTypeStraight;
}

void CDetour::Run()
{
	pData->vfUpdateDetourPoint();
	pData->AI_Path.DestNode		= getAI().m_tpaGraph[pData->m_tNextGP].tNodeID;
	
	pData->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.obj->Position().distance_to(pData->Position()) + pData->m_tSelectorCover.m_fSearchRange;
	pData->m_tSelectorCover.m_fOptEnemyDistance = 15;
	pData->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.obj->Position().distance_to(pData->Position()) + 3.f;
	
	pData->vfChoosePointAndBuildPath(&pData->m_tSelectorCover, 0, false, 0);

	// Установить параметры движения
	pData->Motion.m_tParams.SetParams	(eMotionWalkFwd,m_cfWalkSpeed,m_cfWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pData->Motion.m_tTurn.Set			(eMotionWalkTurnLeft, eMotionWalkTurnRight,m_cfWalkTurningSpeed,m_cfWalkTurnRSpeed,m_cfWalkMinAngle);

	SetNextThink(1000);
}

bool CDetour::CheckCompletion()
{	
	// если большая дистанция || враг забыт
	return false;
}


//---------------------------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Biting state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Biting::SetState(IState *pS)
{
	if (CurrentState != pS) {
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

