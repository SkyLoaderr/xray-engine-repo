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
// CMotionParams implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMotionParams::SetParams(AI_Biting::EPostureAnim p, AI_Biting::EActionAnim a, float s, float r_s, float y, TTime t, u32 m)
{
	mask =m;
	posture = p;
	action = a;
	speed = s;
	r_speed = r_s;
	yaw = y;
	speed = s;
}

void CMotionParams::ApplyData(CAI_Biting *pData)
{
	if ((mask & MASK_ANIM) == MASK_ANIM) {
		pData->m_tPostureAnim = posture; 
		pData->m_tActionAnim = action;
	}

	if ((mask & MASK_SPEED) == MASK_SPEED) pData->m_fCurSpeed = speed; 
	if ((mask & MASK_R_SPEED) == MASK_R_SPEED) pData->r_torso_speed = r_speed; 
	if ((mask & MASK_YAW) == MASK_YAW) pData->r_torso_target.yaw = yaw;

	pData->r_target = pData->r_torso_target;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionTurn implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMotionTurn::Set(AI_Biting::EPostureAnim p_left, AI_Biting::EActionAnim a_left, 
					  AI_Biting::EPostureAnim p_right, AI_Biting::EActionAnim a_right,
					  float s, float r_s, float min_angle)
{

	TurnLeft.SetParams(p_left,a_left,s,r_s,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	TurnRight.SetParams(p_right,a_right,s,r_s,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);

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

void CMotionSequence::Add(AI_Biting::EPostureAnim p, AI_Biting::EActionAnim a, float s, float r_s, float y, TTime t, u32 m)
{
	CMotionParams tS;
	tS.SetParams(p,a,s,r_s,y,t,m);

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
		if ((pData->m_tActionAnimPrevFrame == eActionIdle) && (pData->m_tPostureAnimPrevFrame == ePostureLie) &&
			((pData->m_tActionAnim != eActionIdle) || (pData->m_tPostureAnim != ePostureLie))) {
				m_tSeq.Add(ePostureLie,eActionStandUp,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
				m_tSeq.Switch();
				m_tSeq.ApplyData(pData);
			}

		//!---
	} else {
		m_tSeq.ApplyData(pData);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// IState implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
IState::IState(CAI_Biting *p) 
{
	pData = p;
}

void IState::Execute(bool bNothingChanged) 
{
	m_dwCurrentTime = pData->m_dwCurrentUpdate;

	switch (m_tState) {
		case STATE_NOT_ACTIVE:
		case STATE_INIT:
			Init();
		case STATE_RUN:
			if (!CheckCompletion()) {
				Run();
				break;
			}
		case STATE_DONE:
			Done();
			break;
	}
}

void IState::Init()
{
	m_dwStartTime = m_dwCurrentTime;
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
	m_dwStartTime		= 0;
	m_dwCurrentTime		= 0;
	m_tState			= STATE_NOT_ACTIVE;	
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
			pData->Motion.m_tParams.SetParams(ePostureStand,eActionWalkFwd,m_cfWalkSpeed,m_cfWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Set(ePostureStand,eActionWalkTurnLeft,ePostureStand,eActionWalkTurnRight, 
									  m_cfWalkTurningSpeed,m_cfWalkTurnRSpeed,m_cfWalkMinAngle);
			break;
		case ACTION_STAND:
			pData->Motion.m_tParams.SetParams(ePostureStand,eActionIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Clear();
			break;
		case ACTION_LIE:
			pData->Motion.m_tParams.SetParams(ePostureLie,eActionIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Clear();
			break;
		case ACTION_TURN:
			pData->Motion.m_tParams.SetParams(ePostureStand,eActionIdleTurnLeft,0,m_cfStandTurnRSpeed, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
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
		pData->Motion.m_tSeq.Add(ePostureStand,eActionLieDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pData->Motion.m_tSeq.Switch();

		dwMinRand = 5000;
		dwMaxRand = 7000;

	} else  {	
		m_tAction = ACTION_TURN;
		pData->r_torso_target.yaw = angle_normalize(pData->r_torso_target.yaw + PI_DIV_2);

		dwMinRand = 1200;
		dwMaxRand = 1700;

	}
	
	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);;
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

			pData->Motion.m_tParams.SetParams(ePostureStand,eActionRun,m_cfRunAttackSpeed,m_cfRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Set(ePostureStand,eActionRunTurnLeft,ePostureStand,eActionRunTurnRight, 
									  m_cfRunAttackTurnSpeed,m_cfRunAttackTurnRSpeed,m_cfRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:
			if (m_bAttackRat)

				pData->Motion.m_tParams.SetParams(ePostureStand,eActionAttack2,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			else 
				pData->Motion.m_tParams.SetParams(ePostureStand,eActionAttack,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);			
				
			pData->Motion.m_tTurn.Clear();
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
		FORCE_ANIMATION_SELECT();
	} 

	if (!Motion.m_tSeq.Playing) {
		if (m_tActionAnimPrevFrame != m_tActionAnim || m_tPostureAnimPrevFrame != m_tPostureAnim) {
			FORCE_ANIMATION_SELECT();
		}	
	}
	//--------------------------------------

	// Сохранение предыдущей анимации
	m_tActionAnimPrevFrame = m_tActionAnim;
	m_tPostureAnimPrevFrame = m_tPostureAnim;
}
