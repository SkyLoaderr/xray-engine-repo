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

#ifdef AI_COMPILER
template <
	typename _cell_type,
	u32		 rows,
	u32		 columns,
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CTestTable<_cell_type,rows,columns>,
		_DataStorage,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManagerBase <
			CTestTable<_cell_type,rows,columns>,
			_DataStorage,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CTestTable<_cell_type,rows,columns> _Graph;
	typedef typename CPathManagerBase <
		_Graph,
		_DataStorage,
		_dist_type,
		_index_type,
		_iteration_type
	> inherited;
protected:
	int					_i,_j;
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
		_i						= goal_node_index / (columns + 2);
		_j						= goal_node_index % (columns + 2);
	}

	IC	_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2, const typename _Graph::const_iterator &i) const
	{
		VERIFY					(graph);
		return					(graph->get_edge_weight(node_index1,node_index2));
	}

	IC	_dist_type	estimate		(const _index_type node_index) const
	{
		VERIFY					(graph);
		int						i,j;
		i						= node_index / (columns + 2);
		j						= node_index % (columns + 2);
//		return					(_abs(_i - i - _j + j) + _abs(_i - i));
		return					(_abs(_j - j) + _abs(_i - i));
	}

	IC	bool		is_limit_reached(const _iteration_type	iteration_count) const
	{
		VERIFY					(data_storage);
		return					(false);
	}

	IC	bool		is_accessible	(const _index_type node_index) const
	{
		VERIFY					(graph);
		return					(graph->is_accessible(node_index));
	}

	IC	bool		is_metric_euclidian() const
	{
		return					(true);
	}
};
#endif

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelGraph,
		_DataStorage,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManagerBase <
			CLevelGraph,
			_DataStorage,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef typename CPathManagerBase <
				_Graph,
				_DataStorage,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
//	int					x1;
	float				y1;
//	int					z1;
	int					x2;
	float				y2;
	int					z2;
	int					x3;
	float				y3;
	int					z3;
	float				square_size_xz;
	float				square_size_y;
	float				m_sqr_distance_xz;

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
		square_size_xz			= graph->header().cell_size();
		square_size_y			= graph->header().factor_y();
		m_sqr_distance_xz		= _sqr(graph->header().cell_size());
	}

	IC	void		init			()
	{
		const _Graph::CVertex	&tNode1	= *graph->vertex(start_node_index);
		graph->unpack_xz		(tNode1,x2,z2);
		y2						= (float)(tNode1.position().y);
		
		const _Graph::CVertex	&tNode2	= *graph->vertex(goal_node_index);
		graph->unpack_xz		(tNode2,x3,z3);
		y3						= (float)(tNode2.position().y);
	}

	IC	_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2, const _Graph::const_iterator &i)
	{
		VERIFY					(graph);
		
		const _Graph::CVertex	&tNode1 = *graph->vertex(node_index2);

		y2						= (float)(tNode1.position().y);

		return					(_sqrt(square_size_y*(float)_sqr(y2 - y1) + m_sqr_distance_xz));
	}

	IC	_dist_type	estimate		(const _index_type node_index) const
	{
		VERIFY					(graph);
		return					(_sqrt((float)(square_size_xz*float(_sqr(x3 - x2) + _sqr(z3 - z2)) + square_size_y*(float)_sqr(y3 - y2))));
	}

	IC	bool		is_goal_reached	(const _index_type node_index)
	{
		if (node_index == goal_node_index)
			return				(true);
		
		_Graph::CVertex			&tNode0 = *graph->vertex(node_index);

		y1						= (float)(tNode0.position().y);

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

	IC	bool		is_metric_euclidian() const
	{
		return					(true);
	}
};

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CGameGraph,
		_DataStorage,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManagerBase <
			CGameGraph,
			_DataStorage,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CGameGraph _Graph;
	typedef typename CPathManagerBase <
		_Graph,
		_DataStorage,
		_dist_type,
		_index_type,
		_iteration_type
	> inherited;
protected:
	const _Graph::CVertex	*goal_vertex;
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
		goal_vertex				= &graph->vertex(goal_node_index);
	}

	IC	_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2, const _Graph::const_iterator &i) const
	{
		VERIFY					(graph);
		return					((*i).distance());
	}

	IC	_dist_type	estimate		(const _index_type node_index) const
	{
		VERIFY					(graph);
		return					(goal_vertex->game_point().distance_to(graph->vertex(node_index).game_point()));
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

	IC	bool		is_metric_euclidian() const
	{
		return					(true);
	}
};