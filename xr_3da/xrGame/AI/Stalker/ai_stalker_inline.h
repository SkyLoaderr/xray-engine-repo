////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	BOOL CAI_Stalker::UsedAI_Locations				()
{
	return				(TRUE);
}

template <typename _evaluator_type>
CCoverPoint *CAI_Stalker::best_cover(const Fvector &position, float radius, _evaluator_type &evaluator)
{
	if (evaluator.inertia())
		return		(evaluator.selected());

	evaluator.initialize(position);

	ai().cover_manager().covers().nearest	(position,radius,m_nearest);
	xr_vector<CCoverPoint*>::const_iterator	I = m_nearest.begin();
	xr_vector<CCoverPoint*>::const_iterator	E = m_nearest.end();
	for ( ; I != E; ++I)
		evaluator.evaluate(*I);

	evaluator.finalize();

	return			(evaluator.selected());
}