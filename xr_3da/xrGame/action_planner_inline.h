////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_inline.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _object_type,\
		typename _world_operator,\
		typename _condition_evaluator\
	>
#define CPlanner				CActionPlanner<_object_type,_world_operator,_condition_evaluator>

TEMPLATE_SPECIALIZATION
IC	CPlanner::CActionPlanner			()
{
	init					();
}

TEMPLATE_SPECIALIZATION
IC	CPlanner::~CActionPlanner			()
{
	m_object				= 0;
}

TEMPLATE_SPECIALIZATION
void CPlanner::init				()
{
	m_initialized			= false;
}

TEMPLATE_SPECIALIZATION
void CPlanner::Load				(LPCSTR section)
{
	{
		OPERATOR_VECTOR::iterator	I = m_operators.begin();
		OPERATOR_VECTOR::iterator	E = m_operators.end();
		for ( ; I != E; ++I)
			(*I).get_operator()->Load(section);
	}
	{
		EVALUATOR_MAP::iterator		I = m_evaluators.begin();
		EVALUATOR_MAP::iterator		E = m_evaluators.end();
		for ( ; I != E; ++I)
			(*I).second->Load		(section);
	}
}

TEMPLATE_SPECIALIZATION
void CPlanner::reinit				(_object_type *object, bool clear_all)
{
	inherited::reinit		(clear_all);
	m_object				= object;
	m_current_action_id		= _action_id_type(-1);
	{
		OPERATOR_VECTOR::iterator	I = m_operators.begin();
		OPERATOR_VECTOR::iterator	E = m_operators.end();
		for ( ; I != E; ++I)
			if (!clear_all)
				(*I).get_operator()->reinit(object,clear_all);
			else
				xr_delete	((*I).m_operator);
		if (clear_all)
			m_operators.clear	();
	}
	{
		EVALUATOR_MAP::iterator		I = m_evaluators.begin();
		EVALUATOR_MAP::iterator		E = m_evaluators.end();
		for ( ; I != E; ++I)
			if (!clear_all)
				(*I).second->reinit	(object);
			else
				xr_delete	((*I).second);
		if (clear_all)
			m_evaluators.clear	();
	}
	m_actuality				= true;
	m_initialized			= false;
}

TEMPLATE_SPECIALIZATION
void CPlanner::reload				(LPCSTR section)
{
	{
		OPERATOR_VECTOR::iterator	I = m_operators.begin();
		OPERATOR_VECTOR::iterator	E = m_operators.end();
		for ( ; I != E; ++I)
			(*I).get_operator()->reload(section);
	}
	{
		EVALUATOR_MAP::iterator		I = m_evaluators.begin();
		EVALUATOR_MAP::iterator		E = m_evaluators.end();
		for ( ; I != E; ++I)
			(*I).second->reload		(section);
	}
}

TEMPLATE_SPECIALIZATION
void CPlanner::update				(u32 time_delta)
{
	solve						();

	VERIFY						(!solution().empty());

	if (initialized()) {
		if (current_action_id() != solution().front()) {
			current_action().finalize	();
			m_current_action_id			= solution().front();
			current_action().initialize	();
		}
	}
	else {
		m_initialized				= true;
		m_current_action_id			= solution().front();
		current_action().initialize	();
	}

	current_action().execute	();

	CPlanner					*action_planner = dynamic_cast<CPlanner*>(&current_action());
	if (action_planner)
		action_planner->update	(time_delta);
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::COperator &CPlanner::action	(const _action_id_type action_id)
{
	return					(*get_operator(action_id));
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::_action_id_type CPlanner::current_action_id	() const
{
	VERIFY					(initialized());
	return					(m_current_action_id);
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::COperator &CPlanner::current_action	()
{
	return					(action(current_action_id()));
}

TEMPLATE_SPECIALIZATION
IC	bool CPlanner::initialized	() const
{
	return					(m_initialized);
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_condition	(_world_operator *action, _condition_type condition_id, _value_type condition_value)
{
	action->add_condition	(CWorldProperty(condition_id,condition_value));
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_effect		(_world_operator *action, _condition_type condition_id, _value_type condition_value)
{
	action->add_effect		(CWorldProperty(condition_id,condition_value));
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner