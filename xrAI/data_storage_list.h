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

template <
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	
class CDataStorageSL :
	public CDataStorageBaseIndexBlock <
		DataStorageList::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		DataStorageBaseIndex::SGraphIndexNode<
			DataStorageList::SGraphNode<
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
	typedef DataStorageList::SGraphNode<
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

	_dist_type			max_distance;
	CGraphNode			*list_head;
	CGraphNode			*list_tail;

public:
						CDataStorageSL	(const _index_type node_count, const _dist_type _max_distance = _dist_type(u32(-1))) :
							inherited(node_count + 2)
	{
		max_distance			= _max_distance;
	}

	virtual				~CDataStorageSL	()
	{
	}

	IC		void		init			()
	{
		inherited::init			();
		ZeroMemory				(nodes,2*sizeof(CGraphNode));
		list_head				= nodes + node_count++;
		list_tail				= nodes + node_count++;
		list_head->next			= list_tail;
		list_tail->f()			= max_distance;
	}

	IC		bool		is_opened_empty	() const
	{
		return					(list_head->next == list_tail);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		for (CGraphNode *i = list_head; ; i = i->next)
			if (i->next->f() >= node.f()) {
				node.next		= i->next;
				i->next			= &node;
				break;
			}
	}

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
	{
		VERIFY					(!is_opened_empty());

		for (CGraphNode *i = list_head; ; i = i->next)
			if (&node == i->next) {
				if (i->f() <= node.f())
					return;
				i->next			= i->next->next;
				break;
			}

		if (node.f() <= list_head->next->f()) {
			node.next			= list_head->next;
			list_head->next		= &node;
		}
		else
			for ( i = list_head; ; i = i->next)
				if (i->next->f() >= node.f()) {
					node.next = i->next;
					i->next = &node;
					break;
				}
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
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

template <
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	
class CDataStorageUL :
	public CDataStorageBaseIndexBlock <
		DataStorageList::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		DataStorageBaseIndex::SGraphIndexNode<
			DataStorageList::SGraphNode<
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
	typedef DataStorageList::SGraphNode<
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

	_dist_type			max_distance;
	CGraphNode			*list_head;
	CGraphNode			*list_tail;

public:
						CDataStorageUL	(const _index_type node_count, const _dist_type _max_distance = _dist_type(u32(-1))) :
							inherited(node_count + 2)
	{
		max_distance			= _max_distance;
	}

	virtual				~CDataStorageUL	()
	{
	}

	IC		void		init			()
	{
		inherited::init			();
		ZeroMemory				(nodes,2*sizeof(CGraphNode));
		list_head				= nodes + node_count++;
		list_tail				= nodes + node_count++;
		list_head->next			= list_tail;
		list_tail->f()			= max_distance;
	}

	IC		bool		is_opened_empty	() const
	{
		return					(list_head->next == list_tail);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		node.next				= list_head->next;
		list_head->next			= &node;
	}

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
	{
		VERIFY					(!is_opened_empty());
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
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
		_dist_type				fmin = max_distance;
		for (CGraphNode *i = list_head, *best_prev = 0; i->next != list_tail; i = i->next)
			if (i->next->f() < fmin) {
				fmin			= i->next->f();
				best_prev		= i;
			}
		VERIFY					(best_prev);
		if (best_prev != list_head) {
			CGraphNode			*best = best_prev->next;
			best_prev->next		= best->next;
			best->next			= list_head->next;
			list_head->next		= best;
		}
		return					(*list_head->next);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		inherited::get_path		(path,&get_best());
	}
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
class CDataStorageDLSL :
	public CDataStorageBaseIndexBlock <
		DataStorageListDoubleLinked::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		DataStorageBaseIndex::SGraphIndexNode<
			DataStorageListDoubleLinked::SGraphNode<
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

	_dist_type			max_distance;
	_dist_type			switch_factor;
	CGraphNode			*list_head;
	CGraphNode			*list_tail;

public:
						CDataStorageDLSL(const _index_type node_count, const _dist_type _max_distance = _dist_type(u32(-1))) :
							inherited(node_count + 2)
	{
		max_distance			= _max_distance;
		switch_factor			= 1;
	}

	virtual				~CDataStorageDLSL()
	{
	}

	IC		void		set_switch_factor(const _dist_type _switch_factor)
	{
		switch_factor			= _switch_factor;
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

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
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
			if (node.prev->f() - node.f() < switch_factor*(node.f() - list_head->next->f()))
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

template <
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	
class CDataStorageDLUL :
	public CDataStorageBaseIndexBlock <
		DataStorageListDoubleLinked::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		DataStorageBaseIndex::SGraphIndexNode<
			DataStorageListDoubleLinked::SGraphNode<
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
	typedef DataStorageListDoubleLinked::SGraphNode<
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

	_dist_type			max_distance;
	CGraphNode			*list_head;
	CGraphNode			*list_tail;

public:
						CDataStorageDLUL(const _index_type node_count, const _dist_type _max_distance = _dist_type(u32(-1))) :
							inherited(node_count + 2)
	{
		max_distance			= _max_distance;
	}

	virtual				~CDataStorageDLUL()
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
		list_head->next->prev	= &node;
		node.next				= list_head->next;
		list_head->next			= &node;
		node.prev				= list_head;
	}

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
	{
		VERIFY					(!is_opened_empty());
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
		_dist_type				fmin = max_distance;
		for (CGraphNode *i = list_head->next, *best = 0; i; i = i->next)
			if (i->f() < fmin) {
				fmin			= i->f();
				best			= i;
			}
		VERIFY					(best);
		if (best->prev != list_head) {
			best->prev->next	= best->next;
			best->next->prev	= best->prev;
			best->next			= list_head->next;
			best->prev			= list_head;
			list_head->next->prev = best;
			list_head->next		= best;
		}
		return					(*list_head->next);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		inherited::get_path		(path,&get_best());
	}
};

template <
	u32			cheap_node_count,
	bool		keep_fixed_count		= true,
	bool		keep_filled				= false,
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	
class CDataStorageCheapList :
	public CDataStorageBaseIndexBlock <
		DataStorageListDoubleLinked::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		DataStorageBaseIndex::SGraphIndexNode<
			DataStorageListDoubleLinked::SGraphNode<
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
	typedef DataStorageListDoubleLinked::SGraphNode<
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

	_dist_type			max_distance;
	u32					cheap_count;
	bool				filled_over;
	CGraphNode			*list_head;
	CGraphNode			*list_tail;
	CGraphNode			*cheap_list_head;
	CGraphNode			*cheap_list_tail;

public:
						CDataStorageCheapList(const _index_type node_count, const _dist_type _max_distance = _dist_type(u32(-1))) :
							inherited(node_count + 2)
	{
		max_distance			= _max_distance;
	}

	virtual				~CDataStorageCheapList()
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
		cheap_list_head			= cheap_list_tail = 0;
		cheap_count				= 0;
		if (keep_filled)
			filled_over			= false;
	}

	IC		bool		is_opened_empty	() const
	{
		return					((list_head->next == list_tail) && (!cheap_list_head));
	}

	IC		void		verify_cheap	()
	{
//		u32						count = 0;
//		for (CGraphNode *i = cheap_list_head; i; i = i->next, count++) {
//			VERIFY(!i->next || (i->f() <= i->next->f()));
//		}
//		VERIFY					(count == cheap_count);
//		count					= 0;
//		for (CGraphNode *i = cheap_list_tail; i; i = i->prev, count++) {
//			VERIFY(!i->prev || (i->f() >= i->prev->f()));
//		}
//		VERIFY					(count == cheap_count);
//		count					= 0;
//		for (CGraphNode *i = list_head->next; i != list_tail; i = i->next, count++) {
//			VERIFY				(i->f() >= cheap_list_tail->f());
//		}
//		u32						count1 = 0;
//		for (CGraphNode *i = list_tail->prev; i != list_head; i = i->prev, count1++) {
//			VERIFY				(i->f() >= cheap_list_tail->f());
//		}
//		VERIFY					(count == count1);
//		VERIFY					((cheap_count < cheap_node_count) || filled_over);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		
		verify_cheap			();
		if (!add_cheap_count(node)) {
			verify_cheap			();
			list_head->next->prev = &node;
			node.next			= list_head->next;
			list_head->next		= &node;
			node.prev			= list_head;
			verify_cheap			();
		}
		else
			verify_cheap			();
	}

	IC		void		add_cheap		(CGraphNode &node)
	{
		if (cheap_list_head->f() >= node.f()) {
			cheap_list_head->prev	= &node;
			node.next				= cheap_list_head;
			node.prev				= 0;
			cheap_list_head			= &node;
			cheap_count++;
			return;
		}

//		if ((cheap_list_tail->f() - node.f())*1 < (node.f() - cheap_list_head->f())*3)
			for (CGraphNode *i = cheap_list_tail->prev; ; i = i->prev) {
				if (i->f() <= node.f()) {
					node.next		= i->next;
					node.prev		= i;
					i->next->prev	= &node;
					i->next			= &node;
					cheap_count++;
					return;
				}
			}
//		else
//			for (CGraphNode *i = cheap_list_head->next; ; i = i->next) {
//				if (i->f() >= node.f()) {
//					node.next		= i;
//					node.prev		= i->prev;
//					i->prev->next	= &node;
//					i->prev			= &node;
//					cheap_count++;
//					return;
//				}
//			}
	}

	IC		void		add_cheap_tail	(CGraphNode &node)
	{
		if (cheap_list_tail->f() < node.f()) {
			cheap_list_tail->next = &node;
			node.prev		= cheap_list_tail;
			node.next		= 0;
			cheap_list_tail	= &node;
			cheap_count++;
			return;
		}
		add_cheap			(node);
	}

	IC		bool		add_cheap_count	(CGraphNode &node)
	{
		if (!cheap_list_tail) {
			cheap_list_head		= cheap_list_tail = &node;
			node.next			= node.prev = 0;
			cheap_count			= 1;
			verify_cheap		();
			return				(true);
		}

		if (keep_filled && !filled_over) {
			verify_cheap		();
			add_cheap_tail		(node);
			if (cheap_count == cheap_node_count)
				filled_over		= true;
			verify_cheap		();
			return				(true);
		}

		if (cheap_list_tail->f() < node.f()) {
			verify_cheap		();
			return				(false);
		}

		add_cheap				(node);

		if (keep_fixed_count && (cheap_count > cheap_node_count))
			remove_cheap_tail	();

		return					(true);
	}

	IC		void		remove_cheap_tail()
	{
		cheap_list_tail->prev->next = 0;
		cheap_list_tail->next		= list_head->next;
		list_head->next->prev		= cheap_list_tail;
		list_head->next				= cheap_list_tail;
		cheap_list_tail				= cheap_list_tail->prev;
		list_head->next->prev		= list_head;
		cheap_count--;
	}

	IC		void		fill_cheap		()
	{
		if (keep_filled)
			filled_over			= false;
		if (list_head->next == list_tail) {
			cheap_list_head		= cheap_list_tail = 0;
			cheap_count			= 0;
			return;
		}
		
		cheap_list_head			= cheap_list_tail = list_head->next;
		list_head->next			= list_head->next->next;
		list_head->next->prev	= list_head;
		cheap_list_head->prev	= cheap_list_head->next = 0;

		cheap_count				= 1;
		for (CGraphNode *i = list_head->next, *j; i != list_tail; i = i->next) {
			if (cheap_count < cheap_node_count) {
				j				= i->prev;
				i->prev->next	= i->next;
				i->next->prev	= i->prev;
				add_cheap_tail	(*i);
				i				= j;
				continue;
			}
			else {
				if (keep_filled)
					filled_over	= true;
				for ( ; i != list_tail; i = i->next) {
					if (i->f() < cheap_list_tail->f()) {
						j							= i->prev;
						i->prev->next				= i->next;
						i->next->prev				= i->prev;
						add_cheap					(*i);
						i							= j;
						remove_cheap_tail			();
					}
				}
				return;
			}
		}
		VERIFY			(cheap_count <= cheap_node_count);
		if (keep_filled && (cheap_count == cheap_node_count))
			filled_over	= true;
	}

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
	{
		VERIFY					(!is_opened_empty());
		VERIFY					(cheap_list_head && cheap_list_tail);
		
//		verify_cheap			();
		for (CGraphNode *i = cheap_list_head; i && (i != &node); i = i->next);
		if (i == &node) {
			if (i->prev && (i->prev->f() > node.f())) {
				for ( ; i->prev && (i->prev->f() > node.f()); i = i->prev);
				if (!i->prev) {
					node.prev->next = node.next;
					if (node.next)
						node.next->prev = node.prev;
					else
						cheap_list_tail = node.prev;
					node.prev		= 0;
					node.next		= cheap_list_head;
					cheap_list_head->prev = &node;
					cheap_list_head	= &node;
					verify_cheap			();
					return;
				}
				else {
					if (node.prev)
						node.prev->next = node.next;
					else
						cheap_list_tail = node.prev;
					if (node.next)
						node.next->prev = node.prev;
					else
						cheap_list_tail = node.prev;
					node.next		= i;
					node.prev		= i->prev;
					i->prev->next	= &node;
					i->prev			= &node;
					verify_cheap			();
					return;
				}
			}
		}
		else {
			if (keep_fixed_count) {
				node.next->prev			= node.prev;
				node.prev->next			= node.next;
				if (!add_cheap_count(node)) {
					node.next->prev		= &node;
					node.prev->next		= &node;
				}
			}
			else {
				CGraphNode				*next = node.next, *prev = node.prev;
				if (add_cheap_count(node)) {
					next->prev			= prev;
					prev->next			= next;
				}
			}
			verify_cheap			();
		}
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		VERIFY					(cheap_list_head);
		
		verify_cheap			();
		cheap_list_head			= cheap_list_head->next;
		cheap_count--;
		
		if (cheap_list_head)
			cheap_list_head->prev = 0;
		else
			fill_cheap			();
		verify_cheap			();
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		VERIFY					(cheap_list_head);
		cheap_list_head->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		() const
	{
		VERIFY					(!is_opened_empty());
		VERIFY					(cheap_list_head && cheap_list_tail);
		return					(*cheap_list_head);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		inherited::get_path		(path,&get_best());
	}
};

namespace DataStorageBucketList {
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
		u32				bucket_id;
	};
	template <
		typename _GraphNode,
		typename _path_id_type
	>
	struct SGraphIndexNode : 
		DataStorageBaseIndex::SGraphIndexNode <
			_GraphNode,
			_path_id_type
		>
	{
//		u32				bucket_id;
	};
	#pragma pack(pop)
};

template <
	u32			bucket_count,
	bool		clear_buckets,
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	
class CDataStorageBucketList :
	public CDataStorageBaseIndexBlock <
		DataStorageBucketList::SGraphNode<
			_dist_type,
			_index_type,
			index_bits,
			mask_bits
		>,
		DataStorageBucketList::SGraphIndexNode<
			DataStorageBucketList::SGraphNode<
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
	typedef DataStorageBucketList::SGraphNode<
		_dist_type,
		_index_type,
		index_bits,
		mask_bits
	> CGraphNode;

	typedef	DataStorageBucketList::SGraphIndexNode<
		CGraphNode,
		_path_id_type
	> SGraphIndexNode;

protected:	
	typedef CDataStorageBaseIndexBlock <
		CGraphNode,
		SGraphIndexNode,
		_dist_type,
		_index_type,
		_path_id_type,
		bEuclidianHeuristics,
		index_bits,
		mask_bits
	> inherited;

	_dist_type			min_bucket_value;
	_dist_type			max_bucket_value;
	CGraphNode			*buckets[bucket_count];
	u32					min_bucket_id;

public:
						CDataStorageBucketList(const _index_type node_count, const _dist_type _min_bucket_value, const _dist_type _max_bucket_value) :
							inherited(node_count)
	{
		min_bucket_value		= _min_bucket_value;
		max_bucket_value		= _max_bucket_value;
		ZeroMemory				(buckets,bucket_count*sizeof(CGraphNode*));
	}

	virtual				~CDataStorageBucketList()
	{
	}


	IC		void		init			()
	{
		inherited::init			();
		min_bucket_id			= bucket_count;
		if (clear_buckets)
			ZeroMemory			(buckets,bucket_count*sizeof(CGraphNode*));
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		buckets[min_bucket_id]->open_close_mask = 0;
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		inherited::get_path		(path,&get_best());
	}
	
	IC		bool		is_opened_empty	() const
	{
		return					(min_bucket_id == bucket_count);
	}

	IC		u32			compute_bucket_id(CGraphNode &node) const
	{
		int					f = int(bucket_count*(node.f() - min_bucket_value)/(max_bucket_value - min_bucket_value));// + _dist_type(1.5));
		if (f < 0)
			f				= 0;
		else
			if (f >= bucket_count)
				f			= bucket_count - 1;
		return					(f);
	}

	IC		void		verify_buckets	()
	{
//		for (u32 i=0; i<bucket_count; i++) {
//			CGraphNode	*j = buckets[i], *k;
//			if (!j || (indexes[j->index()].path_id != cur_path_id) || (indexes[j->index()].node != j))
//				continue;
//			u32			count = 0, count1 = 0;
//			for ( ; j; k=j,j=j->next, count++) {
//				VERIFY	(indexes[j->index()].path_id == cur_path_id);
//				VERIFY	(compute_bucket_id(*j) == i);
//				VERIFY	(!j->prev || (j == j->prev->next));
//				VERIFY	(!j->next || (j == j->next->prev));
//				VERIFY	(!j->next || (j != j->next));
//				VERIFY	(!j->prev || (j != j->prev));
//			}
//			for ( ; k; k=k->prev, count1++) {
//				VERIFY	(indexes[k->index()].path_id == cur_path_id);
//				VERIFY	(compute_bucket_id(*k) == i);
//				VERIFY	(!k->prev || (k == k->prev->next));
//				VERIFY	(!k->next || (k == k->next->prev));
//				VERIFY	(!k->next || (k != k->next));
//				VERIFY	(!k->prev || (k != k->prev));
//			}
//			VERIFY		(count == count1);
//		}
	}

	IC		CGraphNode	&create_node	(const _index_type node_index)
	{
		if (!clear_buckets) {
			if (indexes[node_index].node && (indexes[node_index].node->index() == node_index) && (buckets[indexes[node_index].node->bucket_id] == indexes[node_index].node))
				buckets[indexes[node_index].node->bucket_id] = 0;
			CGraphNode			&node = inherited::create_node(node_index);
			if (buckets[node.bucket_id] == &node)
				buckets[node.bucket_id] = 0;
			return				(node);
		}
		else
			return				(inherited::create_node(node_index));
	}

	IC		void		add_to_bucket	(CGraphNode &node, u32 bucket_id)
	{
		if (bucket_id < min_bucket_id)
			min_bucket_id		= bucket_id;

		node.bucket_id = bucket_id;
		
		CGraphNode			*i = buckets[bucket_id];
		if (!i || (!clear_buckets && (indexes[i->index()].path_id != cur_path_id))) {
			buckets[bucket_id]	= &node;
			node.next			= node.prev = 0;
			verify_buckets		();
			return;
		}

		if (i->f() >= node.f()) {
			buckets[bucket_id]	= &node;
			node.next			= i;
			node.prev			= 0;
			i->prev				= &node;
			verify_buckets		();
			return;
		}
		
		if (!i->next) {
			node.prev			= i;
			node.next			= 0;
			i->next				= &node;
			verify_buckets		();
			return;
		}
		
		for (i = i->next; i->next; i = i->next) {
			if (i->f() >= node.f()) {
				node.next		= i;
				node.prev		= i->prev;
				i->prev->next	= &node;
				i->prev			= &node;
				verify_buckets	();
				return;
			}
		}
		
		if (i->f() >= node.f()) {
			node.next		= i;
			node.prev		= i->prev;
			i->prev->next	= &node;
			i->prev			= &node;
			verify_buckets	();
			return;
		}
		else {
			node.next		= 0;
			node.prev		= i;
			i->next			= &node;
			verify_buckets	();
			return;
		}
		
		verify_buckets	();
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		add_to_bucket			(node,compute_bucket_id(node));
		verify_buckets			();
	}

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
	{
		VERIFY					(!is_opened_empty());
		u32						node_bucket_id = compute_bucket_id(node);
//		if (node_bucket_id == node.bucket_id) {
//			if ((buckets[node.bucket_id] == &node) || (node.prev->f() <= node.f()))
//				return;
//			if (node.next) {
//				node.prev->next = node.next;
//				node.next->prev = node.prev;
//			}
//			else
//				node.prev->next = 0;
//			if (buckets[node.bucket_id]->f() >= node.f()) {
//				node.next		= buckets[node.bucket_id];
//				node.prev		= 0;
//				buckets[node.bucket_id]->prev = &node;
//				buckets[node.bucket_id] = &node;
//				return;
//			}
//			for (CGraphNode *i = node.prev->prev; ; i = i->prev)
//				if (i->f() <= node.f()) {
//					i->next->prev = &node;
//					node.next	= i->next;
//					node.prev	= i;
//					i->next		= &node;
//					return;
//				}
//			NODEFAULT;
//		}
		if (node.prev)
			node.prev->next		= node.next;
		else {
			VERIFY				(buckets[node.bucket_id] == &node);
			buckets[node.bucket_id] = node.next;
		}
		if (node.next)
			node.next->prev		= node.prev;

		verify_buckets			();
		add_to_bucket			(node,node_bucket_id);
		verify_buckets			();
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		verify_buckets			();
		VERIFY					(buckets[min_bucket_id] && (indexes[buckets[min_bucket_id]->index()].path_id == cur_path_id));
		buckets[min_bucket_id]	= buckets[min_bucket_id]->next;
		if (buckets[min_bucket_id])
			buckets[min_bucket_id]->prev = 0;
		verify_buckets			();
	}

	IC		CGraphNode	&get_best		()
	{
		VERIFY					(!is_opened_empty());
		if (!buckets[min_bucket_id]) {
			verify_buckets		();
			if (!clear_buckets)
				for (++min_bucket_id; !buckets[min_bucket_id] || (indexes[buckets[min_bucket_id]->index()].path_id != cur_path_id) || (indexes[buckets[min_bucket_id]->index()].node != buckets[min_bucket_id]); ++min_bucket_id);
			else
				for (++min_bucket_id; !buckets[min_bucket_id]; ++min_bucket_id);
			VERIFY				(min_bucket_id < bucket_count);
			verify_buckets		();
		}
		return					(*buckets[min_bucket_id]);
	}
};


