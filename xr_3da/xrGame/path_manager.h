////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path managers
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_base.h"
#include "path_manager_params.h"
#include "game_graph.h"
#include "level_graph.h"
#ifdef AI_COMPILER
#include "test_table.h"
#else
#include "path_manager_level_selector.h"
#include "path_manager_game_selector.h"
#endif

template <
	typename _Graph,
	typename _DataStorage,
	typename _Parameters,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager : 
		public CPathManagerBase <
			_Graph,
			_DataStorage,
			_Parameters,
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
	typename _Parameters,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CTestTable<_cell_type,rows,columns>,
		_DataStorage,
		_Parameters,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManagerBase <
			CTestTable<_cell_type,rows,columns>,
			_DataStorage,
			_Parameters,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CTestTable<_cell_type,rows,columns> _Graph;
	typedef typename CPathManagerBase <
		_Graph,
		_DataStorage,
		_Parameters,
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
				const _Parameters		&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
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
		// Manhattan heuritics
		int						i,j;
		i						= node_index / (columns + 2);
		j						= node_index % (columns + 2);
		return					(_abs(_j - j) + _abs(_i - i));
	}

	IC	bool		is_limit_reached(const _iteration_type	iteration_count) const
	{
		VERIFY					(data_storage);
		return					(false);
	}

	IC	bool		is_metric_euclidian() const
	{
		return					(true);
	}
};
#endif

