////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_no_data.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_vertex_base.h"
#include "loki/emptytype.h"

template <
	typename _vertex_id_type,
	typename _graph_type
>
class graph_vertex<
		Loki::EmptyType,
		_vertex_id_type,
		_graph_type
	> : 
	public	
		graph_vertex_base<
			_vertex_id_type,
			_graph_type
		> 
{
private:
	typedef graph_vertex_base<_vertex_id_type,_graph_type>	inherited;

public:
	IC		graph_vertex	(const _vertex_id_type &vertex_id, size_t *edge_count) :
				inherited	(vertex_id,edge_count)
	{
	}
};

#include "graph_vertex_no_data_inline.h"