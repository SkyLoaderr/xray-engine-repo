////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_a_star.cpp
//	Created 	: 21.03.2002
//  Modified 	: 05.04.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding the x-est path between two nodes
////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "stdafx.h"
#include "ai_a_star.h"
#include "ai_space.h"

#define ASSIGN_GOODNESS(t)		(t)->f = (t)->g + (t)->h;
#define OPEN_MASK				1
#define mNodeStructure(x)		(*(this->Node(x)))
#define mNode(x)				(this->Node(x))
#define MAX_VALUE				100000.0
#define MAX_NODES				65535

float fSize,fYSize,fSize2,fYSize2,fCriteriaLightWeight,fCriteriaCoverWeight,fCriteriaDistanceWeight,fCriteriaEnemyViewWeight;

static DWORD dwAStarStaticCounter = 0;
TNode		*taHeap;
TIndexNode	*tpaIndexes;

#define OPTINAL_ENEMY_DISTANCE 40.f

IC float ffCriteria(NodeCompressed tNode0, NodeCompressed tNode1)
{
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	float fCover = 1/((float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

	float fLight = (float)(tNode1.light)/255.f;
	
	return(fLight*fCriteriaLightWeight + fCover*fCriteriaCoverWeight + fCriteriaDistanceWeight*_sqrt((float)(fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + 0*fYSize2*_sqr(y2 - y1))));
}

IC float ffCriteria(NodeCompressed tNode0, NodeCompressed tNode1, NodeCompressed tEnemyNode, float fOptimalEnemyDistance)
{
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	float x3 = (float)(tEnemyNode.p1.x) + (float)(tEnemyNode.p0.x);
	float y3 = (float)(tEnemyNode.p1.y) + (float)(tEnemyNode.p0.y);
	float z3 = (float)(tEnemyNode.p1.z) + (float)(tEnemyNode.p0.z);

	float fCover = (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f;

	float fLight = (float)(tNode1.light)/255.f;
	
	return(fCriteriaEnemyViewWeight*_sqr(_sqrt((float)(fSize2*(_sqr(x3 - x1) + _sqr(z3 - z1)) + fYSize2*_sqr(y3 - y1))) - fOptimalEnemyDistance) + fLight*fCriteriaLightWeight + fCover*fCriteriaCoverWeight + fCriteriaDistanceWeight*(float)sqrt((float)(fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + fYSize2*_sqr(y2 - y1))));
}

IC float ffCriteria(NodeCompressed tNode0, NodeCompressed tNode1, Fvector tEnemyPosition, float fOptimalEnemyDistance)
{
	/**/
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	float x3 = tEnemyPosition.x/fSize;
	float y3 = tEnemyPosition.y/fYSize;
	float z3 = tEnemyPosition.z/fSize;

	float fCover = (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f;

	float fLight = (float)(tNode1.light)/255.f;
	
	return(fCriteriaEnemyViewWeight*(_sqrt((float)(fSize2*(_sqr(x3 - x1) + _sqr(z3 - z1)) + fYSize2*_sqr(y3 - y1))) - fOptimalEnemyDistance) + fLight*fCriteriaLightWeight + fCover*fCriteriaCoverWeight + fCriteriaDistanceWeight*(float)sqrt((float)(fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + fYSize2*_sqr(y2 - y1))));
}

IC void vfUpdateSuccessors(TNode *tpList, float dDifference)
{
	TNode *tpTemp = taHeap + tpList->tpForward;
	while (tpTemp - taHeap) {
		if (tpTemp->tpForward)
			vfUpdateSuccessors(taHeap + tpTemp->tpForward,dDifference);
		tpTemp->g -= dDifference;
		ASSIGN_GOODNESS(tpTemp)

		if (tpTemp->ucOpenCloseMask)
			if (taHeap[tpTemp->tpOpenedPrev].f > tpTemp->f) {
				taHeap[tpTemp->tpOpenedPrev].tpOpenedNext = tpTemp->tpOpenedNext;
				if (tpTemp->tpOpenedNext)
					taHeap[tpTemp->tpOpenedNext].tpOpenedPrev = tpTemp->tpOpenedPrev;
				float dTemp = tpTemp->f;
				TNode *tpTemp1 = tpTemp;
				for (tpTemp = taHeap + tpTemp->tpOpenedPrev; tpTemp - taHeap; tpTemp = taHeap + tpTemp->tpOpenedPrev)
					if (tpTemp->f <= dTemp) {
						tpTemp1->tpOpenedNext = tpTemp->tpOpenedNext;
						tpTemp1->tpOpenedPrev = tpTemp - taHeap;
						if (tpTemp->tpOpenedNext)
							taHeap[tpTemp->tpOpenedNext].tpOpenedPrev = tpTemp1 - taHeap;
						tpTemp->tpOpenedNext = tpTemp1 - taHeap;
						tpTemp = tpTemp1;
						break;
					}
			}

		tpTemp = taHeap + tpTemp->tpNext;
	}
}

void CAI_Space::vfLoadSearch()
{
	fSize2		= _sqr(fSize = this->m_header.size)/4;
	fYSize2		= _sqr(fYSize = (float)(this->m_header.size_y/32767.0))/4;

	DWORD M1	= Engine.mem_Usage();
	DWORD S1	= (MAX_NODES + 1)  * sizeof(TNode);		
	taHeap		= (TNode *)		xr_malloc(S1); 
	ZeroMemory(taHeap,S1);
	DWORD S2	= (this->m_header.count)* sizeof(TIndexNode);	
	tpaIndexes	= (TIndexNode *)xr_malloc(S2); 
	ZeroMemory(tpaIndexes,S2);
	DWORD M2	= Engine.mem_Usage();
	Msg			("* AI path-finding-structures: %d K",(M2-M1)/(1024));
}

void CAI_Space::vfUnloadSearch()
{
	_FREE(taHeap);
	_FREE(tpaIndexes);
}

float CAI_Space::vfFindTheXestPath(DWORD dwStartNode, DWORD dwGoalNode, AI::Path& Result, float fLightWeight, float fCoverWeight, float fDistanceWeight)
{
	Device.Statistic.AI_Path.Begin();
	// initialization
	dwAStarStaticCounter++;

	fCriteriaLightWeight = fLightWeight;
	fCriteriaCoverWeight = fCoverWeight;
	fCriteriaDistanceWeight = fDistanceWeight;

	DWORD uiHeap = 0;

	TNode  *tpOpenedList = taHeap + uiHeap++,
		   *tpTemp       = taHeap + uiHeap++,
		   *tpTemp1,
		   *tpTemp2,
		   *tpBestNode;
	
	memset(tpOpenedList,0,sizeof(TNode));
	memset(tpTemp,0,sizeof(TNode));
	
	tpaIndexes[dwStartNode].tpNode = uiHeap - 1;
	tpaIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

	tpOpenedList->tpOpenedNext = tpTemp - taHeap;
	tpTemp->iIndex = dwStartNode;
	tpTemp->g = 0.0;
	tpTemp->h = ffCriteria(mNodeStructure(dwStartNode),mNodeStructure(dwGoalNode));
	tpTemp->tpOpenedPrev = tpOpenedList - taHeap;
	tpTemp->ucOpenCloseMask = OPEN_MASK;
	ASSIGN_GOODNESS(tpTemp)
	
	uint uiNodeCount = 0;
	while (tpOpenedList->tpOpenedNext) {
		
		// finding the node being estimated as the cheapest among the opened ones
		tpBestNode = taHeap + tpOpenedList->tpOpenedNext;
		
		// remove that node from the opened list and put that node to the closed list
		tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
		if (tpBestNode->tpOpenedNext)
			taHeap[tpBestNode->tpOpenedNext].tpOpenedPrev = tpOpenedList - taHeap;
		tpBestNode->ucOpenCloseMask = 0;

		// check if that node is our goal
		if (tpBestNode->iIndex == (int)dwGoalNode) {

			float fDistance = 0.0;
			tpTemp1 = tpBestNode;
			tpTemp = taHeap + tpTemp1->tpBack;
			for (uint i=1; tpTemp - taHeap; tpTemp1 = tpTemp, tpTemp = taHeap + tpTemp->tpBack, i++) //;
				fDistance += ffGetDistanceBetweenNodeCenters(tpTemp1->iIndex,tpTemp->iIndex);
			Result.Nodes.resize(i);

			tpTemp1 = tpBestNode;
			Result.Nodes[--i] = tpBestNode->iIndex;
			tpTemp = taHeap + tpTemp1->tpBack;
			for (uint j=1; tpTemp - taHeap; tpTemp = taHeap + tpTemp->tpBack, j++)
				Result.Nodes[i - j] = tpTemp->iIndex;
				
			Device.Statistic.AI_Path.End();
			return(fDistance);
		}
		
		NodeLink *taLinks = (NodeLink *)((uchar *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->links, iNodeIndex;
		if (iCount) {
			iNodeIndex = this->UnpackLink(taLinks[0]);
			for (int i=0; i<iCount; i++) {
				// checking if that node the node of the moving object 
				if (q_mark[iNodeIndex])
					continue;
				// checking if that node is in the path of the BESTNODE ones
				iNodeIndex = this->UnpackLink(taLinks[i]);
				if (tpaIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					bool bOk = true;
					tpTemp = taHeap + tpaIndexes[iNodeIndex].tpNode;
					if (!(tpTemp->ucOpenCloseMask)) {
						int iBestIndex = tpBestNode->iIndex;
						tpTemp2 = taHeap + tpTemp->tpForward;
						while (tpTemp2 - taHeap) {
							if (tpTemp2->iIndex == iBestIndex) {
								bOk = false;
								break;
							}
							tpTemp2 = taHeap + tpTemp2->tpForward;
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
							tpTemp->tpBack = tpBestNode - taHeap;
							if (taHeap[tpTemp->tpOpenedPrev].f > tpTemp->f) {
								taHeap[tpTemp->tpOpenedPrev].tpOpenedNext = tpTemp->tpOpenedNext;
								if (tpTemp->tpOpenedNext)
									taHeap[tpTemp->tpOpenedNext].tpOpenedPrev = tpTemp->tpOpenedPrev;
								float dTemp = tpTemp->f;
								tpTemp1 = tpTemp;
								for (tpTemp = taHeap + tpTemp->tpOpenedPrev; tpTemp - taHeap; tpTemp = taHeap + tpTemp->tpOpenedPrev)
									if (tpTemp->f <= dTemp) {
										tpTemp1->tpOpenedNext = tpTemp->tpOpenedNext;
										tpTemp1->tpOpenedPrev = tpTemp - taHeap;
										if (tpTemp->tpOpenedNext)
											taHeap[tpTemp->tpOpenedNext].tpOpenedPrev = tpTemp1 - taHeap;
										tpTemp->tpOpenedNext = tpTemp1 - taHeap;
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
							tpTemp->tpBack = tpBestNode - taHeap;
						}
						continue;
					}
				}
				else {
					tpTemp2 = taHeap + uiHeap++;
					tpTemp2->tpNext = tpTemp2->tpForward = tpTemp2->tpOpenedNext = tpTemp2->tpOpenedPrev = 0;
					tpaIndexes[iNodeIndex].tpNode = uiHeap - 1;
					tpaIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode - taHeap;
					tpTemp2->g = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex));

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = ffCriteria(mNodeStructure(dwGoalNode),mNodeStructure(iNodeIndex));
					ASSIGN_GOODNESS(tpTemp2)
					
					tpTemp  = tpOpenedList;
					tpTemp1 = taHeap + tpOpenedList->tpOpenedNext;
					float dTemp = tpTemp2->f;
					bool bOk = false;
					while (tpTemp1 - taHeap) {
						if (tpTemp1->f >= dTemp) {
							tpTemp2->tpOpenedNext = tpTemp1 - taHeap;
							tpTemp2->tpOpenedPrev = tpTemp - taHeap;
							tpTemp->tpOpenedNext = tpTemp2 - taHeap;
							tpTemp1->tpOpenedPrev = tpTemp2 - taHeap;
							bOk = true;
							break;
						}
						tpTemp  = tpTemp1;
						tpTemp1 = taHeap + tpTemp1->tpOpenedNext;
					}
					if (!bOk) {
						tpTemp->tpOpenedNext = tpTemp2 - taHeap;
						tpTemp2->tpOpenedPrev = tpTemp - taHeap;
					}
					tpTemp2->ucOpenCloseMask = OPEN_MASK;
					
					// make it a BESTNODE successor
					tpTemp1 = taHeap + tpBestNode->tpForward;
					tpBestNode->tpForward = tpTemp2 - taHeap;
					tpTemp2->tpNext = tpTemp1 - taHeap;
				}
			}
		}
		uiNodeCount++;
		if (uiHeap > MAX_NODES)
			break;
	}
	
	Result.Nodes.clear();
	Device.Statistic.AI_Path.End();
	return(MAX_VALUE);
}

float CAI_Space::vfFindTheXestPath(DWORD dwStartNode, DWORD dwGoalNode, AI::Path& Result, NodeCompressed& tEnemyNode, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight)
{
	Device.Statistic.AI_Path.Begin();
	// initialization
	dwAStarStaticCounter++;

	fCriteriaLightWeight = fLightWeight;
	fCriteriaCoverWeight = fCoverWeight;
	fCriteriaDistanceWeight = fDistanceWeight;

	WORD uiHeap = 0;

	TNode  *tpOpenedList = taHeap + uiHeap++,
		   *tpTemp       = taHeap + uiHeap++,
		   *tpTemp1,
		   *tpTemp2,
		   *tpBestNode;
	
	memset(tpOpenedList,0,sizeof(TNode));
	memset(tpTemp,0,sizeof(TNode));
	
	tpaIndexes[dwStartNode].tpNode = uiHeap - 1;
	tpaIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

	tpOpenedList->tpOpenedNext = tpTemp - taHeap;
	tpTemp->iIndex = dwStartNode;
	tpTemp->g = 0.0;
	tpTemp->h = ffCriteria(mNodeStructure(dwStartNode),mNodeStructure(dwGoalNode));
	tpTemp->tpOpenedPrev = tpOpenedList - taHeap;
	tpTemp->ucOpenCloseMask = OPEN_MASK;
	ASSIGN_GOODNESS(tpTemp)
	
	uint uiNodeCount = 0;
	while (tpOpenedList->tpOpenedNext) {
		
		// finding the node being estimated as the cheapest among the opened ones
		tpBestNode = taHeap + tpOpenedList->tpOpenedNext;
		
		// remove that node from the opened list and put that node to the closed list
		tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
		if (tpBestNode->tpOpenedNext)
			taHeap[tpBestNode->tpOpenedNext].tpOpenedPrev = tpOpenedList - taHeap;
		tpBestNode->ucOpenCloseMask = 0;

		// check if that node is our goal
		if (tpBestNode->iIndex == (int)dwGoalNode) {

			float fDistance = 0.0;
			tpTemp1 = tpBestNode;
			tpTemp = taHeap + tpTemp1->tpBack;
			for (uint i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = taHeap + tpTemp->tpBack, i++) ;

			Result.Nodes.resize(i);

			tpTemp1 = tpBestNode;
			Result.Nodes[--i] = tpBestNode->iIndex;
			tpTemp = taHeap + tpTemp1->tpBack;
			for (uint j=1; tpTemp; tpTemp = taHeap + tpTemp->tpBack, j++)
				Result.Nodes[i - j] = tpTemp->iIndex;
				
			Device.Statistic.AI_Path.End();
			return(fDistance);
		}
		
		NodeLink *taLinks = (NodeLink *)((uchar *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->links, iNodeIndex;
		if (iCount) {
			iNodeIndex = this->UnpackLink(taLinks[0]);
			for (int i=0; i<iCount; i++) {
				// checking if that node the node of the moving object 
				if (q_mark[iNodeIndex])
					continue;
				// checking if that node is in the path of the BESTNODE ones
				iNodeIndex = this->UnpackLink(taLinks[i]);
				if (tpaIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					bool bOk = true;
					tpTemp = taHeap + tpaIndexes[iNodeIndex].tpNode;
					if (!(tpTemp->ucOpenCloseMask)) {
						int iBestIndex = tpBestNode->iIndex;
						tpTemp2 = taHeap + tpTemp->tpForward;
						while (tpTemp2) {
							if (tpTemp2->iIndex == iBestIndex) {
								bOk = false;
								break;
							}
							tpTemp2 = taHeap + tpTemp2->tpForward;
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
							tpTemp->tpBack = tpBestNode - taHeap;
							if (taHeap[tpTemp->tpOpenedPrev].f > tpTemp->f) {
								taHeap[tpTemp->tpOpenedPrev].tpOpenedNext = tpTemp->tpOpenedNext;
								if (tpTemp->tpOpenedNext)
									taHeap[tpTemp->tpOpenedNext].tpOpenedPrev = tpTemp->tpOpenedPrev;
								float dTemp = tpTemp->f;
								tpTemp1 = tpTemp;
								for (tpTemp = taHeap + tpTemp->tpOpenedPrev; tpTemp; tpTemp = taHeap + tpTemp->tpOpenedPrev)
									if (tpTemp->f <= dTemp) {
										tpTemp1->tpOpenedNext = tpTemp->tpOpenedNext;
										tpTemp1->tpOpenedPrev = tpTemp - taHeap;
										if (tpTemp->tpOpenedNext)
											taHeap[tpTemp->tpOpenedNext].tpOpenedPrev = tpTemp1 - taHeap;
										tpTemp->tpOpenedNext = tpTemp1 - taHeap;
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
							tpTemp->tpBack = tpBestNode - taHeap;
						}
						continue;
					}
				}
				else {
					tpTemp2 = taHeap + uiHeap++;
					tpTemp2->tpNext = tpTemp2->tpForward = tpTemp2->tpOpenedNext = tpTemp2->tpOpenedPrev = 0;
					tpaIndexes[iNodeIndex].tpNode = uiHeap - 1;
					tpaIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode - taHeap;
					tpTemp2->g = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex));

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = ffCriteria(mNodeStructure(dwGoalNode),mNodeStructure(iNodeIndex));
					ASSIGN_GOODNESS(tpTemp2)
					
					tpTemp  = tpOpenedList;
					tpTemp1 = taHeap + tpOpenedList->tpOpenedNext;
					float dTemp = tpTemp2->f;
					bool bOk = false;
					while (tpTemp1) {
						if (tpTemp1->f >= dTemp) {
							tpTemp2->tpOpenedNext = tpTemp1 - taHeap;
							tpTemp2->tpOpenedPrev = tpTemp - taHeap;
							tpTemp->tpOpenedNext = tpTemp2 - taHeap;
							tpTemp1->tpOpenedPrev = tpTemp2 - taHeap;
							bOk = true;
							break;
						}
						tpTemp  = tpTemp1;
						tpTemp1 = taHeap + tpTemp1->tpOpenedNext;
					}
					if (!bOk) {
						tpTemp->tpOpenedNext = tpTemp2 - taHeap;
						tpTemp2->tpOpenedPrev = tpTemp - taHeap;
					}
					tpTemp2->ucOpenCloseMask = OPEN_MASK;
					
					// make it a BESTNODE successor
					tpTemp1 = taHeap + tpBestNode->tpForward;
					tpBestNode->tpForward = tpTemp2 - taHeap;
					tpTemp2->tpNext = tpTemp1 - taHeap;
				}
			}
		}
		uiNodeCount++;
		if (uiHeap > MAX_NODES)
			break;
	}
	
	Result.Nodes.clear();
	Device.Statistic.AI_Path.End();
	return(MAX_VALUE);
}

float CAI_Space::vfFindTheXestPath(DWORD dwStartNode, DWORD dwGoalNode, AI::Path& Result, Fvector tEnemyPosition, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight)
{
	Device.Statistic.AI_Path.Begin();
	// initialization
	dwAStarStaticCounter++;

	fCriteriaLightWeight = fLightWeight;
	fCriteriaCoverWeight = fCoverWeight;
	fCriteriaDistanceWeight = fDistanceWeight;

	WORD uiHeap = 0;

	TNode  *tpOpenedList = taHeap + uiHeap++,
		   *tpTemp       = taHeap + uiHeap++,
		   *tpTemp1,
		   *tpTemp2,
		   *tpBestNode;
	
	memset(tpOpenedList,0,sizeof(TNode));
	memset(tpTemp,0,sizeof(TNode));
	
	tpaIndexes[dwStartNode].tpNode = uiHeap - 1;
	tpaIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

	tpOpenedList->tpOpenedNext = tpTemp - taHeap;
	tpTemp->iIndex = dwStartNode;
	tpTemp->g = 0.0;
	tpTemp->h = ffCriteria(mNodeStructure(dwStartNode),mNodeStructure(dwGoalNode));
	tpTemp->tpOpenedPrev = tpOpenedList - taHeap;
	tpTemp->ucOpenCloseMask = OPEN_MASK;
	ASSIGN_GOODNESS(tpTemp)
	
	uint uiNodeCount = 0;
	while (tpOpenedList->tpOpenedNext) {
		
		// finding the node being estimated as the cheapest among the opened ones
		tpBestNode = taHeap + tpOpenedList->tpOpenedNext;
		
		// remove that node from the opened list and put that node to the closed list
		tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
		if (tpBestNode->tpOpenedNext)
			taHeap[tpBestNode->tpOpenedNext].tpOpenedPrev = tpOpenedList - taHeap;
		tpBestNode->ucOpenCloseMask = 0;

		// check if that node is our goal
		if (tpBestNode->iIndex == (int)dwGoalNode) {

			float fDistance = 0.0;
			tpTemp1 = tpBestNode;
			tpTemp = taHeap + tpTemp1->tpBack;
			for (uint i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = taHeap + tpTemp->tpBack, i++) ;

			Result.Nodes.resize(i);

			tpTemp1 = tpBestNode;
			Result.Nodes[--i] = tpBestNode->iIndex;
			tpTemp = taHeap + tpTemp1->tpBack;
			for (uint j=1; tpTemp; tpTemp = taHeap + tpTemp->tpBack, j++)
				Result.Nodes[i - j] = tpTemp->iIndex;
				
			Device.Statistic.AI_Path.End();
			return(fDistance);
		}
		
		NodeLink *taLinks = (NodeLink *)((uchar *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->links, iNodeIndex;
		if (iCount) {
			iNodeIndex = this->UnpackLink(taLinks[0]);
			for (int i=0; i<iCount; i++) {
				// checking if that node the node of the moving object 
				if (q_mark[iNodeIndex])
					continue;
				// checking if that node is in the path of the BESTNODE ones
				iNodeIndex = this->UnpackLink(taLinks[i]);
				if (tpaIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					bool bOk = true;
					tpTemp = taHeap + tpaIndexes[iNodeIndex].tpNode;
					if (!(tpTemp->ucOpenCloseMask)) {
						int iBestIndex = tpBestNode->iIndex;
						tpTemp2 = taHeap + tpTemp->tpForward;
						while (tpTemp2) {
							if (tpTemp2->iIndex == iBestIndex) {
								bOk = false;
								break;
							}
							tpTemp2 = taHeap + tpTemp2->tpForward;
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
							tpTemp->tpBack = tpBestNode - taHeap;
							if (taHeap[tpTemp->tpOpenedPrev].f > tpTemp->f) {
								taHeap[tpTemp->tpOpenedPrev].tpOpenedNext = tpTemp->tpOpenedNext;
								if (tpTemp->tpOpenedNext)
									taHeap[tpTemp->tpOpenedNext].tpOpenedPrev = tpTemp->tpOpenedPrev;
								float dTemp = tpTemp->f;
								tpTemp1 = tpTemp;
								for (tpTemp = taHeap + tpTemp->tpOpenedPrev; tpTemp; tpTemp = taHeap + tpTemp->tpOpenedPrev)
									if (tpTemp->f <= dTemp) {
										tpTemp1->tpOpenedNext = tpTemp->tpOpenedNext;
										tpTemp1->tpOpenedPrev = tpTemp - taHeap;
										if (tpTemp->tpOpenedNext)
											taHeap[tpTemp->tpOpenedNext].tpOpenedPrev = tpTemp1 - taHeap;
										tpTemp->tpOpenedNext = tpTemp1 - taHeap;
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
							tpTemp->tpBack = tpBestNode - taHeap;
						}
						continue;
					}
				}
				else {
					tpTemp2 = taHeap + uiHeap++;
					tpTemp2->tpNext = tpTemp2->tpForward = tpTemp2->tpOpenedNext = tpTemp2->tpOpenedPrev = 0;
					tpaIndexes[iNodeIndex].tpNode = uiHeap - 1;
					tpaIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode - taHeap;
					tpTemp2->g = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex));

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = ffCriteria(mNodeStructure(dwGoalNode),mNodeStructure(iNodeIndex));
					ASSIGN_GOODNESS(tpTemp2)
					
					tpTemp  = tpOpenedList;
					tpTemp1 = taHeap + tpOpenedList->tpOpenedNext;
					float dTemp = tpTemp2->f;
					bool bOk = false;
					while (tpTemp1) {
						if (tpTemp1->f >= dTemp) {
							tpTemp2->tpOpenedNext = tpTemp1 - taHeap;
							tpTemp2->tpOpenedPrev = tpTemp - taHeap;
							tpTemp->tpOpenedNext = tpTemp2 - taHeap;
							tpTemp1->tpOpenedPrev = tpTemp2 - taHeap;
							bOk = true;
							break;
						}
						tpTemp  = tpTemp1;
						tpTemp1 = taHeap + tpTemp1->tpOpenedNext;
					}
					if (!bOk) {
						tpTemp->tpOpenedNext = tpTemp2 - taHeap;
						tpTemp2->tpOpenedPrev = tpTemp - taHeap;
					}
					tpTemp2->ucOpenCloseMask = OPEN_MASK;
					
					// make it a BESTNODE successor
					tpTemp1 = taHeap + tpBestNode->tpForward;
					tpBestNode->tpForward = tpTemp2 - taHeap;
					tpTemp2->tpNext = tpTemp1 - taHeap;
				}
			}
		}
		uiNodeCount++;
		if (uiHeap > MAX_NODES)
			break;
	}
	
	Result.Nodes.clear();
	Device.Statistic.AI_Path.End();
	return(MAX_VALUE);
}