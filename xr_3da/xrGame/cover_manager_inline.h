////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_manager_inline.h
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CCoverManager::edge_vertex		(u32 index)
{
	CLevelGraph::CVertex	*v = ai().level_graph().vertex(index);
	return					(
		!ai().level_graph().valid_vertex_id(v->link(0)) ||
		!ai().level_graph().valid_vertex_id(v->link(1)) ||
		!ai().level_graph().valid_vertex_id(v->link(2)) ||
		!ai().level_graph().valid_vertex_id(v->link(3))
	);
}

IC	bool CCoverManager::cover			(CLevelGraph::CVertex *v, u32 index0, u32 index1)
{
	return					(
		ai().level_graph().valid_vertex_id(v->link(index0)) &&
		ai().level_graph().valid_vertex_id(ai().level_graph().vertex(v->link(index0))->link(index1)) &&
		temp[ai().level_graph().vertex(v->link(index0))->link(index1)]
	);
}

IC	bool CCoverManager::critical_point	(CLevelGraph::CVertex *v, u32 index, u32 index0, u32 index1)
{
	return					(
		!ai().level_graph().valid_vertex_id(v->link(index)) &&
		(
			!ai().level_graph().valid_vertex_id(v->link(index0)) || 
			!ai().level_graph().valid_vertex_id(v->link(index1)) ||
			cover(v,index0,index) || 
			cover(v,index1,index)
		)
	);
}

IC	bool CCoverManager::critical_cover	(u32 index)
{
	CLevelGraph::CVertex	*v = ai().level_graph().vertex(index);
	return					(
		critical_point(v,0,1,3) || 
		critical_point(v,2,1,3) || 
		critical_point(v,1,0,2) || 
		critical_point(v,3,0,2)
	);
}

IC	CCoverManager::CPointQuadTree &CCoverManager::covers	() const
{
	return					(*m_covers);
}
