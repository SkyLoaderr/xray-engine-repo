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

/**
#pragma pack(push,4)
typedef struct tagSNode {
	int			ucOpenCloseMask:8;
	int			iIndex:24;
	float		f;
	float		g;
	float		h;
	tagSNode	*tpNext;
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
	IC void vfUpdateSuccessors(SNode *tpList, float dDifference)
	{
		SNode *tpTemp = tpList->tpForward;
		while (tpTemp) {
			if (tpTemp->tpForward)
				vfUpdateSuccessors(tpTemp->tpForward,dDifference);
			tpTemp->g -= dDifference;
			tpTemp->f = tpTemp->g + tpTemp->h;

			if (tpTemp->ucOpenCloseMask)
				if (tpTemp->tpOpenedPrev->f > tpTemp->f) {
					tpTemp->tpOpenedPrev->tpOpenedNext = tpTemp->tpOpenedNext;
					if (tpTemp->tpOpenedNext)
						tpTemp->tpOpenedNext->tpOpenedPrev = tpTemp->tpOpenedPrev;
					float dTemp = tpTemp->f;
					SNode *tpTemp1 = tpTemp;
					for (tpTemp = tpTemp->tpOpenedPrev; ; tpTemp = tpTemp->tpOpenedPrev)
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
public:
	void vfFindOptimalPath(
			SNode		*tpHeap,
			SIndexNode	*tpIndexes,
			u32			&dwAStarStaticCounter,
			SData		&tData,
			u32			dwStartNode, 
			u32			dwGoalNode, 
			float		&fDistance, 
			float		fMaxDistance, 
			Fvector		tStartPosition, 
			Fvector		tFinishPosition, 
			vector<u32> &tpaNodes)
	{
		// initialization
		dwAStarStaticCounter++;

		u32					dwHeap = 0;
		CTemplateNode		tTemplateNode(tData);

		SNode  *tpOpenedList = tpHeap + dwHeap++,
			*tpTemp       = tpIndexes[dwStartNode].tpNode = tpHeap + dwHeap++,
			*tpTemp1,
			*tpTemp2,
			*tpBestNode;
		
		memset(tpOpenedList,0,sizeof(SNode));
		memset(tpTemp,0,sizeof(SNode));
		
		tpIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

		tpOpenedList->tpOpenedNext = tpTemp;
		tpTemp->iIndex = dwStartNode;
		tpTemp->g = 0.0;
		tpTemp->h = tTemplateNode.ffAnticipate(dwStartNode);
		tpTemp->tpOpenedPrev = tpOpenedList;
		tpTemp->ucOpenCloseMask = 1;
		tpTemp->f = tpTemp->g + tpTemp->h;
		
		while (tpOpenedList->tpOpenedNext) {
			
			// finding the node being estimated as the cheapest among the opened ones
			tpBestNode = tpOpenedList->tpOpenedNext;

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
			int iBestIndex = tpBestNode->iIndex;
			if (iBestIndex == (int)dwGoalNode) {
				fDistance = tpBestNode->g;
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
					if (fCumulativeDistance + fLastDirectDistance >= fMaxDistance) {
						fDistance = MAX_VALUE;
						return;
					}
				}
				fDirectDistance = ffCheckPositionInDirection(dwNode,tPosition,tFinishPosition,fMaxDistance);
				if (fDirectDistance == MAX_VALUE)
					fDistance = fCumulativeDistance + fLastDirectDistance + tFinishPosition.distance_to(tfGetNodeCenter(tpaNodes[tpaNodes.size() - 1]));
				else
					fDistance = fCumulativeDistance + fDirectDistance;
				return;
			}
			
			CTemplateNode::iterator tIterator;
			CTemplateNode::iterator tEnd;
			tTemplateNode.begin(iBestIndex,tIterator,tEnd);
			for (  ; tIterator != tEnd; tIterator++) {
				// checking if that node is in the path of the BESTNODE ones
				int iNodeIndex = tTemplateNode.get_value(tIterator);
				// checking if that node is in the path of the BESTNODE ones
				if (tpIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					tpTemp = tpIndexes[iNodeIndex].tpNode;
					
					// initialize node
					float dG = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex);
					
					// check if this node is already in the opened list
					if (tpTemp->ucOpenCloseMask) {
						if (tpTemp->g > dG) {
							tpTemp->g = dG;
							tpTemp->f = tpTemp->g + tpTemp->h;
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
					else {
						if (tpTemp->g > dG) {
							vfUpdateSuccessors(tpTemp,tpTemp->g - dG);
							tpTemp->g = dG;
							tpTemp->f = tpTemp->g + tpTemp->h;
							tpTemp->tpBack = tpBestNode;
						}
						continue;
					}
				}
				else {
					tpTemp2 = tpIndexes[iNodeIndex].tpNode = tpHeap + dwHeap++;
					tpTemp2->tpNext = tpTemp2->tpForward = tpTemp2->tpOpenedNext = tpTemp2->tpOpenedPrev = 0;
					tpIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex);

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = tTemplateNode.ffAnticipate();
					tpTemp2->f = tpTemp2->g + tpTemp2->h;
					
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
					tpTemp2->ucOpenCloseMask = 1;
					
					// make it a BESTNODE successor
					tpTemp1 = tpBestNode->tpForward;
					tpBestNode->tpForward = tpTemp2;
					tpTemp2->tpNext = tpTemp1;
				}
			}
			if (dwHeap > m_header.count)
				break;
		}
		
		fDistance = MAX_VALUE;
	}
};
/**/
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
		
		while (tpOpenedList->tpOpenedNext != tpOpenedEnd) {
			
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
			if (dwHeap > m_header.count)
				break;
		}
		fValue = MAX_VALUE;
	}
};
/**
#pragma pack(push,4)
typedef struct tagSNode {
	int			ucOpenCloseMask:8;
	int			iIndex:24;
	float		f;
	float		g;
	float		h;
	tagSNode	*tpForward;
	tagSNode	*tpBack;
} SNode;

typedef struct tagSIndexNode {
	SNode		*tpNode;
	u32			dwTime;
} SIndexNode;
#pragma pack(pop)

class CComparePredicate {
public:
	__forceinline bool operator()(const SNode *tpNode1, const SNode *tpNode2) const
	{
		return(tpNode1->f > tpNode2->f);
	};
};

template<class CTemplateNode, class SData> class CAStarSearch {
private:
	u32						m_dwMaxNodeCount;
	u32						m_dwHeapSize;
	SNode					**m_tppHeap;
public:
	CAStarSearch(u32 dwMaxNodeCount)
	{
		m_dwMaxNodeCount	= dwMaxNodeCount;
		m_tppHeap			= (SNode **)xr_malloc((dwMaxNodeCount + 1)*sizeof(SNode *));
	}
	
	~CAStarSearch()
	{
		_FREE(m_tppHeap);
	}

	void vfFindOptimalPath(
			SNode			*tpHeap,
			SIndexNode		*tpIndexes,
			u32				&dwAStarStaticCounter,
			SData			&tData,
			u32				dwStartNode, 
			u32				dwGoalNode, 
			float			fMaxValue, 
			float			&fValue, 
			vector<u32>		&tpaNodes,
			bool			bUseMarks)
	{
		Device.Statistic.AI_Path.Begin();
		
		// initialization
		dwAStarStaticCounter++;

		u32					dwHeap = 0;
		CTemplateNode		tTemplateNode(tData);

		SNode				*tpTemp = tpIndexes[dwStartNode].tpNode = tpHeap + dwHeap++,
							*tpTemp1,
							*tpTemp2,
							*tpBestNode;
		
		memset(tpTemp,0,sizeof(SNode));
		
		tpIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

		tpTemp->iIndex = dwStartNode;
		tpTemp->g = 0.0;
		tpTemp->h = tTemplateNode.ffAnticipate(dwStartNode);
		tpTemp->ucOpenCloseMask = 1;
		tpTemp->f = tpTemp->g + tpTemp->h;
		m_tppHeap[m_dwHeapSize = 1] = tpTemp;
		
		//!!!
		while (m_dwHeapSize) {
			
			// finding the node being estimated as the cheapest among the opened ones
			//!!!
			tpBestNode = m_tppHeap[1];
			
			// checking if the distance is not too large
			if (tpBestNode->f >= fMaxValue) {
				fValue = fMaxValue;
				tpaNodes.clear();
				return;
			}

			// check if that node is our goal
			int iBestIndex = tpBestNode->iIndex;
			if (iBestIndex == (int)dwGoalNode) {

				fValue = tpBestNode->f;
				tpTemp1 = tpBestNode;
				tpTemp = tpTemp1->tpBack;
				for (u32 i=1; tpTemp; tpTemp = tpTemp->tpBack, i++) 
					tpTemp1 = tpTemp;

				tpaNodes.resize(i);

				tpTemp1 = tpBestNode;
				tpaNodes[--i] = tpBestNode->iIndex;
				tpTemp = tpTemp1->tpBack;
				for (u32 j=1; tpTemp; tpTemp = tpTemp->tpBack, j++)
					tpaNodes[i - j] = tpTemp->iIndex;
					
				Device.Statistic.AI_Path.End();
				return;
			}
			
			// remove that node from the opened list and put that node to the closed list
			tpBestNode->ucOpenCloseMask = 0;
			pop_heap(m_tppHeap + 1,m_tppHeap + m_dwHeapSize-- + 1,CComparePredicate());

			// iterating on children/neighbours
			CTemplateNode::iterator tIterator;
			CTemplateNode::iterator tEnd;
			tTemplateNode.begin(iBestIndex,tIterator,tEnd);
			for (  ; tIterator != tEnd; tIterator++) {
				// checking if that node is in the path of the BESTNODE ones
				int iNodeIndex = tTemplateNode.get_value(tIterator);
				// checking if that node the node of the moving object 
				if ((bUseMarks) && (!tTemplateNode.bfCheckIfAccessible(iNodeIndex)))
					continue;
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
							for (SNode **tpIndex = m_tppHeap + 1; *tpIndex != tpTemp; tpIndex++);
							push_heap(m_tppHeap + 1,tpIndex + 1,CComparePredicate());
						}
					}
					continue;
				}
				else {
					tpTemp2 = tpIndexes[iNodeIndex].tpNode = tpHeap + dwHeap++;
					tpTemp2->tpForward = 0;
					tpIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex,tIterator);

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = tTemplateNode.ffAnticipate();
					tpTemp2->f = tpTemp2->g + tpTemp2->h;
					
					tpTemp2->ucOpenCloseMask = 1;
					
					// make it a BESTNODE successor
					tpBestNode->tpForward = tpTemp2;
					
					m_tppHeap[++m_dwHeapSize] = tpTemp2;
					push_heap(m_tppHeap + 1,m_tppHeap + m_dwHeapSize + 1,CComparePredicate());
				}
			}
			if (dwHeap > m_dwMaxNodeCount)
				break;
		}
		
		tpaNodes.clear();
		fValue = fMaxValue;
		Device.Statistic.AI_Path.End();
	}
};
/**/
