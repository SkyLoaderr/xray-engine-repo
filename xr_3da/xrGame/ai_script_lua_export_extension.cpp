////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_space.h"
#include "ai_script_lua_extension.h"
#include "ai_script_effector.h"
#include "../cameramanager.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
#include "ParticlesObject.h"
#include "ArtifactMerger.h"
#include "actor.h"
#include "ai_script_classes.h"

using namespace luabind;
using namespace Script;

CRenderDevice &get_device()
{
	return		(Device);
}

CLuaGameObject *tpfGetActor()
{
	CActor *l_tpActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (l_tpActor)
		return(xr_new<CLuaGameObject>(dynamic_cast<CGameObject*>(l_tpActor)));
	else
		return(0);
}

CCameraManager &get_camera_manager()
{
	return		(Level().Cameras);
}

CLuaGameObject *get_object_by_name(LPCSTR caObjectName)
{
	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
	if (l_tpGameObject)
		return		(xr_new<CLuaGameObject>(l_tpGameObject));
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

void Script::vfExportArtifactMerger(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
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

void Script::vfExportEffector(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
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

void Script::vfExportLevel(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine,"level")
	[
		// declarations
		def("cameras",							get_camera_manager),
		def("object",							get_object_by_name, adopt(return_value)),
		def("actor",							tpfGetActor, adopt(return_value)),
		def("set_artifact_merge",				&CArtifactMerger::SetArtifactMergeFunctor),
		def("get_weather",						get_weather),
		def("set_weather",						set_weather)
	];

	module(tpLuaVirtualMachine)
	[
		def("device",							get_device)
	];
}

void Script::vfExportParticles(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CParticlesObject>("particles")
			.def(								constructor<LPCSTR,bool>())
			.def("position",					&CParticlesObject::Position)
			.def("play_at_pos",					&CParticlesObject::play_at_pos)
			.def("stop",						&CParticlesObject::Stop)
	];
}

template<typename T>
CLuaGameObject *get_memory_object(T &object)
{
	if (!object.m_object || !dynamic_cast<CGameObject*>(object.m_object))
		return		(0);
	return			(xr_new<CLuaGameObject>(dynamic_cast<CGameObject*>(object.m_object)));
}

int get_sound_type(const CSoundObject &sound_object)
{
	return			((int)sound_object.m_sound_type);
}

u32 bit_and(u32 i, u32 j)
{
	return			(i & j);
}

u32 bit_or(u32 i, u32 j)
{
	return			(i | j);
}

u32 bit_xor(u32 i, u32 j)
{
	return			(i ^ j);
}

u32 bit_not(u32 i)
{
	return			(~i);
}

void Script::vfExportMemoryObjects(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
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
		
		class_<MemorySpace::CSoundObject,MemorySpace::CVisibleObject>("sound_memory_object")
			.enum_("sound_types")
			[
				value("no_sound",				int(SOUND_TYPE_NO_SOUND					)),
				value("weapon",					int(SOUND_TYPE_WEAPON					)),
				value("item",					int(SOUND_TYPE_ITEM						)),
				value("monster",				int(SOUND_TYPE_MONSTER					)),
				value("anomaly",				int(SOUND_TYPE_ANOMALY					)),
				value("world",					int(SOUND_TYPE_WORLD					)),
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
				value("world_object_ambient",	int(SOUND_TYPE_WORLD_AMBIENT			)),
				value("weapon_pistol",			int(SOUND_TYPE_WEAPON_PISTOL			)),
				value("weapon_gun",				int(SOUND_TYPE_WEAPON_GUN				)),
				value("weapon_submachinegun",	int(SOUND_TYPE_WEAPON_SUBMACHINEGUN		)),
				value("weapon_machinegun",		int(SOUND_TYPE_WEAPON_MACHINEGUN		)),
				value("weapon_sniper_rifle",	int(SOUND_TYPE_WEAPON_SNIPERRIFLE		)),
				value("weapon_grenade_launcher",int(SOUND_TYPE_WEAPON_GRENADELAUNCHER	)),
				value("weapon_rocket_launcher",	int(SOUND_TYPE_WEAPON_ROCKETLAUNCHER	))
			]

			.def("type",					&MemorySpace::CSoundObject::sound_type)
			.def_readonly("power",			&MemorySpace::CSoundObject::m_power),

		def("get_memory_object",			get_memory_object<CHitObject>),
		def("get_memory_object",			get_memory_object<CSoundObject>),
		def("bit_and",						bit_and),
		def("bit_or",						bit_or),
		def("bit_xor",						bit_xor),
		def("bit_not",						bit_not)

	];
}