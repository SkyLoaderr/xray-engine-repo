////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\ai_monsters_misc.h"

#undef	WRITE_TO_LOG
//Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
//#define WRITE_TO_LOG(s) m_bStopThinking = true;
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
			case eStalkerStateDrop : {
				Drop();
				break;
			}
			case eStalkerStateLookingOver : {
				LookingOver();
				break;
			}
			case eStalkerStateSearching: {
				Searching();
				break;
			}
		}
		m_bStateChanged		= m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
};

void CAI_Stalker::vfAddStateToList(EStalkerStates eState)
{
	if ((m_tStateList.size()) && (m_tStateList[m_tStateList.size() - 1].eState == eState)) {
		m_tStateList[m_tStateList.size() - 1].dwTime = m_dwCurrentUpdate;
		return;
	}
	if (m_tStateList.size() >= MAX_STATE_LIST_SIZE)
		m_tStateList.erase(u32(0));
	SStalkerStates tStalkerStates;
	tStalkerStates.dwTime = m_dwCurrentUpdate;
	tStalkerStates.eState = eState;
	m_tStateList.push_back(tStalkerStates);
};

void CAI_Stalker::Death()
{
	WRITE_TO_LOG("Death");
	
	DropItem();
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
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Weapons->ActiveWeapon()->GetAmmoElapsed());
}

void CAI_Stalker::Drop()
{
	WRITE_TO_LOG("Drop");
	VERIFY(Weapons->ActiveWeapon());
	DropItem();
}

void CAI_Stalker::LookingOver()
{
	WRITE_TO_LOG("Looking over");
	m_fCurSpeed = 0.f;
	SWITCH_TO_NEW_STATE_AND_UPDATE(eStalkerStateSearching);
}

void CAI_Stalker::Searching()
{
	WRITE_TO_LOG("Searching");

	vfChoosePointAndBuildPath(m_tSelectorFreeHunting);

	vfSetMovementType(eBodyStateStand,eMovementTypeWalk,eLookTypeView);
}
