////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_heap.h
//	Created 	: 21.03.2002
//  Modified 	: 20.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Heap data storages
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "data_storage_base.h"

namespace DataStorageHeap {
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
};

template <
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	
class CDataStorageBinaryHeap :
	public CDataStorageBaseIndexBlock <
		DataStorageHeap::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		_dist_type,
		_index_type,
		_path_id_type,
		bEuclidianHeuristics,
		index_bits,
		mask_bits
	>
{
public:
	typedef DataStorageHeap::SGraphNode<
		_dist_type,
		_index_type,
		index_bits,
		mask_bits
	> CGraphNode;
protected:	
	typedef CDataStorageBaseIndexBlock <
		CGraphNode,
		_dist_type,
		_index_type,
		_path_id_type,
		bEuclidianHeuristics,
		index_bits,
		mask_bits
	> inherited;

	class CGraphNodePredicate {
	public:
		IC			bool	operator()(CGraphNode *node1, CGraphNode *node2)
		{
			return				(node1->f() > node2->f());
		};
	};

	CGraphNode			**heap;
	CGraphNode			**heap_head;
	CGraphNode			**heap_tail;

public:
						CDataStorageBinaryHeap(const _index_type node_count) : 
							inherited(node_count)
	{
		u32						memory_usage = 0;
		u32						byte_count;
		
		byte_count				= (node_count)*sizeof(CGraphNode*);
		heap					= (CGraphNode**)xr_malloc(byte_count);
		ZeroMemory				(heap,byte_count);
		memory_usage			+= byte_count;

		Msg						("* Data storage allocated %d bytes of memory",memory_usage);
	}

	virtual				~CDataStorageBinaryHeap()
	{
		xr_free					(heap);
	}

	IC		void		init			()
	{
		inherited::init			();
		heap_head				= heap_tail = heap;
	}

	IC		bool		is_opened_empty	() const
	{
		VERIFY					(heap_head <= heap_tail);
		return					(heap_head == heap_tail);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		VERIFY					(heap_head <= heap_tail);
		node.open_close_mask	= 1;
		*heap_tail				= &node;
		std::push_heap			(heap_head,++heap_tail,CGraphNodePredicate());
	}

	IC		void		decrease_opened	(CGraphNode &node, _dist_type value)
	{
		VERIFY					(!is_opened_empty());
		for (CGraphNode **i = heap_head; *i != &node; ++i);
		std::push_heap			(heap_head,i + 1,CGraphNodePredicate());
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		std::pop_heap			(heap_head,heap_tail--,CGraphNodePredicate());
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		(*heap_head)->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		() const
	{
		VERIFY					(!is_opened_empty());
		return					(**heap_head);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		inherited::get_path		(path,&get_best());
	}
};

template <
	u32			heap_count,
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	
class CDataStorageMultiBinaryHeap :
	public CDataStorageBaseIndexBlock <
		DataStorageHeap::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		_dist_type,
		_index_type,
		_path_id_type,
		bEuclidianHeuristics,
		index_bits,
		mask_bits
	>
{
public:
	typedef DataStorageHeap::SGraphNode<
		_dist_type,
		_index_type,
		index_bits,
		mask_bits
	> CGraphNode;
protected:	
	typedef CDataStorageBaseIndexBlock <
		CGraphNode,
		_dist_type,
		_index_type,
		_path_id_type,
		bEuclidianHeuristics,
		index_bits,
		mask_bits
	> inherited;

	class CGraphNodePredicate {
	public:
		IC			bool	operator()(CGraphNode *node1, CGraphNode *node2)
		{
			return				(node1->f() > node2->f());
		};
	};

	struct SBinaryHeap {
		CGraphNode			**heap;
		CGraphNode			**heap_head;
		CGraphNode			**heap_tail;
	};

	SBinaryHeap				heaps[heap_count];
	SBinaryHeap				*best_heap;

public:
						CDataStorageMultiBinaryHeap(const _index_type node_count) : 
							inherited(node_count)
	{
		u32						memory_usage = 0;
		u32						byte_count;
		
		byte_count				= (node_count/heap_count + 1)*heap_count*sizeof(CGraphNode*);
		for (u32 i=0; i<heap_count; i++) {
			heaps[i].heap		= (CGraphNode**)xr_malloc(byte_count);
			ZeroMemory			(heaps[i].heap,byte_count);
			memory_usage		+= byte_count;
		}

		Msg						("* Data storage allocated %d bytes of memory",memory_usage);
	}

	virtual				~CDataStorageMultiBinaryHeap()
	{
		for (u32 i=0; i<heap_count; i++)
			xr_free				(heaps[i].heap);
	}

	IC		void		init			()
	{
		inherited::init			();
		for (u32 i=0; i<heap_count; i++)
			heaps[i].heap_head	= heaps[i].heap_tail = heaps[i].heap;
	}

	IC		bool		is_opened_empty	() const
	{
		for (u32 i=0; i<heap_count; i++) {
			VERIFY				(heaps[i].heap_head <= heaps[i].heap_tail);
			if (heaps[i].heap_head != heaps[i].heap_tail)
				return			(false);
		}
		return					(true);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		SBinaryHeap				&heap = heaps[node.index() % heap_count];
		VERIFY					(heap.heap_head <= heap.heap_tail);
		*heap.heap_tail			= &node;
		std::push_heap			(heap.heap_head,++heap.heap_tail,CGraphNodePredicate());
	}

	IC		void		decrease_opened	(CGraphNode &node, _dist_type value)
	{
		VERIFY					(!is_opened_empty());
		SBinaryHeap				&heap = heaps[node.index() % heap_count];
		for (CGraphNode **i = heap.heap_head; *i != &node; ++i);
		std::push_heap			(heap.heap_head,i + 1,CGraphNodePredicate());
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		std::pop_heap			(best_heap->heap_head,best_heap->heap_tail--,CGraphNodePredicate());
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		(*best_heap->heap_head)->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		()
	{
		VERIFY					(!is_opened_empty());
		best_heap				= 0;
		_dist_type				f = _dist_type(0x7fffffff);
		for (u32 i=0; i<heap_count; i++) {
			if ((heaps[i].heap_head < heaps[i].heap_tail) && (*heaps[i].heap_head)->f() < f) {
				best_heap		= heaps + i;
				f				= (*heaps[i].heap_head)->f();
			}
		}
		VERIFY					(best_heap);
		return					(**best_heap->heap_head);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		inherited::get_path		(path,&get_best());
	}
};
