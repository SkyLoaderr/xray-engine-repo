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
#define MAX_VALUE				100000.0
#define MAX_NODES				65535

float fSize,fYSize,fSize2,fYSize2,fCriteriaLightWeight,fCriteriaCoverWeight,fCriteriaDistanceWeight,fCriteriaEnemyViewWeight;

static u32 dwAStarStaticCounter = 0;
TNode		*taHeap;
TIndexNode	*tpaIndexes;

void vfLoadSearch()
{
	fSize2		= _sqr(fSize = m_header.size)/4;
	fYSize2		= _sqr(fYSize = (float)(m_header.size_y/32767.0))/4;
	u32 S1	= (MAX_NODES + 1)*sizeof(TNode);
	taHeap		= (TNode *)xr_malloc(S1);
	ZeroMemory(taHeap,S1);
	u32 S2	= (m_header.count)*sizeof(TIndexNode);
	tpaIndexes	= (TIndexNode *)xr_malloc(S2);
	ZeroMemory(tpaIndexes,S2);
	Msg("* AI path-finding structures: %d K",(S1 + S2)/(1024));
}

void vfUnloadSearch()
{
	_FREE(taHeap);
	_FREE(tpaIndexes);
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

float vfFindTheXestPath(u32 dwStartNode, u32 dwGoalNode, float &fDistance)
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
			for (u32  i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++) ;

			return(fDistance);
		}
		
		NodeLink *taLinks = (NodeLink *)((unsigned char *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->links, iNodeIndex;
		if (iCount) {
			iNodeIndex = UnpackLink(taLinks[0]);
			for (int i=0; i<iCount; i++) {
				// checking if that node is in the path of the BESTNODE ones
				iNodeIndex = UnpackLink(taLinks[i]);
				// checking if that node the node of the moving object 
//				if (q_mark[iNodeIndex])
//					continue;
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
					//*(u32 *)(tpTemp2) = 0;
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
		if (uiHeap > MAX_NODES)
			break;
	}
	
	return(MAX_VALUE);
}
