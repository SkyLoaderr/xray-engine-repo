////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _object_type>
IC	CMotivationAction<_object_type>::CMotivationAction	(const CWorldState &goal) :
	m_goal			(goal)
{
}

template <typename _object_type>
IC	const typename CMotivationAction<_object_type>::CWorldState	&CMotivationAction<_object_type>::goal	() const
{
	return			(m_goal);
}
