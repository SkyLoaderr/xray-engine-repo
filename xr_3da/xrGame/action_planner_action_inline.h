////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action_inline.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CPlanner			CActionPlannerAction<_object_type>

TEMPLATE_SPECIALIZATION
IC	CPlanner::CActionPlannerAction	()
{
	init						();
}

TEMPLATE_SPECIALIZATION
IC	CPlanner::~CActionPlannerAction	()
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
void CPlanner::reinit					(_object_type *object, bool clear_all = false)
{
	inherited_planner::reinit	(clear_all);
	inherited_action::reinit	(object);
}

TEMPLATE_SPECIALIZATION
void CPlanner::reload					(LPCSTR section)
{
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
}

TEMPLATE_SPECIALIZATION
bool CPlanner::completed				() const
{
	return						(inherited_action::completed());
}

TEMPLATE_SPECIALIZATION
void CPlanner::update					(u32 time_delta)
{
	execute						();
	
	inherited_planner::update	(time_delta);

	VERIFY						(!solution().empty());

	if (current_operator() != solution().front()) {
		action(current_operator()).finalize();
		set_current_operator	(solution().front());
	}

	action(current_operator()).initialize();
	action(current_operator()).execute();

	inherited_planner			*action_planner = dynamic_cast<inherited_planner*>(&action(current_operator()));
	if (action_planner)
		action_planner->update	(time_delta);
}

TEMPLATE_SPECIALIZATION
void CPlanner::execute				()
{
	inherited_action::execute	();
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner