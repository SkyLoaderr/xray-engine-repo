////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path managers
////////////////////////////////////////////////////////////////////////////

#pragma once

//#include "path_manager_base.h"

//template <
//	typename _Graph,
//	typename _DataStorage,
//	typename _dist_type			= float,
//	typename _index_type		= u32,
//	typename _iteration_type	= u32
//>
//class CPathManager : 
//	public	CPathManagerBase<
//		_Graph,
//		_DataStorage,
//		_dist_type,
//		_index_type,
//		_iteration_type
//	>
//{
//};
//
//template <
//	typename _DataStorage,
//	typename _dist_type,
//	typename _index_type,
//	typename _iteration_type
//>	
//	class CPathManager<
//			CAI_Map,
//			_DataStorage,
//			_dist_type,
//			_index_type,
//			_iteration_type
//		> :
//	public CPathManagerBase <
//		CAI_Map,
//		_DataStorage,
//		_dist_type,
//		_index_type,
//		_iteration_type
//	>
//{
//protected:
//	typedef CAI_Map _Graph;
//	typedef CPathManagerBase <
//		_Graph,
//		_DataStorage,
//		_dist_type,
//		_index_type,
//		_iteration_type
//	> inherited;
//	float				x1;
//	float				y1;
//	float				z1;
//	float				x2;
//	float				y2;
//	float				z2;
//	float				x3;
//	float				y3;
//	float				z3;
//	float				m_fSize2;
//	float				m_fYSize2;
//	u32					m_dwLastBestNode;
//public:
//	
//	virtual				~CPathManager	()
//	{
//	}
//
//	virtual	void		setup			(
//				const _Graph			*_graph,
//				_DataStorage			*_data_storage,
//				xr_vector<_index_type>	*_path,
//				const _index_type		_start_node_index,
//				const _index_type		_goal_node_index,
//				const _index_type		_max_visited_node_count	= _index_type(u32(-1)),
//				const _dist_type		_max_range				= _dist_type(6000),
//				const _iteration_type	_max_iteration_count	= _iteration_type(u32(-1))
//			)
//	{
//		inherited::setup(
//			_graph,
//			_data_storage,
//			_path,
//			_start_node_index,
//			_goal_node_index,
//			_max_visited_node_count,
//			_max_range,
//			_max_iteration_count
//		);
//		m_fSize2				= graph->m_fSize2;
//		m_fYSize2				= graph->m_fYSize2;
//	}
//
//	virtual void		init			()
//	{
//		inherited::init			();
//		m_dwLastBestNode		= u32(-1);
//		{
//			_Graph::InternalNode&tNode1	= *graph->Node(start_node_index);
//			x2					= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
//			y2					= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
//			z2					= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
//		}
//		{
//			_Graph::InternalNode&tNode1	= *graph->Node(goal_node_index);
//			x3					= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
//			y3					= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
//			z3					= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
//		}
//	}
//
////	virtual	bool		is_goal_reached	(const _index_type node_index)
////	{
////		if (inherited::is_goal_reached(node_index))
////			return				(true);
////
////		_Graph::InternalNode	&tNode0 = *graph->Node(node_index);
////
////		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
////		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
////		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
////
////		return					(false);
////	}
//
//	virtual	_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2)
//	{
//		if (m_dwLastBestNode != node_index1) {
//			m_dwLastBestNode = node_index1;
//			_Graph::InternalNode &tNode0 = *graph->Node(node_index1), &tNode1 = *graph->Node(node_index2);
//
//			x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
//			y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
//			z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
//
//			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
//			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
//			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
//
//			return(_sqrt((float)(m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + m_fYSize2*_sqr(y2 - y1))));
//		}
//		else {
//			_Graph::InternalNode &tNode1 = *graph->Node(node_index2);
//
//			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
//			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
//			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
//
//			return(_sqrt((float)(m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + m_fYSize2*_sqr(y2 - y1))));
//		}
//	}
//
//	virtual	_dist_type	estimate		(const _index_type node_index)
//	{
//		return(_sqrt((float)(m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + m_fYSize2*_sqr(y3 - y2))));
//	}
//
//	virtual	void		create_path		()
//	{
//		inherited::create_path	();
//	}
//	
//	virtual	bool		is_limit_reached(const _iteration_type	iteration_count) const
//	{
//		return					(false);
//	}
//
//	virtual	bool		is_accessible	(const _index_type node_index) const
//	{
//		return					(true);
//	}
//
//	virtual	bool		is_metric_euclidian()
//	{
//		return					(true);
//	}
//};
//
template <
	typename _DataStorage,
	typename _dist_type = float,
	typename _index_type = u32,
	typename _iteration_type = u32
