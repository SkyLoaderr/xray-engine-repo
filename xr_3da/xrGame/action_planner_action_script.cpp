////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action_script.cpp
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner_action_wrapper.h"
#include "script_space.h"
#include "script_game_object.h"
#include <luabind/adopt_policy.hpp>

using namespace luabind;

template <typename T1, typename T2>
T1 *_dynamic_cast(T2 *p2)
{
	return			(dynamic_cast<T1*>(p2));
}

void bind_action_to_action_planner(CScriptActionPlanner *planner, const CScriptActionPlanner::_edge_type &id, CScriptActionPlannerAction *action)
{
	planner->add_operator	(id, action);
}

void CActionPlannerAction<CScriptGameObject>::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptActionPlannerAction,CScriptActionPlannerActionWrapper,bases<CScriptActionPlanner,CScriptActionBase> >("planner_action")
			.def(								constructor<>())
			.def(								constructor<CScriptGameObject*>())
			.def(								constructor<CScriptGameObject*,LPCSTR>())
			.def("reinit",						&CScriptActionPlannerActionWrapper::reinit,		&CScriptActionPlannerActionWrapper::reinit_static)
			.def("update",						&CScriptActionPlannerActionWrapper::update,		&CScriptActionPlannerActionWrapper::update_static)
			.def("initialize",					&CScriptActionPlannerActionWrapper::initialize,	&CScriptActionPlannerActionWrapper::initialize_static)
			.def("execute",						&CScriptActionPlannerActionWrapper::execute,	&CScriptActionPlannerActionWrapper::execute_static)
			.def("finalize",					&CScriptActionPlannerActionWrapper::finalize,	&CScriptActionPlannerActionWrapper::finalize_static)
			.def("weight",						&CScriptActionPlannerActionWrapper::weight,		&CScriptActionPlannerActionWrapper::weight_static),

		def		("cast_action_to_planner",		&_dynamic_cast<CScriptActionPlanner,CScriptActionBase>,adopt(return_value) + adopt(_1)),
		def		("cast_planner_to_action",		&_dynamic_cast<CScriptActionBase,CScriptActionPlanner>,adopt(return_value) + adopt(_1)),
		def		("bind_action_to_action_planner",&bind_action_to_action_planner,adopt(_3))
	];
}
