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

#include "ai_script_classes.h"

#include "script_world_property.h"
#include "script_world_state.h"
#include "script_property_evaluator_wrapper.h"
#include "script_action_wrapper.h"
#include "script_action_planner_wrapper.h"

#include "script_motivation_wrapper.h"
#include "script_motivation_action_wrapper.h"
#include "script_motivation_manager_wrapper.h"
#include "script_motivation_action_manager_wrapper.h"

using namespace luabind;
using namespace Script;

void Script::vfExportActionManagement(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CPropertyStorage>("property_storage")
			.def(								constructor<>())
			.def("set_property",				&CPropertyStorage::set_property)
			.def("property",					&CPropertyStorage::property),

		class_<CScriptWorldProperty>("world_property")
			.def(								constructor<CScriptWorldProperty::_condition_type, CScriptWorldProperty::_value_type>())
			.def("condition",					&CScriptWorldProperty::condition)
			.def("value",						&CScriptWorldProperty::value),
//			.def(const_self < CScriptWorldProperty())
//			.def(const_self == CScriptWorldProperty()),

		class_<CScriptWorldState>("world_state")
			.def(								constructor<>())
			.def(								constructor<CScriptWorldState>())
			.def("add_property",				(void (CScriptWorldState::*)(const CScriptWorldState::COperatorCondition &))(CScriptWorldState::add_condition))
			.def("remove_property",				(void (CScriptWorldState::*)(const CScriptWorldState::COperatorCondition::_condition_type &))(CScriptWorldState::remove_condition))
			.def("clear",						&CScriptWorldState::clear)
			.def("includes",					&CScriptWorldState::includes)
			.def("property",					&CScriptWorldState::property)
			.def(const_self < CScriptWorldState())
			.def(const_self == CScriptWorldState()),

		class_<CScriptPropertyEvaluator,CPropertyEvaluatorWrapper>("property_evaluator")
			.def_readonly("object",				&CScriptPropertyEvaluator::m_object)
			.def_readonly("storage",			&CScriptPropertyEvaluator::m_storage)
			.def(								constructor<>())
			.def(								constructor<CLuaGameObject*>())
			.def("reinit",						&CPropertyEvaluatorWrapper::reinit_static)
			.def("evaluate",					&CPropertyEvaluatorWrapper::evaluate_static),

		class_<CScriptAction,CScriptActionWrapper>("action_base")
			.def_readonly("object",				&CScriptAction::m_object)
			.def_readonly("storage",			&CScriptAction::m_storage)
			.def(								constructor<>())
			.def(								constructor<CLuaGameObject*>())
			.def(								constructor<CLuaGameObject*,LPCSTR>())
			.def("add_precondition",			(void (CScriptAction::*)(const CScriptAction::COperatorCondition &))(CScriptAction::add_condition))
			.def("add_effect",					(void (CScriptAction::*)(const CScriptAction::COperatorCondition &))(CScriptAction::add_effect))
			.def("remove_precondition",			(void (CScriptAction::*)(const CScriptAction::COperatorCondition &))(CScriptAction::remove_condition))
			.def("remove_effect",				(void (CScriptAction::*)(const CScriptAction::COperatorCondition &))(CScriptAction::remove_effect))
			.def("reinit",						&CScriptActionWrapper::reinit_static)
			.def("initialize",					&CScriptActionWrapper::initialize_static)
			.def("execute",						&CScriptActionWrapper::execute_static)
			.def("finalize",					&CScriptActionWrapper::finalize_static)
			.def("weight",						&CScriptActionWrapper::weight_static),

		class_<CScriptActionPlanner,CScriptActionPlannerWrapper>("action_planner")
			.def_readonly("object",				&CScriptActionPlanner::m_object)
			.def_readonly("storage",			&CScriptActionPlanner::m_storage)
			.def(								constructor<>())
			.def("reinit",						&CScriptActionPlannerWrapper::reinit_static)
			.def("update",						&CScriptActionPlannerWrapper::update_static)
			.def("add_action",					(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_edge_type &, CScriptActionPlanner::COperator *))(CScriptActionPlanner::add_operator))
			.def("remove_action",				(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_edge_type &))(CScriptActionPlanner::remove_operator))
			.def("action",						&CScriptActionPlanner::action)
			.def("add_evaluator",				(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &))(CScriptActionPlanner::add_evaluator))
			.def("remove_evaluator",			(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &))(CScriptActionPlanner::remove_evaluator))
			.def("evaluator",					(const CScriptActionPlanner::CConditionEvaluator * (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &) const)(CScriptActionPlanner::evaluator))
			.def("current_action_id",			&CScriptActionPlanner::current_action_id)
			.def("current_action",				&CScriptActionPlanner::current_action)
			.def("initialized",					&CScriptActionPlanner::initialized)
			.def("set_goal_world_state",		(void (CScriptActionPlanner::*)(const CScriptActionPlanner::CState &))(CScriptActionPlanner::add_condition))

	];
}

void Script::vfExportMotivationManagement(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CScriptMotivation,CScriptMotivationWrapper>("motivation")
			.def_readonly("object",				&CScriptMotivation::m_object)
			.def(								constructor<>())
			.def("reinit",						&CScriptMotivationWrapper::reinit_static)
			.def("load",						&CScriptMotivationWrapper::Load_static)
			.def("reload",						&CScriptMotivationWrapper::reload_static)
			.def("evaluate",					&CScriptMotivationWrapper::evaluate_static),

		class_<CScriptMotivationAction,CScriptMotivationActionWrapper>("motivation_action")
			.def(								constructor<const CScriptWorldState &>())
			.def("goal",						&CScriptMotivationAction::goal),

		class_<CScriptMotivationManager,CScriptMotivationManagerWrapper>("motivation_manager")
			.def(								constructor<>())
			.def("add_motivation",				&CScriptMotivationManager::add_motivation)
			.def("remove_motivation",			&CScriptMotivationManager::remove_motivation)
			.def("add_connection",				&CScriptMotivationManager::add_connection)
			.def("remove_connection",			&CScriptMotivationManager::remove_connection)
			.def("motivation",					&CScriptMotivationManager::motivation)
			.def("selected",					&CScriptMotivationManager::selected)
			.def("selected_id",					&CScriptMotivationManager::selected_id)
			.def("reinit",						&CScriptMotivationManagerWrapper::reinit_static)
			.def("load",						&CScriptMotivationManagerWrapper::Load_static)
			.def("reload",						&CScriptMotivationManagerWrapper::reload_static)
			.def("update",						&CScriptMotivationManagerWrapper::update_static),

		class_<CScriptMotivationActionManager,CScriptMotivationActionManagerWrapper,bases<CScriptMotivationManager,CScriptActionPlanner> >("motivation_action_manager")
			.def(								constructor<>())
			.def("reinit",						&CScriptMotivationActionManagerWrapper::reinit_static)
			.def("load",						&CScriptMotivationActionManagerWrapper::Load_static)
			.def("reload",						&CScriptMotivationActionManagerWrapper::reload_static)
			.def("update",						&CScriptMotivationActionManagerWrapper::update_static)

	];
}