> class CPathManager1 
{
	typedef CAI_Map _Graph;
protected:
	const _Graph		*graph;
	_DataStorage		*data_storage; 
	xr_vector<_index_type>	*path;
	_index_type			start_node_index;
	_index_type			goal_node_index;
	_dist_type			max_range;
	_iteration_type		max_iteration_count;
	_index_type			max_visited_node_count;
	float				x1;
	float				y1;
	float				z1;
	float				x2;
	float				y2;
	float				z2;
	float				x3;
	float				y3;
	float				z3;
	float				m_fSize2;
	float				m_fYSize2;
	u32					m_dwLastBestNode;
public:

						CPathManager1	(
				const _Graph			*_graph,
				_DataStorage			*_data_storage
							)
	{
		graph					= _graph;
		data_storage			= _data_storage;
		m_fSize2				= graph->m_fSize2;
		m_fYSize2				= graph->m_fYSize2;
	}

	virtual				~CPathManager1	()
	{
	}

	virtual	void		init			()
	{
		m_dwLastBestNode		= u32(-1);
		{
			_Graph::InternalNode&tNode1	= *graph->Node(start_node_index);
			x2					= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2					= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2					= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
		}
		{
			_Graph::InternalNode&tNode1	= *graph->Node(goal_node_index);
			x3					= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y3					= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z3					= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
		}
	}

	virtual	void		setup			(
				xr_vector<_index_type>	*_path,
				const _index_type		_start_node_index,
				const _index_type		_goal_node_index,
				const _index_type		_max_visited_node_count	= _index_type(u32(-1)),
				const _dist_type		_max_range				= _dist_type(6000),
				const _iteration_type	_max_iteration_count	= _iteration_type(u32(-1))
			)
	{
		path					= _path;
		start_node_index		= _start_node_index;
		goal_node_index			= _goal_node_index;
		max_visited_node_count	= _max_visited_node_count;
		max_range				= _max_range;
		max_iteration_count		= _max_iteration_count;
	}

	virtual	_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2)
	{
		VERIFY					(graph);
		if (m_dwLastBestNode != node_index1) {
			m_dwLastBestNode = node_index1;
			_Graph::InternalNode &tNode0 = *graph->Node(node_index1), &tNode1 = *graph->Node(node_index2);

			x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
			y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
			z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);

			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			return(_sqrt((float)(m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + m_fYSize2*_sqr(y2 - y1))));
		}
		else {
			_Graph::InternalNode &tNode1 = *graph->Node(node_index2);

			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			return(_sqrt((float)(m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + m_fYSize2*_sqr(y2 - y1))));
		}
	}

	virtual	_dist_type	estimate		(const _index_type node_index)
	{
		VERIFY					(graph);
		return					(_sqrt((float)(m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + m_fYSize2*_sqr(y3 - y2))));
	}

	virtual	void		create_path		()
	{
		VERIFY					(data_storage && path);
		data_storage->get_path	(*path);
	}

	virtual	_index_type	start_node		()
	{
		return					(start_node_index);
	}

	virtual	bool		is_goal_reached	(const _index_type node_index) const
	{
		return					(node_index == goal_node_index);
	}

	virtual	bool		is_limit_reached(const _iteration_type	iteration_count) const
	{
		VERIFY					(data_storage);
		return					(false);
	}

	virtual	bool		is_accessible	(const _index_type node_index) const
	{
		VERIFY					(graph);
		return					(true);
	}

	virtual	bool		is_metric_euclidian()
	{
		return					(true);
	}
};
