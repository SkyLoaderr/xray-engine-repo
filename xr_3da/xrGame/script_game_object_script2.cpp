////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script2.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script game object script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include <luabind/adopt_policy.hpp>
#include <luabind/iterator_policy.hpp>
#include <luabind/out_value_policy.hpp>

#include "script_game_object.h"

#include "alife_space.h"
#include "script_entity_space.h"
#include "movement_manager_space.h"
#include "pda_space.h"
#include "memory_space.h"
#include "cover_point.h"
#include "script_hit.h"
#include "script_binder_object.h"
#include "script_ini_file.h"
#include "script_sound_info.h"
#include "script_monster_hit_info.h"
#include "script_entity_action.h"
#include "motivation_action_manager.h"
#include "PhysicsShell.h"
#include "helicopter.h"
#include "script_zone.h"
#include "relation_registry.h"

using namespace luabind;

extern CScriptMotivationActionManager *script_motivation_action_manager(CScriptGameObject *obj);

class_<CScriptGameObject> &script_register_game_object1(class_<CScriptGameObject> &instance)
{
	instance
		.enum_("relation")
		[
			value("friend",					int(ALife::eRelationTypeFriend)),
			value("neutral",				int(ALife::eRelationTypeNeutral)),
			value("enemy",					int(ALife::eRelationTypeEnemy)),
			value("dummy",					int(ALife::eRelationTypeDummy))
		]
		.enum_("action_types")
		[
			value("movement",				int(ScriptEntity::eActionTypeMovement)),
			value("watch",					int(ScriptEntity::eActionTypeWatch)),
			value("animation",				int(ScriptEntity::eActionTypeAnimation)),
			value("sound",					int(ScriptEntity::eActionTypeSound)),
			value("particle",				int(ScriptEntity::eActionTypeParticle)),
			value("object",					int(ScriptEntity::eActionTypeObject)),
			value("action_type_count",		int(ScriptEntity::eActionTypeCount))
		]
		.enum_("EPathType")
		[
			value("game_path",				int(MovementManager::ePathTypeGamePath)),
			value("level_path",				int(MovementManager::ePathTypeLevelPath)),
			value("patrol_path",			int(MovementManager::ePathTypePatrolPath)),
			value("no_path",				int(MovementManager::ePathTypeNoPath))
		]
		
		.enum_("callback_types")
		[
			value("trade_start",			int(GameObject::eTradeStart)),
			value("trade_stop",				int(GameObject::eTradeStop)),
			value("trade_sell_buy_item",	int(GameObject::eTradeSellBuyItem)),
			value("trade_perform_operation",int(GameObject::eTradePerformTradeOperation)),
			value("zone_enter",				int(GameObject::eZoneEnter)),
			value("zone_exit",				int(GameObject::eZoneExit)),
			value("death",					int(GameObject::eDeath)),
			value("patrol_path_in_point",	int(GameObject::ePatrolPathInPoint)),
			value("inventory_pda",			int(GameObject::eInventoryPda)),
			value("inventory_info",			int(GameObject::eInventoryInfo)),
			value("use_object",				int(GameObject::eUseObject)),
			value("hit",					int(GameObject::eHit))
		]

		.property("visible",				&CScriptGameObject::getVisible,		&CScriptGameObject::setVisible)
		.property("enabled",				&CScriptGameObject::getEnabled,		&CScriptGameObject::setEnabled)
		.property("health",					&CScriptGameObject::GetHealth,			&CScriptGameObject::SetHealth)
		.property("power",					&CScriptGameObject::GetPower,			&CScriptGameObject::SetPower)
		.property("satiety",				&CScriptGameObject::GetSatiety,		&CScriptGameObject::SetSatiety)
		.property("radiation",				&CScriptGameObject::GetRadiation,		&CScriptGameObject::SetRadiation)
		.property("circumspection",			&CScriptGameObject::GetCircumspection,	&CScriptGameObject::SetCircumspection)
		.property("morale",					&CScriptGameObject::GetMorale,			&CScriptGameObject::SetMorale)

		.def("AddEventCallback",			(void(CScriptGameObject::*)(s16, const luabind::functor<void>&))CScriptGameObject::AddEventCallback)
		.def("AddEventCallback",			(void(CScriptGameObject::*)(s16, const luabind::functor<void>&, const luabind::object&))CScriptGameObject::AddEventCallback)
		.def("RemoveEventCallback",			&CScriptGameObject::RemoveEventCallback)

		.def("position",					&CScriptGameObject::Position)
		.def("direction",					&CScriptGameObject::Direction)
		.def("clsid",						&CScriptGameObject::clsid)
		.def("id",							&CScriptGameObject::ID)
		.def("section",						&CScriptGameObject::Section)
		.def("name",						&CScriptGameObject::Name)
		.def("parent",						&CScriptGameObject::Parent)
		.def("mass",						&CScriptGameObject::Mass)
		.def("cost",						&CScriptGameObject::Cost)
		.def("death_time",					&CScriptGameObject::DeathTime)
		.def("armor",						&CScriptGameObject::Armor)
		.def("max_health",					&CScriptGameObject::DeathTime)
		.def("accuracy",					&CScriptGameObject::Accuracy)
		.def("alive",						&CScriptGameObject::Alive)
		.def("team",						&CScriptGameObject::Team)
		.def("squad",						&CScriptGameObject::Squad)
		.def("group",						&CScriptGameObject::Group)
		.def("change_team",					(void (CScriptGameObject::*)(u8,u8,u8))(CScriptGameObject::ChangeTeam))
		.def("kill",						&CScriptGameObject::Kill)
		.def("hit",							&CScriptGameObject::Hit)
		.def("fov",							&CScriptGameObject::GetFOV)
		.def("range",						&CScriptGameObject::GetRange)
		.def("relation",					&CScriptGameObject::GetRelationType)
		.def("script",						&CScriptGameObject::SetScriptControl)
		.def("get_script",					&CScriptGameObject::GetScriptControl)
		.def("get_script_name",				&CScriptGameObject::GetScriptControlName)
		.def("reset_action_queue",			&CScriptGameObject::ResetActionQueue)
		.def("see",							&CScriptGameObject::CheckObjectVisibility)
		.def("see",							&CScriptGameObject::CheckTypeVisibility)

		.def("who_hit_name",				&CScriptGameObject::WhoHitName)
		.def("who_hit_section_name",		&CScriptGameObject::WhoHitSectionName)
		
		.def("use",							&CScriptGameObject::UseObject)
		.def("rank",						&CScriptGameObject::GetRank)
		.def("command",						CScriptGameObject::AddAction)
		.def("action",						&CScriptGameObject::GetCurrentAction, adopt(result))
		.def("object_count",				&CScriptGameObject::GetInventoryObjectCount)
		.def("object",						(CScriptGameObject *(CScriptGameObject::*)(LPCSTR))(CScriptGameObject::GetObjectByName))
		.def("object",						(CScriptGameObject *(CScriptGameObject::*)(int))(CScriptGameObject::GetObjectByIndex))
		.def("active_item",					&CScriptGameObject::GetActiveItem)
		
		.def("set_callback",				(void (CScriptGameObject::*)(GameObject::ECallbackType, const luabind::functor<void> &))(CScriptGameObject::SetCallback))
		.def("set_callback",				(void (CScriptGameObject::*)(GameObject::ECallbackType, const luabind::functor<void> &, const luabind::object &))(CScriptGameObject::SetCallback))
		.def("clear_callback",				(void (CScriptGameObject::*)(GameObject::ECallbackType))(CScriptGameObject::ClearCallback))

		//.def("set_callback",				(void (CScriptGameObject::*)(const luabind::object &, LPCSTR, const ScriptEntity::EActionType))(CScriptGameObject::SetCallback))
		//.def("set_callback",				(void (CScriptGameObject::*)(const luabind::functor<void> &, const ScriptEntity::EActionType))(CScriptGameObject::SetCallback))
		//.def("clear_callback",				(void (CScriptGameObject::*)(bool))(CScriptGameObject::ClearCallback))
		//.def("clear_callback",				(void (CScriptGameObject::*)(const ScriptEntity::EActionType))(CScriptGameObject::ClearCallback))
		
		.def("patrol",						&CScriptGameObject::GetPatrolPathName)

		.def("get_ammo_in_magazine",		&CScriptGameObject::GetAmmoElapsed)
		.def("get_ammo_total",				&CScriptGameObject::GetAmmoCurrent)
		.def("set_ammo_elapsed",			&CScriptGameObject::SetAmmoElapsed)
		.def("set_queue_size",				&CScriptGameObject::SetQueueSize)
		.def("best_hit",					&CScriptGameObject::GetBestHit)
		.def("best_sound",					&CScriptGameObject::GetBestSound)
		.def("best_enemy",					&CScriptGameObject::GetBestEnemy)
		.def("best_item",					&CScriptGameObject::GetBestItem)
		.def("action_count",				&CScriptGameObject::GetActionCount)
		.def("action_by_index",				&CScriptGameObject::GetActionByIndex)
		
		//.def("set_hear_callback",			(void (CScriptGameObject::*)(const luabind::object &, LPCSTR))(CScriptGameObject::SetSoundCallback))
		//.def("set_hear_callback",			(void (CScriptGameObject::*)(const luabind::functor<void> &))(CScriptGameObject::SetSoundCallback))
		//.def("clear_hear_callback",			&CScriptGameObject::ClearSoundCallback)
		
		.def("memory",						&CScriptGameObject::memory, adopt(result))
		.def("best_weapon",					&CScriptGameObject::best_weapon)
		.def("explode",						&CScriptGameObject::explode)
		.def("get_enemy",					&CScriptGameObject::GetEnemy)
		.def("get_corpse",					&CScriptGameObject::GetCorpse)
		.def("get_enemy_strength",			&CScriptGameObject::GetEnemyStrength)
		.def("get_sound_info",				&CScriptGameObject::GetSoundInfo)
		.def("get_monster_hit_info",		&CScriptGameObject::GetMonsterHitInfo)
		.def("bind_object",					&CScriptGameObject::bind_object,adopt(_2))
		.def("motivation_action_manager",	&script_motivation_action_manager)

		// bloodsucker
		.def("set_invisible",				&CScriptGameObject::set_invisible)
		.def("set_manual_invisibility",		&CScriptGameObject::set_manual_invisibility)
		.def("get_invisible",				&CScriptGameObject::get_invisible)
		.def("get_manual_invisibility",		&CScriptGameObject::get_manual_invisibility)

		// searchlight
		.def("get_current_direction",		&CScriptGameObject::GetCurrentDirection)

		// movement manager
		.def("set_body_state",				&CScriptGameObject::set_body_state			)
		.def("set_movement_type",			&CScriptGameObject::set_movement_type		)
		.def("set_mental_state",			&CScriptGameObject::set_mental_state		)
		.def("set_path_type",				&CScriptGameObject::set_path_type			)
		.def("set_detail_path_type",		&CScriptGameObject::set_detail_path_type	)

		.def("body_state",					&CScriptGameObject::body_state				)
		.def("movement_type",				&CScriptGameObject::movement_type			)
		.def("mental_state",				&CScriptGameObject::mental_state			)
		.def("path_type",					&CScriptGameObject::path_type				)
		.def("detail_path_type",			&CScriptGameObject::detail_path_type		)

		//
		.def("set_desired_position",		(void (CScriptGameObject::*)())(CScriptGameObject::set_desired_position))
		.def("set_desired_position",		(void (CScriptGameObject::*)(const Fvector *))(CScriptGameObject::set_desired_position))
		.def("set_desired_direction",		(void (CScriptGameObject::*)())(CScriptGameObject::set_desired_direction))
		.def("set_desired_direction",		(void (CScriptGameObject::*)(const Fvector *))(CScriptGameObject::set_desired_direction))
		.def("set_node_evaluator",			(void (CScriptGameObject::*)())(CScriptGameObject::set_node_evaluator))
		.def("set_node_evaluator",			(void (CScriptGameObject::*)(CAbstractVertexEvaluator *))(CScriptGameObject::set_node_evaluator))
		.def("set_path_evaluator",			(void (CScriptGameObject::*)())(CScriptGameObject::set_path_evaluator))
		.def("set_path_evaluator",			(void (CScriptGameObject::*)(CAbstractVertexEvaluator *))(CScriptGameObject::set_path_evaluator))
		.def("set_patrol_path",				&CScriptGameObject::set_patrol_path)
		.def("set_dest_level_vertex_id",	&CScriptGameObject::set_dest_level_vertex_id)
		.def("level_vertex_id",				&CScriptGameObject::level_vertex_id)
		.def("add_animation",				(void (CScriptGameObject::*)(LPCSTR))(CScriptGameObject::add_animation))
		.def("add_animation",				(void (CScriptGameObject::*)(LPCSTR, bool))(CScriptGameObject::add_animation))
		.def("clear_animations",			&CScriptGameObject::clear_animations)
		.def("animation_count",				&CScriptGameObject::animation_count)
		.def("eat",							&CScriptGameObject::eat)

		.def("set_patrol_extrapolate_callback",	(void (CScriptGameObject::*)())(CScriptGameObject::set_patrol_extrapolate_callback))
		.def("set_patrol_extrapolate_callback",	(void (CScriptGameObject::*)(const luabind::functor<bool> &))(CScriptGameObject::set_patrol_extrapolate_callback))
		.def("set_patrol_extrapolate_callback",	(void (CScriptGameObject::*)(const luabind::functor<bool> &, const luabind::object &))(CScriptGameObject::set_patrol_extrapolate_callback))

		.def("extrapolate_length",			(float (CScriptGameObject::*)() const)(CScriptGameObject::extrapolate_length))
		.def("extrapolate_length",			(void (CScriptGameObject::*)(float))(CScriptGameObject::extrapolate_length))

		.def("set_fov",						&CScriptGameObject::set_fov)
		.def("set_range",					&CScriptGameObject::set_range)

	;return	(instance);
}