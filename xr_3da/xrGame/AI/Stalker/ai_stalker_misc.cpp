////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\..\\a_star.h"
#include "..\\ai_monsters_misc.h"

void CAI_Stalker::vfBuildPathToDestinationPoint(CAISelectorBase *S)
{
	// building a path from and to
	if (S)
		getAI().m_tpAStar->ffFindOptimalPath(AI_NodeID,AI_Path.DestNode,AI_Path,S->m_dwEnemyNode,S->fOptEnemyDistance);
	else
		getAI().m_tpAStar->ffFindMinimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);
	
	if (AI_Path.Nodes.size() > 0) {
		// if path is long enough then build travel line
		AI_Path.BuildTravelLine(Position());
		AI_Path.TravelStart = 0;
	}
	else {
		// if path is too short then clear it (patch for ExecMove)
		AI_Path.TravelPath.clear();
	}
	AI_Path.bNeedRebuild = FALSE;
}

void CAI_Stalker::vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
//	if ((!m_dwLastRangeSearch) || (AI_Path.fSpeed < EPS_L) || ((S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SEARCH_TIME_INTERVAL) && (::Random.randF(0,1) < float(S.m_dwCurTime - m_dwLastRangeSearch)/MAX_TIME_RANGE_SEARCH))) 
	{
		
//		m_dwLastRangeSearch = S.m_dwCurTime;
		Device.Statistic.AI_Node.Begin();
		Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
		Device.Statistic.AI_Node.End();
		// search for the best node according to the 
		// selector evaluation function in the radius N meteres
		float fOldCost;
		getAI().q_Range_Bit(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
		
		// if search has found _new_ best node then 
//		if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
		if ((AI_Path.DestNode != S.BestNode) && (S.BestCost < (fOldCost - S.fLaziness))){
			AI_Path.DestNode		= S.BestNode;
			AI_Path.bNeedRebuild	= TRUE;
			//vfAddToSearchList();
		} 
		
		if (AI_Path.TravelPath.empty() || (AI_Path.TravelPath.size() - 1 <= AI_Path.TravelStart))
			AI_Path.bNeedRebuild = TRUE;
		
//		if (AI_Path.bNeedRebuild)
//			m_dwLastSuccessfullSearch = S.m_dwCurTime;
	}
}

void CAI_Stalker::vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
	// checking if leader is dead then make myself a leader
	if (Leader->g_Health() <= 0)
		Leader = this;
	// setting watch mode to false
	if (Leader != this) {
		S.m_tLeader = Leader;
		S.m_tLeaderPosition = Leader->Position();
		S.m_tpLeaderNode = Leader->AI_Node;
		S.m_tLeaderNode = Leader->AI_NodeID;
	}
	// otherwise assign leader to null
	else {
		S.m_tLeader = 0;
		S.m_tLeaderPosition.set(0,0,0);
		S.m_tpLeaderNode = NULL;
		S.m_tLeaderNode = u32(-1);
	}
//	S.m_tHitDir			= tHitDir;
//	S.m_dwHitTime		= dwHitTime;
	
	S.m_dwCurTime		= m_dwCurrentUpdate;
	//Msg("%d : %d",S.m_dwHitTime,S.m_dwCurTime);
	
	S.m_tMe				= this;
	S.m_tpMyNode		= AI_Node;
	S.m_tMyPosition		= Position();
	
//	if (Enemy.Enemy) {
//		bBuildPathToLostEnemy = false;
//		// saving an enemy
//		vfSaveEnemy();
//		
//		S.m_tEnemy			= Enemy.Enemy;
//		S.m_tEnemyPosition	= Enemy.Enemy->Position();
//		S.m_dwEnemyNode		= Enemy.Enemy->AI_NodeID;
//		S.m_tpEnemyNode		= Enemy.Enemy->AI_Node;
//	}
//	else {
//		S.m_tEnemy			= tSavedEnemy;
//		S.m_tEnemyPosition	= tSavedEnemyPosition;
//		S.m_dwEnemyNode		= dwSavedEnemyNodeID;
//		S.m_tpEnemyNode		= tpSavedEnemyNode;
//	}
	
	S.taMembers = &(Squad.Groups[g_Group()].Members);
	
	//if (S.m_tLeader)
	//	S.taMembers.push_back(S.m_tLeader);
}

void CAI_Stalker::vfChoosePointAndBuildPath(CAISelectorBase &tSelector)
{
	INIT_SQUAD_AND_LEADER;
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint	(0);
	else {
		vfInitSelector					(tSelector,Squad,Leader);
		vfSearchForBetterPosition		(m_tSelectorFreeHunting,Squad,Leader);
	}
}

void CAI_Stalker::vfSetMovementType(EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType)
{
	m_tBodyState	= tBodyState;
	m_tMovementType = tMovementType;
	m_tLookType		= tLookType;
	
	m_fCurSpeed		= 1.f;

	if (AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 2) > AI_Path.TravelStart)) {
		switch (m_tBodyState) {
			case eBodyStateCrouch : {
				m_fCurSpeed *= m_fCrouchFactor;
				break;
			}
			case eBodyStateStand : {
				break;
			}
			default : NODEFAULT;
		}
		switch (m_tMovementType) {
			case eMovementTypeWalk : {
				m_fCurSpeed *= m_fWalkFactor;
				break;
			}
			case eMovementTypeRun : {
				m_fCurSpeed *= m_fRunFactor;
				break;
			}
			default : m_fCurSpeed = 0.f;
		}
	}
	else
		m_fCurSpeed = 0.f;
	
	switch (m_tLookType) {
		case eLookTypeDirection : {
			SetDirectionLook();
			break;
		}
		case eLookTypeView : {
			SetLessCoverLook();
			break;
		}
	}
}