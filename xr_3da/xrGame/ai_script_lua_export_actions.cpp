////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export_actions.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export actions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_space.h"
#include "ai_script_lua_extension.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
#include "script_world_property.h"
#include "script_world_state.h"
#include "script_property_evaluator.h"
#include "script_action.h"
#include "script_action_planner.h"
#include "ai_script_classes.h"

using namespace luabind;
using namespace Script;

void Script::vfExportActionManagement(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CGraphEngine::CWorldProperty>("world_property_abstract")
			.def(								constructor<CGraphEngine::CWorldProperty::_condition_type, CGraphEngine::CWorldProperty::_value_type>()),

		class_<CScriptWorldProperty,CGraphEngine::CWorldProperty>("world_property")
			.def(								constructor<CScriptWorldProperty::_condition_type, CScriptWorldProperty::_value_type>())
			.def("condition",					&CScriptWorldProperty::condition)
			.def("value",						&CScriptWorldProperty::value)
			.def(const_self < CScriptWorldProperty())
			.def(const_self == CScriptWorldProperty()),

		class_<CGraphEngine::CWorldState>("world_state_abstract")
			.def(								constructor<>()),

		class_<CScriptWorldState,CGraphEngine::CWorldState>("world_state")
			.def(								constructor<>())
			.def(								constructor<CScriptWorldState>())
			.def("add_property",				(void (CScriptWorldState::*)(const CScriptWorldState::COperatorCondition &))(CScriptWorldState::add_condition))
			.def("remove_property",				&CScriptWorldState::remove_condition)
			.def("clear",						&CScriptWorldState::clear)
			.def("includes",					&CScriptWorldState::includes)
			.def("property",					&CScriptWorldState::property)
			.def(const_self < CScriptWorldState())
			.def(const_self == CScriptWorldState()),

		class_<CPropertyEvaluator<CLuaGameObject> >("property_evaluator_abstract")
			.def(								constructor<>()),

		class_<CScriptPropertyEvaluator,CPropertyEvaluatorWrapper,CPropertyEvaluator<CLuaGameObject> >("property_evaluator")
			.def(								constructor<>())
			.def(								constructor<CLuaGameObject*>())
			.def("reinit",						&CPropertyEvaluatorWrapper::reinit_static)
			.def("evaluate",					&CPropertyEvaluatorWrapper::evaluate_static),

		class_<CActionBase<CLuaGameObject> >("action_abstract")
			.def(								constructor<>()),

		class_<CScriptAction,CScriptActionWrapper,CActionBase<CLuaGameObject> >("action_base")
			.def(								constructor<>())
			.def(								constructor<CLuaGameObject*>())
			.def(								constructor<CLuaGameObject*,LPCSTR>())
			.def("add_precondition",			&CScriptAction::add_condition)
			.def("add_effect",					&CScriptAction::add_effect)
			.def("remove_precondition",			&CScriptAction::remove_condition)
			.def("remove_effect",				&CScriptAction::remove_effect)
			.def("reinit",						&CScriptActionWrapper::reinit_static)
			.def("initialize",					&CScriptActionWrapper::initialize_static)
			.def("execute",						&CScriptActionWrapper::execute_static)
			.def("finalize",					&CScriptActionWrapper::finalize_static)
			.def("weight",						&CScriptActionWrapper::weight_static),

		class_<CActionPlanner<CLuaGameObject> >("action_planner_abstract")
			.def(								constructor<>()),

		class_<CScriptActionPlanner,CScriptActionPlannerWrapper,CActionPlanner<CLuaGameObject> >("action_planner")
			.def(								constructor<>())
			.def("reinit",						&CScriptActionPlannerWrapper::reinit_static)
			.def("update",						&CScriptActionPlannerWrapper::update_static)
			.def("add_action",					&CScriptActionPlanner::add_operator)
			.def("remove_action",				&CScriptActionPlanner::remove_operator)
			.def("action",						&CScriptActionPlanner::action)
			.def("add_evaluator",				&CScriptActionPlanner::add_evaluator)
			.def("remove_evaluator",			&CScriptActionPlanner::remove_evaluator)
			.def("evaluator",					&CScriptActionPlanner::evaluator)
			.def("current_action_id",			&CScriptActionPlanner::current_action_id)
			.def("current_action",				&CScriptActionPlanner::current_action)
			.def("initialized",					&CScriptActionPlanner::initialized)
			.def("set_goal_world_state",		&CScriptActionPlanner::set_target_state)
	];
}
