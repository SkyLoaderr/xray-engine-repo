////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action.h"

template <typename _object_type>
void CMotivationActionManager<_object_type>::reinit	(_object_type *object, bool clear_all)
{
	CSMotivationManager::reinit	(object,clear_all);
	CSActionManager::reinit		(object,clear_all);
}

template <typename _object_type>
void CMotivationActionManager<_object_type>::Load	(LPCSTR section)
{
	CSMotivationManager::Load	(section);
	CSActionManager::Load		(section);
}

template <typename _object_type>
void CMotivationActionManager<_object_type>::reload	(LPCSTR section)
{
	CSMotivationManager::reload	(section);
	CSActionManager::reload		(section);
}

template <typename _object_type>
void CMotivationActionManager<_object_type>::update	(u32 time_delta)
{
	CSMotivationManager::update			(time_delta);
	
	const CSMotivation					*motivation = CSMotivationManager::selected	();
	const CSMotivationAction			*action		= dynamic_cast<const CSMotivationAction*>(motivation);
	VERIFY								(action);
	CSActionManager::set_target_state	(action->goal());

	CSActionManager::update				(time_delta);
}
