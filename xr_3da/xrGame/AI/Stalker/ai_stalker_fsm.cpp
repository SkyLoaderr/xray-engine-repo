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
	m_bStopThinking = true;\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
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
	
	DropItemSendMessage();
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
}

void CAI_Stalker::RetreatUnknown()
{
	WRITE_TO_LOG("Retreating unknown");
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

void CAI_Stalker::SearchCorp()
{
	WRITE_TO_LOG("Searching corp");
	
//	vfStopFire();
//
//	SelectEnemy(m_tEnemy);
//
//	// I see enemy
//	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(m_tEnemy.Enemy,eStalkerStateAttack);
//
//	vfCheckForItems();
//
//	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_tpWeaponToTake,eStalkerStateTakeItem);
//
//	if (AI_Path.DestNode != m_dwSavedEnemyNodeID) {
//		AI_Path.DestNode = m_dwSavedEnemyNodeID;
//		vfBuildPathToDestinationPoint(0,false,&m_tSavedEnemy->Position());
//	}
//
//	if (vPosition.distance_to(m_tSavedEnemy->Position()) < EPS_L) {
//		m_tSavedEnemy = 0;
//		GO_TO_PREV_STATE_THIS_UPDATE;
//	}
//	
//	Fvector						tTemp;
//	m_tSavedEnemy->svCenter		(tTemp);
//	vfSetMovementType			(eBodyStateStand,eMovementTypeWalk,eLookTypePoint,tTemp);
//	
//	if (m_fCurSpeed < EPS_L)
//		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::HolsterItem()
{
	WRITE_TO_LOG("Holstering item");

	if (m_bStateChanged) {
		VERIFY(Weapons->ActiveWeapon());
		Weapons->ActiveWeapon()->Hide();
	}

	CHECK_IF_GO_TO_PREV_STATE(Weapons->ActiveWeapon()->STATE != CWeapon::eHiding);
}

void CAI_Stalker::TakeItem()
{
	WRITE_TO_LOG("Taking item");
	
	vfStopFire();

	SelectEnemy(m_tEnemy);

	// I see enemy
	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(m_tEnemy.Enemy,eStalkerStateAttack);

	vfCheckForItems();

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_tpWeaponToTake);
	
	AI_Path.DestNode = m_tpWeaponToTake->AI_NodeID;
	if (AI_Path.TravelPath.empty() || AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P.distance_to(m_tpWeaponToTake->Position()) > EPS_L)
		vfBuildPathToDestinationPoint(0,false,&(m_tpWeaponToTake->Position()));

	vfSetMovementType			(eBodyStateStand,eMovementTypeWalk,eLookTypePoint,m_tpWeaponToTake->Position());
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::DropItem()
{
	WRITE_TO_LOG("Dropping item");

	if (m_bStateChanged)
		DropItemSendMessage();
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!Weapons->ActiveWeapon());
}

void CAI_Stalker::Recharge()
{
	WRITE_TO_LOG("Pursuiting known");
	
	vfStopFire();

	SelectEnemy(m_tEnemy);

	VERIFY(Weapons->ActiveWeapon());
	
	Weapons->ActiveWeapon()->Reload();
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Weapons->ActiveWeapon()->GetAmmoElapsed());

	if (m_tEnemy.Enemy) {
		vfChoosePointAndBuildPath	(m_tSelectorReload);
		vfSetMovementType		(eBodyStateStand,eMovementTypeRun,eLookTypePoint, m_tEnemy.Enemy->Position());
	}
	else
		vfSetMovementType		(eBodyStateStand,eMovementTypeStand,eLookTypeDanger);
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::Attack()
{
	WRITE_TO_LOG("Attacking");
	
	SelectEnemy(m_tEnemy);

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(!m_tEnemy.Enemy,eStalkerStatePursuitKnown);

	EStalkerStates eState = EStalkerStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),eStalkerStateAttack,eStalkerStateDefend,eStalkerStateRetreatKnown));
	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(eState != m_eCurrentState,eState);

	vfSetFire					(false,*getGroup());

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(Weapons->ActiveWeapon() && !Weapons->ActiveWeapon()->GetAmmoElapsed(),eStalkerStateRecharge);

	vfChoosePointAndBuildPath	(m_tSelectorFreeHunting);

	vfSetFire					(true,*getGroup());

	vfSetMovementType			(eBodyStateStand,eMovementTypeWalk,eLookTypeFirePoint,m_tEnemy.Enemy->Position());
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::AccomplishTask()
{
	WRITE_TO_LOG				("Accomplishing task");

	vfStopFire();

	SelectEnemy(m_tEnemy);

	// I see enemy
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_tEnemy.Enemy,eStalkerStateAttack);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfCheckIfSound(),eStalkerStatePursuitUnknown);

	// I have to recharge active weapon
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(Weapons->ActiveWeapon() && !Weapons->ActiveWeapon()->GetAmmoElapsed(),eStalkerStateRecharge);

	vfCheckForItems();

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_tpWeaponToTake,eStalkerStateTakeItem);

	// I have to search the corp
	//CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE((m_tSavedEnemy && (!m_tSavedEnemy->g_Alive())),eStalkerStateSearchCorp);

	if (!AI_Path.Nodes.size() || (AI_Path.Nodes[AI_Path.Nodes.size() - 1] != AI_Path.DestNode))
		vfBuildPathToDestinationPoint		(0,true);

	vfSetMovementType			(eBodyStateStand,eMovementTypeWalk,eLookTypeDanger);
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::RetreatKnown()
{
	WRITE_TO_LOG("Retreating known");

	vfStopFire();

	SelectEnemy(m_tEnemy);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_tEnemy.Enemy);

	// I see enemy
	EStalkerStates eState = EStalkerStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),eStalkerStateAttack,eStalkerStateDefend,eStalkerStateRetreatKnown));
	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(eState != m_eCurrentState,eState);

	// I have to recharge active weapon
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(Weapons->ActiveWeapon() && !Weapons->ActiveWeapon()->GetAmmoElapsed(),eStalkerStateRecharge);

	//vfCheckForItems();

	//CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_tpWeaponToTake,eStalkerStateTakeItem);

	vfChoosePointAndBuildPath	(m_tSelectorRetreat,true);

	vfSetMovementType			(eBodyStateStand,eMovementTypeRun,eLookTypePoint,m_tEnemy.Enemy->Position());
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::PursuitUnknown()
{
	WRITE_TO_LOG("Pursuiting unknown");

	int						iIndex;
	SelectSound				(iIndex);
	
	CHECK_IF_GO_TO_PREV_STATE(iIndex == -1);

	m_tSavedEnemy			= m_tpaDynamicSounds[iIndex].tpEntity;
	m_tSavedEnemyPosition	= m_tpaDynamicSounds[iIndex].tSavedPosition;
	m_dwSavedEnemyNodeID	= m_tpaDynamicSounds[iIndex].dwNodeID;
	m_tpSavedEnemyNode		= getAI().Node(m_dwSavedEnemyNodeID);
	m_dwLostEnemyTime		= m_tpaDynamicSounds[iIndex].dwTime;
	m_tMySavedPosition		= m_tpaDynamicSounds[iIndex].tMySavedPosition;
	m_dwMyNodeID			= m_tpaDynamicSounds[iIndex].dwMyNodeID;

	if (m_tSavedEnemy) {
		EStalkerStates eState = EStalkerStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),eStalkerStateAttack,eStalkerStateDefend,eStalkerStateRetreatKnown));
		if (eState == eStalkerStateRetreatKnown)
			GO_TO_NEW_STATE_THIS_UPDATE(eStalkerStateRetreatKnown);
	}

	GO_TO_NEW_STATE_THIS_UPDATE(eStalkerStatePursuitKnown);
}

