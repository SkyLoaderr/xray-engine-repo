////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_manager_inline.h
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

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
	START_PROFILE("AI/Covers/best_cover")

	if (evaluator.inertia(radius) && (!evaluator.selected() || evaluator.accessible(evaluator.selected()->position())))
		return				(evaluator.selected());

	evaluator.initialize	(position);

	covers().nearest		(position,radius,m_nearest);

	float					radius_sqr = _sqr(radius);

	xr_vector<CCoverPoint*>::const_iterator	I = m_nearest.begin();
	xr_vector<CCoverPoint*>::const_iterator	E = m_nearest.end();
	for ( ; I != E; ++I) {
		if (radius_sqr < position.distance_to_sqr((*I)->position()))
			continue;

		if (_abs(position.y - (*I)->position().y) > 3.f)
			continue;

		if (!evaluator.accessible((*I)->position()))
			continue;

		if (!restrictor(*I))
			continue;

		evaluator.evaluate	(*I,restrictor.weight(*I));
	}

	evaluator.finalize		();

	return					(evaluator.selected());
	
	STOP_PROFILE
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

IC	float CCoverManager::weight				(const CCoverPoint *) const
{
	return					(1.f);
}
