////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_inline.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CPlanner				CActionBase<_object_type>

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
	{
		OPERATOR_VECTOR::iterator	I = m_operators.begin();
		OPERATOR_VECTOR::iterator	E = m_operators.end();
		for ( ; I != E; ++I)
			(*I).get_operator()->reinit(object);
	}
	{
		EVALUATOR_MAP::iterator		I = m_evaluators.begin();
		EVALUATOR_MAP::iterator		E = m_evaluators.end();
		for ( ; I != E; ++I)
			(*I).second->reinit		(object);
	}
	m_actuality				= true;
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
	solve					();
}

TEMPLATE_SPECIALIZATION
IC	CPlanner::COperator &CPlanner::action	(u32 action_id)
{
	return				(*get_operator(action_id));
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner
