////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_constructor.h
//	Created 	: 21.03.2002
//  Modified 	: 28.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Data storage constructor
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename T> class CEmptyClassTemplate {};
template <typename T1, typename T2> class CEmptyClassTemplate2 {};

#include "data_storage_index_base_allocator_constructor.h"

template <
	typename _algorithm, 
	typename _index, 
	typename _base, 
	typename _allocator,
	template <typename _T> class _vertex = CEmptyClassTemplate,
	template <
		typename _1,
		typename _2
	>
	class	 _base_allocator_constructor = CConstructorBaseAllocator,
	template <
		typename _1, 
		typename _2,
		typename _3,
		template <
			typename _1,
			typename _2
		>
		class	 _4
	>
	class	 _index_base_allocator_constructor = CConstructorVertex
>
struct CDataStorageConstructor : 
	public _algorithm::CDataStorage<
		_index_base_allocator_constructor<
			_index,
			_base,
			_allocator,
			_base_allocator_constructor
		>,
		_vertex
	>
{
	typedef typename _algorithm::CDataStorage<
		_index_base_allocator_constructor<
			_index,
			_base,
			_allocator,
			_base_allocator_constructor
		>,
		_vertex
	> inherited; 

	typedef typename inherited::CGraphVertex	CGraphVertex;
	typedef typename CGraphVertex::_index_type	_index_type;

	IC	CDataStorageConstructor (const u32 vertex_count) :
		inherited(vertex_count)
	{
	}
};
