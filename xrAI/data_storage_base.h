////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_base.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Data storages
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace DataStorageBase {
	#pragma pack(push,4)
	template <
		typename _dist_type
	> 
	struct SGraphNode {
		_dist_type		_f;
		_dist_type		_g;
		_dist_type		_h;

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
	#pragma pack(pop)
};

template <
	typename	_GraphNode,
	typename	_dist_type				= float,
	typename	_index_type				= u32,
	bool		bEuclidianHeuristics	= true
>
class CDataStorageBase
{
protected:
	_index_type			max_node_count;
public:
						CDataStorageBase(const _index_type node_count)
	{
		max_node_count			= node_count;
	}

	virtual				~CDataStorageBase()
	{
	}

	IC		void		assign_parent	(_GraphNode	&neighbour, _GraphNode *parent)
	{
		neighbour.back			= parent;
	}

	IC		void		update_successors(_GraphNode &tpNeighbour)
	{
		VERIFY					(false);
	}

	IC		void		get_path		(xr_vector<_index_type> &path, _GraphNode *best)
	{
		_GraphNode				*t1 = best, *t2 = best->back;
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

namespace DataStorageBaseIndex {
	#pragma pack(push,4)
	template <
		typename _dist_type, 
		typename _index_type, 
		u8 index_bits, 
		u8 mask_bits
	>
	struct SGraphNode : 
		DataStorageBase::SGraphNode<
			_dist_type
		> 
	{
		_index_type		open_close_mask	: mask_bits;
		_index_type		_index			: index_bits;

		IC	_index_type	index() const
		{
			return				(_index);
		}
	};
	#pragma pack(pop)
};

template <
	typename	_GraphNode,
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>
class CDataStorageBaseIndex : 
	public CDataStorageBase <
		_GraphNode,
		_dist_type,
		_index_type,
		bEuclidianHeuristics
	>
{
protected:
	typedef public CDataStorageBase <
		_GraphNode,
		_dist_type,
		_index_type,
		bEuclidianHeuristics
	> inherited;

	#pragma pack(push,4)
	struct SGraphIndexNode {
		_GraphNode		*node;
		_path_id_type	path_id;
	};
	#pragma pack(pop)

	_path_id_type		cur_path_id;
	SGraphIndexNode		*indexes;

public:

						CDataStorageBaseIndex(const _index_type node_count) :
						inherited(node_count)
	{
		cur_path_id				= _path_id_type(0);

		u32						memory_usage = 0;
		u32						byte_count;
		
		byte_count				= (node_count)*sizeof(SGraphIndexNode);
		indexes					= (SGraphIndexNode*)xr_malloc(byte_count);
		ZeroMemory				(indexes,byte_count);
		memory_usage			+= byte_count;

		Msg						("* Data storage allocated %d bytes of memory",memory_usage);
	}

	virtual				~CDataStorageBaseIndex()
	{
		xr_free					(indexes);
	}

	IC		void		init			()
	{
		cur_path_id++;
	}

	IC		bool		is_opened		(const _GraphNode &node) const
	{
		return					(!!node.open_close_mask);
	}

	IC		bool		is_visited(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		return					(indexes[node_index].path_id == cur_path_id);
	}

	IC		bool		is_closed		(const _GraphNode &node) const
	{
		return					(is_visited(node) && !is_opened(node));
	}

	IC		_GraphNode	&get_node		(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		VERIFY					(is_visited(node_index));
		return					(*indexes[node_index].node);
	}
};

template <
	typename _GraphNode,
	typename _index_type = u32
>
class CDataStorageBlock
{
protected:
	_index_type			node_count;
	_GraphNode			*nodes;
public:
						CDataStorageBlock(const _index_type node_count)
	{
		u32						memory_usage = 0;
		u32						byte_count;

		byte_count				= (node_count)*sizeof(_GraphNode);
		nodes					= (_GraphNode*)xr_malloc(byte_count);
		ZeroMemory				(nodes,byte_count);
		memory_usage			+= byte_count;

		Msg						("* Data storage allocated %d bytes of memory",memory_usage);
	}

	virtual				~CDataStorageBlock()
	{
		xr_free					(nodes);
	}

	IC		void		init			()
	{
		node_count				= _index_type(0);
	}

	IC		const _index_type	get_visited_node_count() const
	{
		return					(node_count);
	}

	IC		_GraphNode	&create_node	(const _index_type node_index)
	{
		return					(*(nodes + node_count++));
	}
};

template <
	typename	_GraphNode,
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>
class CDataStorageBaseIndexBlock : 
	public CDataStorageBaseIndex <
		_GraphNode,
		_dist_type,
		_index_type,
		_path_id_type,
		bEuclidianHeuristics,
		index_bits,
		mask_bits
	>,
	public CDataStorageBlock <
		_GraphNode,
		_index_type
	>
{
protected:
	typedef CDataStorageBaseIndex <
		_GraphNode,
		_dist_type,
		_index_type,
		_path_id_type,
		bEuclidianHeuristics,
		index_bits,
		mask_bits
	> inherited1;

	typedef CDataStorageBlock <
		_GraphNode,
		_index_type
	> inherited2;

public:
						CDataStorageBaseIndexBlock(const _index_type node_count) :
							inherited1(node_count), 
							inherited2(node_count)
	{
	}

	virtual				~CDataStorageBaseIndexBlock()
	{
	}

	IC		void		init			()
	{
		inherited1::init		();
		inherited2::init		();
	}

	IC		_GraphNode	&create_node	(const _index_type node_index)
	{
		VERIFY					(node_index < max_node_count);
		_GraphNode				*node = indexes[node_index].node = &inherited2::create_node(node_index);
		indexes[node_index].path_id = cur_path_id;
		node->_index			= node_index;
		return					(*node);
	}
};