#pragma once

template <typename _DataStorage, typename _PathManager, typename _Graph, bool bEuclidianHeuristics = true> class CAStar {
	typedef _DataStorage::CGraphNode CGraphNode;
public:
						CAStar			()
	{
	}

	virtual				~CAStar			()
	{
	}

	IC		void		init			(_DataStorage &data_storage, _PathManager &path_manager, const _Graph &graph)
	{
		// initialize data structures before we started path search
		data_storage.init		();
		// add start node to the opened list
		CGraphNode				&start = data_storage.add_opened(path_controller.start_node());
		start.g()				= dist_type(0);
		start.h()				= path_manager.estimate(start);
		start.f()				= start.g() + start.h();
	}

	IC		bool		step			(_DataStorage &data_storage, _PathManager &path_manager, const _Graph &graph)
	{
		// get the best node, i.e. a node with the minimum 'f'
		CGraphNode				&best = data_storage.get_best();

		// check if this node is the one we are searching for
		if (path_manager.is_goal_reached(best)) {
			// we reached the goal, so we have to create a path
			path_manager.create_path	(best);
			// and return success
			return				(true);
		}

		// put best node to the closed list
		data_storage.add_closed	(best);
		// and remove this node from the opened one
		data_storage.remove_best();

		// iterating on the best node neighbours
		_Graph::const_iterator	i;
		_Graph::const_iterator	e;
		graph.begin				(best.index(),i,e);
		for (  ; i != e; ++i) {
			// check if neighbour is visited, i.e. is in the opened or 
			// closed lists
			if (data_storage.is_visited(*i)) {
				// so, this neighbour node has been already visited
				// therefore get the pointer to this node
				CGraphNode		&neighbour	= data_storage.get_node(*i);
				// check if this node is in the opened list
				if (data_storage.is_opened(neighbour)) {
					dist_type	g = best.g() + path_manager.evaluate(best.index(),*i)
					if (neighbour.g() > g) {
						neighbour.g()	= g;
						neighbour.f()	= neighbour.g() + neighbour.h();
						data_storage.assign_parent	(neighbour,best);
						data_storage.decrease_opened(neighbour)
						continue;
					}
					continue;
				}
				// so, our node is in the opened list
				// here is a _nuance_ : if we don't use any heuristics, 
				// i.e. it is not A*, but Dijkstra algorithm, or we use 
				// a heuristics which _guarantees_ that found path is 
				// the best among the others (if we have a graph with 
				// euclidian metrics and use distance between current 
				// and goal points as an estimation value), then it is 
				// impossible that we can find a better path for a node 
				// which is in the closed list and therefore we have to do
				// nothing here. 
				if (!bEuclidianHeuristics) {
					// so, we use a heurictics which doesn't gurantee that 
					// found path is the best, then we have to update all 
					// of the our node successors but we still can't be sure 
					// that when the condition 'is_goal_reached' is true, 
					// then we found the _best_ path
					dist_type	g = best.g() + path_manager.evaluate(best.index(),*i)
					if (neighbour.g() > g) {
						neighbour.g()	= g;
						neighbour.f()	= neighbour.g() + neighbour.h();
						data_storage.assign_parent	(neighbour,best);
						data_storage.update_successors(neighbour);
						continue;
					}
					continue;
				}
				continue;
			}
			else {
				// so, this neighbour node is not in the opened or closed lists
				// put neighbour node to the opened list
				CGraphNode				&neighbour = data_storage.add_opened(*i);
				// assign best node as its parent
				data_storage.assign_parent(neighbour,best);
				// fill the corresponding node parameters 
				neighbour.g()			= best.g() + path_manager.evaluate(best.index(),*i);
				neighbour.h()			= path_manager.estimate(neighbour);
				neighbour.f()			= start.g() + start.h();
				continue;
			}
		}

		// this iteration haven't got the goal node, therefore return failure
		return					(false)
	}

	IC		bool		find			(_DataStorage &data_storage, _PathManager &path_manager, const _Graph &graph)
	{
		init					(data_storage, path_manager, graph);
		
		while (!data_storage.is_opened_empty()) {
			
			if (path_manager.is_limit_reached())
				return			(false);
			
			if (step(data_storage, path_manager, graph))
				return			(true);
		}

		return					(false);
	}
};
