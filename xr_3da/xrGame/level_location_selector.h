////////////////////////////////////////////////////////////////////////////
//	Module 		: level_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_location_selector.h"
#include "level_graph.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class 
	CBaseLocationSelector<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type
	> :
	public CAbstractLocationSelector <
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type
	>
{
public:

};

#include "level_location_selector_inline.h"
