////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_list.h
//	Created 	: 21.03.2002
//  Modified 	: 20.10.2003
//	Author		: Dmitriy Iassenev
//	Description : List data storages
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "data_storage_base.h"

namespace DataStorageList {
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
	};
	#pragma pack(pop)
};

namespace DataStorageListDoubleLinked {
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
};

template <
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	
class CDataStorageDLSL :
	public CDataStorageBaseIndexBlock <
		DataStorageListDoubleLinked::SGraphNode<
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
	typedef DataStorageListDoubleLinked::SGraphNode<
		_dist_type,
		_index_type,
		index_bits,
		mask_bits
	> CGraphNode;
	
	typedef DataStorageBase::SGraphNode<
		_dist_type
	> SGraphNode;
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

	_dist_type			max_distance;
	CGraphNode			*list_head;
	CGraphNode			*list_tail;

public:
						CDataStorageDLSL(const _index_type node_count, const _dist_type _max_distance = _dist_type(u32(-1))) :
							inherited(node_count)
	{
		max_distance			= _max_distance;
	}

	virtual				~CDataStorageDLSL()
	{
	}

	IC		void		init			()
	{
		inherited::init			();
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

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		for (CGraphNode *i = list_head->next; ; i = i->next)
			if (i->f() >= node.f()) {
				node.next		= i;
				node.prev		= i->prev;
				i->prev->next	= &node;
				i->prev			= &node;
				break;
			}
	}

	IC		void		decrease_opened	(CGraphNode &node)
	{
		VERIFY					(!is_opened_empty());

		if (node.prev->f() <= node.f())
			return;

		node.prev->next			= node.next;
		node.next->prev			= node.prev;

		if (node.f() <= list_head->next->f()) {
			node.prev			= list_head;
			node.next			= list_head->next;
			list_head->next->prev = &node;
			list_head->next		= &node;
		}
		else {
			if (node.prev->f() - node.f() < node.f() - list_head->next->f())
				for (CGraphNode *i = node.prev->prev; ; i = i->prev) {
					if (i->f() <= node.f()) {
						node.next = i->next;
						node.prev = i;
						i->next->prev = &node;
						i->next = &node;
						break;
					}
				}
			else
				for (CGraphNode *i = list_head->next; ; i = i->next)
					if (i->f() >= node.f()) {
						node.next = i;
						node.prev = i->prev;
						i->prev->next = &node;
						i->prev = &node;
						break;
					}
		}
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		list_head->next->next->prev	= list_head;
		list_head->next			= list_head->next->next;
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		list_head->next->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		() const
	{
		VERIFY					(!is_opened_empty());
		return					(*list_head->next);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		inherited::get_path		(path,&get_best());
	}
};
