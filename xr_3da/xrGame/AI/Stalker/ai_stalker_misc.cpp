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

#define TIME_TO_SEARCH 60000

void CAI_Stalker::vfBuildPathToDestinationPoint(CAISelectorBase *S, bool bCanStraighten, Fvector *tpDestinationPosition)
{
	// building a path from and to
	if ((AI_Path.DestNode == AI_NodeID) && !tpDestinationPosition) {
		AI_Path.Nodes.clear();
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;
		m_fCurSpeed = 0;
		AI_Path.bNeedRebuild = FALSE;
		return;
	}

	if (S)
		getAI().m_tpAStar->ffFindOptimalPath(AI_NodeID,AI_Path.DestNode,AI_Path,S->m_dwEnemyNode,S->fOptEnemyDistance);
	else
		getAI().m_tpAStar->ffFindMinimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);
	
	if (AI_Path.Nodes.size() > 0) {
		Device.Statistic.AI_Path.Begin();
		// if path is long enough then build travel line
		if (!bCanStraighten) {
			AI_Path.BuildTravelLine(Position());
			if (tpDestinationPosition) {
				VERIFY(!AI_Path.TravelPath.empty());
				if (AI_Path.TravelPath[AI_Path.TravelPath.size() - 2].P.distance_to(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) < AI_Path.TravelPath[AI_Path.TravelPath.size() - 2].P.distance_to(*tpDestinationPosition)) {
					AI::CPathNodes::CTravelNode T;
					T.P = *tpDestinationPosition;
					AI_Path.TravelPath.push_back(T);
				}
				else {
					AI_Path.TravelPath[AI_Path.TravelPath.size() - 2].P = *tpDestinationPosition;
				}
			}
		}
		else {
			vector<Fvector>		tpaPoints(0);
			vector<Fvector>		tpaDeviations(0);
			vector<Fvector>		tpaTravelPath(0);
			vector<u32>			tpaPointNodes(0);
			u32					N = AI_Path.Nodes.size();
			Fvector				tStartPosition = vPosition;
			u32					dwCurNode = AI_NodeID;
			tpaPoints.push_back	(vPosition);
			tpaPointNodes.push_back(dwCurNode);

			for (u32 i=1; i<=N; i++)
				if (i<N) {
					if (!getAI().bfCheckNodeInDirection(dwCurNode,tStartPosition,AI_Path.Nodes[i])) {
						if (dwCurNode != AI_Path.Nodes[i - 1])
							tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
						else
							tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[i]));
						tpaPointNodes.push_back(dwCurNode);
					}
				}
				else
					if (tpDestinationPosition)
						if (getAI().dwfCheckPositionInDirection(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
							if (dwCurNode != AI_Path.Nodes[i - 1])
								tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
							tpaPoints.push_back(tStartPosition = *tpDestinationPosition);
						}
			
			if ((!tpDestinationPosition) && (tStartPosition.distance_to(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1])) > getAI().Header().size))
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
				getAI().vfCreateFastRealisticPath(tpaLine,tpaPointNodes[i-1],tpaDeviations,tpaTravelPath,tpaNodes,false,false,0,0);
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
		vfInitSelector(S,Squad,Leader);
		float fOldCost;
		if (AI_Path.DestNode != u32(-1)) {
			NodeCompressed*	T = getAI().Node(AI_Path.DestNode);
			BOOL			bStop;
			fOldCost		= S.Estimate(T,getAI().u_SqrDistance2Node(vPosition,T),bStop);
			Msg				("PrevNode : %d, cost : %f",AI_Path.DestNode,fOldCost);
		}
		Device.Statistic.AI_Node.Begin();
		Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
		Device.Statistic.AI_Node.End();
		// search for the best node according to the 
		// selector evaluation function in the radius N meteres
		//Msg("[%f][%f][%f]",VPUSH(S.m_tEnemyPosition));
		getAI().q_Range_Bit(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
		
		// if search has found _new_ best node then 
//		if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
		Msg("CurrNode : %d, cost %f",S.BestNode,S.BestCost);
		if ((AI_Path.DestNode != S.BestNode) && (S.BestCost < (fOldCost - S.fLaziness))){
			Msg("ChooNode : %d, cost %f",S.BestNode,S.BestCost);
			AI_Path.DestNode		= S.BestNode;
			AI_Path.bNeedRebuild	= TRUE;
			//vfAddToSearchList();
		} 
		else
			Msg("ChooNode : %d, cost %f",AI_Path.DestNode,fOldCost);
		
//		if (AI_Path.TravelPath.empty() || (AI_Path.TravelPath.size() - 1 <= AI_Path.TravelStart))
//			AI_Path.bNeedRebuild = TRUE;
		
//		if (AI_Path.bNeedRebuild)
//			m_dwLastSuccessfullSearch = S.m_dwCurTime;
	}
}

