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

void CAI_Stalker::vfBuildPathToDestinationPoint(CAISelectorBase *S, bool bCanStraighten)
{
	// building a path from and to
	if (S)
		getAI().m_tpAStar->ffFindOptimalPath(AI_NodeID,AI_Path.DestNode,AI_Path,S->m_dwEnemyNode,S->fOptEnemyDistance);
	else
		getAI().m_tpAStar->ffFindMinimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);
	
	if (AI_Path.Nodes.size() > 0) {
		Device.Statistic.AI_Path.Begin();
		// if path is long enough then build travel line
		if (!bCanStraighten) {
			AI_Path.BuildTravelLine(Position());
		}
		else {
			vector<Fvector>		tpaPoints(0);
			vector<Fvector>		tpaDeviations(0);
			vector<Fvector>		tpaTravelPath(0);
			tpaPoints.push_back(vPosition);
			u32 N = AI_Path.Nodes.size();
			Fvector			tStartPosition = vPosition;
			u32				dwCurNode = AI_NodeID;

			for (u32 i=1; i<N; i++)
				if (!getAI().bfCheckNodeInDirection(dwCurNode,tStartPosition,AI_Path.Nodes[i]))
					if (dwCurNode != AI_Path.Nodes[i - 1])
						tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
					else
						tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[i]));
			
			if (tStartPosition.distance_to(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1])) > getAI().Header().size)
				tpaPoints.push_back(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1]));
			
			tpaDeviations.resize(N = tpaPoints.size());
			
			AI_Path.TravelPath.clear();
			AI_Path.Nodes.clear();
			for (i=1; i<N; i++) {
				vector<Fvector>	tpaLine;
				vector<u32>		tpaNodes;
				tpaLine.clear();
				tpaLine.push_back(tpaPoints[i-1]);
				tpaLine.push_back(tpaPoints[i]);
				getAI().vfCreateFastRealisticPath(tpaLine,getAI().q_LoadSearch(tpaLine[0]),tpaDeviations,tpaTravelPath,tpaNodes,false,false,0,0);
				u32 n = tpaTravelPath.size();
				AI::CPathNodes::CTravelNode	T;
				for (u32 j= i<2?0:1; j<n; j++) {
					T.P = tpaTravelPath[j];
					AI_Path.TravelPath.push_back(T);
					AI_Path.Nodes.push_back(tpaNodes[j]);
				}
			}
	//		vector<Fvector>		tpaPoints(0);
	//		vector<Fvector>		tpaDeviations(0);
	//		vector<Fvector>		tpaTravelPath(0);
	//		tpaPoints.push_back(vPosition);
	//		u32 N = AI_Path.Nodes.size();
	//		Fvector			tStartPosition = vPosition;
	//		u32				dwCurNode = AI_NodeID;
	//
	//		for (u32 i=1; i<N; i++)
	//			if (!getAI().bfCheckNodeInDirection(dwCurNode,tStartPosition,AI_Path.Nodes[i]))
	//				if (dwCurNode != AI_Path.Nodes[i - 1])
	//					tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
	//				else
	//					tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[i]));
	//		
	//		if (tStartPosition.distance_to(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1])) > getAI().Header().size)
	//			tpaPoints.push_back(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1]));
	//		
	//		tpaDeviations.resize(N = tpaPoints.size());
	//		
	//		getAI().vfCreateFastRealisticPath(tpaPoints,AI_NodeID,tpaDeviations,tpaTravelPath,AI_Path.Nodes,false,false);
	//		
	//		N = tpaTravelPath.size();
	//		AI::CPathNodes::CTravelNode	T;
	//		AI_Path.TravelPath.clear();
	//		for (i=0; i<N; i++) {
	//			T.P = tpaTravelPath[i];
	//			AI_Path.TravelPath.push_back(T);
	//		}
		}
		Device.Statistic.AI_Path.End();
		//////////////////////////////////////////////////////////////////////////
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

void CAI_Stalker::vfChoosePointAndBuildPath(CAISelectorBase &tSelector, bool bCanStraighten)
{
	INIT_SQUAD_AND_LEADER;
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint	(0,bCanStraighten);
	else {
		vfInitSelector					(tSelector,Squad,Leader);
		vfSearchForBetterPosition		(m_tSelectorFreeHunting,Squad,Leader);
	}
}