template <
	typename _DataStorage,
	typename _Parameters,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CGameGraph,
		_DataStorage,
		_Parameters,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManagerBase <
			CGameGraph,
			_DataStorage,
			_Parameters,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CGameGraph _Graph;
	typedef typename CPathManagerBase <
		_Graph,
		_DataStorage,
		_Parameters,
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
			const _Parameters		&parameters
		)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
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

	IC		bool		is_accessible	(const _index_type vertex_id) const
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
		CLevelGraph,
		_DataStorage,
		PathManagers::SBaseParameters<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManagerBase <
			CLevelGraph,
			_DataStorage,
			PathManagers::SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef PathManagers::SBaseParameters<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManagerBase <
				_Graph,
				_DataStorage,
				_Parameters,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	float				y1;
	int					x2;
	float				y2;
	int					z2;
	int					x3;
	float				y3;
	int					z3;
	float				square_size_y;
	float				m_sqr_distance_xz;
	_Graph::CVertex		*best_node;

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
				const _Parameters		&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
		);
		m_sqr_distance_xz		= _sqr(graph->header().cell_size());
		square_size_y			= _sqr((float)(graph->header().factor_y()/32767.0));;
	}

	IC	void		init			()
	{
		const _Graph::CVertex	&tNode1	= *graph->vertex(start_node_index);
		graph->unpack_xz		(tNode1,x2,z2);
		y2						= (float)(tNode1.position().y());
		
		const _Graph::CVertex	&tNode2	= *graph->vertex(goal_node_index);
		graph->unpack_xz		(tNode2,x3,z3);
		y3						= (float)(tNode2.position().y());
	}

	IC	_dist_type	evaluate		(const _index_type /**node_index1/**/, const _index_type node_index2, const _Graph::const_iterator &/**i/**/)
	{
		VERIFY					(graph);
		
		const _Graph::CVertex	&tNode1 = *graph->vertex(node_index2);

		y2						= (float)(tNode1.position().y());

		return					(_sqrt(square_size_y*(float)_sqr(y2 - y1) + m_sqr_distance_xz));
	}

	IC	_dist_type	estimate		(const _index_type /**node_index/**/) const
	{
		VERIFY					(graph);
		return					(_sqrt((float)(m_sqr_distance_xz*float(_sqr(x3 - x2) + _sqr(z3 - z2)) + square_size_y*(float)_sqr(y3 - y2))));
	}

	IC	bool		is_goal_reached	(const _index_type node_index)
	{
		if (node_index == goal_node_index)
			return				(true);
		
		best_node				= graph->vertex(node_index);
		y1						= (float)(best_node->position().y());

		return					(false);
	}

	IC	bool		is_limit_reached(const _iteration_type	iteration_count) const
	{
		VERIFY					(data_storage);
		return					(inherited::is_limit_reached(iteration_count));
	}

	IC	bool		is_accessible	(const _index_type vertex_id) const
	{
		VERIFY					(graph);
		return					(graph->valid_vertex_id(vertex_id));
	}

	IC	bool		is_metric_euclidian() const
	{
		return					(true);
	}

	IC		void		begin			(const _index_type /**vertex_id/**/, const_iterator &begin, const_iterator &end)
	{
		graph->begin			(best_node,begin,end);
	}

	IC		u32			get_value		(const_iterator &i) const
	{
		return					(graph->value(best_node,i));
	}
};

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelGraph,
		_DataStorage,
		PathManagers::SObstacleParams<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
			CLevelGraph,
			_DataStorage,
			PathManagers::SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef PathManagers::SObstacleParams<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				PathManagers::SBaseParameters<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	bool				m_avoid_dynamic_obstacles;

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
				const _Parameters		&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
		);
		m_avoid_dynamic_obstacles		= parameters.avoid_dynamic_obstacles;
	}

	IC	bool		is_accessible	(const _index_type vertex_id) const
	{
		VERIFY					(graph);
		return					(graph->valid_vertex_id(vertex_id) && (!m_avoid_dynamic_obstacles || graph->is_accessible(vertex_id)));
	}
};

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelGraph,
		_DataStorage,
		PathManagers::SObstaclesLightCover<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
			CLevelGraph,
			_DataStorage,
			PathManagers::SObstacleParams<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef PathManagers::SObstaclesLightCover<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				PathManagers::SObstacleParams<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	float				light_weight;
	float				cover_weight;
	float				distance_weight;

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
				const _Parameters		&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
		);
		light_weight			= parameters.light_weight;
		cover_weight			= parameters.cover_weight;
		distance_weight			= parameters.distance_weight;
	}

	IC	_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2, const _Graph::const_iterator &i)
	{
		VERIFY					(graph);

		const _Graph::CVertex	&tNode1 = *graph->vertex(node_index2);

		y2						= (float)(tNode1.position().y());

		float					cover = 1/(EPS_L + (float)(graph->cover(tNode1)[0])/255.f + (float)(graph->cover(tNode1)[1])/255.f + (float)(graph->cover(tNode1)[2])/255.f  + (float)(graph->cover(tNode1)[3])/255.f);
		float					light = (float)(tNode1.light())/15.f;
		return					(light_weight*light + cover_weight*cover + distance_weight*inherited::evaluate(node_index1,node_index2,i));
	}

	IC	_dist_type	estimate		(const _index_type node_index) const
	{
		VERIFY					(graph);
		return					(inherited::estimate(node_index));
	}
};

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelGraph,
		_DataStorage,
		PathManagers::SObstaclesLightCoverEnemy<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
			CLevelGraph,
			_DataStorage,
			PathManagers::SObstaclesLightCover<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef PathManagers::SObstaclesLightCoverEnemy<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				PathManagers::SObstaclesLightCover<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	Fvector						enemy_position;
	_dist_type					enemy_distance; 
	float						enemy_view_weight;

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
				const _Parameters		&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
		);
		enemy_position			= parameters.enemy_position;
		enemy_distance			= parameters.enemy_distance;
		enemy_view_weight		= parameters.enemy_view_weight;
	}

	IC	_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2, const _Graph::const_iterator &i)
	{
		VERIFY					(graph);
		return					(enemy_view_weight*_sqr(graph->distance(node_index2,enemy_position) - enemy_distance) + inherited::evaluate(node_index1,node_index2,i));
	}

	IC	_dist_type	estimate		(const _index_type node_index) const
	{
		VERIFY					(graph);
		return					(inherited::estimate(node_index));
	}
};

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelGraph,
		_DataStorage,
		PathManagers::SFlooder<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
			CLevelGraph,
			_DataStorage,
			PathManagers::SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef PathManagers::SFlooder<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				PathManagers::SBaseParameters<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
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
				const _Parameters		&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
		);
	}

	IC	bool		is_goal_reached	(const _index_type node_index)
	{
		VERIFY					(path);
		path->push_back			(node_index);
		_Graph::CVertex			&tNode0 = *graph->vertex(node_index);
		y1						= (float)(tNode0.position().y());
		return					(false);
	}

	IC	_dist_type	estimate		(const _index_type node_index) const
	{
		VERIFY					(graph);
		return					(_dist_type(0));
	}

	IC		void		create_path		()
	{
	}
};

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelGraph,
		_DataStorage,
		PathManagers::SPosition<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
			CLevelGraph,
			_DataStorage,
			PathManagers::SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef PathManagers::SPosition<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				PathManagers::SBaseParameters<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	_Parameters			*m_params;

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
				_Parameters				&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_start_node_index,
			parameters
		);
		m_params		= &parameters;
	}

	IC	bool		is_goal_reached	(const _index_type node_index)
	{
		if (!graph->inside(node_index,m_params->m_position)) {
			float						distance = graph->distance(node_index,m_params->m_position);
			if (distance < m_params->m_distance) {
				m_params->m_distance	= distance;
				m_params->m_vertex_id	= node_index;
			}
			best_node					= graph->vertex(node_index);
			y1							= (float)(best_node->position().y());
			return						(false);
		}

		if ((_abs(m_params->m_position.y - ai().level_graph().vertex_plane_y(node_index,m_params->m_position.x,m_params->m_position.z)) >= m_params->m_epsilon)) {
			best_node					= graph->vertex(node_index);
			y1							= (float)(best_node->position().y());
			return						(false);
		}
		else {
			m_params->m_distance		= 0.f;
			m_params->m_vertex_id		= node_index;
			return						(true);
		}
	}
	
	IC		void		create_path		()
	{
	}
};

