////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_a_star.cpp
//	Created 	: 21.03.2002
//  Modified 	: 05.04.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding the x-est path between two nodes
////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "stdafx.h"
#include "xrLevel.h"
#include "ai_nodes.h"
#include "ai_a_star.h"

#define ASSIGN_GOODNESS(t)		(t)->f = (t)->g + (t)->h;
#define OPEN_MASK				1
#define mNodeStructure(x)		(*(Node(x)))
#define mNode(x)				(Node(x))

float fSize,fYSize,fSize2,fYSize2,fCriteriaLightWeight,fCriteriaCoverWeight,fCriteriaDistanceWeight,fCriteriaEnemyViewWeight;

void vfLoadSearch()
{
	fSize2		= _sqr(fSize = m_header.size)/4;
	fYSize2		= _sqr(fYSize = (float)(m_header.size_y/32767.0))/4;
}

IC float ffCriteria(NodeCompressed &tNode0, NodeCompressed &tNode1)
{
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	return(_sqrt((float)(fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + 0*fYSize2*_sqr(y2 - y1))));
}

IC void vfUpdateSuccessors(TNode *tpList, float dDifference)
{
	TNode *tpTemp = tpList->tpForward;
	while (tpTemp) {
		if (tpTemp->tpForward)
			vfUpdateSuccessors(tpTemp->tpForward,dDifference);
		tpTemp->g -= dDifference;
		ASSIGN_GOODNESS(tpTemp)

		if (tpTemp->ucOpenCloseMask)
			if (tpTemp->tpOpenedPrev->f > tpTemp->f) {
				tpTemp->tpOpenedPrev->tpOpenedNext = tpTemp->tpOpenedNext;
				if (tpTemp->tpOpenedNext)
					tpTemp->tpOpenedNext->tpOpenedPrev = tpTemp->tpOpenedPrev;
				float dTemp = tpTemp->f;
				TNode *tpTemp1 = tpTemp;
				for (tpTemp = tpTemp->tpOpenedPrev; tpTemp; tpTemp = tpTemp->tpOpenedPrev)
					if (tpTemp->f <= dTemp) {
						tpTemp1->tpOpenedNext = tpTemp->tpOpenedNext;
						tpTemp1->tpOpenedPrev = tpTemp;
						if (tpTemp->tpOpenedNext)
							tpTemp->tpOpenedNext->tpOpenedPrev = tpTemp1;
						tpTemp->tpOpenedNext = tpTemp1;
						tpTemp = tpTemp1;
						break;
					}
			}

		tpTemp = tpTemp->tpNext;
	}
}

void vfFindTheShortestPath(TNode *taHeap, TIndexNode *tpaIndexes, u32 &dwAStarStaticCounter, u32 dwStartNode, u32 dwGoalNode, float &fDistance, float fMaxDistance, Fvector tStartPosition, Fvector tFinishPosition, vector<u32> &tpaNodes, CCriticalSection &tCriticalSection, bool bDebug)
{
	// initialization
	dwAStarStaticCounter++;

	u32  uiHeap = 0;

	TNode  *tpOpenedList = taHeap + uiHeap++,
		   *tpTemp       = tpaIndexes[dwStartNode].tpNode = taHeap + uiHeap++,
		   *tpTemp1,
		   *tpTemp2,
		   *tpBestNode;
	
	memset(tpOpenedList,0,sizeof(TNode));
	memset(tpTemp,0,sizeof(TNode));
	
	tpaIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

	tpOpenedList->tpOpenedNext = tpTemp;
	tpTemp->iIndex = dwStartNode;
	tpTemp->g = 0.0;
	tpTemp->h = ffCriteria(mNodeStructure(dwStartNode),mNodeStructure(dwGoalNode));
	tpTemp->tpOpenedPrev = tpOpenedList;
	tpTemp->ucOpenCloseMask = OPEN_MASK;
	ASSIGN_GOODNESS(tpTemp)
	
	while (tpOpenedList->tpOpenedNext) {
		
		// finding the node being estimated as the cheapest among the opened ones
		tpBestNode = tpOpenedList->tpOpenedNext;

		// checking if distance is too big
		if (tpBestNode->f >= fMaxDistance) {
			fDistance = MAX_VALUE;
			return;
		}
		
		// remove that node from the opened list and put that node to the closed list
		tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
		if (tpBestNode->tpOpenedNext)
			tpBestNode->tpOpenedNext->tpOpenedPrev = tpOpenedList;
		tpBestNode->ucOpenCloseMask = 0;

		// check if that node is our goal
		if (tpBestNode->iIndex == (int)dwGoalNode) {
			fDistance = tpBestNode->g;
			/**/
			Msg("[%6d][%6d]",dwStartNode,tpBestNode->iIndex);
			float fStraightDistance = fDistance;

			tpTemp1 = tpBestNode;
			tpTemp = tpTemp1->tpBack;

			for (u32 i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++) ;

			tpaNodes.resize(i);

			tpTemp1 = tpBestNode;
			tpaNodes[--i] = tpBestNode->iIndex;
			tpTemp = tpTemp1->tpBack;
			for (u32 j=1; tpTemp; tpTemp = tpTemp->tpBack, j++)
				tpaNodes[i - j] = tpTemp->iIndex;

			if (!bDebug) {
				float fCumulativeDistance = 0, fLastDirectDistance = 0, fDirectDistance;
				Fvector tPosition = tStartPosition;
				u32 dwNode = tpaNodes[0];
				for (i=1; i<(int)tpaNodes.size(); i++) {
					fDirectDistance = ffCheckPositionInDirection(dwNode,tPosition,tfGetNodeCenter(tpaNodes[i]),fMaxDistance);
					if (fDirectDistance == MAX_VALUE) {
						if (fLastDirectDistance == 0) {
							fCumulativeDistance += ffGetDistanceBetweenNodeCenters(dwNode,tpaNodes[i]);
							dwNode = tpaNodes[i];
						}
						else {
							fCumulativeDistance += fLastDirectDistance;
							fLastDirectDistance = 0;
							dwNode = tpaNodes[i-- - 1];
						}
						tPosition = tfGetNodeCenter(dwNode);
					}
					else 
						fLastDirectDistance = fDirectDistance;
				}
				fDirectDistance = ffCheckPositionInDirection(dwNode,tPosition,tFinishPosition,fMaxDistance);
				if (fDirectDistance == MAX_VALUE)
					fDistance = fCumulativeDistance + fLastDirectDistance + tFinishPosition.distance_to(tfGetNodeCenter(tpaNodes[tpaNodes.size() - 1]));
				else
					fDistance = fCumulativeDistance + fDirectDistance;
			}
			else {
				float fCumulativeDistance = 0, fLastDirectDistance = 0, fDirectDistance;
				Fvector tPosition = tStartPosition;
				Msg("Debugging...");
				Msg("%d",tpaNodes.size());
				u32 dwNode = tpaNodes[0];
				Msg("%d",dwNode);
				for (i=1; i<(int)tpaNodes.size(); i++) {
					fDirectDistance = ffCheckPositionInDirection(dwNode,tPosition,tfGetNodeCenter(tpaNodes[i]),fMaxDistance);
					Msg("%6.2f",fDirectDistance);
					if (fDirectDistance == MAX_VALUE) {
						if (fLastDirectDistance == 0) {
							fCumulativeDistance += ffGetDistanceBetweenNodeCenters(dwNode,tpaNodes[i]);
							dwNode = tpaNodes[i];
						}
						else {
							fCumulativeDistance += fLastDirectDistance;
							fLastDirectDistance = 0;
							dwNode = tpaNodes[i-- - 1];
						}
						tPosition = tfGetNodeCenter(dwNode);
					}
					else 
						fLastDirectDistance = fDirectDistance;
				}
				Msg("%6.2f",fDirectDistance);
				fDirectDistance = ffCheckPositionInDirection(dwNode,tPosition,tFinishPosition,fMaxDistance);
				Msg("%6.2f",fDirectDistance);
				if (fDirectDistance == MAX_VALUE)
					fDistance = fCumulativeDistance + fLastDirectDistance + tFinishPosition.distance_to(tfGetNodeCenter(tpaNodes[tpaNodes.size() - 1]));
				else
					fDistance = fCumulativeDistance + fDirectDistance;
			}
			/**/
			tCriticalSection.Enter();
			Msg("[%6d][%6d] : %6.2f -> %6.2f (%6.2f%c)",dwStartNode,tpBestNode->iIndex,fStraightDistance,fDistance,(fStraightDistance - fDistance)/fStraightDistance*100,'%');
			tCriticalSection.Leave();
			return;
		}
		
		NodeLink *taLinks = (NodeLink *)((unsigned char *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->links, iNodeIndex;
		if (iCount) {
			iNodeIndex = UnpackLink(taLinks[0]);
			for (int i=0; i<iCount; i++) {
				// checking if that node is in the path of the BESTNODE ones
				iNodeIndex = UnpackLink(taLinks[i]);
				// checking if that node is in the path of the BESTNODE ones
				if (tpaIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					bool bOk = true;
					tpTemp = tpaIndexes[iNodeIndex].tpNode;
					if (!(tpTemp->ucOpenCloseMask)) {
						int iBestIndex = tpBestNode->iIndex;
						tpTemp2 = tpTemp->tpForward;
						while (tpTemp2) {
							if (tpTemp2->iIndex == iBestIndex) {
								bOk = false;
								break;
							}
							tpTemp2 = tpTemp2->tpForward;
						}
						if (!bOk)
							continue;
					}
					
					// initialize node
					float dG = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex));
					
					// check if this node is already in the opened list
					if (tpTemp->ucOpenCloseMask) {
						if (tpTemp->g > dG) {
							tpTemp->g = dG;
							ASSIGN_GOODNESS(tpTemp)
							tpTemp->tpBack = tpBestNode;
							if (tpTemp->tpOpenedPrev->f > tpTemp->f) {
								tpTemp->tpOpenedPrev->tpOpenedNext = tpTemp->tpOpenedNext;
								if (tpTemp->tpOpenedNext)
									tpTemp->tpOpenedNext->tpOpenedPrev = tpTemp->tpOpenedPrev;
								float dTemp = tpTemp->f;
								tpTemp1 = tpTemp;
								for (tpTemp = tpTemp->tpOpenedPrev; tpTemp; tpTemp = tpTemp->tpOpenedPrev)
									if (tpTemp->f <= dTemp) {
										tpTemp1->tpOpenedNext = tpTemp->tpOpenedNext;
										tpTemp1->tpOpenedPrev = tpTemp;
										if (tpTemp->tpOpenedNext)
											tpTemp->tpOpenedNext->tpOpenedPrev = tpTemp1;
										tpTemp->tpOpenedNext = tpTemp1;
										break;
									}
							}
						}
						continue;
					}

					if (!(tpTemp->ucOpenCloseMask)) {
						if (tpTemp->g > dG) {
							vfUpdateSuccessors(tpTemp,tpTemp->g - dG);
							tpTemp->g = dG;
							ASSIGN_GOODNESS(tpTemp)
							tpTemp->tpBack = tpBestNode;
						}
						continue;
					}
				}
				else {
					tpTemp2 = tpaIndexes[iNodeIndex].tpNode = taHeap + uiHeap++;
					tpTemp2->tpNext = tpTemp2->tpForward = tpTemp2->tpOpenedNext = tpTemp2->tpOpenedPrev = 0;
					tpaIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex));

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = ffCriteria(mNodeStructure(dwGoalNode),mNodeStructure(iNodeIndex));
					ASSIGN_GOODNESS(tpTemp2)
					
					tpTemp  = tpOpenedList;
					tpTemp1 = tpOpenedList->tpOpenedNext;
					float dTemp = tpTemp2->f;
					bool bOk = false;
					while (tpTemp1) {
						if (tpTemp1->f >= dTemp) {
							tpTemp2->tpOpenedNext = tpTemp1;
							tpTemp2->tpOpenedPrev = tpTemp;
							tpTemp->tpOpenedNext = tpTemp2;
							tpTemp1->tpOpenedPrev = tpTemp2;
							bOk = true;
							break;
						}
						tpTemp  = tpTemp1;
						tpTemp1 = tpTemp1->tpOpenedNext;
					}
					if (!bOk) {
						tpTemp->tpOpenedNext = tpTemp2;
						tpTemp2->tpOpenedPrev = tpTemp;
					}
					tpTemp2->ucOpenCloseMask = OPEN_MASK;
					
					// make it a BESTNODE successor
					tpTemp1 = tpBestNode->tpForward;
					tpBestNode->tpForward = tpTemp2;
					tpTemp2->tpNext = tpTemp1;
				}
			}
		}
		if (uiHeap > m_header.count)
			break;
	}
	
	fDistance = MAX_VALUE;
}
