////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star_search.h
//	Created 	: 21.03.2002
//  Modified 	: 16.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding optimal path from source node to destnation one
////////////////////////////////////////////////////////////////////////////

#pragma once

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

	u32			m_dwMaxNodeCount;

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


public:
	CAStarSearch()
	{
		m_dwMaxNodeCount = 65535;
	}

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
		
		while (tpOpenedList->tpOpenedNext) {
			
			// finding the node being estimated as the cheapest among the opened ones
			tpBestNode = tpOpenedList->tpOpenedNext;
			
			if (tpBestNode->f >= fMaxValue) {
				fValue = fMaxValue;
				tpaNodes.clear();
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

				fValue = tpBestNode->f;
				tpTemp1 = tpBestNode;
				tpTemp = tpTemp1->tpBack;
				for (u32 i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++) ;

				tpaNodes.resize(i);

				tpTemp1 = tpBestNode;
				tpaNodes[--i] = tpBestNode->iIndex;
				tpTemp = tpTemp1->tpBack;
				for (u32 j=1; tpTemp; tpTemp = tpTemp->tpBack, j++)
					tpaNodes[i - j] = tpTemp->iIndex;
					
				Device.Statistic.AI_Path.End();
				return;
			}
			
			CTemplateNode::iterator tIterator;
			CTemplateNode::iterator tEnd;
			tTemplateNode.begin(iBestIndex,tIterator,tEnd);
			for (  ; tIterator != tEnd; tIterator++) {
				// checking if that node is in the path of the BESTNODE ones
				int iNodeIndex = tTemplateNode.get_value(tIterator);
				if (iNodeIndex == 408) {
					iNodeIndex = iNodeIndex;
				}
				// checking if that node the node of the moving object 
				if ((bUseMarks) && (!tTemplateNode.bfCheckIfAccessible(iNodeIndex)))
					continue;
				// checking if that node is in the path of the BESTNODE ones
				if (tpIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					bool bOk = true;
					tpTemp = tpIndexes[iNodeIndex].tpNode;
					if (!(tpTemp->ucOpenCloseMask)) {
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
					float dG = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex);
					
					// check if this node is already in the opened list
					if (tpTemp - tpHeap == 62) {
						tpTemp = tpTemp;
					}
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

					if (!(tpTemp->ucOpenCloseMask)) {
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
			if (dwHeap > m_dwMaxNodeCount)
				break;
		}
		
		tpaNodes.clear();
		fValue = fMaxValue;
		Device.Statistic.AI_Path.End();
	}
};