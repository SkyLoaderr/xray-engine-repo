////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "problem_solver.h"
#include "action_base.h"
#include "property_evaluator.h"
#include "property_storage.h"

template <
	typename _object_type,
	typename _world_operator = CActionBase<_object_type>,
	typename _condition_evaluator = CPropertyEvaluator<_object_type>
>
class CActionPlanner : 
	public CProblemSolver<
		CGraphEngine::CWorldProperty,
		CGraphEngine::CWorldState,
		_world_operator,
		_condition_evaluator,
		u32
	> 
{
public:
	typedef CProblemSolver<
		CGraphEngine::CWorldProperty,
		CGraphEngine::CWorldState,
		_world_operator,
		_condition_evaluator,
		u32
	>												CProblemSolver;
	typedef CProblemSolver							inherited;
	typedef typename inherited::_edge_type			_action_id_type;
	typedef CGraphEngine::CWorldProperty			CWorldProperty;
	typedef CGraphEngine::CWorldState				CWorldState;

protected:
	bool					m_initialized;
	_action_id_type			m_current_action_id;

#ifdef LOG_ACTION
public:
	bool					m_use_log;
	string64				m_temp_string;
#endif

public:
	_object_type			*m_object;
	CPropertyStorage		m_storage;

public:
							CActionPlanner			();
	virtual					~CActionPlanner			();
			void			init					();
	virtual	void			Load					(LPCSTR section);
	virtual	void			reinit					(_object_type *object, bool clear_all = false);
	virtual	void			reload					(LPCSTR section);
	virtual	void			update					(u32 time_delta);
	IC		COperator		&action					(const _action_id_type &action_id);
	IC		_action_id_type	current_action_id		() const;
	IC		COperator		&current_action			();
	IC		bool			initialized				() const;
	IC		void			add_condition			(_world_operator *action, _condition_type condition_id, _value_type condition_value);
	IC		void			add_effect				(_world_operator *action, _condition_type condition_id, _value_type condition_value);
	virtual void			clear					();
#ifdef LOG_ACTION
	virtual LPCSTR			action2string			(const _action_id_type &action_id);
	virtual LPCSTR			property2string			(const _condition_type &action_id);
#endif
};

#include "action_planner_inline.h"