////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_script.cpp
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner_wrapper.h"
#include "script_space.h"
#include "script_game_object.h"
#include <luabind/adopt_policy.hpp>

using namespace luabind;

void CActionPlanner<CScriptGameObject>::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptActionPlanner,CScriptActionPlannerWrapper>("action_planner")
			.def_readonly("object",				&CScriptActionPlanner::m_object)
			.def_readonly("storage",			&CScriptActionPlanner::m_storage)
			.def(								constructor<>())
			.def("reinit",						&CScriptActionPlanner::reinit,	&CScriptActionPlannerWrapper::reinit_static)
			.def("update",						&CScriptActionPlanner::update,	&CScriptActionPlannerWrapper::update_static)
			.def("add_action",					(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_edge_type &, CScriptActionPlanner::COperator *))(CScriptActionPlanner::add_operator),adopt(_3))
			.def("remove_action",				(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_edge_type &))(CScriptActionPlanner::remove_operator))
			.def("action",						&CScriptActionPlanner::action)
			.def("add_evaluator",				(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &, CScriptActionPlanner::CConditionEvaluator *))(CScriptActionPlanner::add_evaluator),adopt(_3))
			.def("remove_evaluator",			(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &))(CScriptActionPlanner::remove_evaluator))
			.def("evaluator",					(CScriptActionPlanner::CConditionEvaluator * (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &) const)(CScriptActionPlanner::evaluator))
			.def("current_action_id",			&CScriptActionPlanner::current_action_id)
			.def("current_action",				&CScriptActionPlanner::current_action)
			.def("initialized",					&CScriptActionPlanner::initialized)
			.def("set_goal_world_state",		(void (CScriptActionPlanner::*)(const CScriptActionPlanner::CState &))(CScriptActionPlanner::add_condition))
			.def("clear",						&CScriptActionPlanner::clear)
	];
}
