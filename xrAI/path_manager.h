////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path managers
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_base.h"

template <
	typename _Graph,
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager : 
		public CPathManagerBase <
			_Graph,
			_DataStorage,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
};

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CAI_Map,
		_DataStorage,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManagerBase <
			CAI_Map,
			_DataStorage,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CAI_Map _Graph;
	typedef typename CPathManagerBase <
				CAI_Map,
				_DataStorage,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	int					x1;
	float				y1;
	int					z1;
	int					x2;
	float				y2;
	int					z2;
	int					x3;
	float				y3;
	int					z3;
	float				square_size_xz;
	float				square_size_y;
public:

	virtual				~CPathManager	()
	{
	}

	IC		void		setup			(
				const _Graph			*_graph,
				_DataStorage			*_data_storage,
				xr_vector<_index_type>	*_path,
				const _index_type		_start_node_index,
				const _index_type		_goal_node_index,
				const _index_type		_max_visited_node_count	= _index_type(u32(-1)),
				const _dist_type		_max_range				= _dist_type(6000),
				const _iteration_type	_max_iteration_count	= _iteration_type(u32(-1))
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			_max_visited_node_count,
			_max_range,
			_max_iteration_count
		);
		square_size_xz			= graph->m_fSize2;
		square_size_y			= graph->m_fYSize2;
	}

	IC	void		init			()
	{
		_Graph::InternalNode	&tNode1	= *graph->Node(start_node_index);
		x2						= (int)(tNode1.p1.x) + (int)(tNode1.p0.x);
		y2						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2						= (int)(tNode1.p1.z) + (int)(tNode1.p0.z);
		
		_Graph::InternalNode	&tNode2	= *graph->Node(goal_node_index);
		x3						= (int)(tNode2.p1.x) + (int)(tNode2.p0.x);
		y3						= (float)(tNode2.p1.y) + (float)(tNode2.p0.y);
		z3						= (int)(tNode2.p1.z) + (int)(tNode2.p0.z);
	}

	IC	_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2)
	{
		VERIFY					(graph);
		
		_Graph::InternalNode	&tNode1 = *graph->Node(node_index2);

		x2						= (int)(tNode1.p1.x) + (int)(tNode1.p0.x);
		y2						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2						= (int)(tNode1.p1.z) + (int)(tNode1.p0.z);

		return					(_sqrt((float)(square_size_xz*float(_sqr(x2 - x1) + _sqr(z2 - z1)) + square_size_y*(float)_sqr(y2 - y1))));
	}

	IC	_dist_type	estimate		(const _index_type node_index)
	{
		VERIFY					(graph);
		return					(_sqrt((float)(square_size_xz*float(_sqr(x3 - x2) + _sqr(z3 - z2)) + square_size_y*(float)_sqr(y3 - y2))));
	}

	IC	void		create_path		()
	{
		VERIFY					(data_storage && path);
		data_storage->get_path	(*path);
	}

	IC	_index_type	start_node		()
	{
		return					(start_node_index);
	}

	IC	bool		is_goal_reached	(const _index_type node_index)
	{
		if (node_index == goal_node_index)
			return				(true);
		
		_Graph::InternalNode	&tNode0 = *graph->Node(node_index);

		x1						= (int)(tNode0.p1.x) + (int)(tNode0.p0.x);
		y1						= (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1						= (int)(tNode0.p1.z) + (int)(tNode0.p0.z);

		return					(false);
	}

	IC	bool		is_limit_reached(const _iteration_type	iteration_count) const
	{
		VERIFY					(data_storage);
		return					(false);
	}

	IC	bool		is_accessible	(const _index_type node_index) const
	{
		VERIFY					(graph);
		return					(true);
	}

	IC	bool		is_metric_euclidian()
	{
		return					(true);
	}
};
