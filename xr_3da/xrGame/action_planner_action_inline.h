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
	inherited_planner::reinit	(object,clear_all);
	inherited_action::reinit	(object,storage,clear_all);
	set_target_state			(effects());
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
	current_action().finalize	();
	inherited_action::finalize	();
	m_initialized				= false;
}

TEMPLATE_SPECIALIZATION
bool CPlanner::completed				() const
{
	return						(inherited_action::completed());
}

#ifdef LOG_ACTION
TEMPLATE_SPECIALIZATION
IC	void CPlanner::set_use_log			(bool value)
{
	inherited_action::set_use_log		(value);
	inherited_planner::set_use_log		(value);
}
#endif

TEMPLATE_SPECIALIZATION
void CPlanner::execute					()
{
	inherited_action::execute	();
	update						();
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_condition		(_world_operator *action, _condition_type condition_id, _value_type condition_value)
{
	inherited_planner::add_condition	(action,condition_id,condition_value);
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_effect			(_world_operator *action, _condition_type condition_id, _value_type condition_value)
{
	inherited_planner::add_effect		(action,condition_id,condition_value);
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner