////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_space.h
//	Created 	: 28.03.2003
//  Modified 	: 28.03.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker types and structures
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\..\\a_star.h"
#include "..\\ai_monsters_misc.h"

#define MIN_RANGE_SEARCH_TIME_INTERVAL	 2000
#define TIME_TO_SEARCH					60000
#define DODGE_AMPLITUDE					.5f
#define MAX_DODGE_DISTANCE				1.5f

void CAI_Stalker::vfInitSelector(IBaseAI_NodeEvaluator &S, CSquad &Squad, CEntity* &Leader)
{
	S.m_tHitDir			= m_tHitDir;
	S.m_dwHitTime		= m_dwHitTime;
	S.m_dwCurTime		= m_dwCurrentUpdate;
	S.m_tMe				= this;
	S.m_tpMyNode		= AI_Node;
	S.m_tMyPosition		= Position();
	
	if (m_tEnemy.Enemy) {
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
	
	S.m_taMembers		= &(Squad.Groups[g_Group()].Members);
	S.m_dwStartNode		= AI_NodeID;
	S.m_tStartPosition	= vPosition;
}

void CAI_Stalker::vfSearchForBetterPosition(IBaseAI_NodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader)
{
	Device.Statistic.AI_Range.Begin();
	
	if ((!m_dwLastRangeSearch) || (AI_Path.TravelPath.empty()) || (int(AI_Path.TravelStart) > int(AI_Path.TravelPath.size()) - 4) || (AI_Path.fSpeed < EPS_L) || ((tNodeEvaluator.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SEARCH_TIME_INTERVAL))) {
		
		m_dwLastRangeSearch = tNodeEvaluator.m_dwCurTime;
		
		vfInitSelector(tNodeEvaluator,Squad,Leader);
		
		float fOldCost = MAX_NODE_ESTIMATION_COST;
		
		if (AI_Path.DestNode != u32(-1)) {
			tNodeEvaluator.m_tpCurrentNode	= getAI().Node(AI_Path.DestNode);
			tNodeEvaluator.m_fDistance		= vPosition.distance_to(getAI().tfGetNodeCenter(AI_Path.DestNode));
			fOldCost						= tNodeEvaluator.ffEvaluateNode();
//			Msg								("Old  : [%d][%f]",AI_NodeID,fOldCost);
		}

		Squad.Groups[g_Group()].GetAliveMemberInfo(tNodeEvaluator.m_taMemberPositions, tNodeEvaluator.m_taMemberNodes, tNodeEvaluator.m_taDestMemberPositions, tNodeEvaluator.m_taDestMemberNodes, this);
		
		tNodeEvaluator.vfShallowGraphSearch(getAI().q_mark_bit);
		
//		Msg									("Best : [%d][%f]",tNodeEvaluator.m_dwBestNode,tNodeEvaluator.m_fBestCost);
//		Msg									("Params : %f - [%f][%f][%f][%f][%f][%f]",m_tEnemy.Enemy->Position().distance_to(vPosition),tNodeEvaluator.m_fMaxEnemyDistance,tNodeEvaluator.m_fOptEnemyDistance,tNodeEvaluator.m_fMinEnemyDistance,tNodeEvaluator.m_fMaxEnemyDistanceWeight,tNodeEvaluator.m_fOptEnemyDistanceWeight,tNodeEvaluator.m_fMinEnemyDistanceWeight);
		if ((AI_Path.DestNode != tNodeEvaluator.m_dwBestNode) && (tNodeEvaluator.m_fBestCost < fOldCost - 0.f)){
			AI_Path.DestNode		= tNodeEvaluator.m_dwBestNode;
			m_tPathState			= ePathStateBuildNodePath;
			vfAddToSearchList		();
			m_bIfSearchFailed		= false;
		} 
		else
			m_bIfSearchFailed		= true;
	}

	Device.Statistic.AI_Range.End();
}

void CAI_Stalker::vfBuildPathToDestinationPoint(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	Device.Statistic.AI_Path.Begin();
	
	if (AI_Path.DestNode == AI_NodeID) {
		AI_Path.Nodes.clear();
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;
		m_fCurSpeed = 0;
		m_tPathState			= ePathStateSearchNode;
		Device.Statistic.AI_Path.End();
		return;
	}
	
	if (tpNodeEvaluator)
		getAI().m_tpAStar->ffFindOptimalPath(AI_NodeID,AI_Path.DestNode,AI_Path,tpNodeEvaluator->m_dwEnemyNode,tpNodeEvaluator->m_fOptEnemyDistance);
	else
		getAI().m_tpAStar->ffFindMinimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);
	
	if (AI_Path.Nodes.empty())
		m_tPathState = ePathStateSearchNode;
	else
		m_tPathState = ePathStateBuildTravelLine;
	
	Device.Statistic.AI_Path.End();
}

void CAI_Stalker::vfBuildTravelLine(Fvector *tpDestinationPosition)
{
	Device.Statistic.AI_Path.Begin();
	
	if (m_tPathType == ePathTypeCriteria) {
		AI_Path.BuildTravelLine	(vPosition);
		m_tPathState			= ePathStateSearchNode;
		AI_Path.TravelStart		= 0;
	}
	else {
		m_tpaPoints.clear			();
		m_tpaDeviations.clear		();
		m_tpaTravelPath.clear		();
		m_tpaPointNodes.clear		();

		u32							N = AI_Path.Nodes.size();
		Fvector						tStartPosition = vPosition;
		u32							dwCurNode = AI_NodeID;
		m_tpaPoints.push_back		(vPosition);
		m_tpaPointNodes.push_back	(dwCurNode);

		for (u32 i=1; i<=N; i++)
			if (i<N) {
				if (!getAI().bfCheckNodeInDirection(dwCurNode,tStartPosition,AI_Path.Nodes[i])) {
					if (dwCurNode != AI_Path.Nodes[i - 1])
						m_tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
					else
						m_tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[i]));
					m_tpaPointNodes.push_back(dwCurNode);
				}
			}
			else
				if (tpDestinationPosition)
					if (getAI().dwfCheckPositionInDirection(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
						if (dwCurNode != AI_Path.Nodes[i - 1])
							m_tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
						m_tpaPoints.push_back(tStartPosition = *tpDestinationPosition);
					}
		
		if ((!tpDestinationPosition) && (tStartPosition.distance_to(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1])) > getAI().Header().size))
			m_tpaPoints.push_back(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1]));
		
		m_tpaDeviations.resize	(N = m_tpaPoints.size());
		
		if (m_tPathType == ePathTypeStraight)
			AI_Path.TravelPath.clear();
		else
			m_tpaTempPath.clear();
		AI_Path.Nodes.clear		();
		
		for (i=1; i<N; i++) {
			m_tpaLine.clear();
			m_tpaLine.push_back(m_tpaPoints[i-1]);
			m_tpaLine.push_back(m_tpaPoints[i]);
			getAI().vfCreateFastRealisticPath(m_tpaLine,m_tpaPointNodes[i-1],m_tpaDeviations,m_tpaTravelPath,m_tpaNodes,false,false,0,0);
			u32 n = m_tpaTravelPath.size();
			AI::CTravelNode	T;
			for (u32 j= i<2?0:1; j<n; j++) {
				T.P = m_tpaTravelPath[j];
				if (m_tPathType == ePathTypeStraight)
					AI_Path.TravelPath.push_back(T);
				else
					m_tpaTempPath.push_back(T.P);
				AI_Path.Nodes.push_back(m_tpaNodes[j]);
			}
		}
		
		AI_Path.Nodes[AI_Path.Nodes.size() - 1] = AI_Path.DestNode;
		
		if (m_tPathType == ePathTypeStraight) {
			AI_Path.TravelStart = 0;
			m_tPathState		= ePathStateSearchNode;
		}
		else
			m_tPathState		= ePathStateDodgeTravelLine;
	}
	
	Device.Statistic.AI_Path.End();
}

void CAI_Stalker::vfDodgeTravelLine()
{
	Device.Statistic.AI_Path.Begin();

	int							N = m_tpaTempPath.size();
	AI_Path.TravelPath.resize	(N);
	Fvector						tLeft, tRight, tStartPosition;
	
	if (N) {
		tStartPosition						= m_tpaTempPath[N - 1];
		tStartPosition.sub					(vPosition);
		float								yaw, pitch;
		tStartPosition.getHP				(yaw,pitch);
		
		yaw									+= PI_DIV_2;
		tStartPosition.setHP				(yaw,pitch);
		tStartPosition.normalize_safe		();
		tLeft								= tStartPosition;
		
		yaw									+= PI;
		tStartPosition.setHP				(yaw,pitch);
		tStartPosition.normalize_safe		();
		tRight								= tStartPosition;
		
		AI_Path.TravelPath[0].floating		= FALSE;
		AI_Path.TravelPath[0].P				= m_tpaTempPath[0];
		AI_Path.TravelPath[N - 1].floating	= FALSE;
		AI_Path.TravelPath[N - 1].P			= m_tpaTempPath[N - 1];
		for (int i=1; i < N - 1; i++)
			if (i & 1) {
				Fvector						tPoint;
				if ((i % 4) == 1) {
					float					fDistance = DODGE_AMPLITUDE*_min(m_tpaTempPath[i].distance_to(m_tpaTempPath[i - 1]),MAX_DODGE_DISTANCE);
					tPoint					= tLeft;
					tPoint.mul				(fDistance);
					tPoint.add				(m_tpaTempPath[i]);
				}
				else
					if ((i % 4) == 3) {
						float				fDistance = DODGE_AMPLITUDE*_min(m_tpaTempPath[i].distance_to(m_tpaTempPath[i - 1]),MAX_DODGE_DISTANCE);
						tPoint				= tRight;
						tPoint.mul			(fDistance);
						tPoint.add			(m_tpaTempPath[i]);
					}
				
				u32							dwNode1, dwNode2;
				dwNode1						= getAI().dwfCheckPositionInDirection(AI_Path.Nodes[i - 1],m_tpaTempPath[i - 1],tPoint);
				dwNode2						= getAI().dwfCheckPositionInDirection(AI_Path.Nodes[i + 1],m_tpaTempPath[i + 1],tPoint);

				AI_Path.TravelPath[i].floating	= FALSE;
				
				if ((dwNode1 != -1) && (dwNode2 != -1)) {
					AI_Path.TravelPath[i].P		= tPoint;
					AI_Path.TravelPath[i].P.y	= getAI().ffGetY(*getAI().Node(dwNode1),AI_Path.TravelPath[i].P.x,AI_Path.TravelPath[i].P.z);
				}
				else
					AI_Path.TravelPath[i].P		= m_tpaTempPath[i];
			}
			else {
				AI_Path.TravelPath[i].floating	= FALSE;
				AI_Path.TravelPath[i].P			= m_tpaTempPath[i];
			}
	}
	
	AI_Path.TravelStart			= 0;
	m_tPathState				= ePathStateSearchNode;

	Device.Statistic.AI_Path.End();
}

void CAI_Stalker::vfChoosePointAndBuildPath(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition)
{
	INIT_SQUAD_AND_LEADER;

	if (m_tPrevPathType != m_tPathType) {
		m_tPrevPathType		= m_tPathType;
		m_tPathState		= ePathStateSearchNode;
		AI_Path.DestNode	= u32(-1);
	}
	switch (m_tPathState) {
		case ePathStateSearchNode : {
			if (tpNodeEvaluator)
				vfSearchForBetterPosition(*tpNodeEvaluator,Squad,Leader);
			else
				m_tPathState = ePathStateBuildNodePath;
			break;
		}
		case ePathStateBuildNodePath : {
			vfBuildPathToDestinationPoint(tpNodeEvaluator);
			break;
		}
		case ePathStateBuildTravelLine : {
			vfBuildTravelLine(tpDestinationPosition);
			break;
		}
		case ePathStateDodgeTravelLine : {
			vfDodgeTravelLine();
			break;
		}
	}
}