void CAI_Stalker::vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
//	S.m_tHitDir			= tHitDir;
//	S.m_dwHitTime		= dwHitTime;
	
	S.m_dwCurTime		= m_dwCurrentUpdate;
	//Msg("%d : %d",S.m_dwHitTime,S.m_dwCurTime);
	
	S.m_tMe				= this;
	S.m_tpMyNode		= AI_Node;
	S.m_tMyPosition		= Position();
	
	if (m_tEnemy.Enemy) {
		// saving an enemy
		vfSaveEnemy();
		
		S.m_tEnemy			= m_tEnemy.Enemy;
		S.m_tEnemyPosition	= m_tEnemy.Enemy->Position();
		S.m_dwEnemyNode		= m_tEnemy.Enemy->AI_NodeID;
		S.m_tpEnemyNode		= m_tEnemy.Enemy->AI_Node;
	}
	else {
		S.m_tEnemy			= m_tSavedEnemy;
		S.m_tEnemyPosition	= m_tSavedEnemyPosition;
		S.m_dwEnemyNode		= m_dwSavedEnemyNodeID;
		S.m_tpEnemyNode		= m_tpSavedEnemyNode;
	}
	
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
		//vfInitSelector					(tSelector,Squad,Leader);
		vfSearchForBetterPosition		(tSelector,Squad,Leader);
	}
}

