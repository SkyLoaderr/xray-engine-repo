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
//#define WRITE_TO_LOG(s) m_bStopThinking = true;
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
	m_bStopThinking = true;\
}

#ifndef DEBUG
	#undef	WRITE_TO_LOG
	#define WRITE_TO_LOG(s) m_bStopThinking = true;
#endif

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

void CAI_Stalker::Defend()
{
	WRITE_TO_LOG("Defending");
}

void CAI_Stalker::RetreatKnown()
{
	WRITE_TO_LOG("Retreating known");
}

void CAI_Stalker::RetreatUnknown()
{
	WRITE_TO_LOG("Retreating unknown");
}

void CAI_Stalker::PursuitKnown()
{
	WRITE_TO_LOG("Pursuiting known");
	
//	VERIFY(Weapons->ActiveWeapon());
//	Weapons->ActiveWeapon()->Reload();
//	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Weapons->ActiveWeapon()->GetAmmoElapsed());
}

void CAI_Stalker::PursuitUnknown()
{
	WRITE_TO_LOG("Pursuiting unknown");
}

void CAI_Stalker::SearchCorp()
{
	WRITE_TO_LOG("Searching corp");
}

void CAI_Stalker::vfUpdateSearchPosition()
{
	if (!g_Alive())
		return;
	INIT_SQUAD_AND_LEADER;
	if (this != Leader)	{
		CAI_Stalker *tpLeader = dynamic_cast<CAI_Stalker*>(Leader);
		if (tpLeader) {
//			if (m_tNextGraphPoint.distance_to(tpLeader->m_tNextGraphPoint) > EPS_L)
//				m_eCurrentState = eStalkerStateSearching;
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

void CAI_Stalker::Recharge()
{
	WRITE_TO_LOG("Pursuiting known");
	
	SelectEnemy(m_tEnemy);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_tEnemy.Enemy);

	VERIFY(Weapons->ActiveWeapon());
	Weapons->ActiveWeapon()->Reload();
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Weapons->ActiveWeapon()->GetAmmoElapsed());

	vfChoosePointAndBuildPath	(m_tSelectorReload);

	vfSetMovementType			(eBodyStateStand,eMovementTypeWalk,eLookTypePoint, m_tEnemy.Enemy->Position());
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::AccomplishTask()
{
	WRITE_TO_LOG				("Accomplishing task");

	SelectEnemy(m_tEnemy);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_tEnemy.Enemy,eStalkerStateAttack);

	if (!AI_Path.Nodes.size() || (AI_Path.Nodes[AI_Path.Nodes.size() - 1] != AI_Path.DestNode))
		vfBuildPathToDestinationPoint		(0,true);

	vfSetMovementType			(eBodyStateStand,eMovementTypeWalk,eLookTypeDanger);
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::Attack()
{
	WRITE_TO_LOG("Attacking");
	SelectEnemy(m_tEnemy);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_tEnemy.Enemy);

	EStalkerStates eState = EStalkerStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),eStalkerStateAttack,eStalkerStateDefend,eStalkerStateRetreatKnown));
	if (eState != m_eCurrentState)
		GO_TO_NEW_STATE_THIS_UPDATE(eState);

	vfChoosePointAndBuildPath	(m_tSelectorReload);

	vfSetFire					(true,*getGroup());

	vfSetMovementType			(eBodyStateStand,eMovementTypeWalk,eLookTypeFirePoint,m_tEnemy.Enemy->Position());
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::Think()
{
	vfUpdateSearchPosition();
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
			case eStalkerStateAccomplishingTask : {
				AccomplishTask();
				break;
			}
			case eStalkerStateAttack : {
				Attack();
				break;
			}
			case eStalkerStateDefend : {
				Defend();
				break;
			}
			case eStalkerStateRetreatKnown : {
				RetreatKnown();
				break;
			}
			case eStalkerStateRetreatUnknown : {
				RetreatUnknown();
				break;
			}
			case eStalkerStatePursuitKnown : {
				PursuitKnown();
				break;
			}
			case eStalkerStatePursuitUnknown : {
				PursuitUnknown();
				break;
			}
			case eStalkerStatePursuitSearchCorp : {
				SearchCorp();
				break;
			}
			case eStalkerStateRecharge : {
				Recharge();
				break;
			}
		}
		m_bStateChanged		= m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
};