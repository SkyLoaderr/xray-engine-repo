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
		(!ai().level_graph().valid_vertex_id(v->link(0)) && (v->cover(0) < 12)) ||
		(!ai().level_graph().valid_vertex_id(v->link(1)) && (v->cover(1) < 12)) ||
		(!ai().level_graph().valid_vertex_id(v->link(2)) && (v->cover(2) < 12)) ||
		(!ai().level_graph().valid_vertex_id(v->link(3)) && (v->cover(3) < 12))
	);
}

IC	bool CCoverManager::cover			(CLevelGraph::CVertex *v, u32 index0, u32 index1)
{
	return					(
		ai().level_graph().valid_vertex_id(v->link(index0)) &&
		ai().level_graph().valid_vertex_id(ai().level_graph().vertex(v->link(index0))->link(index1)) &&
		m_temp[ai().level_graph().vertex(v->link(index0))->link(index1)]
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
	VERIFY					(m_covers);
	return					(*m_covers);
}

IC	CCoverManager::CPointQuadTree *CCoverManager::get_covers	()
{
	return					(m_covers);
}

template <typename _evaluator_type, typename _restrictor_type>
IC	CCoverPoint *CCoverManager::best_cover(const Fvector &position, float radius, _evaluator_type &evaluator, const _restrictor_type &restrictor) const
{
	if (evaluator.inertia())
		return				(evaluator.selected());

	evaluator.initialize	(position);

	covers().nearest		(position,radius,m_nearest);

	xr_vector<CCoverPoint*>::const_iterator	I = m_nearest.begin();
	xr_vector<CCoverPoint*>::const_iterator	E = m_nearest.end();
	for ( ; I != E; ++I)
		if (restrictor(*I))
			evaluator.evaluate	(*I);

	evaluator.finalize		();

	return					(evaluator.selected());
}

template <typename _evaluator_type>
IC	CCoverPoint	*CCoverManager::best_cover	(const Fvector &position, float radius, _evaluator_type &evaluator) const
{
	return					(best_cover<_evaluator_type,CCoverManager>(position,radius,evaluator,*this));
}

IC	bool CCoverManager::operator()			(const CCoverPoint *) const
{
	return					(true);
}
