#include "stdafx.h"
#include "xr_creator.h"
#include "ai_space.h"
#include "ai_astar.h"

// Definitions
static Fvector	vLimiterStart;
static float	fLimiter;

class GraphSearchNode
{
public:
	DWORD				NodeID;
	NodeCompressed*		Node;
	Fvector				Center;	// note: for performance reasons in ai_space local coordinates
	
	GraphSearchNode	()						
	{ 
		NodeID	= 0xffffffff;
		Node	= 0;
	}
	GraphSearchNode	( DWORD ID )	
	{ 
		NodeID		= ID;
		Node		= pCreator->AI.Node(ID);
		Fvector		P0,P1;
		pCreator->AI.UnpackPosition	(P0,Node->p0);
		pCreator->AI.UnpackPosition	(P1,Node->p1);
		Center.lerp	(P0,P1,.5f);
	}
	
	// Return the estimated cost to goal from this node
	IC float	GoalDistanceEstimate	( GraphSearchNode &Goal )
	{
		return Center.distance_to		(Goal.Center);
	}

	// Return true if the provided state is the same as this state
	IC bool		IsSameState				( GraphSearchNode &rhs )
	{
		return Node==rhs.Node;
	}

	// Return true if this node is the goal
	IC bool		IsGoal					( GraphSearchNode &Goal )
	{
		return Node==Goal.Node;
	}
	
	// given this node, what does it cost to move to successor. In the case
	// of our map the answer is the map terrain value at this node since that is 
	// conceptually where we're moving
	IC float	GetCost					( GraphSearchNode &successor )
	{
//		if (successor.Center.distance_to_sqr(vLimiterStart)>fLimiter)	return 100;
		return 0;
	}

	// This generates the successors to the given Node. It uses a helper function called
	// AddSuccessor to give the successors to the AStar class. The A* specific initialisation
	// is done for each node internally, so here you just set the state information that
	// is specific to the application
	void		GetSuccessors			( AStarSearch<GraphSearchNode> *astarsearch, GraphSearchNode *parent_node )
	{
		// analyze parent
		DWORD	Parent			= 0xffffffff;
		if (parent_node)		Parent = parent_node->NodeID;

		// iterate on links
		DWORD L_count			= DWORD(Node->link_count);
		NodeLink* L_it			= (NodeLink*)(LPBYTE(Node)+sizeof(NodeCompressed));
		NodeLink* L_end			= L_it+L_count;
		for(; L_it!=L_end; L_it++) 
		{
			DWORD ID			= pCreator->AI.UnpackLink(*L_it);
			if (ID==Parent)		continue;

			astarsearch->AddSuccessor(GraphSearchNode(ID));
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef AStarSearch<GraphSearchNode>	GraphSearch;
void CAI_Space::q_Path(DWORD StartNode, DWORD GoalNode,	AI::Path&	Result)
{
	static  GraphSearch			astar	(8192);

	if (0==vfs)	return;

	Device.Statistic.AI_Path.Begin	();

//	Msg("%d - %d",StartNode,GoalNode);
	
	// Set Start and goal states
	GraphSearchNode nodeStart	(StartNode);
	GraphSearchNode nodeEnd		(GoalNode);
	astar.SetStartAndGoalStates	( nodeStart, nodeEnd	);
	/*
	vLimiterStart.set			( nodeStart.Center		);
	fLimiter					= 
	*/

	// Perform searching
	unsigned int SearchState;
	unsigned int SearchSteps = 0;
	do
	{
		SearchState = astar.SearchStep();
		SearchSteps	++;
	}
	while( SearchState == GraphSearch::SEARCH_STATE_SEARCHING );
	
	// Get result
	Result.Nodes.clear();
	if( SearchState == GraphSearch::SEARCH_STATE_SUCCEEDED )
	{
		GraphSearchNode *node = astar.GetSolutionStart();
		Result.Nodes.push_back(node->NodeID);
		for( ;; )
		{
			node = astar.GetSolutionNext();
			if( 0==node )	break;

			Result.Nodes.push_back(node->NodeID);
		};
		
		// Once you're done with the solution you can free the nodes up
		astar.FreeSolutionNodes();

		// 
//		Msg("* [%d-%d] %d searched, %d length",StartNode,GoalNode,SearchSteps,Result.Nodes.size());
	}
	else if( SearchState == GraphSearch::SEARCH_STATE_FAILED ) 
	{
//		Msg("! PathFinder: Search terminated. Did not find goal state");
	}

	Device.Statistic.AI_Path.End	();
}
