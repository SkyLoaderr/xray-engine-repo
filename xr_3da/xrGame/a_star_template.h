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
								tpTemp->tpOpenedPrev->tpOpenedNext = tpTemp->tpOpenedNext;
								if (tpTemp->tpOpenedNext)
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
			m_dwExtractMinimum++;
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
							m_dwDecreaseValue++;
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
					tpTemp1 = tpBestNode->tpForward;
					tpBestNode->tpForward = tpTemp2;
					
					//!!!
					m_dwInsert++;
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
	//tagSNode	**tpIndex;
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
//	u32						m_dwHeapSize;
	SNode					**m_tppHeap;
	SNode					**m_tppStart;
	SNode					**m_tppEnd;
public:
	u32						m_dwExtractMinimum;
	u32						m_dwDecreaseValue;
	u32						m_dwInsert;
	CAStarSearch(u32 dwMaxNodeCount)
	{
		m_dwMaxNodeCount	= dwMaxNodeCount;
		m_tppHeap			= (SNode **)xr_malloc((dwMaxNodeCount + 1)*sizeof(SNode *));
		m_tppStart			= m_tppHeap + 1;
		m_tppEnd			= m_tppHeap;
		*m_tppEnd			= 0;
		m_dwExtractMinimum  = m_dwDecreaseValue = m_dwInsert = 0;
	}
	
	~CAStarSearch()
	{
		_FREE(m_tppHeap);
	}

	IC void vfHeapify()
	{
//		do {
//			u32 dwLeft  = dwIndex << 1, dwRight = dwLeft + 1, dwSmallest;
//			if ((dwLeft <= m_dwHeapSize) && (m_tppHeap[dwLeft]->f < m_tppHeap[dwIndex]->f))
//				dwSmallest = dwLeft;
//			else
//				dwSmallest = dwIndex;
//			if ((dwRight <= m_dwHeapSize) && (m_tppHeap[dwRight]->f < m_tppHeap[dwSmallest]->f))
//				dwSmallest = dwRight;
//			if (dwSmallest != dwIndex) {
//				SNode *tpTemp = m_tppHeap[dwIndex];
//				m_tppHeap[dwIndex] = m_tppHeap[dwSmallest];
//				m_tppHeap[dwSmallest] = tpTemp;
//				
////				m_tppHeap[dwIndex]->tpIndex = m_tppHeap + dwIndex;
////				m_tppHeap[dwSmallest]->tpIndex = m_tppHeap + dwSmallest;
//				
//				dwIndex = dwSmallest;
//			}
//			else 
//				break;
//		}
//		while (true);
		SNode **tpIndex = m_tppStart;
		do {
			SNode **tpLeft = m_tppHeap + ((tpIndex - m_tppHeap) << 1), **tpRight = tpLeft + 1, **tpSmallest;
			if ((tpLeft <= m_tppEnd) && ((*tpLeft)->f < (*tpIndex)->f))
				tpSmallest = tpLeft;
			else
				tpSmallest = tpIndex;
			if ((tpRight <= m_tppEnd) && ((*tpRight)->f < (*tpSmallest)->f))
				tpSmallest = tpRight;
			if (tpSmallest != tpIndex) {
				SNode *tpTemp = *tpIndex;
				*tpIndex =  *tpSmallest;
				*tpSmallest = tpTemp;
				
//				m_tppHeap[dwIndex]->tpIndex = m_tppHeap + dwIndex;
//				m_tppHeap[dwSmallest]->tpIndex = m_tppHeap + dwSmallest;
				
				tpIndex = tpSmallest;
			}
			else 
				break;
		}
		while (true);
	}
	
	IC void vfExtractMinimum()
	{
//		m_tppHeap[m_dwHeapSize]->tpIndex = m_tppHeap + 1;
		if (*--m_tppEnd) {
			*m_tppStart = *m_tppEnd;
			vfHeapify();
		}
	}
	
	IC void vfFloat(SNode *tpNode, SNode **tppStart)
	{
		float	fKey = tpNode->f;
		while ((tppStart != m_tppStart) && ((*(m_tppHeap + ((tppStart - m_tppHeap) >> 1)))->f > fKey)) {
			*tppStart = *(m_tppHeap + ((tppStart - m_tppHeap) >> 1));
			
//			m_tppHeap[dwIndex]->tpIndex = m_tppHeap + dwIndex;
			
			tppStart = m_tppHeap + ((tppStart - m_tppHeap) >> 1);
		}
		*tppStart = tpNode;
		
//		m_tppHeap[dwIndex]->tpIndex = m_tppHeap + dwIndex;
	}
	
	IC void vfInsert(SNode *tpNode)
	{
		vfFloat(tpNode,++m_tppEnd);
	}
	
	IC void vfDecreaseValue(SNode *tpNode)
	{
		for (SNode **tpIndex = m_tppHeap + 1; *tpIndex != tpNode; tpIndex++);
		vfFloat(tpNode,tpIndex);
		//vfFloat(tpNode,tpNode->tpIndex - m_tppHeap);
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
		*++m_tppEnd = tpTemp;
		//tpTemp->tpIndex = m_tppHeap + 1;
		
		//!!!
		while (*m_tppEnd) {
			
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