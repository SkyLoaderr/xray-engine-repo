////////////////////////////////////////////////////////////////////////////
//	Module 		: level_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_path_manager.h"
#include "level_graph.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class 
	CBasePathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> :
	public CAbstractPathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> 
{
	typedef CAbstractPathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> inherited;
public:
	IC			bool	actual						(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const;
};

#include "level_path_manager_inline.h"
