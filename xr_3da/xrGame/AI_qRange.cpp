#include "stdafx.h"
#include "ai_space.h"

void CAI_Space::q_Range(DWORD StartNode, const Fvector& BasePos, float Range, AI::NodeEstimator& Estimator)
{
	if (0==vfs)	return;

	Device.Statistic.AI_Range.Begin	();

	// Initialize
	BOOL bStop			= FALSE;
	NodePosition		QueryPos;
	PackPosition		(QueryPos,BasePos);
	q_stack.clear		();
	q_stack.push_back	(StartNode);
	q_mark [StartNode]	= true;
	NodeCompressed*	Base= m_nodes_ptr	[StartNode];
	Estimator.BestNode	= StartNode;
	Estimator.BestCost	= Estimator.Estimate(Base,u_SqrDistance2Node(BasePos,Base),bStop);
	float	range_sqr	= Range*Range;

	// Cycle
	for (DWORD it=0; it<q_stack.size(); it++)
	{
		DWORD ID				= q_stack		[it];
		NodeCompressed*	N		= m_nodes_ptr	[ID];
		DWORD L_count			= DWORD(N->link_count);
		NodeLink* L_it			= (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
		NodeLink* L_end			= L_it+L_count;
		for(; L_it!=L_end; L_it++) 
		{
			if (bStop)			break;

			// test node
			DWORD Test			= UnpackLink(*L_it);
			if (q_mark[Test])	continue;

			NodeCompressed*	T	= m_nodes_ptr	[Test];

			float distance_sqr	= u_SqrDistance2Node(BasePos,T);
			if (distance_sqr>range_sqr)	continue;

			// register
			q_mark[Test]		= true;
			q_stack.push_back	(Test);

			// estimate
			float	cost		= Estimator.Estimate(T,distance_sqr,bStop);
			if (cost<Estimator.BestCost)	{
				Estimator.BestCost	= cost;
				Estimator.BestNode	= Test;
			}
		}
		if (bStop)	break;
	}

	// Clear q_marks
	{
		DWORD*	it	= q_stack.begin	();
		DWORD*	end	= q_stack.end	();
		for (; it!=end; it++)	q_mark[*it] = false;
		q_mark [StartNode]	= false;
//		Msg("* stack: %d",q_stack.size());
	}

	Device.Statistic.AI_Range.End		();
}
