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
	inherited_planner			*action_planner = dynamic_cast<inherited_planner*>(&current_action());
	if (action_planner)
		action_planner->update	(time_delta);
}

TEMPLATE_SPECIALIZATION
void CPlanner::execute				()
{
	inherited_action::execute	();

//	if (!solution().empty()) {
//		// therefore execute current operator
//		action(current_operator()).execute();
//		return;
//	}
//
//	if (current_operator() != solution().front()) {
//		if	(
//				!action(current_operator()).completed()
//				&&
//				(action(current_operator()).priority() < action(solution().front()).priority())
//			)
//		{
//			action(current_operator()).execute();
//			return;
//		}
//	}
//	VERIFY						(!solution().empty());
//
//	if (1 == solution().size()) {
//		inherited_planner		*action_planner = dynamic_cast<inherited_planner*>(&current_state());
//		if (!action_planner)
//			current_action().execute	();
//		return;
//	}
//
//	for (;;) {
//		if (!action(solution().front()).completed()) {
//			action(solution().front()).execute();
//			return;
//		}
//		action(solution().front()).finalize();
//
//		follow_solution			();
//
//		action(solution().front()).initialize();
//		action(solution().front()).execute();
//	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner