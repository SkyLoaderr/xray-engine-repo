////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_location_selector.h"
#include "game_graph.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class 
	CBaseLocationSelector<
		CGameGraph,
		typename _VertexEvaluator,
		typename _vertex_id_type
	> :
	public CAbstractLocationSelector <
		CGameGraph,
		typename _VertexEvaluator,
		typename _vertex_id_type
	>
{
public:

};

#include "game_location_selector_inline.h"
