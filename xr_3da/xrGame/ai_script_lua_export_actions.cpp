////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export_actions.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export actions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"

#include "ai_script_classes.h"

#include "xrServer_Objects.h"

#include "script_world_property.h"
#include "script_world_state.h"
#include "script_property_evaluator_wrapper.h"
#include "script_action_wrapper.h"
#include "script_action_planner_wrapper.h"
#include "script_action_planner_action_wrapper.h"

#include "script_motivation_wrapper.h"
#include "script_motivation_action_wrapper.h"
#include "script_motivation_manager_wrapper.h"
#include "script_motivation_action_manager_wrapper.h"
#include "script_binder_object_wrapper.h"

#include "stalker_decision_space.h"
#include "property_evaluator_const.h"

using namespace luabind;

void CScriptEngine::export_action_management()
{
	module(lua())
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

		class_<CPropertyEvaluatorConst<CLuaGameObject>, CScriptPropertyEvaluator>("property_evaluator_const")
		.def(									constructor<CPropertyEvaluatorConst<CLuaGameObject>::_value_type>()),

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
			.def("weight",						&CScriptActionWrapper::weight_static)
			.def("set_weight",					&CScriptAction::set_weight),

		class_<CScriptActionPlanner,CScriptActionPlannerWrapper>("action_planner")
			.def_readonly("object",				&CScriptActionPlanner::m_object)
			.def_readonly("storage",			&CScriptActionPlanner::m_storage)
			.def(								constructor<>())
			.def("reinit",						&CScriptActionPlannerWrapper::reinit_static)
			.def("update",						&CScriptActionPlannerWrapper::update_static)
			.def("add_action",					(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_edge_type &, CScriptActionPlanner::COperator *))(CScriptActionPlanner::add_operator),adopt(_2))
			.def("remove_action",				(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_edge_type &))(CScriptActionPlanner::remove_operator))
			.def("action",						&CScriptActionPlanner::action)
			.def("add_evaluator",				(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &, CScriptActionPlanner::CConditionEvaluator *))(CScriptActionPlanner::add_evaluator),adopt(_2))
			.def("remove_evaluator",			(void (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &))(CScriptActionPlanner::remove_evaluator))
			.def("evaluator",					(const CScriptActionPlanner::CConditionEvaluator * (CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type &) const)(CScriptActionPlanner::evaluator))
			.def("current_action_id",			&CScriptActionPlanner::current_action_id)
			.def("current_action",				&CScriptActionPlanner::current_action)
			.def("initialized",					&CScriptActionPlanner::initialized)
			.def("set_goal_world_state",		(void (CScriptActionPlanner::*)(const CScriptActionPlanner::CState &))(CScriptActionPlanner::add_condition))
			.def("clear",						&CScriptActionPlanner::clear),

		class_<CScriptActionPlannerAction,CScriptActionPlannerActionWrapper,bases<CScriptActionPlanner,CScriptAction> >("planner_action")
			.def(								constructor<>())
			.def("reinit",						&CScriptActionPlannerActionWrapper::reinit_static)
	];
}

struct CStalkerDecisionBlock{};

void CScriptEngine::export_motivation_management()
{
	module(lua())
	[
		class_<CScriptMotivation,CScriptMotivationWrapper>("motivation")
			.def_readonly("object",				&CScriptMotivation::m_object)
			.def(								constructor<>())
			.def("reinit",						&CScriptMotivationWrapper::reinit_static)
			.def("load",						&CScriptMotivationWrapper::Load_static)
			.def("reload",						&CScriptMotivationWrapper::reload_static)
			.def("evaluate",					&CScriptMotivationWrapper::evaluate_static),

		class_<CScriptMotivationAction,CScriptMotivationActionWrapper,CScriptMotivation>("motivation_action")
			.def(								constructor<const CScriptWorldState &>())
			.def("goal",						&CScriptMotivationAction::goal),

		class_<CScriptMotivationManager,CScriptMotivationManagerWrapper>("motivation_manager")
			.def(								constructor<>())
			.def("add_motivation",				&CScriptMotivationManager::add_motivation,adopt(_2))
			.def("remove_motivation",			&CScriptMotivationManager::remove_motivation)
			.def("add_connection",				&CScriptMotivationManager::add_connection)
			.def("remove_connection",			&CScriptMotivationManager::remove_connection)
			.def("motivation",					&CScriptMotivationManager::motivation)
			.def("selected",					&CScriptMotivationManager::selected)
			.def("selected_id",					&CScriptMotivationManager::selected_id)
			.def("reinit",						&CScriptMotivationManagerWrapper::reinit_static)
			.def("load",						&CScriptMotivationManagerWrapper::Load_static)
			.def("reload",						&CScriptMotivationManagerWrapper::reload_static)
			.def("update",						&CScriptMotivationManagerWrapper::update_static)
			.def("clear",						&CScriptMotivationManager::clear),

		class_<CScriptMotivationActionManager,CScriptMotivationActionManagerWrapper,bases<CScriptMotivationManager,CScriptActionPlanner> >("motivation_action_manager")
			.def(								constructor<>())
			.def("reinit",						&CScriptMotivationActionManagerWrapper::reinit_static)
			.def("load",						&CScriptMotivationActionManagerWrapper::Load_static)
			.def("reload",						&CScriptMotivationActionManagerWrapper::reload_static)
			.def("update",						&CScriptMotivationActionManagerWrapper::update_static)
			.def("clear",						&CScriptMotivationActionManager::clear),

		class_<CSE_Abstract>("server_object"),
			
		class_<NET_Packet>("net_packet")
			.def(								constructor<>()),
			
		class_<CScriptBinderObject,CScriptBinderObjectWrapper>("object_binder")
			.def_readonly("object",				&CScriptBinderObject::m_object)
			.def(								constructor<CLuaGameObject*>())
			.def("reinit",						&CScriptBinderObjectWrapper::reinit_static)
			.def("reload",						&CScriptBinderObjectWrapper::reload_static)
			.def("net_spawn",					&CScriptBinderObjectWrapper::net_Spawn_static)
			.def("net_destroy",					&CScriptBinderObjectWrapper::net_Destroy_static)
			.def("net_import",					&CScriptBinderObjectWrapper::net_Import_static)
			.def("net_export",					&CScriptBinderObjectWrapper::net_Export_static),
			
		class_<CStalkerDecisionBlock>("stalker_ids")
			.enum_("motivations")
			[
				value("motivation_global",			StalkerDecisionSpace::eMotivationGlobal),
				value("motivation_alive",			StalkerDecisionSpace::eMotivationAlive),
				value("motivation_dead",			StalkerDecisionSpace::eMotivationDead),
				value("motivation_solve_zone_puzzle",StalkerDecisionSpace::eMotivationSolveZonePuzzle),
				value("motivation_script",			StalkerDecisionSpace::eMotivationScript)
			]
			.enum_("properties")
			[
				value("property_alive",				StalkerDecisionSpace::eWorldPropertyAlive),
				value("property_dead",				StalkerDecisionSpace::eWorldPropertyDead),
				value("property_already_dead",		StalkerDecisionSpace::eWorldPropertyAlreadyDead),
				value("property_alife",				StalkerDecisionSpace::eWorldPropertyALife),
				value("property_puzzle_solved",		StalkerDecisionSpace::eWorldPropertyPuzzleSolved),
				value("property_items",				StalkerDecisionSpace::eWorldPropertyItems),
				value("property_enemy",				StalkerDecisionSpace::eWorldPropertyEnemy),
				value("property_see_enemy",			StalkerDecisionSpace::eWorldPropertySeeEnemy),
				value("property_item_to_kill",		StalkerDecisionSpace::eWorldPropertyItemToKill),
				value("property_found_item_to_kill",StalkerDecisionSpace::eWorldPropertyFoundItemToKill),
				value("property_item_can_kill",		StalkerDecisionSpace::eWorldPropertyItemCanKill),
				value("property_found_ammo",		StalkerDecisionSpace::eWorldPropertyFoundAmmo),
				value("property_ready_to_kill",		StalkerDecisionSpace::eWorldPropertyReadyToKill),
				value("property_kill_distance",		StalkerDecisionSpace::eWorldPropertyKillDistance),
				value("property_script",			StalkerDecisionSpace::eWorldPropertyScript)
			]
			.enum_("action")
			[
				value("action_already_dead",		StalkerDecisionSpace::eWorldOperatorAlreadyDead),
				value("action_dead",				StalkerDecisionSpace::eWorldOperatorDead),
				value("action_free_no_alife",		StalkerDecisionSpace::eWorldOperatorFreeNoALife),
				value("action_free",				StalkerDecisionSpace::eWorldOperatorFreeALife),
				value("action_gather_items",		StalkerDecisionSpace::eWorldOperatorGatherItems),
				value("action_get_kill_distance",	StalkerDecisionSpace::eWorldOperatorGetKillDistance),
				value("action_get_enemy",			StalkerDecisionSpace::eWorldOperatorGetEnemy),
				value("action_get_enemy_seen",		StalkerDecisionSpace::eWorldOperatorGetEnemySeen),
				value("action_get_item_to_kill",	StalkerDecisionSpace::eWorldOperatorGetItemToKill),
				value("action_find_item_to_kill",	StalkerDecisionSpace::eWorldOperatorFindItemToKill),
				value("action_make_item_killing",	StalkerDecisionSpace::eWorldOperatorMakeItemKilling),
				value("action_find_ammo",			StalkerDecisionSpace::eWorldOperatorFindAmmo),
				value("action_script",				StalkerDecisionSpace::eWorldOperatorScript),
				value("action_get_ready_to_kill_very_aggressive",	StalkerDecisionSpace::eWorldOperatorGetReadyToKillVeryAggressive),
				value("action_kill_enemy_very_aggressive",			StalkerDecisionSpace::eWorldOperatorKillEnemyVeryAggressive),
				value("action_get_ready_to_kill_aggressive",		StalkerDecisionSpace::eWorldOperatorGetReadyToKillAggressive),
				value("action_kill_enemy_aggressive",				StalkerDecisionSpace::eWorldOperatorKillEnemyAggressive),
				value("action_get_ready_to_kill_moderate",			StalkerDecisionSpace::eWorldOperatorGetReadyToKillModerate),
				value("action_kill_enemy_moderate",					StalkerDecisionSpace::eWorldOperatorKillEnemyModerate),
				value("action_get_ready_to_kill_avoid",				StalkerDecisionSpace::eWorldOperatorGetReadyToKillAvoid),
				value("action_kill_enemy_avoid",					StalkerDecisionSpace::eWorldOperatorKillEnemyAvoid),
				value("action_retreat_from_enemy",					StalkerDecisionSpace::eWorldOperatorRetreatFromEnemy)
			]
	];
}