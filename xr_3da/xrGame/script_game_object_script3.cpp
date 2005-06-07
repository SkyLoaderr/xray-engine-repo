////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script3.cpp
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
#include "HangingLamp.h"
#include "holder_custom.h"
#include "script_zone.h"
#include "relation_registry.h"
#include "GameTaskDefs.h"

using namespace luabind;

class_<CScriptGameObject> &script_register_game_object2(class_<CScriptGameObject> &instance)
{
	instance
		.def("add_sound",					(u32 (CScriptGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32))(CScriptGameObject::add_sound))
		.def("add_sound",					(u32 (CScriptGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32,LPCSTR))(CScriptGameObject::add_sound))
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
		.def("get_current_point_index",		&CScriptGameObject::get_current_patrol_point_index)
		.def("path_completed",				&CScriptGameObject::path_completed)
		.def("patrol_path_make_inactual",	&CScriptGameObject::patrol_path_make_inactual)
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
//		.def("set_sight",					(void (CScriptGameObject::*)(const MemorySpace::CMemoryInfo *memory_object, bool	torso_look))(CScriptGameObject::set_sight))

		// object handler
		.def("set_item",					(void (CScriptGameObject::*)(MonsterSpace::EObjectAction ))(CScriptGameObject::set_item))
		.def("set_item",					(void (CScriptGameObject::*)(MonsterSpace::EObjectAction, CScriptGameObject *))(CScriptGameObject::set_item))
		.def("set_item",					(void (CScriptGameObject::*)(MonsterSpace::EObjectAction, CScriptGameObject *, u32))(CScriptGameObject::set_item))
		.def("set_item",					(void (CScriptGameObject::*)(MonsterSpace::EObjectAction, CScriptGameObject *, u32, u32))(CScriptGameObject::set_item))

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
		.def("base_in_restrictions",		&CScriptGameObject::base_in_restrictions)
		.def("base_out_restrictions",		&CScriptGameObject::base_out_restrictions)
		.def("accessible",					&CScriptGameObject::accessible_position)
		.def("accessible",					&CScriptGameObject::accessible_vertex_id)
		.def("accessible_nearest",			&CScriptGameObject::accessible_nearest, out_value(_3))

		//////////////////////////////////////////////////////////////////////////
		.def("limping",						&CScriptGameObject::limping)
		//////////////////////////////////////////////////////////////////////////
		.def("enable_attachable_item",		&CScriptGameObject::enable_attachable_item)
		.def("attachable_item_enabled",		&CScriptGameObject::attachable_item_enabled)
		.def("weapon_strapped",				&CScriptGameObject::weapon_strapped)
		.def("weapon_unstrapped",			&CScriptGameObject::weapon_unstrapped)

		//////////////////////////////////////////////////////////////////////////
		//inventory owner
		//////////////////////////////////////////////////////////////////////////

		.enum_("EPdaMsg")
		[
			value("dialog_pda_msg",			int(ePdaMsgDialog)),
			value("info_pda_msg",			int(ePdaMsgInfo)),
			value("no_pda_msg",				int(ePdaMsgMax))
		]

		.def("give_info_portion",			&CScriptGameObject::GiveInfoPortion)
		.def("give_info_portion_via_pda",	&CScriptGameObject::GiveInfoPortionViaPda)
		.def("disable_info_portion",		&CScriptGameObject::DisableInfoPortion)
		.def("give_game_news",				&CScriptGameObject::GiveGameNews)
		.def("set_news_show_time",			&CScriptGameObject::SetNewsShowTime)
		

		.def("has_info",					&CScriptGameObject::HasInfo)
		.def("dont_has_info",				&CScriptGameObject::DontHasInfo)
		.def("get_info_time",				&CScriptGameObject::GetInfoTime)

		.def("get_task_state",				&CScriptGameObject::GetGameTaskState)
		.def("set_task_state",				&CScriptGameObject::SetGameTaskState)
		.def("get_all_tasks",				&CScriptGameObject::GetAllGameTasks)

		.def("send_pda_message",			&CScriptGameObject::SendPdaMessage)

		.def("is_talking",					&CScriptGameObject::IsTalking)
		.def("stop_talk",					&CScriptGameObject::StopTalk)
		.def("enable_talk",					&CScriptGameObject::EnableTalk)
		.def("disable_talk",				&CScriptGameObject::DisableTalk)
		.def("is_talk_enabled",				&CScriptGameObject::IsTalkEnabled)
		.def("enable_trade",				&CScriptGameObject::EnableTrade)
		.def("disable_trade",				&CScriptGameObject::DisableTrade)
		.def("is_trade_enabled",			&CScriptGameObject::IsTradeEnabled)

		.def("inventory_for_each",			&CScriptGameObject::ForEachInventoryItems)
		.def("drop_item",					&CScriptGameObject::DropItem)
		.def("transfer_item",				&CScriptGameObject::TransferItem)
		.def("transfer_money",				&CScriptGameObject::TransferMoney)
		.def("money",						&CScriptGameObject::Money)

		.def("switch_to_trade",				&CScriptGameObject::SwitchToTrade)
		.def("switch_to_talk",				&CScriptGameObject::SwitchToTalk)
		.def("run_talk_dialog",				&CScriptGameObject::RunTalkDialog)
		.def("actor_sleep",					&CScriptGameObject::ActorSleep)
		
		.def("hide_weapon",					&CScriptGameObject::HideWeapon)
		.def("restore_weapon",				&CScriptGameObject::RestoreWeapon)
		
		.def("set_start_dialog",			&CScriptGameObject::SetStartDialog)
		.def("get_start_dialog",			&CScriptGameObject::GetStartDialog)
		.def("restore_default_start_dialog",&CScriptGameObject::RestoreDefaultStartDialog)

		//////////////////////////////////////////////////////////////////////////

		.def("goodwill",					&CScriptGameObject::GetGoodwill)
		.def("set_goodwill",				&CScriptGameObject::SetGoodwill)
		.def("change_goodwill",				&CScriptGameObject::ChangeGoodwill)

		.def("community_goodwill",			&CScriptGameObject::GetCommunityGoodwill)
		.def("set_community_goodwill",		&CScriptGameObject::SetCommunityGoodwill)
		.def("change_community_goodwill",	&CScriptGameObject::ChangeCommunityGoodwill)

		.def("general_goodwill",			&CScriptGameObject::GetAttitude)
		.def("set_relation",				&CScriptGameObject::SetRelation)



		//////////////////////////////////////////////////////////////////////////
		.def("profile_name",				&CScriptGameObject::ProfileName)
		.def("character_name",				&CScriptGameObject::CharacterName)
		.def("character_rank",				&CScriptGameObject::CharacterRank)
		.def("set_character_rank",			&CScriptGameObject::SetCharacterRank)
		.def("character_reputation",		&CScriptGameObject::CharacterReputation)
		.def("set_character_reputation",	&CScriptGameObject::SetCharacterReputation)
		.def("character_community",			&CScriptGameObject::CharacterCommunity)
		.def("set_character_community",		&CScriptGameObject::SetCharacterCommunity)

		.def("get_actor_relation_flags",	&CScriptGameObject::get_actor_relation_flags)
		.def("set_actor_relation_flags",	&CScriptGameObject::set_actor_relation_flags)
		.def("snd_character_profile_sect",	&CScriptGameObject::snd_character_profile_sect)

//		.def("set_character_pda_info",		&CScriptGameObject::set_character_pda_info)
//		.def("get_character_pda_info",		&CScriptGameObject::get_character_pda_info)
//		.def("set_pda_info",				&CScriptGameObject::set_pda_info)
//		.def("get_pda_info",				&CScriptGameObject::get_pda_info)

		.enum_("ACTOR_RELATIONS")
		[
			value("relation_attack",						int(RELATION_REGISTRY::ATTACK)),
			value("relation_fight_help_monster",			int(RELATION_REGISTRY::FIGHT_HELP_MONSTER)),
			value("relation_fight_help_human",				int(RELATION_REGISTRY::FIGHT_HELP_HUMAN)),
			value("relation_kill",							int(RELATION_REGISTRY::KILL))
		]

		.enum_("CLSIDS")
		[
			value("no_pda_msg",				int(ePdaMsgMax))
		]

		//CustomZone
		.def("enable_anomaly",              &CScriptGameObject::EnableAnomaly)
		.def("disable_anomaly",             &CScriptGameObject::DisableAnomaly)
		.def("get_anomaly_power",			&CScriptGameObject::GetAnomalyPower)
		.def("set_anomaly_power",			&CScriptGameObject::SetAnomalyPower)

		//HELICOPTER
		.def("get_helicopter",              &CScriptGameObject::get_helicopter)
		.def("get_hanging_lamp",            &CScriptGameObject::get_hanging_lamp)
		.def("get_physics_shell",			&CScriptGameObject::get_physics_shell)
		.def("get_holder_class",			&CScriptGameObject::get_custom_holder)
		.def("get_current_holder",			&CScriptGameObject::get_current_holder)
		//usable object
		.def("set_tip_text",				&CScriptGameObject::SetTipText)
		.def("set_tip_text_default",		&CScriptGameObject::SetTipTextDefault)
		.def("set_nonscript_usable",		&CScriptGameObject::SetNonscriptUsable)

		// Script Zone
		.def("active_zone_contact",			&CScriptGameObject::active_zone_contact)
		.def("inside",						(bool (CScriptGameObject::*)(const Fvector &, float) const)(&CScriptGameObject::inside))
		.def("inside",						(bool (CScriptGameObject::*)(const Fvector &) const)(&CScriptGameObject::inside))
		.def("set_fastcall",				&CScriptGameObject::set_fastcall)
		.def("info_add",					&CScriptGameObject::info_add)
		.def("info_clear",					&CScriptGameObject::info_clear)

		// monster jumper
		.def("jump",						&CScriptGameObject::jump)

	;return	(instance);
}
