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

void CAI_Stalker::vfUpdateSearchPosition()
{
	if (!g_Alive())
		return;
	INIT_SQUAD_AND_LEADER;
	if (this != Leader)	{
		CAI_Stalker *tpLeader = dynamic_cast<CAI_Stalker*>(Leader);
		if (tpLeader) {
			if (m_tNextGraphPoint.distance_to(tpLeader->m_tNextGraphPoint) > EPS_L)
				m_eCurrentState = eStalkerStateSearching;
			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
		}
	}
	else {
		//Msg("%s : %d -> %d (%d)",cName(),m_tCurGP,m_tNextGP,m_dwTimeToChange);
		if ((Level().timeServer() >= m_dwTimeToChange) && (getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex == m_tNextGP)) {
			m_tNextGP					= getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
			vfChooseNextGraphPoint		();
			m_tNextGraphPoint.set		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
			AI_Path.DestNode			= getAI().m_tpaGraph[m_tNextGP].tNodeID;
			//Msg("Next graph point %d",m_tNextGP);
		}
		else
			AI_Path.DestNode			= getAI().m_tpaGraph[m_tNextGP].tNodeID;
	}
}

void CAI_Stalker::Think()
{
	//vfUpdateSearchPosition();
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
			case eStalkerStateFiring: {
				Firing();
				break;
			}
			case eStalkerStateUnderFire: {
				UnderFire();
				break;
			}
			case eStalkerStatePursuit: {
				Pursuit();
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
//	WRITE_TO_LOG("Searching");
//
////	SelectEnemy(m_Enemy);
//
////	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_Enemy.Enemy,eStalkerStateFiring);
//
////	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(Level().timeServer() - m_dwLastHitTime > 3000,eStalkerStateUnderFire);
//
//	if (!AI_Path.Nodes.size() || (AI_Path.Nodes[AI_Path.Nodes.size() - 1] != AI_Path.DestNode))
//		vfBuildPathToDestinationPoint		(0);
//
//	vfSetMovementType(eBodyStateStand,eMovementTypeWalk,eLookTypeSearch);
//	if (m_fCurSpeed < EPS_L)
//		r_torso_target.yaw = r_target.yaw;
	WRITE_TO_LOG("Searching");

	vfChoosePointAndBuildPath(m_tSelectorFreeHunting);

	Fvector tDummy;
	u32		dwTime = Level().timeServer();
	tDummy.setHP(angle_normalize_signed(2*PI*dwTime/20000),0);
	vfSetMovementType(eBodyStateStand,eMovementTypeWalk,eLookTypePoint,tDummy);

	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw = r_target.yaw;
}

void CAI_Stalker::Recharge()
{
	WRITE_TO_LOG("Recharge");
	VERIFY(Weapons->ActiveWeapon());
	Weapons->ActiveWeapon()->Reload();
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Weapons->ActiveWeapon()->GetAmmoElapsed());
}

void CAI_Stalker::UnderFire()
{
	WRITE_TO_LOG("Under fire");


}

void CAI_Stalker::Pursuit()
{
	WRITE_TO_LOG("Pursuit");
}

void CAI_Stalker::Firing()
{
	WRITE_TO_LOG("Searching");

	SelectEnemy(m_Enemy);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_Enemy.Enemy);

//	vfChoosePointAndBuildPath();

	vfSetMovementType(eBodyStateStand,eMovementTypeWalk,eLookTypePoint,m_Enemy.Enemy->Position());

//	vfSetFire(true);
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw = r_target.yaw;
}