void CAI_Stalker::PursuitKnown()
{
	WRITE_TO_LOG("Pursuiting known");
	
	INIT_SQUAD_AND_LEADER;

	if (m_bStateChanged) {
		if (Leader == this) {
			getGroup()->m_tpaSuspiciousNodes.clear();
			vfFindAllSuspiciousNodes(m_dwSavedEnemyNodeID,m_tSavedEnemyPosition,m_tSavedEnemyPosition,_min(20.f,_min(1*8.f*vPosition.distance_to(m_tSavedEnemyPosition)/4.5f,60.f)),*getGroup());
			vfClasterizeSuspiciousNodes(*getGroup());
			m_iCurrentSuspiciousNodeIndex = -1;
			return;
		}
		m_iCurrentSuspiciousNodeIndex = -1;
	}
	
	vfStopFire					();

	SelectEnemy					(m_tEnemy);

	// I see enemy
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_tEnemy.Enemy,eStalkerStateAttack);

	CHECK_IF_GO_TO_NEW_STATE	(bfCheckIfSound(),eStalkerStatePursuitUnknown);

	// I have to recharge active weapon
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(Weapons->ActiveWeapon() && !Weapons->ActiveWeapon()->GetAmmoElapsed(),eStalkerStateRecharge);

	vfCheckForItems				();

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_tpWeaponToTake,eStalkerStateTakeItem);

	vfChooseSuspiciousNode		(m_tSelectorFreeHunting);
	
	vfSetMovementType			(eBodyStateStand,eMovementTypeRun,eLookTypePatrol);
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::Defend()
{
	WRITE_TO_LOG("Defending");
	
	SelectEnemy(m_tEnemy);

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(!m_tEnemy.Enemy,eStalkerStatePursuitKnown);

	EStalkerStates eState = EStalkerStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),eStalkerStateAttack,eStalkerStateDefend,eStalkerStateRetreatKnown));
	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(eState != m_eCurrentState,eState);

	vfSetFire					(false,*getGroup());

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(Weapons->ActiveWeapon() && !Weapons->ActiveWeapon()->GetAmmoElapsed(),eStalkerStateRecharge);

	vfChoosePointAndBuildPath	(m_tSelectorReload);

	vfSetFire					(true,*getGroup());

	vfSetMovementType			(eBodyStateStand,eMovementTypeWalk,eLookTypeFirePoint,m_tEnemy.Enemy->Position());
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::Think()
{
	vfUpdateDynamicObjects	();
	vfUpdateSearchPosition	();
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
			case eStalkerStateSearchCorp : {
				SearchCorp();
				break;
			}
			case eStalkerStateRecharge : {
				Recharge();
				break;
			}
			case eStalkerStateHolsterItem : {
				HolsterItem();
				break;
			}
			case eStalkerStateTakeItem : {
				TakeItem();
				break;
			}
			case eStalkerStateDropItem : {
				DropItem();
				break;
			}
		}
		m_bStateChanged		= m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
};