void CAI_Stalker::vfChooseNextGraphPoint()
{
	_GRAPH_ID						tGraphID		= m_tNextGP;
	u16								wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CALifeGraph::SGraphEdge			*tpaEdges		= (CALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);
	
	int								iPointCount		= (int)m_tpaTerrain.size();
	int								iBranches		= 0;
	for (int i=0; i<wNeighbourCount; i++)
		for (int j=0; j<iPointCount; j++)
			if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP))
				iBranches++;
	if (!iBranches) {
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
					m_tCurGP		= m_tNextGP;
					m_tNextGP		= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
					m_dwTimeToChange= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
					return;
				}
		}
	}
	else {
		int							iChosenBranch = ::Random.randI(0,iBranches);
		iBranches					= 0;
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
	Device.Statistic.AI_Range.Begin	();

	Group.m_tpaSuspiciousNodes.clear();

	BOOL bStop = FALSE;
	
	CAI_Space &AI = getAI();
	NodePosition QueryPos;
	AI.PackPosition(QueryPos,BasePos);

	AI.q_stack.clear();
	AI.q_stack.push_back(StartNode);
	AI.q_mark_bit[StartNode] = true;
	
	float range_sqr		= Range*Range;
	float fEyeFov;
	CEntityAlive *tpEnemy = dynamic_cast<CEntityAlive*>(m_tSavedEnemy);
	if (tpEnemy)
		fEyeFov = tpEnemy->ffGetFov();
	else
		fEyeFov = ffGetFov();
	fEyeFov *=PI/180.f;

	INIT_SQUAD_AND_LEADER;
	
	m_tpaNodeStack.clear();
	
	for (int i=0; i<(int)Group.Members.size(); i++)
		vfMarkVisibleNodes(Group.Members[i]);

	for (u32 it=0; it<AI.q_stack.size(); it++) {
		u32 ID = AI.q_stack[it];

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
	Device.Statistic.AI_Range.End();
}

#define GROUP_RADIUS	15.f

void CAI_Stalker::vfClasterizeSuspiciousNodes(CGroup &Group)
{
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

void CAI_Stalker::vfChooseSuspiciousNode(IBaseAI_NodeEvaluator &tSelector)
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
			vfBuildPathToDestinationPoint(0);
			m_bActionStarted = true;
		}
		else {
			m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
			m_tSelectorRetreat.m_tpEnemyNode = getAI().Node(m_dwMyNodeID);
			m_tSelectorRetreat.m_tMyPosition = vPosition;
			m_tSelectorRetreat.m_tpMyNode = AI_Node;
			vfChoosePointAndBuildPath(&m_tSelectorRetreat);
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
				vfChoosePointAndBuildPath(&m_tSelectorRetreat);
				m_iCurrentSuspiciousNodeIndex = -1;
				m_bActionStarted = true;
			}
		}
		else
			if (m_iCurrentSuspiciousNodeIndex != -1) {
				if (AI_Path.DestNode != Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID) {
					AI_Path.DestNode = Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
					vfBuildPathToDestinationPoint(0);
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
			if ((fCost = getAI().ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[i].dwNodeID,dwNodeID)) < fMin) {
				fMin = fCost;
				Index = i;
			}
		}
	}
	
	if (Index >= 0)
		return(Index);

	for (int i=0; i<(int)Group.m_tpaSuspiciousNodes.size(); i++) {
		if (Group.m_tpaSuspiciousNodes[i].dwSearched)
			continue;
		if (Group.m_tpaSuspiciousNodes[i].fCost < fMin) {
			fMin = Group.m_tpaSuspiciousNodes[i].fCost;
			Index = i;
		}
	}

	if (Index == -1) {
		for (int i=0; i<(int)Group.m_tpaSuspiciousNodes.size(); i++) {
			if (Group.m_tpaSuspiciousNodes[i].dwSearched)
				continue;
			if (Group.m_tpaSuspiciousNodes[i].fCost < fMin) {
				fMin = Group.m_tpaSuspiciousNodes[i].fCost;
				Index = i;
			}
		}
	}
	return(Index);
}