////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _object_type,\
		template <typename _object_type> class _motivation_type\
	>

#define CSMotivationActionManager CMotivationActionManager<_object_type,_motivation_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivationActionManager::CMotivationActionManager	()
{
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
CSMotivationActionManager::~CMotivationActionManager	()
{
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::reinit	(_object_type *object, bool clear_all)
{
	CSMotivationManager::reinit	(object,clear_all);
	CSActionPlanner::reinit		(object,clear_all);
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::Load	(LPCSTR section)
{
	CSMotivationManager::Load	(section);
	CSActionPlanner::Load		(section);
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::reload	(LPCSTR section)
{
	CSMotivationManager::reload	(section);
	CSActionPlanner::reload		(section);
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::update	(u32 time_delta)
{
	CSMotivationManager::update			(time_delta);
	
	const CSMotivation					*motivation = CSMotivationManager::selected	();
	const CSMotivationAction			*action		= dynamic_cast<const CSMotivationAction*>(motivation);
	VERIFY								(action);
	CSActionPlanner::set_target_state	(action->goal());

	CSActionPlanner::update				(time_delta);
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::clear	()
{
	CSMotivationManager::clear			();
	CSActionPlanner::clear				();
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivationActionManager