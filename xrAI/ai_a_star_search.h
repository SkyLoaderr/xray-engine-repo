////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star_search.h
//	Created 	: 21.03.2002
//  Modified 	: 16.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding optimal path from source node to destnation one
////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_VALUE				1000000.0

typedef struct tagSAIMapData {
	u32			dwFinishNode;
} SAIMapData;

#pragma pack(push,4)
typedef struct tagSNode {
	int			ucOpenCloseMask:8;
	int			iIndex:24;
	float		f;
	float		g;
	float		h;
	tagSNode	*tpForward;
	tagSNode	*tpBack;
	tagSNode	*tpOpenedNext;
	tagSNode	*tpOpenedPrev;
} SNode;

typedef struct tagSIndexNode {
	SNode		*tpNode;
	u32			dwTime;
} SIndexNode;
#pragma pack(pop)

template<class CTemplateNode, class SData> class CAStarSearch {
private:
public:
	void vfFindOptimalPath(
			SNode		*tpHeap,
			SIndexNode	*tpIndexes,
			u32			&dwAStarStaticCounter,
			SData		&tData,
			u32			dwStartNode, 
			u32			dwGoalNode, 
			float		&fValue, 
			float		fMaxValue, 
			Fvector		tStartPosition, 
			Fvector		tFinishPosition, 
			vector<u32> &tpaNodes)
	{
		// initialization
		dwAStarStaticCounter++;
		u32 dwMaxCount		= m_header.count + 2;

		u32					dwHeap = 0;
		CTemplateNode		tTemplateNode(tData);

		SNode	*tpOpenedList = tpHeap + dwHeap++,
				*tpOpenedEnd = tpHeap + dwHeap++,
				*tpTemp       = tpIndexes[dwStartNode].tpNode = tpHeap + dwHeap++,
				*tpTemp1,
				*tpTemp2,
				*tpBestNode;
		
		memset(tpOpenedList,0,3*sizeof(SNode));
		tpOpenedEnd->f = MAX_VALUE;
		
		tpIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

		tpTemp->iIndex = dwStartNode;
		tpTemp->g = 0.0;
		tpTemp->h = tTemplateNode.ffAnticipate(dwStartNode);
		tpTemp->tpOpenedPrev = tpOpenedList;
		tpTemp->tpOpenedNext = tpOpenedEnd;
		tpTemp->ucOpenCloseMask = 1;
		tpTemp->f = tpTemp->g + tpTemp->h;
		
		tpOpenedList->tpOpenedNext = tpTemp;

		tpOpenedEnd->tpOpenedPrev = tpTemp;
		
		while (true) {
			
			// finding the node being estimated as the cheapest among the opened ones
			tpBestNode = tpOpenedList->tpOpenedNext;
			
			// checking if the distance is not too large
			if (tpBestNode->f >= fMaxValue) {
				fValue = MAX_VALUE;
				return;
			}

			// check if that node is our goal
			int iBestIndex = tpBestNode->iIndex;
			if (iBestIndex == (int)dwGoalNode) {
				fValue = tpBestNode->g;

				tpTemp1 = tpBestNode;
				tpTemp = tpTemp1->tpBack;

				for (u32 i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++) ;

				tpaNodes.resize(i);

				tpTemp1 = tpBestNode;
				tpaNodes[--i] = tpBestNode->iIndex;
				tpTemp = tpTemp1->tpBack;
				for (u32 j=1; tpTemp; tpTemp = tpTemp->tpBack, j++)
					tpaNodes[i - j] = tpTemp->iIndex;

				float fCumulativeDistance = 0, fLastDirectDistance = 0, fDirectDistance;
				Fvector tPosition = tStartPosition;
				u32 dwNode = tpaNodes[0];
				for (i=1; i<(int)tpaNodes.size(); i++) {
					fDirectDistance = ffCheckPositionInDirection(dwNode,tPosition,tfGetNodeCenter(tpaNodes[i]),fMaxValue);
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
					if (fCumulativeDistance + fLastDirectDistance >= fMaxValue) {
						fValue = MAX_VALUE;
						return;
					}
				}
				fDirectDistance = ffCheckPositionInDirection(dwNode,tPosition,tFinishPosition,fMaxValue);
				if (fDirectDistance == MAX_VALUE)
					fValue = fCumulativeDistance + fLastDirectDistance + tFinishPosition.distance_to(tfGetNodeCenter(tpaNodes[tpaNodes.size() - 1]));
				else
					fValue = fCumulativeDistance + fDirectDistance;
				return;
			}
			
			// remove that node from the opened list and put that node to the closed list
			tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
			tpBestNode->tpOpenedNext->tpOpenedPrev = tpOpenedList;
			tpBestNode->ucOpenCloseMask = 0;

			// iterating on children/neighbours
			CTemplateNode::iterator tIterator;
			CTemplateNode::iterator tEnd;
			tTemplateNode.begin(iBestIndex,tIterator,tEnd);
			for (  ; tIterator != tEnd; tIterator++) {
				// checking if that node is in the path of the BESTNODE ones
				int iNodeIndex = tTemplateNode.get_value(tIterator);
				// checking if that node is in the path of the BESTNODE ones
				if (tpIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					// check if this node is already in the opened list
					tpTemp = tpIndexes[iNodeIndex].tpNode;
					if (tpTemp->ucOpenCloseMask) {
						// initialize node
						float dG = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex,tIterator);
						if (tpTemp->g > dG) {
							tpTemp->g = dG;
							tpTemp->f = tpTemp->g + tpTemp->h;
							tpTemp->tpBack = tpBestNode;
							if (tpTemp->tpOpenedPrev->f > tpTemp->f) {
								// decreasing value
								tpTemp->tpOpenedPrev->tpOpenedNext = tpTemp->tpOpenedNext;
								tpTemp->tpOpenedNext->tpOpenedPrev = tpTemp->tpOpenedPrev;
								float fTemp = tpTemp->f;
								if (fTemp <= tpOpenedList->tpOpenedNext->f) {
									tpTemp->tpOpenedPrev = tpOpenedList;
									tpTemp->tpOpenedNext = tpOpenedList->tpOpenedNext;
									tpOpenedList->tpOpenedNext->tpOpenedPrev = tpTemp;
									tpOpenedList->tpOpenedNext = tpTemp;
									continue;
								}
								else {
									tpTemp1 = tpTemp;
									if (tpTemp->tpOpenedPrev->f - tpTemp->f < tpTemp->f - tpOpenedList->tpOpenedNext->f)
										for (tpTemp = tpTemp->tpOpenedPrev->tpOpenedPrev; ; tpTemp = tpTemp->tpOpenedPrev) {
											if (tpTemp->f <= fTemp) {
												tpTemp1->tpOpenedNext = tpTemp->tpOpenedNext;
												tpTemp1->tpOpenedPrev = tpTemp;
												tpTemp->tpOpenedNext->tpOpenedPrev = tpTemp1;
												tpTemp->tpOpenedNext = tpTemp1;
												break;
											}
										}
									else
										for (tpTemp  = tpOpenedList->tpOpenedNext; ; tpTemp  = tpTemp->tpOpenedNext)
											if (tpTemp->f >= fTemp) {
												tpTemp1->tpOpenedNext = tpTemp;
												tpTemp1->tpOpenedPrev = tpTemp->tpOpenedPrev;
												tpTemp->tpOpenedPrev->tpOpenedNext = tpTemp1;
												tpTemp->tpOpenedPrev = tpTemp1;
												break;
											}
								}
							}
						}
					}
					continue;
				}
				else {
					tpTemp2 = tpIndexes[iNodeIndex].tpNode = tpHeap + dwHeap++;
					tpTemp2->tpForward = tpTemp2->tpOpenedNext = tpTemp2->tpOpenedPrev = 0;
					tpIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex,tIterator);
					tpTemp2->h = tTemplateNode.ffAnticipate();
					tpTemp2->f = tpTemp2->g + tpTemp2->h;
					
					// put that node to the opened list if wasn't found there and in the closed one
					float fTemp = tpTemp2->f;
					for (tpTemp = tpOpenedList->tpOpenedNext; ; tpTemp = tpTemp->tpOpenedNext)
						if (tpTemp->f >= fTemp) {
							tpTemp2->tpOpenedNext = tpTemp;
							tpTemp2->tpOpenedPrev = tpTemp->tpOpenedPrev;
							tpTemp->tpOpenedPrev->tpOpenedNext = tpTemp2;
							tpTemp->tpOpenedPrev = tpTemp2;
							break;
						}
					tpTemp2->ucOpenCloseMask = 1;
					
					// make it a BESTNODE successor
					tpBestNode->tpForward = tpTemp2;
				}
			}
			if (dwHeap > dwMaxCount)
				break;
		}
		fValue = MAX_VALUE;
	}
};