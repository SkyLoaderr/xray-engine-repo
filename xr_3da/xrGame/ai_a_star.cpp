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

#define ASSIGN_GOODNESS(t)	(t)->f = (t)->g + (t)->h;
#define SQR(t)				((t)*(t))
#define OPEN_MASK			1
#define mNodeStructure(x)	(*(this->Node(x)))
#define mNode(x)			(this->Node(x))
#define MAX_VALUE			100000.0
#define MAX_NODES			4096

float fSize,fYSize,fSize2,fYSize2;

TNode  *taHeap,**tpaIndexes;

__forceinline float ffCriteria(NodeCompressed tNode0, NodeCompressed tNode1)
{
	/**
	float x1 = (fSize*(float)(tNode0.p1.x) + fSize*(float)(tNode0.p0.x))/2;
	float y1 = (fYSize*(float)(tNode0.p1.y) + fYSize*(float)(tNode0.p0.y))/2;
	float z1 = (fSize*(float)(tNode0.p1.z) + fSize*(float)(tNode0.p0.z))/2;
	
	float x2 = (fSize*(float)(tNode1.p1.x) + fSize*(float)(tNode1.p0.x))/2;
	float y2 = (fYSize*(float)(tNode1.p1.y) + fYSize*(float)(tNode1.p0.y))/2;
	float z2 = (fSize*(float)(tNode1.p1.z) + fSize*(float)(tNode1.p0.z))/2;
	
	return((float)sqrt((float)(SQR(x2 - x1) + SQR(z2 - z1) + SQR(y2 - y1))));
	/**/
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	float fCover = (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f;

	float fLight = (float)(tNode1.light)/255.f;
	
	return(fLight*10 + fCover*5 + 30*(float)sqrt((float)(fSize2*(SQR(x2 - x1) + SQR(z2 - z1)) + fYSize2*SQR(y2 - y1))));
	/**/
}

__forceinline void vfUpdateSuccessors(TNode *tpList, float dDifference)
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
	fSize2 = SQR(fSize = this->m_header.size)/4;
	fYSize2 = SQR(fYSize = (float)(this->m_header.size_y/32767.0))/4;

	taHeap = (TNode *)calloc(this->m_header.count + 1,sizeof(TNode));
	tpaIndexes = (TNode **)calloc(this->m_header.count,sizeof(TNode *));
}

void CAI_Space::vfUnloadSearch()
{
	free(taHeap);
	free(tpaIndexes);
}

float CAI_Space::vfFindTheXestPath(DWORD dwStartNode, DWORD dwGoalNode, AI::Path& Result)
{
	// initialization
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
		if (tpBestNode->iIndex == dwGoalNode) {

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
				
			return(fDistance);
		}
		
		NodeLink *taLinks = (NodeLink *)((uchar *)mNode(tpBestNode->iIndex) + sizeof(NodeCompressed));
		int iCount = iCount = mNode(tpBestNode->iIndex)->link_count, iNodeIndex;
		if (iCount) {
			iNodeIndex = this->UnpackLink(taLinks[0]);
			tpTemp = tpaIndexes[iNodeIndex];
			for (int i=0; i<iCount; i++)
				// checking if that node is in the path of the BESTNODE ones
				if (tpTemp = tpaIndexes[iNodeIndex = this->UnpackLink(taLinks[i])]) {
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
		uiNodeCount++;
		if (uiNodeCount >= MAX_NODES)
			break;
	}
	
	return(MAX_VALUE);
}