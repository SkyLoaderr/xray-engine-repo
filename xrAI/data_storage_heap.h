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
		DataStorageBaseIndex::SGraphIndexNode<
			DataStorageHeap::SGraphNode<
				_dist_type,
				_index_type,
				index_bits,
				mask_bits
			>,
			_path_id_type
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
		DataStorageBaseIndex::SGraphIndexNode<
			CGraphNode,
			_path_id_type
		>,
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

namespace DataStorageMultiBinaryHeap {
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
		SGraphNode		*next;
		SGraphNode		*prev;
	};
	#pragma pack(pop)
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
		DataStorageMultiBinaryHeap::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		DataStorageBaseIndex::SGraphIndexNode<
			DataStorageMultiBinaryHeap::SGraphNode<
				_dist_type,
				_index_type,
				index_bits,
				mask_bits
			>,
			_path_id_type
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
	typedef DataStorageMultiBinaryHeap::SGraphNode<
		_dist_type,
		_index_type,
		index_bits,
		mask_bits
	> CGraphNode;
protected:	
	typedef CDataStorageBaseIndexBlock <
		CGraphNode,
		DataStorageBaseIndex::SGraphIndexNode<
			CGraphNode,
			_path_id_type
		>,
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

	_dist_type				max_distance;
	SBinaryHeap				heaps[heap_count];
	CGraphNode				*list_head;
	CGraphNode				*list_tail;

public:
						CDataStorageMultiBinaryHeap(const _index_type node_count, const _dist_type _max_distance = _dist_type(u32(-1))) :
							inherited(node_count + 2)
	{
		max_distance			= _max_distance;
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
		ZeroMemory				(nodes,2*sizeof(CGraphNode));
		list_head				= nodes + node_count++;
		list_tail				= nodes + node_count++;
		list_head->next			= list_tail;
		list_tail->prev			= list_head;
		list_tail->f()			= max_distance;
	}

	IC		bool		is_opened_empty	() const
	{
		return					(list_head->next == list_tail);
	}


	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		list_head->next->open_close_mask = 0;
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		inherited::get_path		(path,&get_best());
	}
	
	IC		u32			compute_heap_index(const _index_type node_index)
	{
		return					(node_index % heap_count);
	}

	IC		void		push_front		(CGraphNode &node)
	{
		for (CGraphNode *i = node.prev; ;i = i->prev)
			if (i->f() <= node.f()) {
				node.next		= i->next;
				node.prev		= i;
				i->next->prev	= &node;
				i->next			= &node;
				return;
			}
		VERIFY					(false);
		for (CGraphNode *i=list_head->next; i!=list_tail; i = i->next) {
			VERIFY(i->prev->f() <= i->f());
		}
		for (CGraphNode *i=list_tail->prev; i!=list_head; i = i->prev) {
			VERIFY(i->prev->f() <= i->f());
		}
	}

	IC		void		push_back		(CGraphNode &node)
	{
		for (CGraphNode *i = node.next; ;i = i->next)
			if (i->f() >= node.f()) {
				node.next		= i;
				node.prev		= i->prev;
				i->prev->next	= &node;
				i->prev			= &node;
				return;
			}
		VERIFY					(false);
		for (CGraphNode *i=list_head->next; i!=list_tail; i = i->next) {
			VERIFY(i->prev->f() <= i->f());
		}
		for (CGraphNode *i=list_tail->prev; i!=list_head; i = i->prev) {
			VERIFY(i->prev->f() <= i->f());
		}
	}

	IC		void		push_front(CGraphNode *old_head, CGraphNode *new_head)
	{
		old_head->prev->next = old_head->next;
		new_head->prev = old_head->next->prev = old_head->prev;
		push_front			(*new_head);
	}

	IC		void		push_back(CGraphNode *old_head, CGraphNode *new_head)
	{
		new_head->next = old_head->prev->next = old_head->next;
		old_head->next->prev = old_head->prev;
		push_back			(*new_head);
	}

	IC		void		push_front_if_needed(CGraphNode *old_head, CGraphNode *new_head)
	{
		if (old_head != new_head)
			push_front			(old_head,new_head);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		SBinaryHeap				&heap = heaps[compute_heap_index(node.index())];
		VERIFY					(heap.heap_head <= heap.heap_tail);
		if (heap.heap_head == heap.heap_tail) {
			*heap.heap_tail		= &node;
			std::push_heap		(heap.heap_head,++heap.heap_tail,CGraphNodePredicate());
			(*heap.heap_head)->next = list_head->next;
			push_back			(**heap.heap_head);
			return;
		}
		CGraphNode				*heap_head = *heap.heap_head;
		*heap.heap_tail			= &node;
		std::push_heap			(heap.heap_head,++heap.heap_tail,CGraphNodePredicate());
		push_front_if_needed	(heap_head, *heap.heap_head);
	}

	IC		void		decrease_opened	(CGraphNode &node, _dist_type value)
	{
		VERIFY					(!is_opened_empty());
		SBinaryHeap				&heap = heaps[compute_heap_index(node.index())];
		CGraphNode				*heap_head = *heap.heap_head;
		for (CGraphNode **i = heap.heap_head; *i != &node; ++i);
		std::push_heap			(heap.heap_head,i + 1,CGraphNodePredicate());
		push_front_if_needed	(heap_head, *heap.heap_head);
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		SBinaryHeap				*best_heap = heaps + compute_heap_index(list_head->next->index());
		CGraphNode				*heap_head = *best_heap->heap_head;
		std::pop_heap			(best_heap->heap_head,best_heap->heap_tail--,CGraphNodePredicate());
		if (best_heap->heap_head != best_heap->heap_tail)
			push_back			(heap_head,*best_heap->heap_head);
		else {
			list_head->next = list_head->next->next;
			list_head->next->prev = list_head;
		}
	}

	IC		CGraphNode	&get_best		()
	{
		VERIFY					(!is_opened_empty());
		return					(*list_head->next);
	}
};
