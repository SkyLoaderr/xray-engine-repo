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
#define MAX_NODES				1048576

float fSize,fYSize,fSize2,fYSize2,fCriteriaLightWeight,fCriteriaCoverWeight,fCriteriaDistanceWeight,fCriteriaEnemyViewWeight;

TNode  *taHeap,**tpaIndexes;

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

void CAI_Space::vfLoadSearch()
{
	fSize2		= _sqr(fSize = this->m_header.size)/4;
	fYSize2		= _sqr(fYSize = (float)(this->m_header.size_y/32767.0))/4;

	DWORD S1	= (this->m_header.count + 1) *	sizeof(TNode);	taHeap		= (TNode *) xr_malloc(S1); ZeroMemory(taHeap,S1);
	DWORD S2	= (this->m_header.count)	 *	sizeof(TNode*);	tpaIndexes	= (TNode **)xr_malloc(S2); ZeroMemory(tpaIndexes,S2);
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
	fCriteriaLightWeight = fLightWeight;
	fCriteriaCoverWeight = fCoverWeight;
	fCriteriaDistanceWeight = fDistanceWeight;

	uint uiHeap = 0;

	memset(taHeap,0,(this->m_header.count + 1)*sizeof(TNode));
	memset(tpaIndexes,0,this->m_header.count*sizeof(TNode *));

	TNode  *tpOpenedList = taHeap + uiHeap++,
		   *tpTemp       = tpaIndexes[dwStartNode] = taHeap + uiHeap++,
		   *tpTemp1,
		   *tpTemp2,
		   *tpBestNode;

	tpOpenedList->tpOpenedNext = tpTemp;
	tpTemp->iIndex = dwStartNode;
	tpTemp->g = 0.0;
	tpTemp->h = ffCriteria(mNodeStructure(dwStartNode),mNodeStructure(dwGoalNode));
	tpTemp->tpOpenedPrev = tpOpenedList;
	tpTemp->ucOpenCloseMask = OPEN_MASK;
	ASSIGN_GOODNESS(tpTemp)
	
	uint uiNodeCount = 0;
	while (tpOpenedList->tpOpenedNext) {
		
		// finding the node being estimated as the cheapest among the opened ones
		tpBestNode = tpOpenedList->tpOpenedNext;
		
		// remove that node from the opened list and put that node to the closed list
		tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
		if (tpBestNode->tpOpenedNext)
			tpBestNode->tpOpenedNext->tpOpenedPrev = tpOpenedList;
		tpBestNode->ucOpenCloseMask = 0;

		// check if that node is our goal
		if (tpBestNode->iIndex == (int)dwGoalNode) {

			float fDistance = 0.0;
			tpTemp1 = tpBestNode;
			tpTemp = tpTemp1->tpBack;
			for (uint i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++)
				fDistance += ffCriteria(mNodeStructure(tpTemp1->iIndex),mNodeStructure(tpTemp->iIndex));

			Result.Nodes.resize(i);

			tpTemp1 = tpBestNode;
			Result.Nodes[--i] = tpBestNode->iIndex;
			tpTemp = tpTemp1->tpBack;
			for (uint j=1; tpTemp; tpTemp = tpTemp->tpBack, j++)
				Result.Nodes[i - j] = tpTemp->iIndex;
				
			Device.Statistic.AI_Path.End();
			return(fDistance);
		}
		
		NodeLink *taLinks = (NodeLink *)((uchar *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->links, iNodeIndex;
		if (iCount) {
			iNodeIndex = this->UnpackLink(taLinks[0]);
			tpTemp = tpaIndexes[iNodeIndex];
			for (int i=0; i<iCount; i++) {
				// checking if that node is in the path of the BESTNODE ones
				iNodeIndex = this->UnpackLink(taLinks[i]);
				// checking if that node the node of the moving object 
				if (q_mark[iNodeIndex])
					continue;
				// checking if that node is in the path of the BESTNODE ones
				if (tpTemp = tpaIndexes[iNodeIndex]) {
					bool bOk = true;
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
					tpTemp2 = tpaIndexes[iNodeIndex] = taHeap + uiHeap++;
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
		uiNodeCount++;
		if (uiNodeCount >= MAX_NODES)
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
	fCriteriaLightWeight = fLightWeight;
	fCriteriaCoverWeight = fCoverWeight;
	fCriteriaDistanceWeight = fDistanceWeight;
	fCriteriaEnemyViewWeight = fEnemyViewWeight;

	uint uiHeap = 0;

	memset(taHeap,0,(this->m_header.count + 1)*sizeof(TNode));
	memset(tpaIndexes,0,this->m_header.count*sizeof(TNode *));

	TNode  *tpOpenedList = taHeap + uiHeap++,
		   *tpTemp       = tpaIndexes[dwStartNode] = taHeap + uiHeap++,
		   *tpTemp1,
		   *tpTemp2,
		   *tpBestNode;

	tpOpenedList->tpOpenedNext = tpTemp;
	tpTemp->iIndex = dwStartNode;
	tpTemp->g = 0.0;
	tpTemp->h = ffCriteria(mNodeStructure(dwStartNode),mNodeStructure(dwGoalNode), tEnemyNode, fOptimalEnemyDistance);
	tpTemp->tpOpenedPrev = tpOpenedList;
	tpTemp->ucOpenCloseMask = OPEN_MASK;
	ASSIGN_GOODNESS(tpTemp)
	
	uint uiNodeCount = 0;
	while (tpOpenedList->tpOpenedNext) {
		
		// finding the node being estimated as the cheapest among the opened ones
		tpBestNode = tpOpenedList->tpOpenedNext;
		
		// remove that node from the opened list and put that node to the closed list
		tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
		if (tpBestNode->tpOpenedNext)
			tpBestNode->tpOpenedNext->tpOpenedPrev = tpOpenedList;
		tpBestNode->ucOpenCloseMask = 0;

		// check if that node is our goal
		if (tpBestNode->iIndex == (int)dwGoalNode) {

			float fDistance = 0.0;
			tpTemp1 = tpBestNode;
			tpTemp = tpTemp1->tpBack;
			for (uint i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++)
				fDistance += ffCriteria(mNodeStructure(tpTemp1->iIndex),mNodeStructure(tpTemp->iIndex), tEnemyNode, fOptimalEnemyDistance);

			Result.Nodes.resize(i);

			tpTemp1 = tpBestNode;
			Result.Nodes[--i] = tpBestNode->iIndex;
			tpTemp = tpTemp1->tpBack;
			for (uint j=1; tpTemp; tpTemp = tpTemp->tpBack, j++)
				Result.Nodes[i - j] = tpTemp->iIndex;
				
			Device.Statistic.AI_Path.End();
			return(fDistance);
		}
		
		NodeLink *taLinks = (NodeLink *)((uchar *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->links, iNodeIndex;
		if (iCount) {
			iNodeIndex = this->UnpackLink(taLinks[0]);
			tpTemp = tpaIndexes[iNodeIndex];
			for (int i=0; i<iCount; i++) {
				iNodeIndex = this->UnpackLink(taLinks[i]);
				// checking if that node the node of the moving object 
				if (q_mark[iNodeIndex])
					continue;
				// checking if that node is in the path of the BESTNODE ones
				if (tpTemp = tpaIndexes[iNodeIndex]) {
					bool bOk = true;
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
					float dG = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex), tEnemyNode, fOptimalEnemyDistance);
					
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
					tpTemp2 = tpaIndexes[iNodeIndex] = taHeap + uiHeap++;
					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex), tEnemyNode, fOptimalEnemyDistance);

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = ffCriteria(mNodeStructure(dwGoalNode),mNodeStructure(iNodeIndex), tEnemyNode, fOptimalEnemyDistance);
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
		uiNodeCount++;
		if (uiNodeCount >= MAX_NODES)
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
	fCriteriaLightWeight = fLightWeight;
	fCriteriaCoverWeight = fCoverWeight;
	fCriteriaDistanceWeight = fDistanceWeight;
	fCriteriaEnemyViewWeight = fEnemyViewWeight;

	uint uiHeap = 0;

	memset(taHeap,0,(this->m_header.count + 1)*sizeof(TNode));
	memset(tpaIndexes,0,this->m_header.count*sizeof(TNode *));

	TNode  *tpOpenedList = taHeap + uiHeap++,
		   *tpTemp       = tpaIndexes[dwStartNode] = taHeap + uiHeap++,
		   *tpTemp1,
		   *tpTemp2,
		   *tpBestNode;

	tpOpenedList->tpOpenedNext = tpTemp;
	tpTemp->iIndex = dwStartNode;
	tpTemp->g = 0.0;
	tpTemp->h = ffCriteria(mNodeStructure(dwStartNode),mNodeStructure(dwGoalNode), tEnemyPosition, fOptimalEnemyDistance);
	tpTemp->tpOpenedPrev = tpOpenedList;
	tpTemp->ucOpenCloseMask = OPEN_MASK;
	ASSIGN_GOODNESS(tpTemp)
	
	uint uiNodeCount = 0;
	while (tpOpenedList->tpOpenedNext) {
		
		// finding the node being estimated as the cheapest among the opened ones
		tpBestNode = tpOpenedList->tpOpenedNext;
		
		// remove that node from the opened list and put that node to the closed list
		tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
		if (tpBestNode->tpOpenedNext)
			tpBestNode->tpOpenedNext->tpOpenedPrev = tpOpenedList;
		tpBestNode->ucOpenCloseMask = 0;

		// check if that node is our goal
		if (tpBestNode->iIndex == (int)dwGoalNode) {

			float fDistance = 0.0;
			tpTemp1 = tpBestNode;
			tpTemp = tpTemp1->tpBack;
			for (uint i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++)
				fDistance += ffCriteria(mNodeStructure(tpTemp1->iIndex),mNodeStructure(tpTemp->iIndex), tEnemyPosition, fOptimalEnemyDistance);

			Result.Nodes.resize(i);

			tpTemp1 = tpBestNode;
			Result.Nodes[--i] = tpBestNode->iIndex;
			tpTemp = tpTemp1->tpBack;
			for (uint j=1; tpTemp; tpTemp = tpTemp->tpBack, j++)
				Result.Nodes[i - j] = tpTemp->iIndex;
				
			Device.Statistic.AI_Path.End();
			return(fDistance);
		}
		
		NodeLink *taLinks = (NodeLink *)((uchar *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->links, iNodeIndex;
		if (iCount) {
			iNodeIndex = this->UnpackLink(taLinks[0]);
			tpTemp = tpaIndexes[iNodeIndex];
			for (int i=0; i<iCount; i++) {
				iNodeIndex = this->UnpackLink(taLinks[i]);
				// checking if that node the node of the moving object 
				if (q_mark[iNodeIndex])
					continue;
				// checking if that node is in the path of the BESTNODE ones
				if (tpTemp = tpaIndexes[iNodeIndex]) {
					bool bOk = true;
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
					float dG = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex), tEnemyPosition, fOptimalEnemyDistance);
					
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
					tpTemp2 = tpaIndexes[iNodeIndex] = taHeap + uiHeap++;
					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + ffCriteria(mNodeStructure(tpBestNode->iIndex),mNodeStructure(iNodeIndex), tEnemyPosition, fOptimalEnemyDistance);

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = ffCriteria(mNodeStructure(dwGoalNode),mNodeStructure(iNodeIndex), tEnemyPosition, fOptimalEnemyDistance);
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
		uiNodeCount++;
		if (uiNodeCount >= MAX_NODES)
			break;
	}
	
	Result.Nodes.clear();
	Device.Statistic.AI_Path.End();
	return(MAX_VALUE);
}