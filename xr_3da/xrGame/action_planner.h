////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "problem_solver.h"

template <
	typename _object_type,
	typename _world_operator,
	typename _condition_evaluator
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
protected:
	typedef CProblemSolver<
		CGraphEngine::CWorldProperty,
		CGraphEngine::CWorldState,
		_world_operator,
		_condition_evaluator,
		u32
	>												CProblemSolver;
	typedef CProblemSolver							inherited;
	typedef typename CProblemSolver::_edge_type		_action_id_type;

protected:
	bool					m_initialized;
	_action_id_type			m_current_action_id;
	_object_type			*m_object;

private:
	IC		void			set_current_action		(const _action_id_type action_id);

public:
							CActionPlanner			();
	virtual					~CActionPlanner			();
			void			init					();
	virtual	void			Load					(LPCSTR section);
	virtual	void			reinit					(_object_type *object, bool clear_all = false);
	virtual	void			reload					(LPCSTR section);
	virtual	void			update					(u32 time_delta);
	IC		COperator		&action					(const _action_id_type action_id);
	IC		_action_id_type	current_action_id		() const;
	IC		COperator		&current_action			();
	IC		bool			initialized				() const;
};

#include "action_planner_inline.h"