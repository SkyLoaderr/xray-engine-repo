////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\ai_monsters_misc.h"

#undef	WRITE_TO_LOG
//#define WRITE_TO_LOG(s) m_bStopThinking = true;
	//Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);
#define WRITE_TO_LOG(s) {\
	m_bStopThinking = true;\
}

#ifndef DEBUG
	#undef	WRITE_TO_LOG
	#define WRITE_TO_LOG(s) m_bStopThinking = true;
#endif

void CAI_Stalker::Think()
{
	m_dwUpdateCount++;
	m_dwLastUpdate			= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();
	m_bStopThinking			= false;
	do {
		m_ePreviousState	= m_eCurrentState;
		switch (m_eCurrentState) {
			case eStalkerStateDie : {
				Death();
				break;
			}
			case eStalkerStateTurnOver : {
				TurnOver();
				break;
			}
			case eStalkerStateWaitForAnimation : {
				WaitForAnimation();
				break;
			}
			case eStalkerStateWaitForTime : {
				WaitForTime();
				break;
			}
			case eStalkerStateRecharge : {
				Recharge();
				break;
			}
			case eStalkerStateLookingOver : {
				LookingOver();
				break;
			}
		}
		m_bStateChanged		= m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
};

void CAI_Stalker::Death()
{
	WRITE_TO_LOG("Death");
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
}

void CAI_Stalker::TurnOver()
{
	WRITE_TO_LOG("Turn over");
}

void CAI_Stalker::WaitForAnimation()
{
	WRITE_TO_LOG("Wait for animation");
}

void CAI_Stalker::WaitForTime()
{
	WRITE_TO_LOG("Wait for time");
}

void CAI_Stalker::Recharge()
{
	WRITE_TO_LOG("Recharge");
	VERIFY(Weapons->ActiveWeapon());
	Weapons->ActiveWeapon()->Reload();
	CHECK_IF_GO_TO_PREV_STATE(Weapons->ActiveWeapon()->GetAmmoCurrent());
}

void CAI_Stalker::LookingOver()
{
	WRITE_TO_LOG("Looking over");
	
	if (Weapons->ActiveWeapon())
		Weapons->ActiveWeapon()->FireEnd();
	if (!::Random.randI(100))
		if (Weapons->ActiveWeapon()) {
			CHECK_IF_SWITCH_TO_NEW_STATE(!Weapons->ActiveWeapon()->GetAmmoCurrent(),eStalkerStateRecharge);
			Weapons->ActiveWeapon()->FireStart();
		}
}
