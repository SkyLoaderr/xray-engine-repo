////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "ai_script_effector.h"
#include "../cameramanager.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
#include "ParticlesObject.h"
#include "ArtifactMerger.h"
#include "actor.h"
#include "ai_script_classes.h"
#include "ai_script_hit.h"
#include "ai/stalker/ai_stalker.h"
#include "ai_alife_space.h"
#include "script_binder_object.h"
#include "pdamsg.h"

using namespace luabind;

CRenderDevice &get_device()
{
	return		(Device);
}

CLuaGameObject *tpfGetActor()
{
	CActor *l_tpActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (l_tpActor)
		return	(dynamic_cast<CGameObject*>(l_tpActor)->lua_game_object());
	else
		return	(0);
}

CLuaGameObject *get_object_by_name(LPCSTR caObjectName)
{
	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
	if (l_tpGameObject)
		return		(l_tpGameObject->lua_game_object());
	else
		return		(0);
}

LPCSTR get_weather	()
{
	return			(g_pGamePersistent->Environment.GetWeather());
}

void set_weather	(LPCSTR weather_name)
{
	VERIFY3			(g_pGamePersistent->Environment.Weathers.find(weather_name) != g_pGamePersistent->Environment.Weathers.end(),"There is no weather with the specified name ",weather_name);
	return			(g_pGamePersistent->Environment.SetWeather(weather_name));
}

void set_time_factor(float time_factor)
{
	Level().SetGameTimeFactor(time_factor);
}

float get_time_factor()
{
	return			(Level().GetGameTimeFactor());
}

float cover_in_direction(u32 level_vertex_id, const Fvector &direction)
{
	float			y,p;
	direction.getHP	(y,p);
	return			(ai().level_graph().cover_in_direction(y,level_vertex_id));
}

void CScriptEngine::export_artifact_merger()
{
	module(lua())
	[
		class_<CArtifactMerger>("artifact_merger")
		.def("get_mercury_ball_num",	&CArtifactMerger::GetMercuryBallNum)
		.def("get_gravi_num",			&CArtifactMerger::GetGraviArtifactNum)
		.def("get_black_drops_num",		&CArtifactMerger::GetBlackDropsNum)
		.def("get_needles_num",			&CArtifactMerger::GetNeedlesNum)

		.def("destroy_mercury_ball",	&CArtifactMerger::DestroyMercuryBall)
		.def("destroy_gravi",			&CArtifactMerger::DestroyGraviArtifact)
		.def("destroy_black_drops",		&CArtifactMerger::DestroyBlackDrops)
		.def("destroy_needles",			&CArtifactMerger::DestroyNeedles)

		.def("spawn_mercury_ball",	&CArtifactMerger::SpawnMercuryBall)
		.def("spawn_gravi",			&CArtifactMerger::SpawnGraviArtifact)
		.def("spawn_black_drops",	&CArtifactMerger::SpawnBlackDrops)
		.def("spawn_needles",		&CArtifactMerger::SpawnNeedles)
	];
}

void CScriptEngine::export_effector()
{
	module(lua())
		[
			class_<SPPInfo::SDuality>("duality")
			.def_readwrite("h",					&SPPInfo::SDuality::h)
			.def_readwrite("v",					&SPPInfo::SDuality::v)
			.def(								constructor<>()),

			class_<SPPInfo::SColor>	("color")
			.def_readwrite("r",					&SPPInfo::SColor::r)
			.def_readwrite("g",					&SPPInfo::SColor::g)
			.def_readwrite("b",					&SPPInfo::SColor::b)
			.def(								constructor<>()),

		class_<SPPInfo::SNoise>("noise")
			.def_readwrite("intensity",			&SPPInfo::SNoise::intensity)
			.def_readwrite("grain",				&SPPInfo::SNoise::grain)
			.def_readwrite("fps",				&SPPInfo::SNoise::fps)
			.def(								constructor<>()),

		class_<SPPInfo>("effector_params")
			.def_readwrite("blur",				&SPPInfo::blur)
			.def_readwrite("gray",				&SPPInfo::gray)
			.def_readwrite("dual",				&SPPInfo::duality)
			.def_readwrite("noise",				&SPPInfo::noise)
			.def_readwrite("color_base",		&SPPInfo::color_base)
			.def_readwrite("color_gray",		&SPPInfo::color_gray)
			.def_readwrite("color_add",			&SPPInfo::color_add)
			.def(								constructor<>()),

		class_<CLuaEffector, CLuaEffectorWrapper>("effector")
			.def_readwrite("info",				&CLuaEffector::m_tInfo)
			.def(								constructor<int,float>())
			.def("start",						&CLuaEffector::Add)
			.def("finish",						&CLuaEffector::Remove)
			.def("process",						&CLuaEffectorWrapper::Process_static)
	];
}

void CScriptEngine::export_level()
{
	module(lua(),"level")
	[
		// declarations
		def("object",							get_object_by_name),
		def("actor",							tpfGetActor),
		//def("set_artifact_merge",				SetArtifactMergeFunctor),
		def("get_weather",						get_weather),
		def("set_weather",						set_weather),
		def("set_time_factor",					set_time_factor),
		def("get_time_factor",					get_time_factor),
		def("cover_in_direction",				cover_in_direction)
	];

	module(lua())
	[
		def("device",							get_device)
	];
}

void CScriptEngine::export_particles()
{
	module(lua())
	[
		class_<CParticlesObject>("particles")
			.def(								constructor<LPCSTR,bool>())
			.def("position",					&CParticlesObject::Position)
			.def("play_at_pos",					&CParticlesObject::play_at_pos)
			.def("stop",						&CParticlesObject::Stop)
	];
}

int get_sound_type(const CSoundObject &sound_object)
{
	return			((int)sound_object.m_sound_type);
}

int bit_and(int i, int j)
{
	return			(i & j);
}

int bit_or(int i, int j)
{
	return			(i | j);
}

int bit_xor(int i, int j)
{
	return			(i ^ j);
}

int bit_not(int i)
{
	return			(~i);
}

struct SSoundType{};

void CScriptEngine::export_memory_objects()
{
	module(lua())
	[
		class_<SRotation>("rotation")
			.def_readonly("yaw",			&SRotation::yaw)
			.def_readonly("pitch",			&SRotation::pitch),
			
		class_<MemorySpace::SObjectParams>("object_params")
			.def_readonly("level_vertex",	&MemorySpace::SObjectParams::m_level_vertex_id)
			.def_readonly("position",		&MemorySpace::SObjectParams::m_position)
			.def_readonly("orientation",	&MemorySpace::SObjectParams::m_orientation),
			
		class_<MemorySpace::SMemoryObject>("memory_object")
			.def_readonly("game_time",		&MemorySpace::SMemoryObject::m_game_time)
			.def_readonly("level_time",		&MemorySpace::SMemoryObject::m_level_time)
			.def_readonly("update_count",	&MemorySpace::SMemoryObject::m_update_count),

		class_<MemorySpace::CMemoryObject<CEntityAlive>,MemorySpace::SMemoryObject>("entity_memory_object")
			.def_readonly("object_info",	&MemorySpace::CMemoryObject<CEntityAlive>::m_object_params)
			.def_readonly("self_info",		&MemorySpace::CMemoryObject<CEntityAlive>::m_self_params),

		class_<MemorySpace::CMemoryObject<CGameObject>,MemorySpace::SMemoryObject>("game_memory_object")
			.def_readonly("object_info",	&MemorySpace::CMemoryObject<CGameObject>::m_object_params)
			.def_readonly("self_info",		&MemorySpace::CMemoryObject<CGameObject>::m_self_params),

		class_<MemorySpace::CHitObject,MemorySpace::CMemoryObject<CEntityAlive> >("hit_memory_object")
			.def_readonly("direction",		&MemorySpace::CHitObject::m_direction)
			.def_readonly("bone_index",		&MemorySpace::CHitObject::m_bone_index)
			.def_readonly("amount",			&MemorySpace::CHitObject::m_amount),
		
		class_<MemorySpace::CVisibleObject,MemorySpace::CMemoryObject<CGameObject> >("visible_memory_object")
			.def_readonly("visible",		&MemorySpace::CSoundObject::m_visible),

		class_<MemorySpace::CMemoryInfo,MemorySpace::CVisibleObject>("memory_info")
			.def_readonly("visual_info",	&MemorySpace::CMemoryInfo::m_visual_info)
			.def_readonly("sound_info",		&MemorySpace::CMemoryInfo::m_sound_info)
			.def_readonly("hit_info",		&MemorySpace::CMemoryInfo::m_hit_info),

		class_<SSoundType>("snd_type")
			.enum_("sound_types")
			[
				value("no_sound",				int(SOUND_TYPE_NO_SOUND					)),
				value("weapon",					int(SOUND_TYPE_WEAPON					)),
				value("item",					int(SOUND_TYPE_ITEM						)),
				value("monster",				int(SOUND_TYPE_MONSTER					)),
				value("anomaly",				int(SOUND_TYPE_ANOMALY					)),
				value("world",					int(SOUND_TYPE_WORLD					)),
				value("pick_up",				int(SOUND_TYPE_PICKING_UP				)),
				value("drop",					int(SOUND_TYPE_DROPPING					)),
				value("hide",					int(SOUND_TYPE_HIDING					)),
				value("take",					int(SOUND_TYPE_TAKING					)),
				value("use",					int(SOUND_TYPE_USING					)),
				value("shoot",					int(SOUND_TYPE_SHOOTING					)),
				value("empty",					int(SOUND_TYPE_EMPTY_CLICKING			)),
				value("bullet_hit",				int(SOUND_TYPE_BULLET_HIT				)),
				value("reload",					int(SOUND_TYPE_RECHARGING				)),
				value("die",					int(SOUND_TYPE_DYING					)),
				value("injure",					int(SOUND_TYPE_INJURING					)),
				value("step",					int(SOUND_TYPE_STEP						)),
				value("talk",					int(SOUND_TYPE_TALKING					)),
				value("attack",					int(SOUND_TYPE_ATTACKING				)),
				value("eat",					int(SOUND_TYPE_EATING					)),
				value("idle",					int(SOUND_TYPE_IDLE						)),
				value("object_break",			int(SOUND_TYPE_OBJECT_BREAKING			)),
				value("object_collide",			int(SOUND_TYPE_OBJECT_COLLIDING			)),
				value("object_explode",			int(SOUND_TYPE_OBJECT_EXPLODING			)),
				value("ambient",				int(SOUND_TYPE_AMBIENT					)),
				value("item_pick_up",			int(SOUND_TYPE_ITEM_PICKING_UP			)),
				value("item_drop",				int(SOUND_TYPE_ITEM_DROPPING			)),
				value("item_hide",				int(SOUND_TYPE_ITEM_HIDING				)),
				value("item_take",				int(SOUND_TYPE_ITEM_TAKING				)),
				value("item_use",				int(SOUND_TYPE_ITEM_USING				)),
				value("weapon_shoot",			int(SOUND_TYPE_WEAPON_SHOOTING			)),
				value("weapon_empty",			int(SOUND_TYPE_WEAPON_EMPTY_CLICKING	)),
				value("weapon_bullet_hit",		int(SOUND_TYPE_WEAPON_BULLET_HIT		)),
				value("weapon_reload",			int(SOUND_TYPE_WEAPON_RECHARGING		)),
				value("monster_die",			int(SOUND_TYPE_MONSTER_DYING			)),
				value("monster_injure",			int(SOUND_TYPE_MONSTER_INJURING			)),
				value("monster_step",			int(SOUND_TYPE_MONSTER_STEP				)),
				value("monster_talk",			int(SOUND_TYPE_MONSTER_TALKING			)),
				value("monster_attack",			int(SOUND_TYPE_MONSTER_ATTACKING		)),
				value("monster_eat",			int(SOUND_TYPE_MONSTER_EATING			)),
				value("anomaly_idle",			int(SOUND_TYPE_ANOMALY_IDLE				)),
				value("world_object_break",		int(SOUND_TYPE_WORLD_OBJECT_BREAKING	)),
				value("world_object_collide",	int(SOUND_TYPE_WORLD_OBJECT_COLLIDING	)),
				value("world_object_explode",	int(SOUND_TYPE_WORLD_OBJECT_EXPLODING	)),
				value("world_ambient",			int(SOUND_TYPE_WORLD_AMBIENT			))
			],

		class_<MemorySpace::CSoundObject,MemorySpace::CVisibleObject>("sound_memory_object")
			.def("type",					&MemorySpace::CSoundObject::sound_type)
			.def_readonly("power",			&MemorySpace::CSoundObject::m_power),

		def("bit_and",						bit_and),
		def("bit_or",						bit_or),
		def("bit_xor",						bit_xor),
		def("bit_not",						bit_not)

	];
}

