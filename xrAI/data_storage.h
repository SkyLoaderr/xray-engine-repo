#pragma once

template <typename dist_type = float, typename index_type = u32, typename epoch_type = u32, bool bEuclidianHeuristics = true, u8 index_bits = 24, u8 mask_bits = 8, u8 pragma_pack = 4> class CDataStorage {
	#pragma pack(push,pragma_pack)
	struct SGraphNode {
		index_type		open_close_mask	: mask_bits;
		index_type		index			: index_bits;
		dist_type		f;
		dist_type		g;
		dist_type		h;
		SGraphNode		*back;

		IC	index_type	index() const
		{
			return				(index);
		}

		IC	dist_type	&f() const
		{
			return				(f);
		}

		IC	dist_type	&g() const
		{
			return				(g);
		}

		IC	dist_type	&h() const
		{
			return				(h);
		}
	};

	struct SGraphIndexNode {
		SGraphNode		*node;
		epoch_type		epoch_id;
	};
#pragma pack(pop)

	class CGraphNodePredicate {
	public:
		IC			bool	operator()(const CGraphNode *node1, const CGraphNode *node2) const
		{
			return				(node1->f > node2->f);
		};
	};

	epoch_type			epoch_count;
	index_type			node_count;
	SGraphNode			*nodes;
	SGraphIndexNode		*indexes;
	SGraphNode			**heap;
	SGraphNode			**heap_start;
	SGraphNode			**heap_end;

public:
	typedef SGraphNode CGraphNode;

						CDataStorage	(const index_type node_count)
	{
		epoch_count				= epoch_type(0);
		
		size_t					memory_usage = 0, byte_count;
		
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

	IC		void		init			(const index_type node_index)
	{
		epoch_count++;
		node_count				= index_type(0);
		heap_start				= heap + 0;
		heap_end				= heap + 1;
	}

	IC		bool		is_opened_empty	() const
	{
		return					(heap_start == heap_end);
	}

	IC		bool		is_opened		(const CGraphNode &node) const
	{
		return					(node.open_close_mask);
	}

	IC		bool		is_visited(const index_type node_index) const
	{
		return					(indexes[node_index].epoch_id == epoch_count);
	}

	IC		bool		is_closed		(const CGraphNode &node) const
	{
		return					(is_visited() && !is_opened());
	}

	IC		CGraphNode	&add_opened		(const index_type node_index)
	{
		CGraphNode				*node = indexes[node_index].node = nodes + node_count++;
		indexes[node_index].epoch_count	= epoch_count;
		Memory.mem_fill			(node,0,sizeof(CGraphNode));
		node->index()			= node_index;
		node->open_close_mask	= 1;
		*heap_end				= node;
		std::push_heap			(heap_start,++heap_end,CGraphNodePredicate());
		return					(*node);
	}

	IC		void		decrease_opened	(CGraphNode &node)
	{
		for (CGraphNode **i = heap_start; *i != &node; ++i);
		std::push_heap			(heap_start,i + 1,CGraphNodePredicate());
	}

	IC		void		remove_best		()
	{
		std::pop_heap			(heap_start,heap_end--,CGraphNodePredicate());
	}

	IC		void		add_closed		()
	{
		(*heap_start)->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		() const
	{
		VERIFY					(!is_opened_empty());
		return					(**heap_start);
	}

	IC		CGraphNode	&get_node		(const index_type node_index)
	{
		VERIFY					(is_visited(node_index));
		return					(*indexes[node_index].node);
	}

	IC		void		assign_parent	(CGraphNode	&tpNeighbour, CGraphNode &tpParent)
	{
		tpNeighbour.back		= &tpParent;
	}
	
	IC		void		update_successors(CGraphNode &tpNeighbour)
	{
		VERIFY					(false);
	}

	IC		void		create_path		()
	{
	}
};