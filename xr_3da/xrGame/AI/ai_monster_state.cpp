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
