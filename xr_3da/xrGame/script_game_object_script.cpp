////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script.cpp
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
#include "script_monster_space.h"
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

using namespace luabind;

void CScriptGameObject::OnEventRaised(s16 event, NET_Packet& P)
{
	P.read_start();
	CALLBACK_IT it = m_callbacks.find(event);
	if(it==m_callbacks.end())
		return;
	SCRIPT_CALLBACK_EXECUTE_1((*it).second->m_callback, &P );
}

extern CScriptMotivationActionManager *script_motivation_action_manager(CScriptGameObject *obj);

void CScriptGameObject::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptGameObject>("game_object")
			.enum_("relation")
			[
				value("friend",					int(ALife::eRelationTypeFriend)),
				value("neutral",				int(ALife::eRelationTypeNeutral)),
				value("enemy",					int(ALife::eRelationTypeEnemy)),
				value("dummy",					int(ALife::eRelationTypeDummy))
			]
			.enum_("rank")
			[
				value("novice",					int(ALife::eStalkerRankNovice)),
				value("experienced",			int(ALife::eStalkerRankExperienced)),
				value("veteran",				int(ALife::eStalkerRankVeteran)),
				value("master",					int(ALife::eStalkerRankMaster)),
				value("dummy",					int(ALife::eStalkerRankDummy))
			]
			.enum_("action_types")
			[
				value("movement",				int(ScriptMonster::eActionTypeMovement)),
				value("watch",					int(ScriptMonster::eActionTypeWatch)),
				value("animation",				int(ScriptMonster::eActionTypeAnimation)),
				value("sound",					int(ScriptMonster::eActionTypeSound)),
				value("particle",				int(ScriptMonster::eActionTypeParticle)),
				value("object",					int(ScriptMonster::eActionTypeObject)),
				value("action_type_count",		int(ScriptMonster::eActionTypeCount))
			]
			.enum_("EPathType")
			[
				value("game_path",				int(MovementManager::ePathTypeGamePath)),
				value("level_path",				int(MovementManager::ePathTypeLevelPath)),
				value("enemy_search",			int(MovementManager::ePathTypeEnemySearch)),
				value("patrol_path",			int(MovementManager::ePathTypePatrolPath)),
				value("no_path",				int(MovementManager::ePathTypeNoPath))
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
			.def("AddEventCallback",			(void(CScriptGameObject::*)(s16, const luabind::object&, LPCSTR))CScriptGameObject::AddEventCallback)

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
			.def("set_callback",				(void (CScriptGameObject::*)(const luabind::functor<void> &, bool))(CScriptGameObject::SetCallback))
			.def("set_callback",				(void (CScriptGameObject::*)(const luabind::object &, LPCSTR, bool))(CScriptGameObject::SetCallback))
			.def("set_callback",				(void (CScriptGameObject::*)(const luabind::object &, LPCSTR, const ScriptMonster::EActionType))(CScriptGameObject::SetCallback))
			.def("set_callback",				(void (CScriptGameObject::*)(const luabind::functor<void> &, const ScriptMonster::EActionType))(CScriptGameObject::SetCallback))
			.def("clear_callback",				(void (CScriptGameObject::*)(bool))(CScriptGameObject::ClearCallback))
			.def("clear_callback",				(void (CScriptGameObject::*)(const ScriptMonster::EActionType))(CScriptGameObject::ClearCallback))
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
			.def("set_hit_callback",			(void (CScriptGameObject::*)(const luabind::object &, LPCSTR))(CScriptGameObject::SetHitCallback))
			.def("set_hit_callback",			(void (CScriptGameObject::*)(const luabind::functor<void> &))(CScriptGameObject::SetHitCallback))
			.def("clear_hit_callback",			&CScriptGameObject::ClearHitCallback)
			.def("set_hear_callback",			(void (CScriptGameObject::*)(const luabind::object &, LPCSTR))(CScriptGameObject::SetSoundCallback))
			.def("set_hear_callback",			(void (CScriptGameObject::*)(const luabind::functor<void> &))(CScriptGameObject::SetSoundCallback))
			.def("clear_hear_callback",			&CScriptGameObject::ClearSoundCallback)
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


			// movement manager
			.def("set_body_state",				&CScriptGameObject::set_body_state			)
			.def("set_movement_type",			&CScriptGameObject::set_movement_type		)
			.def("set_mental_state",			&CScriptGameObject::set_mental_state		)
			.def("set_path_type",				&CScriptGameObject::set_path_type			)
			.def("set_detail_path_type",		&CScriptGameObject::set_detail_path_type	)
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
			
			// sound_player
			
			.def("add_sound",					(u32 (CScriptGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32))(CScriptGameObject::add_sound))
			.def("add_sound",					(u32 (CScriptGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32,LPCSTR))(CScriptGameObject::add_sound))
			.def("add_sound",					(u32 (CScriptGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32,LPCSTR,LPCSTR))(CScriptGameObject::add_sound))
			.def("remove_sound",				&CScriptGameObject::remove_sound)
			.def("set_sound_mask",				&CScriptGameObject::set_sound_mask)
			.def("play_sound",					(void (CScriptGameObject::*)(u32))(CScriptGameObject::play_sound))
			.def("play_sound",					(void (CScriptGameObject::*)(u32,u32))(CScriptGameObject::play_sound))
			.def("play_sound",					(void (CScriptGameObject::*)(u32,u32,u32))(CScriptGameObject::play_sound))
			.def("play_sound",					(void (CScriptGameObject::*)(u32,u32,u32,u32))(CScriptGameObject::play_sound))
			.def("play_sound",					(void (CScriptGameObject::*)(u32,u32,u32,u32,u32))(CScriptGameObject::play_sound))
			.def("play_sound",					(void (CScriptGameObject::*)(u32,u32,u32,u32,u32,u32))(CScriptGameObject::play_sound))
			.def("binded_object",				&CScriptGameObject::binded_object)
			.def("set_previous_point",			&CScriptGameObject::set_previous_point)
			.def("set_start_point",				&CScriptGameObject::set_start_point)
			.def("enable_memory_object",		&CScriptGameObject::enable_memory_object)
			.def("active_sound_count",			(int (CScriptGameObject::*)())(&CScriptGameObject::active_sound_count))
			.def("active_sound_count",			(int (CScriptGameObject::*)(bool))(&CScriptGameObject::active_sound_count))
			.def("best_cover",					&CScriptGameObject::best_cover)
			.def("safe_cover",					&CScriptGameObject::safe_cover)
			.def("spawn_ini",					&CScriptGameObject::spawn_ini)
			.def("memory_visible_objects",		&CScriptGameObject::memory_visible_objects, return_stl_iterator)
			.def("memory_sound_objects",		&CScriptGameObject::memory_sound_objects, return_stl_iterator)
			.def("memory_hit_objects",			&CScriptGameObject::memory_hit_objects, return_stl_iterator)
			.def("not_yet_visible_objects",		&CScriptGameObject::not_yet_visible_objects, return_stl_iterator)
			.def("visibility_threshold",		&CScriptGameObject::visibility_threshold)
			.def("enable_vision",				&CScriptGameObject::enable_vision)
			.def("vision_enabled",				&CScriptGameObject::vision_enabled)
			.def("set_sound_threshold",			&CScriptGameObject::set_sound_threshold)
			.def("restore_sound_threshold",		&CScriptGameObject::restore_sound_threshold)

			// sight manager
			.def("set_sight",					(void (CScriptGameObject::*)(SightManager::ESightType sight_type, const Fvector *vector3d, u32 dwLookOverDelay))(CScriptGameObject::set_sight))
			.def("set_sight",					(void (CScriptGameObject::*)(SightManager::ESightType sight_type, bool torso_look, bool path))(CScriptGameObject::set_sight))
			.def("set_sight",					(void (CScriptGameObject::*)(SightManager::ESightType sight_type, const Fvector &vector3d, bool torso_look))(CScriptGameObject::set_sight))
			.def("set_sight",					(void (CScriptGameObject::*)(SightManager::ESightType sight_type, const Fvector *vector3d))(CScriptGameObject::set_sight))
			.def("set_sight",					(void (CScriptGameObject::*)(CScriptGameObject *object_to_look, bool torso_look))(CScriptGameObject::set_sight))
			.def("set_sight",					(void (CScriptGameObject::*)(CScriptGameObject *object_to_look, bool torso_look, LPCSTR bone_name))(CScriptGameObject::set_sight))
			.def("set_sight",					(void (CScriptGameObject::*)(const MemorySpace::CMemoryInfo *memory_object, bool	torso_look))(CScriptGameObject::set_sight))

			// object handler
			.def("set_item",					(void (CScriptGameObject::*)(MonsterSpace::EObjectAction ))(CScriptGameObject::set_item))
			.def("set_item",					(void (CScriptGameObject::*)(MonsterSpace::EObjectAction, CScriptGameObject *))(CScriptGameObject::set_item))

			.def("bone_position",				&CScriptGameObject::bone_position)

			.def("is_body_turning",				&CScriptGameObject::is_body_turning)

			//////////////////////////////////////////////////////////////////////////
			// Space restrictions
			//////////////////////////////////////////////////////////////////////////
			.def("add_restrictions",			&CScriptGameObject::add_restrictions)
			.def("remove_restrictions",			&CScriptGameObject::remove_restrictions)
			.def("remove_all_restrictions",		&CScriptGameObject::remove_all_restrictions)
			.def("in_restrictions",				&CScriptGameObject::in_restrictions)
			.def("out_restrictions",			&CScriptGameObject::out_restrictions)
			.def("accessible",					&CScriptGameObject::accessible_position)
			.def("accessible",					&CScriptGameObject::accessible_vertex_id)
			.def("accessible_nearest",			&CScriptGameObject::accessible_nearest, out_value(_3))

			//////////////////////////////////////////////////////////////////////////
			.def("limping",						&CScriptGameObject::limping)
			//////////////////////////////////////////////////////////////////////////
			.def("enable_attachable_item",		&CScriptGameObject::enable_attachable_item)
			.def("attachable_item_enabled",		&CScriptGameObject::attachable_item_enabled)

			//////////////////////////////////////////////////////////////////////////
			//inventory owner
			//////////////////////////////////////////////////////////////////////////

			.enum_("EPdaMsg")
			[
				value("dialog_pda_msg",			int(ePdaMsgDialog)),
				value("info_pda_msg",			int(ePdaMsgInfo)),
				value("no_pda_msg",				int(ePdaMsgMax))
			]

			.def("set_pda_callback",			(void (CScriptGameObject::*)(const luabind::functor<void>&))(CScriptGameObject::SetPdaCallback))
			.def("set_pda_callback",			(void (CScriptGameObject::*)(const luabind::object &, LPCSTR))(CScriptGameObject::SetPdaCallback))
			.def("set_info_callback",			(void (CScriptGameObject::*)(const luabind::functor<void>&))(CScriptGameObject::SetInfoCallback))
			.def("set_info_callback",			(void (CScriptGameObject::*)(const luabind::object &, LPCSTR))(CScriptGameObject::SetInfoCallback))
			.def("clear_pda_callback",			&CScriptGameObject::ClearPdaCallback)
			.def("clear_info_callback",			&CScriptGameObject::ClearInfoCallback)

			.def("give_info_portion",			&CScriptGameObject::GiveInfoPortion)
			.def("give_info_portion_via_pda",	&CScriptGameObject::GiveInfoPortionViaPda)
			.def("disable_info_portion",		&CScriptGameObject::DisableInfoPortion)
			.def("has_info",					&CScriptGameObject::HasInfo)
			.def("dont_has_info",				&CScriptGameObject::DontHasInfo)

			.def("get_task_state",				&CScriptGameObject::GetGameTaskState)
			.def("set_task_state",				&CScriptGameObject::SetGameTaskState)

			.def("send_pda_message",			&CScriptGameObject::SendPdaMessage)

			.def("is_talking",					&CScriptGameObject::IsTalking)
			.def("stop_talk",					&CScriptGameObject::StopTalk)
			.def("enable_talk",					&CScriptGameObject::EnableTalk)
			.def("disable_talk",				&CScriptGameObject::DisableTalk)
			.def("is_talk_enabled",				&CScriptGameObject::IsTalkEnabled)

			.def("transfer_item",				&CScriptGameObject::TransferItem)
			.def("transfer_money",				&CScriptGameObject::TransferMoney)
			.def("money",						&CScriptGameObject::Money)
			
			.def("switch_to_trade",				&CScriptGameObject::SwitchToTrade)
			.def("switch_to_talk",				&CScriptGameObject::SwitchToTalk)
			.def("run_talk_dialog",				&CScriptGameObject::RunTalkDialog)
			.def("actor_sleep",					&CScriptGameObject::ActorSleep)

			.def("set_start_dialog",			&CScriptGameObject::SetStartDialog)
			.def("get_start_dialog",			&CScriptGameObject::GetStartDialog)
			.def("restore_default_start_dialog",&CScriptGameObject::RestoreDefaultStartDialog)

			.def("set_goodwill",				&CScriptGameObject::SetGoodwill)
			.def("goodwill",					&CScriptGameObject::GetGoodwill)
			.def("set_relation",				&CScriptGameObject::SetRelation)
			
			.def("need_to_answer_pda",			&CScriptGameObject::NeedToAnswerPda)
			.def("answer_pda",					&CScriptGameObject::AnswerPda)
			
			.def("character_name",				&CScriptGameObject::CharacterName)
			.def("character_rank",				&CScriptGameObject::CharacterRank)
			.def("set_character_rank",			&CScriptGameObject::SetCharacterRank)
			.def("character_reputation",		&CScriptGameObject::CharacterReputation)
			.def("set_character_reputation",	&CScriptGameObject::SetCharacterReputation)
			.def("character_community",			&CScriptGameObject::CharacterCommunity)
			.def("set_character_community",		&CScriptGameObject::SetCharacterCommunity)
			

			//////////////////////////////////////////////////////////////////////////
			// Trader
			//////////////////////////////////////////////////////////////////////////
			.def("set_trade_callback",			(void (CScriptGameObject::*)(const luabind::functor<void> &))(CScriptGameObject::SetTradeCallback))
			.def("set_trade_callback",			(void (CScriptGameObject::*)(const luabind::object &, LPCSTR))(CScriptGameObject::SetTradeCallback))			
			.def("clear_trade_callback",		(void (CScriptGameObject::*)())(CScriptGameObject::ClearTradeCallback))

			.enum_("CLSIDS")
			[
				value("no_pda_msg",				int(ePdaMsgMax))
			]
			
			//HELICOPTER
			
			.def("get_helicopter",              &CScriptGameObject::get_helicopter)
			.def("get_physics_shell",			&CScriptGameObject::get_physics_shell)

			//usable object
			.def("set_use_callback",			(void (CScriptGameObject::*)(const luabind::functor<void> &))(CScriptGameObject::SetUseCallback))
			.def("set_use_callback",			(void (CScriptGameObject::*)(const luabind::object &, LPCSTR))(CScriptGameObject::SetUseCallback))
			.def("clear_use_callback",			(void (CScriptGameObject::*)(void))(CScriptGameObject::ClearUseCallback))
			.def("set_tip_text",				&CScriptGameObject::SetTipText)
			.def("set_tip_text_default",		&CScriptGameObject::SetTipTextDefault)
			.def("set_nonscript_usable",		&CScriptGameObject::SetNonscriptUsable)
	];
}