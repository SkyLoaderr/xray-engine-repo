////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action_inline.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CPlanner				CActionPlannerAction<_object_type>

TEMPLATE_SPECIALIZATION
IC	CPlanner::CActionPlannerAction	(_object_type *object, LPCSTR action_name) :
	inherited_action			(object,action_name)
{
	init						();
}

TEMPLATE_SPECIALIZATION
CPlanner::~CActionPlannerAction		()
{
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::init				()
{
}

TEMPLATE_SPECIALIZATION
void CPlanner::Load					(LPCSTR section)
{
	inherited_planner::Load		(section);
	inherited_action::Load		(section);
}

TEMPLATE_SPECIALIZATION
void CPlanner::reinit					(_object_type *object, CPropertyStorage *storage, bool clear_all)
{
#ifdef LOG_ACTION
	inherited_planner::m_use_log = inherited_action::m_use_log;
#endif
	inherited_planner::reinit	(object,clear_all);
	inherited_action::reinit	(object,storage,clear_all);
	set_target_state			(effects());
}

TEMPLATE_SPECIALIZATION
void CPlanner::reload					(LPCSTR section)
{
#ifdef LOG_ACTION
	inherited_planner::m_use_log = inherited_action::m_use_log;
#endif
	inherited_planner::reload	(section);
	inherited_action::reload	(section);
}

TEMPLATE_SPECIALIZATION
void CPlanner::initialize				()
{
	inherited_action::initialize();
}

TEMPLATE_SPECIALIZATION
void CPlanner::finalize				()
{
	inherited_action::finalize	();
	current_action().finalize	();
	m_initialized				= false;
}

TEMPLATE_SPECIALIZATION
bool CPlanner::completed				() const
{
	return						(inherited_action::completed());
}

TEMPLATE_SPECIALIZATION
void CPlanner::execute				()
{
#ifdef LOG_ACTION
	inherited_planner::m_use_log = inherited_action::m_use_log;
#endif
	inherited_action::execute	();
	inherited_planner::update	();
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner