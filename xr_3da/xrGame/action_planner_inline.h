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
}

TEMPLATE_SPECIALIZATION
void CPlanner::init				()
{
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
			(*I).get_operator()->reinit(object,clear_all);
	}
	{
		EVALUATOR_MAP::iterator		I = m_evaluators.begin();
		EVALUATOR_MAP::iterator		E = m_evaluators.end();
		for ( ; I != E; ++I)
			(*I).second->reinit		(object);
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

	if (initialized() && (current_action_id() != solution().front())) {
		current_action().finalize();
		set_current_action		(solution().front());
	}

	current_action().initialize	();
	current_action().execute	();

	CPlanner					*action_planner = dynamic_cast<CPlanner*>(&current_action());
	if (action_planner)
		action_planner->update	(time_delta);

	m_initialized				= true;
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::COperator &CPlanner::action	(const _action_id_type action_id)
{
	return				(*get_operator(action_id));
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::set_current_action	(const _action_id_type action_id)
{
	m_current_action_id	= action_id;
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::_action_id_type CPlanner::current_action_id	() const
{
	return				(m_current_action_id);
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::COperator &CPlanner::current_action	()
{
	return				(action(current_action_id()));
}

TEMPLATE_SPECIALIZATION
IC	bool CPlanner::initialized	() const
{
	return				(m_initialized);
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner