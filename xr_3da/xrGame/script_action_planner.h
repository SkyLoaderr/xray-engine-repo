////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner.h"

class CScriptActionPlanner : public CActionPlanner<CLuaGameObject> {
protected:
	typedef CActionPlanner<CLuaGameObject> inherited;

public:

	IC		void						set_target_state		(CState &state)
	{
		inherited::set_target_state(state);
	}

	IC		void			add_evaluator			(_condition_type condition_id, CConditionEvaluator *evaluator)
	{
		inherited::add_evaluator(condition_id,evaluator);
	}

	IC		void			remove_evaluator		(_condition_type condition_id)
	{
		inherited::remove_evaluator(condition_id);
	}

	IC		const CConditionEvaluator	*evaluator	(_condition_type condition_id) const
	{
		return				(inherited::evaluator(condition_id));
	}

	IC		void			add_operator			(_edge_type	operator_id, COperator *_operator)
	{
		inherited::add_operator(operator_id,_operator);
	}

	IC		void			remove_operator			(_edge_type	operator_id)
	{
		inherited::remove_operator(operator_id);
	}

	IC		COperator		*action					(_action_id_type action_id)
	{
		return			(&inherited::action(action_id));
	}

	IC		_action_id_type	current_action_id		()
	{
		return			(inherited::current_action_id());
	}

	IC		COperator		*current_action			()
	{
		return			(&inherited::current_action());
	}

	IC		bool			initialized				()
	{
		return			(inherited::initialized());
	}

	virtual	void			reinit					(CLuaGameObject *object, bool clear_all = false)
	{
		inherited::reinit	(object,clear_all);
	}

	virtual	void			update					(u32 time_delta)
	{
		inherited::update	(time_delta);
	}
};

class CScriptActionPlannerWrapper : public CScriptActionPlanner {
protected:
public:
	luabind::object			m_lua_instance;

							CScriptActionPlannerWrapper	(const luabind::object &lua_instance)
	{
	}

	virtual void	reinit				(CLuaGameObject *object, bool clear_all)
	{
		luabind::call_member<void>(m_lua_instance,"reinit",object,clear_all);
	}

	static	void	reinit_static		(CScriptActionPlanner *planner, CLuaGameObject *object, bool clear_all)
	{
		planner->CScriptActionPlanner::reinit(object,clear_all);
	}

	virtual void	update				(u32 time_delta)
	{
		luabind::call_member<void>(m_lua_instance,"update",time_delta);
	}

	static	void	update_static		(CScriptActionPlanner *planner, u32 time_delta)
	{
		planner->CScriptActionPlanner::update(time_delta);
	}
};