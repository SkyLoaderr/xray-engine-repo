////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_solver_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Solver path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename T1,\
		typename T2,\
		typename T3,\
		typename _DataStorage,\
		typename _Parameters,\
		typename _dist_type,\
		typename _index_type,\
		typename _iteration_type\
	>

#define CSolverPathManager CPathManager<CProblemSolver<T1,T2,T3>,_DataStorage,_Parameters,_dist_type,_index_type,_iteration_type>

TEMPLATE_SPECIALIZATION
IC	CSolverPathManager::~CSolverPathManager			()
{
}

TEMPLATE_SPECIALIZATION
IC	void CSolverPathManager::setup					(
		const _Graph			*_graph,
		_DataStorage			*_data_storage,
		xr_vector<_edge_type>	*_path,
		const _index_type		&_start_node_index,
		const _index_type		&_goal_node_index,
		const _Parameters		&params
	)
{
	graph					= _graph;
	data_storage			= _data_storage;
	m_edge_path				= _path;
	start_node_index		= _start_node_index;
	goal_node_index			= _goal_node_index;
	max_visited_node_count	= params.max_visited_node_count;
	max_range				= params.max_range;
	max_iteration_count		= params.max_iteration_count;
}

TEMPLATE_SPECIALIZATION
IC	bool CSolverPathManager::is_goal_reached		(const _index_type &vertex_id) const
{
#ifdef INTENSIVE_MEMORY_USAGE
	#ifndef REVERSE_SEARCH
		return				(vertex_id.includes(goal_node_index));
	#else
		return				(goal_node_index.includes(vertex_id));
	#endif
#else
	#ifndef REVERSE_SEARCH
		return				(vertex_id.includes(goal_node_index,graph->current_state()));
	#else
		return				(goal_node_index.includes(vertex_id));
	#endif
#endif
}

TEMPLATE_SPECIALIZATION
IC	const _index_type &CSolverPathManager::get_value(const_iterator &i) const
{
	return					(graph->value(*best_node_index,i));
}

TEMPLATE_SPECIALIZATION
IC	const typename CSolverPathManager::_edge_type &CSolverPathManager::edge		(const_iterator &i) const
{
	return					((*i).m_operator_id);
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CSolverPathManager::evaluate			(const _index_type &node_index1, const _index_type &node_index2, const const_iterator &i) const
{
	VERIFY					(graph);
	return					(1);//graph->get_edge_weight(node_index1,node_index2,i));
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CSolverPathManager::estimate			(const _index_type &vertex_id) const
{
	VERIFY					(graph);
#ifndef REVERSE_SEARCH
	return					(graph->get_edge_weight(vertex_id,goal_node_index,m_iterator));
#else
	return					(graph->get_edge_weight(vertex_id,start_node_index,m_iterator));
#endif
}

TEMPLATE_SPECIALIZATION
template <typename T>
IC	void CSolverPathManager::create_path			(T &vertex)
{
	VERIFY					(data_storage);
	if (m_edge_path) {
#ifndef REVERSE_SEARCH
		data_storage->get_edge_path	(*m_edge_path,&vertex);
#else
		data_storage->get_edge_path	(*m_edge_path,&vertex,true);
#endif
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CSolverPathManager