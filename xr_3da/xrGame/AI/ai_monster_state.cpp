////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monster_state.cpp
//	Created 	: 06.07.2003
//  Modified 	: 06.07.2003
//	Author		: Serge Zhem
//	Description : FSM states and motion
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\CustomMonster.h"
#include "ai_monster_state.h"

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

void CMotionParams::ApplyData(CCustomMonster *pData)
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

bool CMotionTurn::CheckTurning(CCustomMonster *pData)
{
	if (0 == fMinAngle) return false;

	if (NeedToTurn(pData)){
		if (bLeftSide) TurnLeft.ApplyData(pData);
		else TurnRight.ApplyData(pData);
		return true;
	}

	return false;
}

#define R2D(x) (angle_normalize(x)*180.f/PI)

bool CMotionTurn::NeedToTurn(CCustomMonster *pData)
{
	Msg("--------------------------------------------------------------------------------");
	Msg("BEFOR:: time = [%i]  current = [%f]  target = [%f]", pData->m_dwCurrentUpdate, 
		R2D(pData->r_torso_current.yaw), R2D(pData->r_torso_target.yaw));

	// Если путь построен выполнить SetDirectionLook 
	if (!pData->AI_Path.TravelPath.empty() && (pData->AI_Path.TravelPath.size() > 1)) {
		pData->SetDirectionLook();
	} 

	Msg("AFTER:: time = [%i]  current = [%f]  target = [%f]", pData->m_dwCurrentUpdate, 
		R2D(pData->r_torso_current.yaw), R2D(pData->r_torso_target.yaw));



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

void CMotionSequence::ApplyData(CCustomMonster *pData)
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



//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATE MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// IState implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
IState::IState() 
{
}

void IState::Reset()
{
	m_dwCurrentTime		= 0;
	m_dwNextThink		= 0;
	m_dwTimeLocked		= 0;
	m_tState			= STATE_NOT_ACTIVE;	

	m_bLocked			= false;

	m_dwInertia			= 0;
	m_tPriority			= PRIORITY_NONE;
}


void IState::Execute(TTime cur_time) 
{
	if (m_bLocked) return;

	m_dwCurrentTime = cur_time;

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
