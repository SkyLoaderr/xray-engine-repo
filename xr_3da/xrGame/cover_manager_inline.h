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
	if (evaluator.inertia() && evaluator.accessible(evaluator.selected()->position()))
		return				(evaluator.selected());

	evaluator.initialize	(position);

	covers().nearest		(position,radius,m_nearest);

	xr_vector<CCoverPoint*>::const_iterator	I = m_nearest.begin();
	xr_vector<CCoverPoint*>::const_iterator	E = m_nearest.end();
	for ( ; I != E; ++I)
		if (evaluator.accessible((*I)->position()) && restrictor(*I))
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
