////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star_template.h
//	Created 	: 21.03.2002
//  Modified 	: 26.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Template of A* algortihm for finding optimal path from 
//                source vertex to destnation one in the structure being
//                specified
////////////////////////////////////////////////////////////////////////////

#pragma once

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
	u32			m_dwMaxNodeCount;
public:
	CAStarSearch(u32 dwMaxNodeCount)
	{
		m_dwMaxNodeCount = dwMaxNodeCount;
	}
	
	void vfFindOptimalPath(
			SNode		*tpHeap,
			SIndexNode	*tpIndexes,
			u32			&dwAStarStaticCounter,
			SData		&tData,
			u32			dwStartNode, 
			u32			dwGoalNode, 
			float		fMaxValue, 
			float		&fValue, 
			vector<u32> &tpaNodes,
			bool		bUseMarks)
	{
		Device.Statistic.AI_Path.Begin();
		
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
		
		//!!!
		while (tpOpenedList->tpOpenedNext) {
			
			// finding the node being estimated as the cheapest among the opened ones
			//!!!
			tpBestNode = tpOpenedList->tpOpenedNext;
			
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
			
			//!!!
			// remove that node from the opened list and put that node to the closed list
			tpOpenedList->tpOpenedNext = tpBestNode->tpOpenedNext;
			if (tpBestNode->tpOpenedNext)
				tpBestNode->tpOpenedNext->tpOpenedPrev = tpOpenedList;
			tpBestNode->ucOpenCloseMask = 0;

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
							if (tpTemp->tpOpenedPrev->f > tpTemp->f) {
								// decreasing value
								//!!!
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
					}
					continue;
				}
				else {
					//!!!
					tpTemp2 = tpIndexes[iNodeIndex].tpNode = tpHeap + dwHeap++;
					tpTemp2->tpForward = tpTemp2->tpOpenedNext = tpTemp2->tpOpenedPrev = 0;
					tpIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex,tIterator);

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
	u32						m_dwExtractMinimum;
	u32						m_dwDecreaseValue;
	u32						m_dwInsert;
	CAStarSearch(u32 dwMaxNodeCount)
	{
		m_dwMaxNodeCount	= dwMaxNodeCount;
		m_tppHeap			= (SNode **)xr_malloc((dwMaxNodeCount + 1)*sizeof(SNode *));
		m_dwExtractMinimum  = m_dwDecreaseValue = m_dwInsert = 0;
	}
	
	~CAStarSearch()
	{
		_FREE(m_tppHeap);
	}

	IC void vfHeapify(u32 dwIndex)
	{
		do {
			u32 dwLeft  = dwIndex << 1, dwRight = dwLeft + 1, dwSmallest;
			if ((dwLeft <= m_dwHeapSize) && (m_tppHeap[dwLeft]->f < m_tppHeap[dwIndex]->f))
				dwSmallest = dwLeft;
			else
				dwSmallest = dwIndex;
			if ((dwRight <= m_dwHeapSize) && (m_tppHeap[dwRight]->f < m_tppHeap[dwSmallest]->f))
				dwSmallest = dwRight;
			if (dwSmallest != dwIndex) {
				SNode *tpTemp = m_tppHeap[dwIndex];
				m_tppHeap[dwIndex] = m_tppHeap[dwSmallest];
				m_tppHeap[dwSmallest] = tpTemp;
				dwIndex = dwSmallest;
			}
			else 
				break;
		}
		while (true);
	}
	
	IC void vfExtractMinimum()
	{
		m_tppHeap[1] = m_tppHeap[m_dwHeapSize--];
		if (m_dwHeapSize)
			vfHeapify(1);
	}
	
	IC void vfFloat(SNode *tpNode, u32 dwIndex)
	{
		float	fKey = tpNode->f;
		while ((dwIndex > 1) && (m_tppHeap[dwIndex >> 1]->f > fKey)) {
			m_tppHeap[dwIndex] = m_tppHeap[dwIndex >> 1];
			dwIndex >>= 1;
		}
		m_tppHeap[dwIndex] = tpNode;
	}
	
	IC void vfInsert(SNode *tpNode)
	{
		vfFloat(tpNode,++m_dwHeapSize);
	}
	
	IC void vfDecreaseValue(SNode *tpNode)
	{
		for (SNode **tpIndex = m_tppHeap + 1; *tpIndex != tpNode; tpIndex++);
		vfFloat(tpNode,tpIndex - m_tppHeap);
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
			
			//!!!
			// remove that node from the opened list and put that node to the closed list
			tpBestNode->ucOpenCloseMask = 0;
			vfExtractMinimum();
			//m_dwExtractMinimum++;
			//pop_heap(m_tppHeap + 1,m_tppHeap + m_dwHeapSize-- + 1,CComparePredicate());

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
							vfDecreaseValue(tpTemp);
							//m_dwDecreaseValue++;
							//for (SNode **tpIndex = m_tppHeap + 1; *tpIndex != tpTemp; tpIndex++);
							//push_heap(m_tppHeap + 1,tpIndex + 1,CComparePredicate());
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
					tpTemp1 = tpBestNode->tpForward;
					tpBestNode->tpForward = tpTemp2;
					
					//!!!
					vfInsert(tpTemp2);
					//m_dwInsert++;
					//m_tppHeap[++m_dwHeapSize] = tpTemp2;
					//push_heap(m_tppHeap + 1,m_tppHeap + m_dwHeapSize + 1,CComparePredicate());
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