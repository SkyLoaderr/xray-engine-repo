////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_engine.h
//	Created 	: 21.03.2002
//  Modified 	: 26.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Graph engine
////////////////////////////////////////////////////////////////////////////

#pragma once

//		algorithms
#include "a_star.h"
#include "dijkstra.h"
//		path builders
#include "edge_path.h"
//		vertex managers
#include "vertex_manager_generic.h"
#include "vertex_manager_fixed.h"
#include "vertex_manager_hash_fixed.h"
//		allocators
#include "vertex_allocator_generic.h"
#include "vertex_allocator_fixed.h"
//		priority queues
#include "data_storage_bucket_list.h"
#include "data_storage_cheap_list.h"
#include "data_storage_single_linked_list.h"
#include "data_storage_double_linked_list.h"
#include "boost/f_heap.hpp"
#include "boost/p_heap.hpp"
#include "data_storage_priority_queue.h"
#include "data_storage_binary_heap.h"
#include "data_storage_binary_heap_list.h"
#include "data_storage_multi_binary_heap.h"
//		path_managers
#include "path_manager.h"

class CGraphEngine {
protected:
	// distance type definition
	typedef float _dist_type;
	typedef u32	  _index_type;
	typedef u32	  _iteration_type;

	typedef CProblemSolver<u32,u32,u32>		CSProblemSolver;
	typedef u16								_solver_dist_type;
	typedef CSProblemSolver::_index_type	_solver_index_type;
	typedef CSProblemSolver::_edge_type		_solver_edge_type;

public:
	typedef SBaseParameters<_dist_type,_index_type,_iteration_type>				CBaseParameters;
	typedef SFlooder<_dist_type,_index_type,_iteration_type>					CFlooder;
	typedef SObstacleParams<_dist_type,_index_type,_iteration_type>				CObstacleParams;
	typedef SObstaclesLightCover<_dist_type,_index_type,_iteration_type>		CObstaclesLightCover;
	typedef SObstaclesLightCoverEnemy<_dist_type,_index_type,_iteration_type>	CObstaclesLightCoverEnemy;
	typedef SPosition<_dist_type,_index_type,_iteration_type>					CPositionParameters;
	typedef SStraightLineParams<_dist_type,_index_type,_iteration_type>			CStraightLineParams;

protected:
//	typedef CDataStorageSingleLinkedList<false>				CPriorityQueue;
//	typedef CDataStorageSingleLinkedList<true>				CPriorityQueue;
//	typedef CDataStorageDoubleLinkedList<false>				CPriorityQueue;
//	typedef CDataStorageDoubleLinkedList<true>				CPriorityQueue;
//	typedef CPriorityQueue<boost::fibonacci_heap>			CSolverPriorityQueue;
//	typedef CDataStorageMultiBinaryHeap<4>					CSolverPriorityQueue;
//	typedef CPriorityQueue<boost::pairing_heap>				CSolverPriorityQueue;
//	typedef CDataStorageBucketList<u32,u8,16,false>			CSolverPriorityQueue;
//	typedef CDataStorageBinaryHeapList<4>					CSolverPriorityQueue;
//	typedef CDataStorageCheapList<32,true,true>				CSolverPriorityQueue;
//	typedef CDataStorageDoubleLinkedList<true>				CSolverPriorityQueue;
	typedef CDataStorageBinaryHeap							CSolverPriorityQueue;
	typedef CDataStorageBucketList<u32,u32,8*1024,false>	CPriorityQueue;
//	typedef CDataStorageBinaryHeap							CPriorityQueue;
//	typedef CDataStorageBinaryHeapList<4>					CPriorityQueue;
//	typedef CDataStorageMultiBinaryHeap<4>					CPriorityQueue;
//	typedef CPriorityQueue<boost::fibonacci_heap>			CPriorityQueue;
//	typedef CPriorityQueue<boost::pairing_heap>				CPriorityQueue;
	
	typedef CVertexManagerFixed<u32,u32,8>					CVertexManager;
#ifndef USE_HASH
	typedef CVertexManagerGeneric<u32,_solver_index_type>	CSolverVertexManager;
#else
	typedef CVertexManagerHashFixed<
				u32,
				_solver_index_type,
				4096,
				16*65536
			>												CSolverVertexManager;
#endif
	typedef CVertexAllocatorFixed<65536>					CVertexAllocator;
	typedef CVertexAllocatorFixed<16*65536>					CSolverVertexAllocator;

	typedef CAStar<
		_dist_type,
		CPriorityQueue,
		CVertexManager,
		CVertexAllocator
	>														CAlgorithm;
	typedef CDijkstra<
		_dist_type,
		CSolverPriorityQueue,
		CSolverVertexManager,
		CSolverVertexAllocator,
		true,
		CEdgePath<
			_solver_edge_type,
			true
		>
	>														CSolverAlgorithm;

	CAlgorithm				*m_algorithm;
	CSolverAlgorithm		*m_solver_algorithm;


public:

	IC				CGraphEngine			(u32 max_vertex_count);
	virtual			~CGraphEngine			();
	IC		const CSolverAlgorithm &solver_algorithm() const;

	template <
		typename _Graph,
		typename _Parameters
	>
	IC		bool	search					(
				const _Graph			&graph, 
				const _index_type		&start_node, 
				const _index_type		&dest_node, 
				xr_vector<_index_type>	*node_path,
				const _Parameters		&parameters
			);

	template <
		typename _Graph,
		typename _Parameters
	>
	IC		bool	search					(
				const _Graph			&graph, 
				const _index_type		&start_node, 
				const _index_type		&dest_node, 
				xr_vector<_index_type>	*node_path,
				_Parameters				&parameters
			);

	template <typename _Parameters>
	IC		bool	search					(
		const CSProblemSolver			&graph, 
		const _solver_index_type		&start_node,
		const _solver_index_type		&dest_node,
		xr_vector<_solver_edge_type>	*node_path,
		const _Parameters				&parameters
	);
};

#include "graph_engine_inline.h"