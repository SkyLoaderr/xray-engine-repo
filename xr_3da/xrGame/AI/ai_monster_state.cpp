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
// CMotionSequence implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMotionSequence::Init()
{
	States.clear();
	it = States.end();
	Playing = false;
}

void CMotionSequence::Add(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m,u32 s_m)
{
	CMotionParams tS;
	tS.SetParams(a,s,r_s,y,t,m,s_m);

	States.push_back(tS);
}

void CMotionSequence::Switch()
{
	if (!Playing) it = States.begin();
	else {
		it++; 
		if (it == States.end()) {
			Finish();
			return;
		}
	}

	Playing = true;

	ApplyData();
	pMonster->OnMotionSequenceStart();
}

void CMotionSequence::ApplyData()
{
	it->ApplyData(pMonster);
}

void CMotionSequence::Finish()
{
	Init(); 
	pMonster->OnMotionSequenceEnd();
}


// Выполнить проверки на завершение текущего элемента последовательности (по STOP маскам)
void CMotionSequence::Cycle(u32 cur_time)
{
	if (((it->stop_mask & STOP_TIME_OUT) == STOP_TIME_OUT) && (cur_time > it->time) ||																				// если TIME_OUT
		(((it->stop_mask & STOP_AT_TURNED) == STOP_AT_TURNED) && (getAI().bfTooSmallAngle(pMonster->r_torso_target.yaw, pMonster->r_torso_current.yaw, EPS_L))))	// если необходима остановка, когда произведен поворот 
	{ 
		Switch();
	}
}

void CMotionSequence::OnAnimEnd()
{
	if (Playing) {			// Sequence active?
		if ((it->stop_mask & STOP_ANIM_END) == STOP_ANIM_END) Switch();
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
IState::IState() 
{
	m_tPriority			= PRIORITY_NONE;
}

void IState::Reset()
{
	m_dwCurrentTime		= 0;
	m_dwNextThink		= 0;
	m_dwTimeLocked		= 0;
	m_tState			= STATE_NOT_ACTIVE;	

	m_bLocked			= false;

	m_dwInertia			= 0;
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
	m_dwNextThink			= m_dwCurrentTime;
	m_tState				= STATE_RUN;
	m_dwStateStartedTime	= m_dwCurrentTime;
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
//      метод UnlockState() должен быть переопределен
TTime IState::UnlockState(TTime cur_time)
{
	TTime dt = (m_dwCurrentTime = cur_time) - m_dwTimeLocked;

	m_dwNextThink  += dt;
	m_bLocked		= false;

	return dt;
}
