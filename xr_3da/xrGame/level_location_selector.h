////////////////////////////////////////////////////////////////////////////
//	Module 		: level_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_location_selector.h"
#include "level_navigation_graph.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class 
	CBaseLocationSelector<
		CLevelNavigationGraph,
		_VertexEvaluator,
		_vertex_id_type
	> :
	public CAbstractLocationSelector <
		CLevelNavigationGraph,
		_VertexEvaluator,
		_vertex_id_type
	>
{
	typedef CLevelNavigationGraph _Graph;
	typedef CAbstractLocationSelector <
		_Graph,
		_VertexEvaluator,
		_vertex_id_type
	> inherited;

public:
	IC							CBaseLocationSelector		(CRestrictedObject *object, CSelectorManager *selector_manager);

protected:
	IC	virtual	void			before_search				(_vertex_id_type &vertex_id);
	IC	virtual	void			after_search				();
};

#include "level_location_selector_inline.h"
