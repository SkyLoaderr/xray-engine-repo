////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_data.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_vertex_base.h"

template <
	typename _data_type,
	typename _vertex_id_type,
	typename _graph_type
>
class graph_vertex : 
	public _graph_vertex<
		vertex_id_type,
		_graph_type::edge_type
	> 
{
private:
	typedef _graph_vertex<
				vertex_id_type,
				_graph_type::edge_type
			>					inherited;

private:
	_data_type					m_data;

public:
	IC							graph_vertex	(const _data_type &data, const _vertex_id_type &vertex_id, size_t *edge_count);
	// destructor is not virtual, 
	// since we shouldn't remove vertices,
	// having pointer not to their final class
	IC							~graph_vertex	();
	IC		bool				operator==		(const graph_vertex_base &obj) const;
	IC		const _data_type	&data			() const;
	IC		_data_type			&data			();
	IC		void				data			(const _data_type &data);
};

#include "graph_vertex_data_inline.h"