void CScriptEngine::export_object()
{
	module(lua())
	[
		class_<CAbstractVertexEvaluator>("vertex_evaluator"),

		class_<CLuaGameObject>("game_object")
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
				value("movement",				int(CScriptMonster::eActionTypeMovement)),
				value("watch",					int(CScriptMonster::eActionTypeWatch)),
				value("animation",				int(CScriptMonster::eActionTypeAnimation)),
				value("sound",					int(CScriptMonster::eActionTypeSound)),
				value("particle",				int(CScriptMonster::eActionTypeParticle)),
				value("object",					int(CScriptMonster::eActionTypeObject)),
				value("action_type_count",		int(CScriptMonster::eActionTypeCount))
			]
			.enum_("EPathType")
			[
				value("game_path",				int(CMovementManager::ePathTypeGamePath)),
				value("level_path",				int(CMovementManager::ePathTypeLevelPath)),
				value("enemy_search",			int(CMovementManager::ePathTypeEnemySearch)),
				value("patrol_path",			int(CMovementManager::ePathTypePatrolPath)),
				value("no_path",				int(CMovementManager::ePathTypeNoPath))
			]
			.property("visible",				&CLuaGameObject::getVisible,		&CLuaGameObject::setVisible)
			.property("enabled",				&CLuaGameObject::getEnabled,		&CLuaGameObject::setEnabled)
			.property("health",					&CLuaGameObject::GetHealth,			&CLuaGameObject::SetHealth)
			.property("power",					&CLuaGameObject::GetPower,			&CLuaGameObject::SetPower)
			.property("satiety",				&CLuaGameObject::GetSatiety,		&CLuaGameObject::SetSatiety)
			.property("radiation",				&CLuaGameObject::GetRadiation,		&CLuaGameObject::SetRadiation)
			.property("circumspection",			&CLuaGameObject::GetCircumspection,	&CLuaGameObject::SetCircumspection)
			.property("morale",					&CLuaGameObject::GetMorale,			&CLuaGameObject::SetMorale)

			.def(								constructor<LPCSTR>())
			.def(								constructor<const CLuaGameObject *>())
			.def("position",					&CLuaGameObject::Position)
			.def("direction",					&CLuaGameObject::Direction)
			.def("clsid",						&CLuaGameObject::clsid)
			.def("id",							&CLuaGameObject::ID)
			.def("section",						&CLuaGameObject::Section)
			.def("name",						&CLuaGameObject::Name)
			.def("parent",						&CLuaGameObject::Parent)
			.def("mass",						&CLuaGameObject::Mass)
			.def("cost",						&CLuaGameObject::Cost)
			.def("death_time",					&CLuaGameObject::DeathTime)
			.def("armor",						&CLuaGameObject::Armor)
			.def("max_health",					&CLuaGameObject::DeathTime)
			.def("accuracy",					&CLuaGameObject::Accuracy)
			.def("alive",						&CLuaGameObject::Alive)
			.def("team",						&CLuaGameObject::Team)
			.def("squad",						&CLuaGameObject::Squad)
			.def("group",						&CLuaGameObject::Group)
			.def("kill",						&CLuaGameObject::Kill)
			.def("hit",							&CLuaGameObject::Hit)
			.def("fov",							&CLuaGameObject::GetFOV)
			.def("range",						&CLuaGameObject::GetRange)
			.def("relation",					&CLuaGameObject::GetRelationType)
			.def("script",						&CLuaGameObject::SetScriptControl)
			.def("get_script",					&CLuaGameObject::GetScriptControl)
			.def("get_script_name",				&CLuaGameObject::GetScriptControlName)
			.def("see",							&CLuaGameObject::CheckObjectVisibility)
			.def("see",							&CLuaGameObject::CheckTypeVisibility)

			.def("who_hit_name",				&CLuaGameObject::WhoHitName)
			.def("who_hit_section_name",		&CLuaGameObject::WhoHitSectionName)
			
			.def("use",							&CLuaGameObject::UseObject)
			.def("rank",						&CLuaGameObject::GetRank)
			.def("command",						CLuaGameObject::AddAction)
			.def("action",						&CLuaGameObject::GetCurrentAction, adopt(return_value))
			.def("object_count",				&CLuaGameObject::GetInventoryObjectCount)
			.def("object",						(CLuaGameObject *(CLuaGameObject::*)(LPCSTR))(CLuaGameObject::GetObjectByName))
			.def("object",						(CLuaGameObject *(CLuaGameObject::*)(int))(CLuaGameObject::GetObjectByIndex))
			.def("active_item",					&CLuaGameObject::GetActiveItem)
			.def("set_callback",				(void (CLuaGameObject::*)(const luabind::functor<void> &, bool))(CLuaGameObject::SetCallback))
			.def("set_callback",				(void (CLuaGameObject::*)(const luabind::object &, LPCSTR, bool))(CLuaGameObject::SetCallback))
			.def("set_callback",				(void (CLuaGameObject::*)(const luabind::object &, LPCSTR, const CScriptMonster::EActionType))(CLuaGameObject::SetCallback))
			.def("set_callback",				(void (CLuaGameObject::*)(const luabind::functor<void> &, const CScriptMonster::EActionType))(CLuaGameObject::SetCallback))
			.def("clear_callback",				(void (CLuaGameObject::*)(bool))(CLuaGameObject::ClearCallback))
			.def("clear_callback",				(void (CLuaGameObject::*)(const CScriptMonster::EActionType))(CLuaGameObject::ClearCallback))
			.def("give_info_portion",			&CLuaGameObject::GiveInfoPortion)
			.def("give_info_portion_via_pda",	&CLuaGameObject::GiveInfoPortionViaPda)
			.def("patrol",						&CLuaGameObject::GetPatrolPathName)
			.def("set_trade_callback",			(void (CLuaGameObject::*)(const luabind::functor<void> &))(CLuaGameObject::SetTradeCallback))
			.def("set_trade_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetTradeCallback))			
			.def("clear_trade_callback",		(void (CLuaGameObject::*)())(CLuaGameObject::ClearTradeCallback))
			.def("get_ammo_in_magazine",		&CLuaGameObject::GetAmmoElapsed)
			.def("get_ammo_total",				&CLuaGameObject::GetAmmoCurrent)
			.def("set_queue_size",				&CLuaGameObject::SetQueueSize)
			.def("best_hit",					&CLuaGameObject::GetBestHit)
			.def("best_sound",					&CLuaGameObject::GetBestSound)
			.def("best_enemy",					&CLuaGameObject::GetBestEnemy)
			.def("best_item",					&CLuaGameObject::GetBestItem)
			.def("action_count",				&CLuaGameObject::GetActionCount)
			.def("action_by_index",				&CLuaGameObject::GetActionByIndex)
			.def("set_hit_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetHitCallback))
			.def("set_hit_callback",			(void (CLuaGameObject::*)(const luabind::functor<void> &))(CLuaGameObject::SetHitCallback))
			.def("clear_hit_callback",			&CLuaGameObject::ClearHitCallback)
			.def("set_hear_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetSoundCallback))
			.def("set_hear_callback",			(void (CLuaGameObject::*)(const luabind::functor<void> &))(CLuaGameObject::SetSoundCallback))
			.def("clear_hear_callback",			&CLuaGameObject::ClearSoundCallback)
			.def("memory",						&CLuaGameObject::memory, adopt(return_value))
			.def("best_weapon",					&CLuaGameObject::best_weapon)
			.def("explode",						&CLuaGameObject::explode)
			.def("get_enemy",					&CLuaGameObject::GetEnemy)
			.def("get_corpse",					&CLuaGameObject::GetCorpse)
			.def("get_enemy_strength",			&CLuaGameObject::GetEnemyStrength)
			.def("get_sound_info",				&CLuaGameObject::GetSoundInfo)
			.def("bind_object",					&CLuaGameObject::bind_object,adopt(_1))
			.def("motivation_action_manager",	&CLuaGameObject::motivation_action_manager)

			// movement manager
			.def("set_body_state",				&CLuaGameObject::set_body_state			)
			.def("set_movement_type",			&CLuaGameObject::set_movement_type		)
			.def("set_mental_state",			&CLuaGameObject::set_mental_state		)
			.def("set_path_type",				&CLuaGameObject::set_path_type			)
			.def("set_detail_path_type",		&CLuaGameObject::set_detail_path_type	)
			//
			.def("set_desired_position",		(void (CLuaGameObject::*)())(CLuaGameObject::set_desired_position))
			.def("set_desired_position",		(void (CLuaGameObject::*)(const Fvector *))(CLuaGameObject::set_desired_position))
			.def("set_desired_direction",		(void (CLuaGameObject::*)())(CLuaGameObject::set_desired_direction))
			.def("set_desired_direction",		(void (CLuaGameObject::*)(const Fvector *))(CLuaGameObject::set_desired_direction))
			.def("set_node_evaluator",			(void (CLuaGameObject::*)())(CLuaGameObject::set_node_evaluator))
			.def("set_node_evaluator",			(void (CLuaGameObject::*)(CAbstractVertexEvaluator *))(CLuaGameObject::set_node_evaluator))
			.def("set_path_evaluator",			(void (CLuaGameObject::*)())(CLuaGameObject::set_path_evaluator))
			.def("set_path_evaluator",			(void (CLuaGameObject::*)(CAbstractVertexEvaluator *))(CLuaGameObject::set_path_evaluator))
			.def("set_patrol_path",				&CLuaGameObject::set_patrol_path)
			.def("set_dest_level_vertex_id",	&CLuaGameObject::set_dest_level_vertex_id)
			.def("level_vertex_id",				&CLuaGameObject::level_vertex_id)
			.def("add_animation",				(void (CLuaGameObject::*)(LPCSTR))(CLuaGameObject::add_animation))
			.def("add_animation",				(void (CLuaGameObject::*)(LPCSTR, bool))(CLuaGameObject::add_animation))
			.def("clear_animations",			&CLuaGameObject::clear_animations)
			.def("animation_count",				&CLuaGameObject::animation_count)
			
			// sound_player
			
			.def("add_sound",					(void (CLuaGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32))(CLuaGameObject::add_sound))
			.def("add_sound",					(void (CLuaGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32,LPCSTR))(CLuaGameObject::add_sound))
			.def("remove_sound",				&CLuaGameObject::remove_sound)
			.def("set_sound_mask",				&CLuaGameObject::set_sound_mask)
			.def("play_sound",					(void (CLuaGameObject::*)(u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32,u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32,u32,u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32,u32,u32,u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32,u32,u32,u32,u32))(CLuaGameObject::play_sound))
			.def("binded_object",				&CLuaGameObject::binded_object)
			.def("set_previous_point",			&CLuaGameObject::set_previous_point)
			.def("enable_memory_object",		&CLuaGameObject::enable_memory_object)

			// sight manager
			.def("set_sight",					&CLuaGameObject::set_sight)

			// object handler
			.def("set_item",					(void (CLuaGameObject::*)(MonsterSpace::EObjectAction ))(CLuaGameObject::set_item))
			.def("set_item",					(void (CLuaGameObject::*)(MonsterSpace::EObjectAction, CLuaGameObject *))(CLuaGameObject::set_item))

			//////////////////////////////////////////////////////////////////////////
			//inventory owner
			//////////////////////////////////////////////////////////////////////////

			.enum_("EPdaMsg")
			[
				value("trade_pda_msg",			int(ePdaMsgTrade)),
				value("help_pda_msg",			int(ePdaMsgNeedHelp)),
				value("go_away_pda_msg",		int(ePdaMsgGoAway)),
				value("info_pda_msg",			int(ePdaMsgInfo)),
				value("no_pda_msg",				int(ePdaMsgMax))
			]

			.def("set_pda_callback",			(void (CLuaGameObject::*)(const luabind::functor<void>&))(CLuaGameObject::SetPdaCallback))
			.def("set_pda_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetPdaCallback))
			.def("set_info_callback",			(void (CLuaGameObject::*)(const luabind::functor<void>&))(CLuaGameObject::SetInfoCallback))
			.def("set_info_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetInfoCallback))
			.def("clear_pda_callback",			&CLuaGameObject::ClearPdaCallback)
			.def("clear_info_callback",			&CLuaGameObject::ClearInfoCallback)

			.def("give_info_portion",			&CLuaGameObject::GiveInfoPortion)
			.def("give_info_portion_via_pda",	&CLuaGameObject::GiveInfoPortionViaPda)
			.def("send_pda_message",			&CLuaGameObject::SendPdaMessage)

			.def("is_talking",					&CLuaGameObject::IsTalking)
			.def("stop_talk",					&CLuaGameObject::StopTalk)
			.def("enable_talk",					&CLuaGameObject::EnableTalk)
			.def("disable_talk",				&CLuaGameObject::DisableTalk)
			.def("is_talk_enabled",				&CLuaGameObject::IsTalkEnabled)


			.enum_("CLSIDS")
			[
				value("no_pda_msg",				int(ePdaMsgMax))
			]
	];
}
