///////////////////////////////////////////////////////////////
//	Module 		: xr_a_star.cpp
//	Created 	: 21.03.2002
//  Modified 	: 22.03.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding the shortest path between two nodes
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>

#include "xr_a_star.h"
#include "xr_emu.h"
#include "xr_misc.h"

#define ASSIGN_GOODNESS(t) (t)->f = (t)->g + (t)->h;

void vfFreeOpenedList(TNode *tpList)
{
	while (tpList) {
		TNode *tpTemp = tpList->tpOpenedNext;
		free(tpList);
		tpList = tpTemp;
	}
}

void vfFreeClosedList(TNode *tpList)
{
	while (tpList) {
		TNode *tpTemp = tpList->tpClosedNext;
		free(tpList);
		tpList = tpTemp;
	}
}

void vfUpdateSuccessors(TNode *tpList, double dDifference)
{
	TNode *tpTemp = tpList->tpForward;
	while (tpTemp) {
		if (tpTemp->tpForward)
			vfUpdateSuccessors(tpTemp->tpForward,dDifference);
		tpTemp->g -= dDifference;
		ASSIGN_GOODNESS(tpTemp)
		tpTemp = tpTemp->tpNext;
	}
}

TNode *tpfFindTheShortestPath(int iStartNode, int iFinishNode)
{
	uint uiStartTime = clock();
	TNode *tpOpenedList = (TNode *)calloc(sizeof(TNode),1);
	TNode *tpClosedList = (TNode *)calloc(sizeof(TNode),1);
	TNode *tpTemp = (TNode *)calloc(sizeof(TNode),1);

	tpOpenedList->tpOpenedNext = tpTemp;
	tpTemp->iIndex = iStartNode;
	tpTemp->g = 0.0;
	tpTemp->h = dfGetDistance(*tpaNodes[iStartNode],*tpaNodes[iFinishNode]);
	ASSIGN_GOODNESS(tpTemp)
	
	uint uiNodeCount = 0;
	while (tpOpenedList->tpOpenedNext) {
		
		// finding the node being estimated as the cheapest among the opened ones
		TNode *tpTemp1 = tpOpenedList->tpOpenedNext, *tpBestNode;
		double dMin = DOUBLE_INFINITY;
		
		while (tpTemp1) {
			if (tpTemp1->f < dMin) {
				tpBestNode = tpTemp1;
				dMin = tpTemp1->f;
			}
			tpTemp1 = tpTemp1->tpOpenedNext;
		}

		if (uiNodeCount % 1000 == 0) {
			printf("%6d %7.2f\n",uiNodeCount,tpBestNode->f);
		}
		
		// remove that node from the opened list
		TNode *tpPrev = tpOpenedList;
		TNode *tpTemp2 = tpPrev->tpOpenedNext;
		
		while (tpTemp2) {
			if (tpTemp2->iIndex == tpBestNode->iIndex) {
				tpPrev->tpOpenedNext = tpTemp2->tpOpenedNext;
				break;
			}
			tpPrev = tpTemp2;
			tpTemp2 = tpTemp2->tpOpenedNext;
		}

		// place that node to the closed list
		tpTemp2 = tpClosedList->tpClosedNext;
		tpClosedList->tpClosedNext = tpBestNode;
		tpBestNode->tpClosedNext = tpTemp2;

		// check if that node is our goal
		if (tpBestNode->iIndex == iFinishNode) {
			/**
			for (int i=0; tpTemp1; i++, tpTemp1 = tpTemp1->tpBack)
				printf("%4d %7d\n",i,tpTemp1->iIndex);
			/**/
			
			tpTemp2 = (TNode *)calloc(sizeof(TNode),1);
			tpTemp2->iIndex = tpBestNode->iIndex;

			tpTemp = tpBestNode;
			for ( ; tpTemp; tpTemp = tpTemp->tpBack) {
				tpTemp1 = tpTemp2;
				tpTemp2 = (TNode *)calloc(sizeof(TNode),1);
				tpTemp2->tpNext = tpTemp1;
				tpTemp2->iIndex = tpTemp->iIndex;
			}

			tpTemp2->f = tpBestNode->f;
			tpTemp2->g = tpBestNode->g;
			tpTemp2->h = tpBestNode->h;
			vfFreeOpenedList(tpOpenedList);
			vfFreeClosedList(tpClosedList);

			double dTime = (double)(clock() - uiStartTime)/CLOCKS_PER_SEC;
			if (dTime > 0.0)
				printf("\nPath was found for %7.2fs, speed is %10.2f\n\n",dTime, uiNodeCount/dTime);
			else
				printf("\nPath was found immediately\n\n");
			return(tpTemp2);
		}
		
		NodeLink *taLinks = (NodeLink *)((uchar *)(tpaNodes[tpBestNode->iIndex]) + sizeof(NodeCompressed));
		for (int i=0, iCount = tpaNodes[tpBestNode->iIndex]->link_count, iNodeIndex = ifConvertToDWord(taLinks[i]); i<iCount; i++, iNodeIndex = ifConvertToDWord(taLinks[i])) {
			
			// checking if that node is in the path of the BESTNODE ones
			bool bOk = true;
			tpTemp2 = tpBestNode->tpBack;
			while (tpTemp2) {
				if (tpTemp2->iIndex == iNodeIndex) {
					bOk = false;
					break;
				}
				tpTemp2 = tpTemp2->tpBack;
			}
			if (!bOk)
				continue;
			// initialize node
			tpTemp2 = (TNode *)calloc(sizeof(TNode),1);
			tpTemp2->iIndex = iNodeIndex;
			tpTemp2->tpBack = tpBestNode;
			tpTemp2->g = tpBestNode->g + dfCriteria(*tpaNodes[tpBestNode->iIndex],*tpaNodes[tpTemp2->iIndex]);//dfGetDistance(*tpaNodes[tpBestNode->iIndex],*tpaNodes[tpTemp2->iIndex]);//
			
			// check if this node is already in the opened list
			bool bAlreadyViewed = false;
			tpTemp1 = tpOpenedList->tpOpenedNext;
			while (tpTemp1) {
				if (tpTemp1->iIndex == tpTemp2->iIndex) {
					bAlreadyViewed = true;
					if (tpTemp1->g > tpTemp2->g) {
						tpTemp1->g = tpTemp2->g;
						tpTemp1->tpBack = tpBestNode;
					}
					break;
				}
				tpTemp1 = tpTemp1->tpOpenedNext;
			}

			if (!bAlreadyViewed) {
				// check if this node is already in the closed list
				tpTemp1 = tpClosedList->tpClosedNext;
				while (tpTemp1) {
					if (tpTemp1->iIndex == tpTemp2->iIndex) {
						bAlreadyViewed = true;
						if (tpTemp1->g > tpTemp2->g) {
							double dDifference = tpTemp1->g - tpTemp2->g;
							tpTemp1->g = tpTemp2->g;
							tpTemp1->tpBack = tpBestNode;
							vfUpdateSuccessors(tpTemp1,dDifference);
						}
						break;
					}
					tpTemp1 = tpTemp1->tpClosedNext;
				}
			}

			// put that node to the opened list if wasn't found there and in the closed one
			if (!bAlreadyViewed) {
				tpTemp2->h = dfGetDistance(*tpaNodes[iFinishNode],*tpaNodes[tpTemp2->iIndex]);
				ASSIGN_GOODNESS(tpTemp2)
				tpTemp2->tpOpenedNext = tpOpenedList->tpOpenedNext;
				tpOpenedList->tpOpenedNext = tpTemp2;
				
				// make it a BESTNODE successor
				tpTemp1 = tpBestNode->tpForward;
				tpBestNode->tpForward = tpTemp2;
				tpTemp2->tpNext = tpTemp1;
			}
			else {
				free(tpTemp2);
			}
		}
		uiNodeCount++;
	}
	
	vfFreeOpenedList(tpOpenedList);
	vfFreeClosedList(tpClosedList);

	return(NULL);
}