void CAI_Stalker::vfSetMovementType(EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType)
{
	VERIFY(tLookType != eLookTypePoint);
	Fvector tDummy;
	vfSetMovementType(tBodyState,tMovementType,tLookType,tDummy);
}

void CAI_Stalker::vfSetMovementType(EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType, Fvector &tPointToLook)
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
		case eLookTypeSearch : {
			SetLessCoverLook();
			break;
		}
		case eLookTypeDanger : {
			SetLessCoverLook(AI_Node,PI);
			break;
		}
		case eLookTypePoint : {
			Fvector tTemp;
			tTemp.sub	(tPointToLook,vPosition);
			tTemp.getHP	(r_target.yaw,r_target.pitch);
			r_target.yaw *= -1;
			r_target.pitch *= -0;
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::vfChooseNextGraphPoint()
{
	_GRAPH_ID			tGraphID		= m_tNextGP;
	u16					wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CALifeGraph::SGraphEdge			*tpaEdges		= (CALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);
	
	int					iPointCount		= (int)m_tpaTerrain.size();
	int					iBranches		= 0;
	for (int i=0; i<wNeighbourCount; i++)
		for (int j=0; j<iPointCount; j++)
			if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP))
				iBranches++;
	if (!iBranches) {
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
					m_tCurGP	= m_tNextGP;
					m_tNextGP	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
					m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
					return;
				}
		}
	}
	else {
		int iChosenBranch = ::Random.randI(0,iBranches);
		iBranches = 0;
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP)) {
					if (iBranches == iChosenBranch) {
						m_tCurGP	= m_tNextGP;
						m_tNextGP	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
						m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
						return;
					}
					iBranches++;
				}
		}
	}
}

void CAI_Stalker::vfSetFire(bool bFire, CGroup &Group)
{
	bool bSafeFire = m_bFiring;
	
	if (!Weapons || !Weapons->ActiveWeapon())
		return;

	if (bFire)
		if (m_bFiring)
			if ((int)m_dwStartFireAmmo - (int)Weapons->ActiveWeapon()->GetAmmoElapsed() > ::Random.randI(m_dwFireRandomMin,m_dwFireRandomMax + 1)) {
				q_action.setup(AI::AIC_Action::FireEnd);
				m_bFiring = false;
				m_dwNoFireTime = m_dwCurrentUpdate;
			}
			else {
				if (bfCheckIfCanKillEnemy())
					if (!bfCheckIfCanKillMember()) {
						q_action.setup(AI::AIC_Action::FireBegin);
						m_bFiring = true;
					}
					else {
						q_action.setup(AI::AIC_Action::FireEnd);
						m_bFiring = false;
						m_dwNoFireTime = m_dwCurrentUpdate;
					}
					else {
						q_action.setup(AI::AIC_Action::FireEnd);
						m_bFiring = false;
						m_dwNoFireTime = m_dwCurrentUpdate;
					}
			}
			else {
				if ((int)m_dwCurrentUpdate - (int)m_dwNoFireTime > ::Random.randI(m_dwNoFireTimeMin,m_dwNoFireTimeMax + 1))
					if (bfCheckIfCanKillEnemy())
						if (!bfCheckIfCanKillMember()) {
							m_dwStartFireAmmo = Weapons->ActiveWeapon()->GetAmmoElapsed();
							q_action.setup(AI::AIC_Action::FireBegin);
							m_bFiring = true;
						}
						else {
							q_action.setup(AI::AIC_Action::FireEnd);
							m_bFiring = false;
						}
						else {
							q_action.setup(AI::AIC_Action::FireEnd);
							m_bFiring = false;
						}
						else {
							q_action.setup(AI::AIC_Action::FireEnd);
							m_bFiring = false;
						}
			}
			else {
				q_action.setup(AI::AIC_Action::FireEnd);
				m_bFiring = false;
			}
			
			if ((bSafeFire) && (!m_bFiring))
				Group.m_dwFiring--;
			else
				if ((!bSafeFire) && (m_bFiring))
					Group.m_dwFiring++;
}

void CAI_Stalker::vfStopFire()
{
	if (m_bFiring) {
		q_action.setup(AI::AIC_Action::FireEnd);
		m_bFiring = false;
		m_dwNoFireTime = m_dwCurrentUpdate;
	}
	m_bFiring = false;
}