void CAI_Stalker::vfSetMovementType(EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType)
{
	VERIFY(tLookType != eLookTypePoint);
	Fvector tDummy;
	vfSetMovementType(tBodyState,tMovementType,tLookType,tDummy);
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

void CAI_Stalker::vfSetMovementType(EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType, Fvector &tPointToLook)
{
	m_tBodyState	= tBodyState;
	m_tMovementType = tMovementType;
	m_tLookType		= tLookType;
	
	m_fCurSpeed		= 1.f;

	if (AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 1) > AI_Path.TravelStart)) {
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
		case eLookTypePatrol : {
			SetDirectionLook();
			break;
		}
		case eLookTypeSearch : {
			SetLessCoverLook(AI_Node);
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
			r_target.pitch *= 1;
			break;
		}
		case eLookTypeFirePoint : {
			Fvector tTemp;
			tTemp.sub	(tPointToLook,vPosition);
			tTemp.getHP	(r_target.yaw,r_target.pitch);
			r_target.yaw *= -1;
			r_target.pitch *= 1;
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::vfCheckForItems()
{
	m_tpWeaponToTake = 0;
	objVisible&	Known	= Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	for (u32 i=0; i<Known.size(); i++) {
		CWeapon *tpWeapon = dynamic_cast<CWeapon*>(Known[i].key);
		if (tpWeapon && Weapons->isSlotEmpty(tpWeapon->GetSlot())) {
			m_tpWeaponToTake = tpWeapon;
			break;
		}
	}
}


void CAI_Stalker::vfMarkVisibleNodes(CEntity *tpEntity)
{
	CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
	if (!tpCustomMonster)
		return;

	Fvector tDirection;
	float fRange = tpCustomMonster->ffGetRange();
	
	for (float fIncrement = 0; fIncrement < PI_MUL_2; fIncrement += PI/10.f) {
		tDirection.setHP(fIncrement,0.f);
		getAI().ffMarkNodesInDirection(tpCustomMonster->AI_NodeID,tpCustomMonster->Position(),tDirection,getAI().q_mark_bit,fRange,m_tpaNodeStack);
	}
}

void CAI_Stalker::vfFindAllSuspiciousNodes(u32 StartNode, Fvector tPointPosition, const Fvector& BasePos, float Range, CGroup &Group)
{
	Msg("%d : searching for suspicious nodes",Level().timeServer());

	Device.Statistic.AI_Range.Begin	();

	Group.m_tpaSuspiciousNodes.clear();
	
	BOOL bStop = FALSE;
	
	CAI_Space &AI = getAI();
	NodePosition QueryPos;
	AI.PackPosition(QueryPos,BasePos);

	AI.q_stack.clear();
	AI.q_stack.push_back(StartNode);
	AI.q_mark_bit[StartNode] = true;
	
//	NodeCompressed*	Base = AI.Node(StartNode);
	
	float range_sqr		= Range*Range;
	float fEyeFov;
	CEntityAlive *tpEnemy = dynamic_cast<CEntityAlive*>(m_tSavedEnemy);
	if (tpEnemy)
		fEyeFov = tpEnemy->ffGetFov();
	else
		fEyeFov = ffGetFov();
	fEyeFov *=PI/180.f;

	/**
	Fvector tMyDirection;
	SRotation tMyRotation,tEnemyRotation;
	tMyDirection.sub(tSavedEnemyPosition,vPosition);
	mk_rotation(tMyDirection,tMyRotation);
	tMyDirection.sub(tSavedEnemy->Position(),vPosition);
	mk_rotation(tMyDirection,tEnemyRotation);
	if (tEnemyRotation.yaw > tMyRotation.yaw) {
		if (tEnemyRotation.yaw - tMyRotation.yaw > PI)
			tMyRotation.yaw += PI_MUL_2;
	}
	else
		if (tMyRotation.yaw > tEnemyRotation.yaw)
			if (tMyRotation.yaw - tEnemyRotation.yaw > PI)
				tEnemyRotation.yaw += PI_MUL_2;
    bool bRotation = tMyRotation.yaw < tEnemyRotation.yaw;
	/**/
	INIT_SQUAD_AND_LEADER;
	m_tpaNodeStack.clear();
	for (int i=0; i<(int)Group.Members.size(); i++)
		vfMarkVisibleNodes(Group.Members[i]);

//	Msg("Nodes being checked for suspicious :");
	for (u32 it=0; it<AI.q_stack.size(); it++) {
		u32 ID = AI.q_stack[it];

 		//if (bfCheckForVisibility(ID,tMyRotation,bRotation) && (ID != StartNode))
		//	continue;

		NodeCompressed*	N = AI.Node(ID);
		u32 L_count	= u32(N->links);
		NodeLink* L_it	= (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
		NodeLink* L_end	= L_it+L_count;
		for( ; L_it!=L_end; L_it++) {
			if (bStop)			
				break;
			// test node
			u32 Test = AI.UnpackLink(*L_it);
			if (AI.q_mark_bit[Test])
				continue;

			NodeCompressed*	T = AI.Node(Test);

			float distance_sqr = AI.u_SqrDistance2Node(BasePos,T);
			if (distance_sqr>range_sqr)	
				continue;

			// register
			AI.q_mark_bit[Test]		= true;
			AI.q_stack.push_back	(Test);

			// estimate
			SSearchPlace tSearchPlace;

			Fvector tDirection, tNodePosition = AI.tfGetNodeCenter(T);
			tDirection.sub(tPointPosition,tNodePosition);
			tDirection.normalize_safe();
			SRotation tRotation;
			mk_rotation(tDirection,tRotation);
			float fCost = ffGetCoverInDirection(tRotation.yaw,T);
//			Msg("%d %.2f",Test,fCost);
			if (fCost < .6f) {
				bool bOk = false;
				float fMax = 0.f;
				for (int i=0, iIndex = -1; i<(int)Group.m_tpaSuspiciousNodes.size(); i++) {
					Fvector tP0 = AI.tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID);
					float fDistance = tP0.distance_to(tNodePosition);
					if (fDistance < 10.f) {
						if (fDistance < 3.f) {
							if (fCost < Group.m_tpaSuspiciousNodes[i].fCost) {
								Group.m_tpaSuspiciousNodes[i].dwNodeID = Test;
								Group.m_tpaSuspiciousNodes[i].fCost = fCost;
							}
							bOk = true;
							break;
						}
						tDirection.sub(tP0,tNodePosition);
						tDirection.normalize_safe();
						mk_rotation(tDirection,tRotation);
						if (ffGetCoverInDirection(tRotation.yaw,T) > .3f) {
							if (fCost < Group.m_tpaSuspiciousNodes[i].fCost) {
								Group.m_tpaSuspiciousNodes[i].dwNodeID = Test;
								Group.m_tpaSuspiciousNodes[i].fCost = fCost;
							}
							bOk = true;
							break;
						}
					}
					if (fCost > fMax) {
						fMax = Group.m_tpaSuspiciousNodes[i].fCost;
						iIndex = i;
					}
				}
				if (!bOk) {
					if (Group.m_tpaSuspiciousNodes.size() < MAX_SUSPICIOUS_NODE_COUNT) {
						tSearchPlace.dwNodeID	= Test;
						tSearchPlace.fCost		= fCost;
						tSearchPlace.dwSearched	= 0;
						tSearchPlace.dwGroup	= 0;
						Group.m_tpaSuspiciousNodes.push_back(tSearchPlace);
					}
					else
						if ((fMax > fCost) && (iIndex >= 0)) {
							Group.m_tpaSuspiciousNodes[iIndex].dwNodeID = Test;
							Group.m_tpaSuspiciousNodes[iIndex].fCost = fCost;
						}
				}
			}
		}
		if (bStop)
			break;
	}

	{
		vector<u32>::iterator it	= AI.q_stack.begin();
		vector<u32>::iterator end	= AI.q_stack.end();
		for ( ; it!=end; it++)	
			AI.q_mark_bit[*it] = false;
		
		it	= m_tpaNodeStack.begin();
		end	= m_tpaNodeStack.end();
		for ( ; it!=end; it++)	
			AI.q_mark_bit[*it] = false;
	}
	//AI.q_mark_bit.assign(AI.Header().count,false);
	
//	Msg("Suspicious nodes :");
//	for (int k=0; k<(int)Group.m_tpaSuspiciousNodes.size(); k++)
//		Msg("%d",Group.m_tpaSuspiciousNodes[k].dwNodeID);

	Device.Statistic.AI_Range.End();
}

#define GROUP_RADIUS	15.f

void CAI_Stalker::vfClasterizeSuspiciousNodes(CGroup &Group)
{
//	u32 N = Group.m_tpaSuspiciousNodes.size();
//	m_tpaSuspiciousPoints.resize(N);
//	m_tpaSuspiciousForces.resize(N);
//	for (int i=0; i<N; i++) {
//		m_tpaSuspiciousPoints[i] = getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID);
//		m_tpaSuspiciousForces[i].set(0,0,0);
//	}
//	float fK = .05f;
//	for (int k=0; k<3; k++) {
//		for (int i=0; i<N; i++) {
//			for (int j=0; j<N; j++)
//				if (j != i) {
//					Fvector tDummy;
//					tDummy.sub(m_tpaSuspiciousPoints[j],m_tpaSuspiciousPoints[i]);
//					tDummy.mul(tDummy.magnitude()*fK);
//					m_tpaSuspiciousForces[i].add(tDummy);
//				}
//		}
//		for (int i=0; i<m_tpaSuspiciousPoints.size(); i++) {
//			m_tpaSuspiciousForces[i].div(N);
//			m_tpaSuspiciousPoints[i].add(m_tpaSuspiciousForces[i]);
//			m_tpaSuspiciousForces[i].set(0,0,0);
//		}
//	}
//	for (int i=0, iGroupCounter = 1; i<N; i++, iGroupCounter++) {
//		if (!Group.m_tpaSuspiciousNodes[i].dwGroup) 
//			Group.m_tpaSuspiciousNodes[i].dwGroup = iGroupCounter;
//		for (int j=0; j<N; j++)
//			if (!Group.m_tpaSuspiciousNodes[j].dwGroup && (m_tpaSuspiciousPoints[j].distance_to(m_tpaSuspiciousPoints[i]) < GROUP_RADIUS))
//				Group.m_tpaSuspiciousNodes[j].dwGroup = iGroupCounter;
//	}
//	for ( i=0; i<N; i++)
//		Group.m_tpaSuspiciousNodes[i].dwGroup--;
//	Group.m_tpaSuspiciousGroups.resize(--iGroupCounter);
//	for ( i=0; i<iGroupCounter; i++)
//		Group.m_tpaSuspiciousGroups[i] = 0;

 	u32 N = Group.m_tpaSuspiciousNodes.size();
	for (int i=0, iGroupCounter = 1; i<(int)N; i++, iGroupCounter++) {
		if (!Group.m_tpaSuspiciousNodes[i].dwGroup) 
			Group.m_tpaSuspiciousNodes[i].dwGroup = iGroupCounter;
		for (int j=0; j<(int)N; j++)
			if (!Group.m_tpaSuspiciousNodes[j].dwGroup && (getAI().ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[j].dwNodeID,Group.m_tpaSuspiciousNodes[i].dwNodeID) < GROUP_RADIUS))
				Group.m_tpaSuspiciousNodes[j].dwGroup = iGroupCounter;
	}
	for ( i=0; i<(int)N; i++)
		Group.m_tpaSuspiciousNodes[i].dwGroup--;
	Group.m_tpaSuspiciousGroups.resize(--iGroupCounter);
	for ( i=0; i<iGroupCounter; i++)
		Group.m_tpaSuspiciousGroups[i] = 0;
}

void CAI_Stalker::vfChooseSuspiciousNode(CAISelectorBase &tSelector)
{
	CGroup &Group = *getGroup();
	INIT_SQUAD_AND_LEADER;
	if (m_iCurrentSuspiciousNodeIndex >= (int)Group.m_tpaSuspiciousNodes.size())
		m_iCurrentSuspiciousNodeIndex = -1;
	if ((AI_Path.fSpeed < EPS_L) || (!m_bActionStarted)) {
		if ((m_iCurrentSuspiciousNodeIndex != -1) && (Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID == AI_NodeID))
			Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched = 2;
		if ((m_iCurrentSuspiciousNodeIndex == -1) || (Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched == 2))
			m_iCurrentSuspiciousNodeIndex = ifGetSuspiciousAvailableNode(m_iCurrentSuspiciousNodeIndex,Group);
		if (m_iCurrentSuspiciousNodeIndex != -1) {
			Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched = 1;
			Group.m_tpaSuspiciousGroups[Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwGroup] = 1;
			vfInitSelector(tSelector,Squad,Leader);
			tSelector.m_tpEnemyNode = getAI().Node(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID);
			tSelector.m_tEnemyPosition = getAI().tfGetNodeCenter(tSelector.m_tpEnemyNode);
			AI_Path.DestNode = Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
			vfBuildPathToDestinationPoint(0,true);
			m_bActionStarted = true;
		}
		else {
//			vfInitSelector(m_tSelectorRetreat,Squad,Leader);
//			vfSearchForBetterPosition(m_tSelectorRetreat,Squad,Leader);
//			m_tSavedEnemyPosition = getAI().tfGetNodeCenter(m_dwSavedEnemyNodeID = AI_Path.DestNode);
//			getGroup()->m_tpaSuspiciousNodes.clear();
//			vfFindAllSuspiciousNodes(m_dwSavedEnemyNodeID,m_tSavedEnemyPosition,m_tSavedEnemyPosition,_min(20.f,_min(1*8.f*vPosition.distance_to(m_tSavedEnemyPosition)/4.5f,60.f)),*getGroup());
//			vfClasterizeSuspiciousNodes(*getGroup());
//			m_bActionStarted = false;

			m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
			m_tSelectorRetreat.m_tpEnemyNode = getAI().Node(m_dwMyNodeID);
			m_tSelectorRetreat.m_tMyPosition = vPosition;
			m_tSelectorRetreat.m_tpMyNode = AI_Node;
			vfChoosePointAndBuildPath(m_tSelectorRetreat,true);
			m_iCurrentSuspiciousNodeIndex = -1;
			m_bActionStarted = true;
		}
	}
	else {
		for (int i=0, iCount = 0; i<(int)Group.m_tpaSuspiciousNodes.size(); i++)
			if (Group.m_tpaSuspiciousNodes[i].dwSearched != 2) {
				if ((Group.m_tpaSuspiciousNodes[i].dwNodeID == AI_NodeID) || bfCheckForNodeVisibility(Group.m_tpaSuspiciousNodes[i].dwNodeID, i == m_iCurrentSuspiciousNodeIndex))
					Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
				iCount++;
			}
		if ((m_iCurrentSuspiciousNodeIndex != -1) && (Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched == 2)) {
			AI_Path.TravelPath.clear();
			if (!iCount && (m_dwCurrentUpdate - m_dwLostEnemyTime > TIME_TO_SEARCH)) {
				m_tSavedEnemy = 0;
				GO_TO_PREV_STATE;
			}
			else {
				m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
				m_tSelectorRetreat.m_tpEnemyNode = getAI().Node(m_dwMyNodeID);
				m_tSelectorRetreat.m_tMyPosition = vPosition;
				m_tSelectorRetreat.m_tpMyNode = AI_Node;
				vfChoosePointAndBuildPath(m_tSelectorRetreat,true);
				m_iCurrentSuspiciousNodeIndex = -1;
				m_bActionStarted = true;
			}
		}
		else
			if (m_iCurrentSuspiciousNodeIndex != -1) {
				if (AI_Path.DestNode != Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID) {
					AI_Path.DestNode = Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
					vfBuildPathToDestinationPoint(0,true);
				}
			}
	}
}

int CAI_Stalker::ifGetSuspiciousAvailableNode(int iLastIndex, CGroup &Group)
{
	int Index = -1;
	float fMin = 1000, fCost;
	u32 dwNodeID = AI_NodeID;
	if (iLastIndex >= 0) {
		dwNodeID = Group.m_tpaSuspiciousNodes[iLastIndex].dwNodeID;
		int iLastGroup = Group.m_tpaSuspiciousNodes[iLastIndex].dwGroup;
		for (int i=0; i<(int)Group.m_tpaSuspiciousNodes.size(); i++) {
			if (((int)Group.m_tpaSuspiciousNodes[i].dwGroup != iLastGroup) || Group.m_tpaSuspiciousNodes[i].dwSearched)
				continue;
//			if (Group.m_tpaSuspiciousNodes[i].dwSearched)
//				continue;
//			if (Group.m_tpaSuspiciousNodes[i].fCost < fMin) {
//				fMin = Group.m_tpaSuspiciousNodes[i].fCost;
//				Index = i;
//			}
			if ((fCost = getAI().ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[i].dwNodeID,dwNodeID)) < fMin) {
				fMin = fCost;
				Index = i;
			}
		}
	}
	
	if (Index >= 0)
		return(Index);

	for (int i=0; i<(int)Group.m_tpaSuspiciousNodes.size(); i++) {
//		if (Group.m_tpaSuspiciousGroups[Group.m_tpaSuspiciousNodes[i].dwGroup])
//			continue;
		if (Group.m_tpaSuspiciousNodes[i].dwSearched)
			continue;
		if (Group.m_tpaSuspiciousNodes[i].fCost < fMin) {
			fMin = Group.m_tpaSuspiciousNodes[i].fCost;
			Index = i;
		}
//		if ((fCost = getAI().ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[i].dwNodeID,dwNodeID)) < fMin) {
//			fMin = fCost;
//			Index = i;
//		}
	}

	if (Index == -1) {
		for (int i=0; i<(int)Group.m_tpaSuspiciousNodes.size(); i++) {
			if (Group.m_tpaSuspiciousNodes[i].dwSearched)
				continue;
			if (Group.m_tpaSuspiciousNodes[i].fCost < fMin) {
				fMin = Group.m_tpaSuspiciousNodes[i].fCost;
				Index = i;
			}
//			if ((fCost = getAI().ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[i].dwNodeID,dwNodeID)) < fMin) {
//				fMin = fCost;
//				Index = i;
//			}
		}
	}
	return(Index);
}
