////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_priority_queue.h
//	Created 	: 21.03.2002
//  Modified 	: 22.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Priority queue data storages
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "data_storage_base.h"

template <
	template	<typename CGraphNode, typename _ComparePredicate>	class _PriorityQueue,
//	template	<typename CGraphNode, typename _ComparePredicate, int _Depth=2>	class _PriorityQueue,
//	int			depth,
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>
class CDataStoragePriorityQueue
{
protected:	
	#pragma pack(push,4)
	template <
		typename _dist_type, 
		typename _index_type, 
		u8 index_bits, 
		u8 mask_bits
	>
	struct SGraphNode : 
		DataStorageBaseIndex::SGraphNode <
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>
	{
		SGraphNode		*back;
	};
	#pragma pack(pop)

public:
	typedef SGraphNode <
		_dist_type,
		_index_type,
		index_bits,
		mask_bits
	> CGraphNode;
	
	class CGraphNodePredicate {
	public:
		IC			bool	operator()(const CGraphNode *node1, const CGraphNode *node2) const
		{
			return				(node1->_f > node2->_f);
		};
	};

	typedef _PriorityQueue<CGraphNode*,CGraphNodePredicate> _priority_queue;
//	typedef _PriorityQueue<CGraphNode*,CGraphNodePredicate,depth> _priority_queue;
protected:

	#pragma pack(push,4)
	struct SGraphIndexNode {
		typename _priority_queue::pointer	pq_node;
		CGraphNode							*node;
		_path_id_type						path_id;
	};
	#pragma pack(pop)
	

	_index_type			max_node_count;
	_path_id_type		cur_path_id;
	SGraphIndexNode		*indexes;
	_index_type			node_count;
	CGraphNode			*nodes;
	_priority_queue		priority_queue;
public:	

						CDataStoragePriorityQueue(const _index_type node_count)
	{
		max_node_count			= node_count;
		cur_path_id				= _path_id_type(0);

		u32						memory_usage = 0;
		u32						byte_count;

		byte_count				= (node_count)*sizeof(CGraphNode);
		nodes					= (CGraphNode*)xr_malloc(byte_count);
		ZeroMemory				(nodes,byte_count);
		memory_usage			+= byte_count;

		byte_count				= (node_count)*sizeof(SGraphIndexNode);
		indexes					= (SGraphIndexNode*)xr_malloc(byte_count);
		ZeroMemory				(indexes,byte_count);
		memory_usage			+= byte_count;

//		byte_count				= (node_count)*sizeof(_priority_queue::node);
//		priority_queue.reserved = (_priority_queue::node*)xr_malloc(byte_count);
//		memory_usage			+= byte_count;

//		Msg						("* Data storage allocated %d bytes of memory",memory_usage);
	}

	virtual				~CDataStoragePriorityQueue()
	{
		xr_free					(indexes);
		xr_free					(nodes);
	}

	IC		void		init			()
	{
		cur_path_id++;
		node_count				= _index_type(0);
		while (!priority_queue.empty())
			priority_queue.pop();
//		priority_queue.clear	();
	}

	IC		bool		is_opened_empty	() const
	{
		return					(priority_queue.empty());
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		indexes[node.index()].pq_node = priority_queue.push(&node);
	}

	IC		CGraphNode	&create_node	(const _index_type node_index)
	{
		VERIFY					(node_index < max_node_count);
		CGraphNode				*node = indexes[node_index].node = nodes + node_count++;
		indexes[node_index].path_id = cur_path_id;
		node->_index			= node_index;
		return					(*node);
	}

	IC		CGraphNode	&get_node		(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		VERIFY					(is_visited(node_index));
		return					(*indexes[node_index].node);
	}

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
	{
		VERIFY					(!is_opened_empty());
		VERIFY					(is_opened(node));
//		priority_queue.remove	(indexes[node.index()].pq_node);
//		indexes[node.index()].pq_node = priority_queue.push(&node);
		priority_queue.increase	(indexes[node.index()].pq_node,&node);
	}
	

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		priority_queue.pop		();
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		priority_queue.top()->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		() const
	{
		VERIFY					(!is_opened_empty());
		return					(*priority_queue.top());
	}

	IC		bool		is_opened		(const CGraphNode &node) const
	{
		return					(!!node.open_close_mask);
	}

	IC		bool		is_visited(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		return					(indexes[node_index].path_id == cur_path_id);
	}

	IC		bool		is_closed		(const CGraphNode &node) const
	{
		return					(is_visited(node) && !is_opened(node));
	}

	IC		const _index_type	get_visited_node_count() const
	{
		return					(node_count);
	}

	IC		void		assign_parent	(CGraphNode	&neighbour, CGraphNode *parent)
	{
		neighbour.back			= parent;
	}

	IC		void		update_successors(CGraphNode &tpNeighbour)
	{
		VERIFY					(false);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		CGraphNode				*best = &get_best(), *t1 = best, *t2 = best->back;
		for (_index_type i=1; t2; t1 = t2, t2 = t2->back, i++) ;

		path.resize				(i);

		t1						= best;
		path[--i]				= best->index();
		t2						= t1->back;

		xr_vector<_index_type>::reverse_iterator	I = path.rbegin();
		xr_vector<_index_type>::reverse_iterator	E = path.rend();
		for (++I ; t2; t2 = t2->back, ++I)
			*I = t2->index();
	}
};

template <typename keyType, typename greater = std::greater<keyType> >
class PQi
{
	int							d,N;
	std::vector<int>			pq,	qp;
	const std::vector<keyType>	&a;

	IC		void		exch			(int i, int j)
	{
		int						t = pq[i];
		pq[i]					= pq[j];
		pq[j]					= t;
		qp[pq[i]]				= i;
		qp[pq[j]]				= j;
	}

	IC		void		fixUp			(int k)
	{
		while ((k > 1) && (greater(a[pq[(k + d - 2)/d]],a[pq[k]]))) {
			exch				(k,(k + d - 2)/d);
			k					= (k + d - 2)/d;
		}
	}

	IC		void		fixDown			(int k, int N)
	{
		int						j;
		while ((j = d*(k - 1) + 2) <= N) {
			for (int i = j + 1; (i<j + d) && (i <= N); ++i)
				if (greater(a[pq[j]],a[pq[i]]))
					j = i;
			if (!greater(a[pq[k]],a[pq[j]]))
				break;
			exch				(k,j);
			k					= j;
		}
	}

public:
	PQi				(int N, const std::vector<keyType> &a, int d = 3) :
							a(a), pq(N + 1,0), qp(N + 1,0), N(0), d(d)
	{
	}

	IC		int			empty			() const
	{
		return				(N == 0);
	}

	IC		void		push			(int v)
	{
		pq[++N]				= v;
		qp[v]				= N;
		fixUp				(N);
	}

	IC		int			top				()
	{
		exch				(1,N);
		fixDown				(1,N - 1);
		return				(pq[N--]);
	}

	IC		void		change			(int k)
	{
		fixUp				(qp[k]);
	}
};