#ifndef AI_COMPILER
template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelGraph,
		_DataStorage,
		PathManagers::CAbstractVertexEvaluator,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
			CLevelGraph,
			_DataStorage,
			PathManagers::SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef PathManagers::CAbstractVertexEvaluator _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				PathManagers::SBaseParameters<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	_Parameters		*m_evaluator;

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
				_Parameters				&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
		);
		m_evaluator				= &parameters;
		m_evaluator->max_range	= parameters.m_fSearchRange;
	}

	IC	_dist_type	estimate		(const _index_type node_index) const
	{
		return					(_dist_type(0));
	}


	IC	bool		is_goal_reached	(const _index_type node_index)
	{
		VERIFY					(m_evaluator);
		m_evaluator->m_tpCurrentNode = graph->vertex(node_index);
		m_evaluator->m_fDistance	 = data_storage->get_best().g();
		float					value = m_evaluator->ffEvaluate();
		if (value > m_evaluator->m_fResult) {
			m_evaluator->m_fResult		= value;
			m_evaluator->m_dwBestNode	= node_index;
		}

		_Graph::CVertex			&tNode0 = *graph->vertex(node_index);
		y1						= (float)(tNode0.position().y());

		return					(false);
	}

	IC		void		create_path		()
	{
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
		PathManagers::SVertexType<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
			CGameGraph,
			_DataStorage,
			PathManagers::SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CGameGraph _Graph;
	typedef PathManagers::SVertexType<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				PathManagers::SBaseParameters<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	_Parameters		*m_evaluator;

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
				_Parameters				&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
		);
		m_evaluator		= &parameters;
		m_evaluator->m_vertex_id = _index_type(-1);
	}

	IC	_dist_type	estimate		(const _index_type node_index) const
	{
		return					(_dist_type(0));
	}

	IC	bool		is_goal_reached	(const _index_type node_index)
	{
		VERIFY					(m_evaluator);
		if (graph->mask(m_evaluator->m_vertex_types,graph->vertex(data_storage->get_best().index()).vertex_type())) {
			m_evaluator->m_vertex_id	= data_storage->get_best().index();
			return				(true);
		}
		return					(false);
	}
	
	IC		void		create_path		()
	{
	}
};
#else
template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelGraph,
		_DataStorage,
		PathManagers::SStraightLineParams<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
			CLevelGraph,
			_DataStorage,
			PathManagers::SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
	typedef CLevelGraph _Graph;
	typedef PathManagers::SStraightLineParams<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				PathManagers::SBaseParameters<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	_Parameters			*m_parameters;

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
				_Parameters				&parameters
			)
	{
		inherited::setup(
			_graph,
			_data_storage,
			_path,
			_start_node_index,
			_goal_node_index,
			parameters
		);
		m_parameters				= &parameters;
		m_parameters->m_distance	= m_parameters->max_range;
	}

	IC		void		create_path		()
	{
		inherited::create_path				();

		_dist_type							fCumulativeDistance = 0, fLastDirectDistance = 0, fDirectDistance;

		Fvector								tPosition = m_parameters->m_start_point;
		
		xr_vector<_index_type>::iterator	I = path->begin();
		xr_vector<_index_type>::iterator	E = path->end();
		_index_type							dwNode = *I;
		for ( ++I; I != E; ++I) {
			fDirectDistance					= graph->check_position_in_direction(dwNode,tPosition,graph->vertex_position(*I),m_parameters->max_range);
			if (fDirectDistance == m_parameters->max_range) {
				if (fLastDirectDistance == 0) {
					fCumulativeDistance		+= graph->distance(dwNode,*I);
					dwNode = *I;
				}
				else {
					fCumulativeDistance		+= fLastDirectDistance;
					fLastDirectDistance		= 0;
					dwNode					= *--I;
				}
				tPosition					= graph->vertex_position(dwNode);
			}
			else 
				fLastDirectDistance			= fDirectDistance;
			if (fCumulativeDistance + fLastDirectDistance >= m_parameters->max_range) {
				m_parameters->m_distance	= m_parameters->max_range;
				return;
			}
		}

		fDirectDistance						= graph->check_position_in_direction(dwNode,tPosition,m_parameters->m_dest_point,m_parameters->max_range);
		if (fDirectDistance == m_parameters->max_range)
			m_parameters->m_distance		= fCumulativeDistance + fLastDirectDistance + m_parameters->m_dest_point.distance_to(graph->vertex_position((*path)[path->size() - 1]));
		else
			m_parameters->m_distance		= fCumulativeDistance + fDirectDistance;
	}
};
#endif
