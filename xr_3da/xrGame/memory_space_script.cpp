////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_space_script.cpp
//	Created 	: 25.12.2003
//  Modified 	: 25.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory space script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "memory_space.h"
#include "script_space.h"
#include "script_game_object.h"
#include "gameobject.h"
#include "entity_alive.h"

using namespace luabind;

CScriptGameObject *not_yet_visible_object(const MemorySpace::CNotYetVisibleObject &object)
{
	return			(object.m_object->lua_game_object());
}

int get_sound_type(const CSoundObject &sound_object)
{
	return			((int)sound_object.m_sound_type);
}

template <typename T>
CScriptGameObject *get_memory_object(const MemorySpace::CMemoryObject<T> &memory_object)
{
	return			(memory_object.m_object->lua_game_object());
}

void CMemoryInfo::script_register(lua_State *L)
{
	module(L)
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
			.def_readonly("last_game_time",	&MemorySpace::SMemoryObject::m_last_game_time)
			.def_readonly("last_level_time",&MemorySpace::SMemoryObject::m_last_level_time)
			.def_readonly("first_game_time",&MemorySpace::SMemoryObject::m_first_game_time)
			.def_readonly("first_level_time",&MemorySpace::SMemoryObject::m_first_level_time)
			.def_readonly("update_count",	&MemorySpace::SMemoryObject::m_update_count),

		class_<MemorySpace::CMemoryObject<CEntityAlive>,MemorySpace::SMemoryObject>("entity_memory_object")
			.def_readonly("object_info",	&MemorySpace::CMemoryObject<CEntityAlive>::m_object_params)
			.def_readonly("self_info",		&MemorySpace::CMemoryObject<CEntityAlive>::m_self_params)
			.def("object",					&get_memory_object<CEntityAlive>),

		class_<MemorySpace::CMemoryObject<CGameObject>,MemorySpace::SMemoryObject>("game_memory_object")
			.def_readonly("object_info",	&MemorySpace::CMemoryObject<CGameObject>::m_object_params)
			.def_readonly("self_info",		&MemorySpace::CMemoryObject<CGameObject>::m_self_params)
			.def("object",					&get_memory_object<CGameObject>),

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

		class_<MemorySpace::CSoundObject,MemorySpace::CVisibleObject>("sound_memory_object")
			.def("type",					&MemorySpace::CSoundObject::sound_type)
			.def_readonly("power",			&MemorySpace::CSoundObject::m_power),

		class_<MemorySpace::CNotYetVisibleObject>("not_yet_visible_object")
			.def_readonly("value",			&MemorySpace::CNotYetVisibleObject::m_value)
			.def("object",					&not_yet_visible_object)
	];
}