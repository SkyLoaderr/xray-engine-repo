#include "stdafx.h"
#include "ai_space.h"

#define COEFFICIENT	0.001

void CAI_Space::q_Range(DWORD StartNode, const Fvector& BasePos, float Range, AI::NodeEstimator& Estimator, float &fOldCost)
{
	if (0==vfs)	return;

	Device.Statistic.AI_Range.Begin	();

	// Initialize
	BOOL bStop = FALSE;
	NodePosition QueryPos;
	PackPosition(QueryPos,BasePos);
	q_stack.clear();
	q_stack.push_back(StartNode);
	q_mark [StartNode]	+= 1;
	NodeCompressed*	Base = m_nodes_ptr	[StartNode];
	Estimator.BestNode	= StartNode;
	Estimator.BestCost = MAX_NODE_ESTIMATION_COST;
	fOldCost = Estimator.BestCost = Estimator.Estimate(Base,u_SqrDistance2Node(BasePos,Base),bStop);
	float range_sqr		= Range*Range;

	// Cycle
	for (DWORD it=0; it<q_stack.size(); it++) {
		DWORD ID = q_stack[it];
		NodeCompressed*	N = m_nodes_ptr	[ID];
		DWORD L_count	= DWORD(N->links);
		NodeLink* L_it	= (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
		NodeLink* L_end	= L_it+L_count;
		for( ; L_it!=L_end; L_it++) {
			if (bStop)			
				break;
			// test node
			DWORD Test = UnpackLink(*L_it);
			if (q_mark[Test])
				continue;

			NodeCompressed*	T = m_nodes_ptr[Test];

			float distance_sqr = u_SqrDistance2Node(BasePos,T);
			if (distance_sqr>range_sqr)	
				continue;

			// register
			q_mark[Test]		+= 1;
			q_stack.push_back	(Test);

			// estimate
			float cost = Estimator.Estimate(T,distance_sqr,bStop);
			
			if (cost<Estimator.BestCost) {
				Estimator.BestCost	= cost;
				Estimator.BestNode	= Test;
			}
		}
		if (bStop)
			break;
	}

	// Clear q_marks
	{
		vector<DWORD>::iterator it	= q_stack.begin();
		vector<DWORD>::iterator end	= q_stack.end();
		for ( ; it!=end; it++)	
			q_mark[*it] -= 1;
	}

	Device.Statistic.AI_Range.End();
}

void CAI_Space::q_Range_Bit(DWORD StartNode, const Fvector& BasePos, float Range, AI::NodeEstimator& Estimator, float &fOldCost)
{
	if (0==vfs)	return;

	Device.Statistic.AI_Range.Begin	();

	// Initialize
	BOOL bStop = FALSE;
	NodePosition QueryPos;
	PackPosition(QueryPos,BasePos);
	q_stack.clear();
	q_stack.push_back(StartNode);
	q_mark_bit [StartNode]	= true;
	NodeCompressed*	Base = m_nodes_ptr	[StartNode];
	Estimator.BestNode	= StartNode;
	Estimator.BestCost = MAX_NODE_ESTIMATION_COST;
	fOldCost = Estimator.BestCost = Estimator.Estimate(Base,u_SqrDistance2Node(BasePos,Base),bStop);
	float range_sqr		= Range*Range;

	// Cycle
	for (DWORD it=0; it<q_stack.size(); it++) {
		DWORD ID = q_stack[it];
		NodeCompressed*	N = m_nodes_ptr	[ID];
		DWORD L_count	= DWORD(N->links);
		NodeLink* L_it	= (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
		NodeLink* L_end	= L_it+L_count;
		for( ; L_it!=L_end; L_it++) {
			if (bStop)			
				break;
			// test node
			DWORD Test = UnpackLink(*L_it);
			if (q_mark_bit[Test])
				continue;

			NodeCompressed*	T = m_nodes_ptr[Test];

			float distance_sqr = u_SqrDistance2Node(BasePos,T);
			if (distance_sqr>range_sqr)	
				continue;

			// register
			q_mark_bit[Test]	= true;
			q_stack.push_back	(Test);

			// estimate
			float cost = Estimator.Estimate(T,distance_sqr,bStop);
			
			if (cost<Estimator.BestCost) {
				Estimator.BestCost	= cost;
				Estimator.BestNode	= Test;
			}
		}
		if (bStop)
			break;
	}

	// Clear q_marks
	{
		vector<DWORD>::iterator it	= q_stack.begin();
		vector<DWORD>::iterator end	= q_stack.end();
		for ( ; it!=end; it++)	
			q_mark_bit[*it] = false;
	}

	Device.Statistic.AI_Range.End();
}

void CAI_Space::q_Range(DWORD StartNode, const Fvector& BasePos, float Range, AI::NodeEstimator& Estimator, float &fOldCost, DWORD dwTimeDifference)
{
	if (0==vfs)	return;

	Device.Statistic.AI_Range.Begin	();

	// Initialize
	BOOL bStop = FALSE;
	NodePosition QueryPos;
	PackPosition(QueryPos,BasePos);
	q_stack.clear();
	q_stack.push_back(StartNode);
	q_mark [StartNode]	+= 1;
	NodeCompressed*	Base = m_nodes_ptr	[StartNode];
	Estimator.BestNode	= StartNode;
	Estimator.BestCost = MAX_NODE_ESTIMATION_COST;
	Estimator.BestCost = Estimator.Estimate(Base,u_SqrDistance2Node(BasePos,Base),bStop);
	Estimator.BestCost += dwTimeDifference*COEFFICIENT;
	fOldCost = Estimator.BestCost;
	float range_sqr		= Range*Range;

	// Cycle
	for (DWORD it=0; it<q_stack.size(); it++) {
		DWORD ID = q_stack[it];
		NodeCompressed*	N = m_nodes_ptr	[ID];
		DWORD L_count = DWORD(N->links);
		NodeLink* L_it = (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
		NodeLink* L_end	= L_it+L_count;
		for( ; L_it!=L_end; L_it++) {
			if (bStop)			
				break;
			// test node
			DWORD Test = UnpackLink(*L_it);
			if (q_mark[Test])
				continue;

			NodeCompressed*	T = m_nodes_ptr[Test];

			float distance_sqr = u_SqrDistance2Node(BasePos,T);
			if (distance_sqr>range_sqr)	
				continue;

			// register
			q_mark[Test]		+= 1;
			q_stack.push_back	(Test);

			// estimate
			float cost = Estimator.Estimate(T,distance_sqr,bStop);
			cost += dwTimeDifference*1/sqrtf(distance_sqr)*COEFFICIENT;
			
			if (cost<Estimator.BestCost) {
				Estimator.BestCost	= cost;
				Estimator.BestNode	= Test;
			}
		}
		if (bStop)
			break;
	}

	// Clear q_marks
	{
		vector<DWORD>::iterator it	= q_stack.begin();
		vector<DWORD>::iterator end	= q_stack.end();
		for ( ; it!=end; it++)	
			q_mark[*it] -= 1;
	}

	Device.Statistic.AI_Range.End();
}
