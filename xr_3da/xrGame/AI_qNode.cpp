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

void CAI_Space::q_Range_Bit(u32 StartNode, const Fvector& BasePos, float Range, NodePosition* QueryPosition, u32 &BestNode, float &BestCost)
{
	if (0==vfs)	return;

	Device.Statistic.AI_Range.Begin	();

	// Initialize
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
		for (u32 it=0; it<q_stack.size(); it++) {
			u32 ID = q_stack[it];
			NodeCompressed*	N = m_nodes_ptr	[ID];
			u32 L_count = u32(N->links);
			NodeLink* L_it = (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
			NodeLink* L_end	= L_it+L_count;
			for( ; L_it!=L_end; L_it++) {
				// test node
				u32 Test = UnpackLink(*L_it);
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
				if (Level().AI.u_InsideNode(*T,*QueryPosition)) {
					BestCost = 0.f;
					BestNode = Test;
				}
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
	else {
		BestCost = 0.f;
		BestNode = StartNode;
	}
	// Clear q_marks
	{
		vector<u32>::iterator it	= q_stack.begin();
		vector<u32>::iterator end	= q_stack.end();
		for ( ; it!=end; it++)	
			q_mark_bit[*it] = false;
	}

	Device.Statistic.AI_Range.End();
}

void CAI_Space::q_Range_Bit_X(u32 StartNode, const Fvector& BasePos, float Range, NodePosition* QueryPosition, u32 &BestNode, float &BestCost)
{
	if (0==vfs)	return;

	Device.Statistic.AI_Range.Begin	();

	// Initialize
	q_stack.clear();
	q_stack.push_back(StartNode);
	q_mark_bit_x[StartNode] = true;
	NodeCompressed*	Base = m_nodes_ptr	[StartNode];
	BestNode = StartNode;
	BestCost = MAX_NODE_ESTIMATION_COST;
	if (!(Level().AI.u_InsideNode(*Base,*QueryPosition))) {
		BestCost = u_SqrDistance2Node(BasePos,Base);
		float range_sqr		= Range*Range;

		// Cycle
		for (u32 it=0; it<q_stack.size(); it++) {
			u32 ID = q_stack[it];
			NodeCompressed*	N = m_nodes_ptr	[ID];
			u32 L_count = u32(N->links);
			NodeLink* L_it = (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
			NodeLink* L_end	= L_it+L_count;
			for( ; L_it!=L_end; L_it++) {
				// test node
				u32 Test = UnpackLink(*L_it);
				if (q_mark_bit_x[Test])
					continue;

				NodeCompressed*	T = m_nodes_ptr[Test];

				float distance_sqr = u_SqrDistance2Node(BasePos,T);
				if (distance_sqr>range_sqr)	
					continue;

				// register
				q_mark_bit_x[Test]		= true;
				q_stack.push_back	(Test);

				// estimate
				if (Level().AI.u_InsideNode(*T,*QueryPosition)) {
					BestCost = 0.f;
					BestNode = Test;
				}
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
	else {
		BestCost = 0.f;
		BestNode = StartNode;
	}
	// Clear q_marks
	{
		vector<u32>::iterator it	= q_stack.begin();
		vector<u32>::iterator end	= q_stack.end();
		for ( ; it!=end; it++)	
			q_mark_bit_x[*it] = false;
	}

	Device.Statistic.AI_Range.End();
}

u32 CAI_Space::q_Node(u32 PrevNode, const Fvector& BasePos, bool bShortSearch)
{
	if (0==vfs)	return	0;
	
	Device.Statistic.AI_Node.Begin	();
	
	// Check if we still in old node
	NodePosition	QueryPos;
	PackPosition	(QueryPos,BasePos);

	if (u_InsideNode(*m_nodes_ptr[PrevNode],QueryPos)) {
	//if (bfInsideNode(BasePos,m_nodes_ptr[PrevNode])) {
		Device.Statistic.AI_Node.End();
		return PrevNode;
	}
	
	// Perform neibourhood search
	u32 BestNode;
	float BestCost;
	q_Range_Bit(PrevNode,BasePos,m_header.size*7,&QueryPos,BestNode,BestCost);
	//q_Range_Bit(PrevNode,BasePos,m_header.size*3,BestNode,BestCost);
	if (BestCost <= 7*m_header.size)	{
		// small distance from node
		Device.Statistic.AI_Node.End();
		return BestNode;
	}
	
	if (bShortSearch) {
		Device.Statistic.AI_Node.End		();
		return(PrevNode);
	}
	// degrade to linear search
	int id = q_LoadSearch(BasePos);
	if (id>=0) return u32(id);
	
	// everything failed :(
	// maintain old node
	Device.Statistic.AI_Node.End		();
	return PrevNode;
}