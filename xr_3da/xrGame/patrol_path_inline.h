////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_inline.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CPatrolPath::CVertex *CPatrolPath::point	(shared_str name) const
{
	const_vertex_iterator		I = vertices().begin();
	const_vertex_iterator		E = vertices().end();
	for ( ; I != E; ++I)
		if ((*I)->data().name() == name)
			return				(*I);
	return						(0);
}

template <typename T>
IC	const CPatrolPath::CVertex *CPatrolPath::point	(const Fvector &position, const T &evaluator) const
{
	const CPatrolPath::CVertex	*nearest = 0;
	float						best_distance = flt_max;
	const_vertex_iterator		I = vertices().begin();
	const_vertex_iterator		E = vertices().end();
	for ( ; I != E; ++I) {
		if (!evaluator((*I)->data().position()))
			continue;
		float					distance = (*I)->data().position().distance_to_sqr(position);
		if (distance < best_distance) {
			best_distance		= distance;
			nearest				= *I;
		}
	}
	return						(nearest);
}

IC	const CPatrolPath::CVertex *CPatrolPath::point	(const Fvector &position) const
{
	return						(point(position,CAlwaysTrueEvaluator()));
}
