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
	CActionPlanner<_object_type>	*action_planner = dynamic_cast<CActionPlanner*>(&current_action());
	if (action_planner)
		action_planner->update	(time_delta);
}

TEMPLATE_SPECIALIZATION
void CPlanner::execute				()
{
	inherited_action::execute	();

//	if (current_vertex_id() == dest_vertex_id()) {
//		IGraphManager			*state_manager_interface = dynamic_cast<IGraphManager*>(&current_state());
//		if (!state_manager_interface)
//			current_state().execute	();
//		return;
//	}
//
//	if (path().empty())
//		return;
//
//	for (;;) {
//		if (
//			(internal_state(path().front()).priority() <= internal_state(path().back()).priority()) 
//			&& 
//			!state(path().front()).completed()
//			)
//		{
//			state(path().front()).execute();
//			return;
//		}
//
//		state(path().front()).finalize();
//		follow_solution			();
//		state(path().front()).initialize();
//		state(path().front()).execute();
//		if (path().size() < 2) {
//			follow_solution		();
//			break;
//		}
//	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner