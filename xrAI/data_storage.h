////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Data storages
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _dist_type = float, typename _index_type = u32, typename _epoch_type = u32, bool bEuclidianHeuristics = true, u8 index_bits = 24, u8 mask_bits = 8> class CDataStorage {
	#pragma pack(push,4)
	struct SGraphNode {
		_index_type		open_close_mask	: mask_bits;
		_index_type		_index			: index_bits;
		_dist_type		_f;
		_dist_type		_g;
		_dist_type		_h;
		SGraphNode		*back;

		IC	_index_type	index() const
		{
			return				(_index);
		}

		IC	_dist_type	&f()
		{
			return				(_f);
		}

		IC	_dist_type	&g()
		{
			return				(_g);
		}

		IC	_dist_type	&h()
		{
			return				(_h);
		}
	};

	struct SGraphIndexNode {
		SGraphNode		*node;
		_epoch_type		epoch_id;
	};
#pragma pack(pop)

	class CGraphNodePredicate {
	public:
		IC			bool	operator()(SGraphNode *node1, SGraphNode *node2)
		{
			return				(node1->f() > node2->f());
		};
	};

	_epoch_type			epoch_count;
	_index_type			node_count;
	_index_type			max_node_count;
	SGraphNode			*nodes;
	SGraphIndexNode		*indexes;
	SGraphNode			**heap;
	SGraphNode			**heap_start;
	SGraphNode			**heap_end;

public:
	typedef SGraphNode CGraphNode;

						CDataStorage	(const _index_type node_count)
	{
		epoch_count				= _epoch_type(0);
		max_node_count			= node_count;
		
		u32						memory_usage = 0;
		u32						byte_count;
		
		byte_count				= (node_count + 2)*sizeof(CGraphNode);
		nodes					= (CGraphNode*)xr_malloc(byte_count);
		ZeroMemory				(nodes,byte_count);
		memory_usage			+= byte_count;

		byte_count				= (node_count)*sizeof(SGraphIndexNode);
		indexes					= (SGraphIndexNode*)xr_malloc(byte_count);
		ZeroMemory				(indexes,byte_count);
		memory_usage			+= byte_count;

		byte_count				= (node_count)*sizeof(CGraphNode*);
		heap					= (CGraphNode**)xr_malloc(byte_count);
		ZeroMemory				(heap,byte_count);
		memory_usage			+= byte_count;

		Msg						("* Data storage allocated %d bytes of memory",memory_usage);
	}

	virtual				~CDataStorage	()
	{
		xr_free					(nodes);
		xr_free					(indexes);
		xr_free					(heap);
	}

	IC		void		init			()
	{
		epoch_count++;
		node_count				= _index_type(0);
		heap_start				= heap_end = heap;
	}

	IC		bool		is_opened_empty	() const
	{
		VERIFY					(heap_start <= heap_end);
		return					(heap_start == heap_end);
	}

	IC		bool		is_opened		(const CGraphNode &node) const
	{
		return					(!!node.open_close_mask);
	}

	IC		bool		is_visited(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		return					(indexes[node_index].epoch_id == epoch_count);
	}

	IC		bool		is_closed		(const CGraphNode &node) const
	{
		return					(is_visited(node) && !is_opened(node));
	}

	IC		CGraphNode	&create_node	(const _index_type node_index)
	{
		VERIFY					(node_index < max_node_count);
		CGraphNode				*node = indexes[node_index].node = nodes + node_count++;
		indexes[node_index].epoch_id = epoch_count;
#pragma todo("Dima to Dima : remove next line here")
		node->back				= 0;
		node->_index			= node_index;
		return					(*node);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		VERIFY					(heap_start <= heap_end);
		node.open_close_mask	= 1;
		*heap_end				= &node;
		std::push_heap			(heap_start,++heap_end,CGraphNodePredicate());
	}

	IC		void		decrease_opened	(CGraphNode &node)
	{
		VERIFY					(!is_opened_empty());
		for (CGraphNode **i = heap_start; *i != &node; ++i);
		std::push_heap			(heap_start,i + 1,CGraphNodePredicate());
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		std::pop_heap			(heap_start,heap_end--,CGraphNodePredicate());
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		(*heap_start)->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		() const
	{
		VERIFY					(!is_opened_empty());
		return					(**heap_start);
	}

	IC		CGraphNode	&get_node		(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		VERIFY					(is_visited(node_index));
		return					(*indexes[node_index].node);
	}

	IC		void		assign_parent	(CGraphNode	&tpNeighbour, CGraphNode &tpParent)
	{
		tpNeighbour.back		= &tpParent;
	}

	IC		const _index_type	get_visited_node_count() const
	{
		return					(node_count);
	}
	
	IC		void		update_successors(CGraphNode &tpNeighbour)
	{
		VERIFY					(false);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		CGraphNode				*best = &get_best(), *t1 = best, *t2 = t1->back;
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