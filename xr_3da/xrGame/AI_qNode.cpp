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
	Selector		S(&QueryPos);
	q_Range			(PrevNode,BasePos,m_header.size*3,S);
	if (S.BestCost < m_header.size)	{
		// small distance from node
		Device.Statistic.AI_Node.End();
		return S.BestNode;
	}
	
	// degrade to linear search
	int id = q_LoadSearch(BasePos);
	if (id>=0) return DWORD(id);
	
	// everything failed :(
	// maintain old node
	Device.Statistic.AI_Node.End		();
	return PrevNode;
}
