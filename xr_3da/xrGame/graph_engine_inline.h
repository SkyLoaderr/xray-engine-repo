////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_engine_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Graph engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CGraphEngine::CGraphEngine		(u32 max_vertex_count)
{
	m_algorithm			= xr_new<CAlgorithm>				(max_vertex_count);
	m_algorithm->data_storage().set_min_bucket_value		(_dist_type(0));
	m_algorithm->data_storage().set_max_bucket_value		(_dist_type(2000));
	m_solver_algorithm	= xr_new<CSolverAlgorithm>			(16*1024);
}

IC	CGraphEngine::~CGraphEngine			()
{
	xr_delete			(m_algorithm);
	xr_delete			(m_solver_algorithm);
}

IC	const CGraphEngine::CSolverAlgorithm &CGraphEngine::solver_algorithm() const
{
	return				(*m_solver_algorithm);
}

template <
	typename _Graph,
	typename _Parameters
>
IC	bool CGraphEngine::search		(
		const _Graph			&graph, 
		const _index_type		&start_node, 
		const _index_type		&dest_node, 
		xr_vector<_index_type>	*node_path,
		const _Parameters		&parameters
	)
{
#ifndef AI_COMPILER
	Device.Statistic.AI_Path.Begin();
#endif
	typedef CPathManager<_Graph, CAlgorithm::CDataStorage, _Parameters, _dist_type,_index_type,_iteration_type>	CPathManagerGeneric;

	CPathManagerGeneric			path_manager;

	path_manager.setup			(
		&graph,
		&m_algorithm->data_storage(),
		node_path,
		start_node,
		dest_node,
		parameters
	);
	
	bool						successfull = m_algorithm->find(path_manager);

#ifndef AI_COMPILER
	Device.Statistic.AI_Path.End();
#endif
	return						(successfull);
}

template <
	typename _Graph,
	typename _Parameters
>
IC	bool CGraphEngine::search			(
		const _Graph			&graph, 
		const _index_type		&start_node, 
		const _index_type		&dest_node, 
		xr_vector<_index_type>	*node_path,
		_Parameters				&parameters
	)
{
#ifndef AI_COMPILER
	Device.Statistic.AI_Path.Begin();
#endif
	typedef CPathManager<_Graph, CAlgorithm::CDataStorage, _Parameters, _dist_type,_index_type,_iteration_type>	CPathManagerGeneric;

	CPathManagerGeneric			path_manager;

	path_manager.setup			(
		&graph,
		&m_algorithm->data_storage(),
		node_path,
		start_node,
		dest_node,
		parameters
	);
	
	bool						successfull = m_algorithm->find(path_manager);

#ifndef AI_COMPILER
	Device.Statistic.AI_Path.End();
#endif
	return						(successfull);
}

//template <
//	typename _condition_type,
//	typename _value_type,
//	typename _operator_id_type,
//	typename _Parameters
//>
//IC	bool CGraphEngine::search			(
//		const CProblemSolver<
//			_condition_type,
//			_value_type,
//			_operator_id_type
//		>						&graph, 
//		const typename CProblemSolver<
//			_condition_type,
//			_value_type,
//			_operator_id_type
//		>::_index_type			&start_node,
//		const typename CProblemSolver<
//			_condition_type,
//			_value_type,
//			_operator_id_type
//		>::_index_type			&dest_node, 
//		xr_vector<
//			typename CProblemSolver<
//				_condition_type,
//				_value_type,
//				_operator_id_type
//			>::_edge_type
//		>						*node_path,
//		const _Parameters		&parameters
//	)
//{
//#ifndef AI_COMPILER
//	Device.Statistic.AI_Path.Begin();
//#endif
//	typedef CProblemSolver<
//		_condition_type,
//		_value_type,
//		_operator_id_type
//	>										CSProblemSolver;
//	typedef u8								_dist_type;
//	typedef CSProblemSolver::_index_type	_index_type;
//	typedef CSProblemSolver::_edge_type		_edge_type;
//	typedef CDataStorageCheapList<
//		32,
//		true,
//		true
//	>										CPriorityQueue;
//	typedef CDataStorageVertexManager<
//		u32,
//		_index_type
//	>										CVertexManager;
//	typedef CDataStorageAllocator			CVertexAllocator;
//	typedef CAStar<
//		_dist_type,
//		CPriorityQueue,
//		CVertexManager,
//		CVertexAllocator,
//		true,
//		CEdgePath<
//			_edge_type,
//			true
//		>
//	>										CAlgorithm;
//	typedef CPathManager<
//		CSProblemSolver,
//		CAlgorithm::CDataStorage,
//		_Parameters,
//		_dist_type,
//		_index_type,
//		_iteration_type
//	>										CPathManagerGeneric;
//
//	CAlgorithm					algorithm(u32(-1));
//	CPathManagerGeneric			path_manager;
//
//	path_manager.setup			(
//		&graph,
//		&algorithm.data_storage(),
//		node_path,
//		start_node,
//		dest_node,
//		parameters
//	);
//	
//	bool						successfull = algorithm.find(path_manager);
//
//#ifndef AI_COMPILER
//	Device.Statistic.AI_Path.End();
//#endif
//	return						(successfull);
//}
//
template <
	typename T1,
	typename T2,
	typename T3,
	typename T4,
	typename T5,
	bool	 T6,
	typename T7,
	typename T8,
	typename _Parameters
>
IC	bool CGraphEngine::search(
		const CProblemSolver<
			T1,
			T2,
			T3,
			T4,
			T5,
			T6,
			T7,
			T8
		>								&graph, 
		const _solver_index_type		&start_node,
		const _solver_index_type		&dest_node, 
		xr_vector<_solver_edge_type>	*node_path,
		const _Parameters				&parameters
	)
{
#ifndef AI_COMPILER
	Device.Statistic.AI_Path.Begin();
#endif
	typedef CProblemSolver<T1,T2,T3,T4,T5,T6,T7,T8>	CSProblemSolver;
	typedef CPathManager<CSProblemSolver,CSolverAlgorithm::CDataStorage,_Parameters,_solver_dist_type,_solver_index_type,_iteration_type>	CSolverPathManager;

	CSolverPathManager			path_manager;

	path_manager.setup			(
		&graph,
		&m_solver_algorithm->data_storage(),
		node_path,
		start_node,
		dest_node,
		parameters
	);
	
	bool						successfull = m_solver_algorithm->find(path_manager);

#ifndef AI_COMPILER
	Device.Statistic.AI_Path.End();
#endif
	return						(successfull);
}
