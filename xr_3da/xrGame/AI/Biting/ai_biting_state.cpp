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

void CMotionParams::SetData(CAI_Biting *pData)
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
		if (bLeftSide) TurnLeft.SetData(pData);
		else TurnRight.SetData(pData);
		return true;
	}

	return false;
}

bool CMotionTurn::NeedToTurn(CAI_Biting *pData)
{
	// ���� ���� �������� ��������� SetDirectionLook 
	if (!pData->AI_Path.TravelPath.empty() && (pData->AI_Path.TravelPath.size() > 1)) {
		pData->SetDirectionLook();
	} 

	if (!getAI().bfTooSmallAngle(pData->r_torso_target.yaw,pData->r_torso_current.yaw, fMinAngle)) { // ���� ���� �� ��������� ��...
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

void CMotionSequence::Switch(CAI_Biting *pData)
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

		it->SetData(pData);
}
void CMotionSequence::Finish()
{
	Init(); Finished = true;
}

void CMotionSequence::Cycle(u32 cur_time, CAI_Biting *pData)
{
	if (((it->mask & MASK_TIME) == MASK_TIME) && (cur_time > it->time))	Switch(pData);
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
	// ��������� ����� �� �������� ��������������
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();

	//if (NoEvents()) return;
	
	// FSM 2-�� ������
	switch (m_tAction) {
		case ACTION_WALK:
			pData->Motion.m_tParams.SetParams(ePostureStand,eActionWalkFwd,m_cfWalkSpeed,m_cfWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Set(ePostureStand,eActionWalkTurnLeft,ePostureStand,eActionWalkTurnRight, 
									  m_cfWalkSpeed,m_cfWalkTurnRSpeed,m_cfWalkMinAngle);
			break;
		case ACTION_STAND:
			pData->Motion.m_tParams.SetParams(ePostureStand,eActionIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Clear();
			break;
		case ACTION_LIE:
			if (!pData->MotionSeq.Active()) {
				pData->Motion.m_tParams.SetParams(ePostureLie,eActionIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
				pData->Motion.m_tTurn.Clear();
			}
			break;
		case ACTION_TURN:
			pData->Motion.m_tParams.SetParams(ePostureStand,eActionIdleTurnLeft,0,m_cfStandTurnRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pData->Motion.m_tTurn.Clear();
			break;
	}

}

void CRest::Replanning()
{
	m_dwLastPlanTime = m_dwCurrentTime;	
	u32		rand_val = ::Random.randI(100);

	if (rand_val < 10) {	
		m_tAction = ACTION_WALK;

		// ��������� ���� ������ ����� �����
		pData->vfUpdateDetourPoint();	
		pData->AI_Path.DestNode	= getAI().m_tpaGraph[pData->m_tNextGP].tNodeID;
		pData->m_tPathType = ePathTypeStraight;
		
		pData->vfChoosePointAndBuildPathAtOnce(0,0, false, 0);

	} else if (rand_val < 11) {	
		m_tAction = ACTION_STAND;
	} else if (rand_val < 90) {	
		m_tAction = ACTION_LIE;
		pData->MotionSeq.Add(ePostureStand,eActionLieDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pData->MotionSeq.Switch(pData);
	} else  {	
		m_tAction = ACTION_TURN;
		pData->r_torso_target.yaw += PI_DIV_2;
		pData->r_torso_target.yaw = angle_normalize(pData->r_torso_target.yaw + PI_DIV_2);
	}
	
	m_dwReplanTime = 5000;
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

	// �������� �����
	VisionElem ve;
	if (!pData->Mem.SelectEnemy(ve)) R_ASSERT(false);
	pEnemy = ve.obj;

	// ����������� ������ �����
	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pEnemy);
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

}

void CAttack::Run()
{
	// ����� ���������
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	if (bAttackMelee && (pEnemy->Position().distance_to(pData->vPosition) < m_fDistMax)) 
		m_tAction = ACTION_ATTACK_MELEE;
	else 
		m_tAction = ((pEnemy->Position().distance_to(pData->vPosition) > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);
	
	// ���������� ���������
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
	// ���� ���� ����
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
	//-- �������� ������������� �������� --
	if (MotionSeq.Started) {
		MotionSeq.Playing = true;
		MotionSeq.Started = false;
		MotionSeq.Finished = false;	
		FORCE_ANIMATION_SELECT();
	} 

	if (!MotionSeq.Playing) {
		if (m_tActionAnimPrevFrame != m_tActionAnim || m_tPostureAnimPrevFrame != m_tPostureAnim) {
			FORCE_ANIMATION_SELECT();
		}	
	}
	//--------------------------------------

	m_tActionAnimPrevFrame	= m_tActionAnim;
	m_tPostureAnimPrevFrame	= m_tPostureAnim;
}
