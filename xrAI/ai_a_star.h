////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star.h
//	Created 	: 21.03.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding the x-est path between two nodes
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAI_AI_A_STAR__
#define __XRAI_AI_A_STAR__

////////////////////////////////////////////////////////////////////////////
// included headers
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// type declarations
////////////////////////////////////////////////////////////////////////////

#pragma pack(4)
typedef struct tagTNode {
	int			ucOpenCloseMask:8;
	int			iIndex:24;
	float		f;
	float		g;
	float		h;
	tagTNode	*tpNext;
	tagTNode	*tpForward;
	tagTNode	*tpBack;
	tagTNode	*tpOpenedNext;
	tagTNode	*tpOpenedPrev;
} TNode;

typedef struct tagTIndexNode {
	TNode		*tpNode;
	u32			dwTime;
} TIndexNode;
#pragma pack()

extern void	vfLoadSearch();
extern void vfFindTheShortestPath(TNode *taHeap, TIndexNode *tpaIndexes, u32 &dwAStarStaticCounter, u32 dwStartNode, u32 dwGoalNode, float &fDistance, float fMaxDistance, Fvector tStartPosition, Fvector tFinishPosition, vector<u32> &tpaNodes);

#endif