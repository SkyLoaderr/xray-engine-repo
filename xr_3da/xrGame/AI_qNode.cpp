#include "stdafx.h"
#include "ai_space.h"

class	Selector : public AI::NodeEstimator
{
	NodePosition*	P;
public:
	virtual	float	Estimate(NodeCompressed* Node, float dist, BOOL& bStop)	// min - best, max - worse
	{
		if (Level().AI.u_InsideNode(*Node,*P)) { bStop = TRUE; return 0; }
		else return	dist;
	}
	Selector(NodePosition*	_P) : P(_P) {};
};

void CAI_Space::q_Range_Bit(DWORD StartNode, const Fvector& BasePos, float Range, NodePosition* QueryPosition, DWORD &BestNode, float &BestCost)
{
	if (0==vfs)	return;

	Device.Statistic.AI_Range.Begin	();

	// Initialize
	NodePosition QueryPos;
	PackPosition(QueryPos,BasePos);
	q_stack.clear();
	q_stack.push_back(StartNode);
	q_mark_bit[StartNode] = true;
	NodeCompressed*	Base = m_nodes_ptr	[StartNode];
	BestNode = StartNode;
	BestCost = MAX_NODE_ESTIMATION_COST;
	if (!(Level().AI.u_InsideNode(*Base,*QueryPosition))) {
		BestCost = u_SqrDistance2Node(BasePos,Base);
		float range_sqr		= Range*Range;

		// Cycle
		for (DWORD it=0; it<q_stack.size(); it++) {
			DWORD ID = q_stack[it];
			NodeCompressed*	N = m_nodes_ptr	[ID];
			DWORD L_count = DWORD(N->link_count);
			NodeLink* L_it = (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
			NodeLink* L_end	= L_it+L_count;
			for( ; L_it!=L_end; L_it++) {
				// test node
				DWORD Test = UnpackLink(*L_it);
				if (q_mark_bit[Test])
					continue;

				NodeCompressed*	T = m_nodes_ptr[Test];

				float distance_sqr = u_SqrDistance2Node(BasePos,T);
				if (distance_sqr>range_sqr)	
					continue;

				// register
				q_mark_bit[Test]		= true;
				q_stack.push_back	(Test);

				// estimate
				if (Level().AI.u_InsideNode(*T,*QueryPosition))
					break;
				else {
					float cost = distance_sqr;
					if (cost<BestCost) {
						BestCost	= cost;
						BestNode	= Test;
					}
				}
			}
		}
	}
	// Clear q_marks
	{
		DWORD* it = q_stack.begin();
		DWORD* end = q_stack.end();
		for ( ; it!=end; it++)	
			q_mark_bit[*it] = false;
	}

	Device.Statistic.AI_Range.End();
}

DWORD CAI_Space::q_Node(DWORD PrevNode, const Fvector& BasePos)
{
	if (0==vfs)	return	0;
	
	Device.Statistic.AI_Node.Begin	();
	
	// Check if we still in old node
	NodePosition	QueryPos;
	PackPosition	(QueryPos,BasePos);
	if (u_InsideNode(*m_nodes_ptr[PrevNode],QueryPos)) {
		Device.Statistic.AI_Node.End();
		return PrevNode;
	}
	
	// Perform neibourhood search
	DWORD BestNode;
	float BestCost;
	q_Range_Bit(PrevNode,BasePos,m_header.size*3,&QueryPos,BestNode,BestCost);
	if (BestCost < m_header.size)	{
		// small distance from node
		Device.Statistic.AI_Node.End();
		return BestNode;
	}
	
	// degrade to linear search
	int id = q_LoadSearch(BasePos);
	if (id>=0) return DWORD(id);
	
	// everything failed :(
	// maintain old node
	Device.Statistic.AI_Node.End		();
	return PrevNode;
}
