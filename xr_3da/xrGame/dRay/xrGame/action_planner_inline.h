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
	{
		OPERATOR_VECTOR::iterator	I = m_operators.begin();
		OPERATOR_VECTOR::iterator	E = m_operators.end();
		for ( ; I != E; ++I)
			xr_delete				((*I).m_operator);
	}
	{
		EVALUATOR_MAP::iterator		I = m_evaluators.begin();
		EVALUATOR_MAP::iterator		E = m_evaluators.end();
		for ( ; I != E; ++I)
			xr_delete				((*I).second);
	}
	m_object						= 0;
}

TEMPLATE_SPECIALIZATION
void CPlanner::init				()
{
	m_initialized			= false;
#ifdef LOG_ACTION
	m_use_log				= false;
#endif
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
	m_storage.clear			();
	{
		OPERATOR_VECTOR::iterator	I = m_operators.begin();
		OPERATOR_VECTOR::iterator	E = m_operators.end();
		for ( ; I != E; ++I)
			if (!clear_all) {
				(*I).get_operator()->reinit(object,&m_storage,clear_all);
#ifdef LOG_ACTION
				(*I).get_operator()->m_use_log = m_use_log;
#endif
			}
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
				(*I).second->reinit	(object,&m_storage);
			else
				xr_delete	((*I).second);
		if (clear_all)
			m_evaluators.clear	();
	}
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

#ifdef LOG_ACTION
	// printing solution
	if (m_use_log) {
		if (m_solution_changed) {
			Msg						("%6d : Solution for object %s",Level().timeServer(),object_name());
			for (int i=0; i<(int)solution().size(); ++i)
				Msg					("%s",action2string(solution()[i]));
		}
		if (m_failed) {
			// printing current world state
			{
				Msg					("! ERROR!!! : there is no action sequence, which can transfer current world state to the target world state!!!");
				Msg					("%6d : Current world state",Level().timeServer());
				EVALUATOR_MAP::const_iterator	I = evaluators().begin();
				EVALUATOR_MAP::const_iterator	E = evaluators().end();
				for ( ; I != E; ++I) {
					xr_vector<COperatorCondition>::const_iterator J = std::lower_bound(current_state().conditions().begin(),current_state().conditions().end(),CWorldProperty((*I).first,false));
					char			temp = '?';
					if ((J != current_state().conditions().end()) && ((*J).condition() == (*I).first)) {
						temp		= (*J).value() ? '+' : '-';
						Msg			("%5c : %s",temp,property2string((*I).first));
					}
				}
			}
			// printing target world state
			{
				Msg					("%6d : Target world state",Level().timeServer());
				EVALUATOR_MAP::const_iterator	I = evaluators().begin();
				EVALUATOR_MAP::const_iterator	E = evaluators().end();
				for ( ; I != E; ++I) {
					xr_vector<COperatorCondition>::const_iterator J = std::lower_bound(target_state().conditions().begin(),target_state().conditions().end(),CWorldProperty((*I).first,false));
					char			temp = '?';
					if ((J != target_state().conditions().end()) && ((*J).condition() == (*I).first)) {
						temp		= (*J).value() ? '+' : '-';
						Msg			("%5c : %s",temp,property2string((*I).first));
					}
				}
			}
//			VERIFY2						(!m_failed,"Problem solver couldn't build a valid path - verify your conditions, effects and goals!");
		}
	}
#endif

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
IC	typename CPlanner::COperator &CPlanner::action	(const _action_id_type &action_id)
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

TEMPLATE_SPECIALIZATION
void CPlanner::clear				()
{
	{
		OPERATOR_VECTOR::iterator	I = m_operators.begin();
		OPERATOR_VECTOR::iterator	E = m_operators.end();
		for ( ; I != E; ++I)
			xr_delete				((*I).m_operator);
	}
	{
		EVALUATOR_MAP::iterator		I = m_evaluators.begin();
		EVALUATOR_MAP::iterator		E = m_evaluators.end();
		for ( ; I != E; ++I)
			xr_delete				((*I).second);
	}
	m_operators.clear				();
	m_evaluators.clear				();
}

#ifdef LOG_ACTION
TEMPLATE_SPECIALIZATION
LPCSTR CPlanner::action2string		(const _action_id_type &action_id)
{
	return			(action(action_id).m_action_name);
}

TEMPLATE_SPECIALIZATION
LPCSTR CPlanner::property2string	(const _condition_type &property_id)
{
	return			(itoa(property_id,m_temp_string,10));
}
#endif

TEMPLATE_SPECIALIZATION
LPCSTR CPlanner::object_name		() const
{
	return			("");
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner