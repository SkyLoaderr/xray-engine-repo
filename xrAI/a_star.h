////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Implementation of the A* (a-star) algorithm
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _DataStorage, 
	typename _PathManager, 
	typename _iteration_type = u32, 
	typename _dist_type = float
> class CAStar 
{
	typedef typename _DataStorage::CGraphNode CGraphNode;
public:
						CAStar			()
	{
		// constructor
		// here we do nothing
	}

	virtual				~CAStar			()
	{
		// destructor
		// here we do nothing
	}

	IC		void		init			(_DataStorage &data_storage, _PathManager &path_manager)
	{
		// initialize data structures before we started path search
		data_storage.init		();
		// initialize path manager before we started path search
		path_manager.init		();
		// create a node
		CGraphNode				&start = data_storage.create_node(path_manager.start_node());
		// assign correspoding values to the created node
		start.g()				= _dist_type(0);
		start.h()				= path_manager.estimate(start.index());
		start.f()				= start.g() + start.h();
		// assign null parent to the start node
		data_storage.assign_parent(start,0);
		// add start node to the opened list
		data_storage.add_opened	(start);
	}

	IC		bool		step			(_DataStorage &data_storage, _PathManager &path_manager)
	{
		// get the best node, i.e. a node with the minimum 'f'
		CGraphNode				&best = data_storage.get_best();

		// check if this node is the one we are searching for
		if (path_manager.is_goal_reached(best.index())) {
			// we reached the goal, so we have to create a path
			path_manager.create_path	();
			// and return success
			return				(true);
		}

		// put best node to the closed list
		data_storage.add_best_closed();
		// and remove this node from the opened one
		data_storage.remove_best_opened();

		// iterating on the best node neighbours
		_PathManager::const_iterator	i;
		_PathManager::const_iterator	e;
		path_manager.begin				(best.index(),i,e);
		for (  ; i != e; ++i) {
			// check if neighbour is accessible
			if (!path_manager.is_accessible(path_manager.get_value(i)))
				continue;
			// check if neighbour is visited, i.e. is in the opened or 
			// closed lists
			if (data_storage.is_visited(path_manager.get_value(i))) {
				// so, this neighbour node has been already visited
				// therefore get the pointer to this node
				CGraphNode				&neighbour	= data_storage.get_node(path_manager.get_value(i));
				// check if this node is in the opened list
				if (data_storage.is_opened(neighbour)) {
					// compute 'g' for the node
					_dist_type	g = best.g() + path_manager.evaluate(best.index(),path_manager.get_value(i),i);
					// check if new path is better than the older one
					if (neighbour.g() > g) {
						// so, new path is better
						// assign corresponding values to the node
						_dist_type		d = neighbour.f();
						neighbour.g()	= g;
						neighbour.f()	= neighbour.g() + neighbour.h();
						// assign correct parent to the node to be able
						// to retreive a path
						data_storage.assign_parent	(neighbour,&best);
						// notify data storage about node decreasing value
						data_storage.decrease_opened(neighbour,d);
						// continue iterating on neighbours
						continue;
					}
					// so, new path is worse
					// continue iterating on neighbours
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
				if (!path_manager.is_metric_euclidian()) {
					// so, we use a heurictics which doesn't gurantee that 
					// found path is the best, then we have to update all 
					// of the our node successors but we still can't be sure 
					// that when the condition 'is_goal_reached' is true, 
					// then we found the _best_ path
					
					// check if new path is better than the older one
					_dist_type	g = best.g() + path_manager.evaluate(best.index(),path_manager.get_value(i),i);
					if (neighbour.g() > g) {
						// so, new path is better
						// assign corresponding values to the node
						neighbour.g()	= g;
						neighbour.f()	= neighbour.g() + neighbour.h();
						// assign correct parent to the node to be able
						// to retreive a path
						data_storage.assign_parent	(neighbour,&best);
						// notify data storage about node decreasing value
						// to make it modify all the node successors
						data_storage.update_successors(neighbour);
						// continue iterating on neighbours
						continue;
					}
					// so, new path is worse
					// continue iterating on neighbours
					continue;
				}
				// continue iterating on neighbours
				continue;
			}
			else {
				// so, this neighbour node is not in the opened or closed lists
				// put neighbour node to the opened list
				CGraphNode				&neighbour = data_storage.create_node(path_manager.get_value(i));
				// fill the corresponding node parameters 
				neighbour.g()			= best.g() + path_manager.evaluate(best.index(),path_manager.get_value(i),i);
				neighbour.h()			= path_manager.estimate(neighbour.index());
				neighbour.f()			= neighbour.g() + neighbour.h();
				// assign best node as its parent
				data_storage.assign_parent(neighbour,&best);
				// add start node to the opened list
				data_storage.add_opened	(neighbour);
				// continue iterating on neighbours
				continue;
			}
		}

		// this iteration haven't got the goal node, therefore return failure
		return					(false);
	}

	IC		bool		find			(_DataStorage &data_storage, _PathManager &path_manager)
	{
		// initialize data structures with new search
		init					(data_storage, path_manager);
		// iterate while opened list is not empty
		for (_iteration_type i = _iteration_type(0); !data_storage.is_opened_empty(); ++i) {
			// check if we reached limit
			if (path_manager.is_limit_reached(i))
				// so we reaches limit, return failure
				return			(false);
			
			// so, limit is not reached
			// check if new step will get us success
			if (step(data_storage, path_manager))
				// so this step reached the goal, return success
				return			(true);
		}

		// so, opened list is empty, return failure
		return					(false);
	}
};
