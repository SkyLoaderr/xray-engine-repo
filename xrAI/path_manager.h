////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path managers
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _Graph, typename _DataStorage, typename _dist_type = float, typename _index_type = u32, typename _iteration_type = u32> class CPathManager {
	const _Graph		*graph;
	_DataStorage		*data_storage; 
	xr_vector<_index_type>	*path;
	_index_type			start_node_index;
	_index_type			goal_node_index;
	_dist_type			max_range;
	_iteration_type		max_iteration_count;
	_index_type			max_visited_node_count;
public:

						CPathManager	()
	{
	}

	virtual				~CPathManager	()
	{
	}

			void		init			(
				const _Graph			*graph,
				_DataStorage			*data_storage,
				xr_vector<_index_type>	*path,
				const _index_type		start_node_index,
				const _index_type		goal_node_index,
				const _index_type		max_visited_node_count = _index_type(u32(-1)),
				const _dist_type		max_range = _dist_type(6000),
				const _iteration_type	max_iteration_count = _iteration_type(u32(-1))
			)
	{
		this->graph				= graph;
		this->data_storage		= data_storage;
		this->path				= path;
		this->start_node_index	= start_node_index;
		this->goal_node_index	= goal_node_index;
		this->max_visited_node_count = max_visited_node_count;
		this->max_range			= max_range;
		this->max_iteration_count = max_iteration_count;
	}

	IC		_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2)
	{
		return					(graph->get_edge_weight(node_index1,node_index2));
	}

	IC		_dist_type	estimate		(const _index_type node_index)
	{
		return					(graph->get_edge_weight(node_index,goal_node_index));
	}

	IC		_index_type	start_node		()
	{
		return					(start_node_index);
	}

	IC		bool		is_goal_reached	(const _index_type node_index) const
	{
		return					(node_index == goal_node_index);
	}

	IC		bool		is_limit_reached(const _iteration_type	iteration_count) const
	{
		return					(
			(data_storage->get_best().f() >= max_range)	||
			(iteration_count >= max_iteration_count)		||
			(data_storage->get_visited_node_count() >= max_visited_node_count)
		);
	}

	IC		bool		is_accessible	(const _index_type node_index) const
	{
		return					(true);
	}

	IC		bool		is_metric_euclidian()
	{
		return					(true);
	}

	IC		void		create_path		(const _index_type node_index)
	{
		Msg						("Path is found!");
		data_storage->get_path	(*path);
		xr_vector<_index_type>::const_iterator	i = path->begin();
		xr_vector<_index_type>::const_iterator	e = path->end();
		for ( ; i != e; ++i)
			Msg					("%d",*i);
		Msg						("Total : %d nodes",e - path->begin());
	}
};