////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_sound.h"
#include "ai_script_classes.h"

using namespace luabind;

void CScriptSound::script_register(lua_State *L)
{
	module(L)
	[
		class_<CSound_params>("sound_params")
			.def_readwrite("position",			&CSound_params::position)
			.def_readwrite("volume",			&CSound_params::volume)
			.def_readwrite("frequency",			&CSound_params::freq)
			.def_readwrite("min_distance",		&CSound_params::min_distance)
			.def_readwrite("max_distance",		&CSound_params::max_distance),
			
		class_<CScriptSound>("sound_object")
			.enum_("sound_play_type")
			[
				value("looped",					sm_Looped),
				value("s2d",					sm_2D),
				value("s3d",					0)
			]
			.property("frequency",				&CScriptSound::GetFrequency,	&CScriptSound::SetFrequency)
			.property("min_distance",			&CScriptSound::GetMinDistance,	&CScriptSound::SetMinDistance)
			.property("max_distance",			&CScriptSound::GetMaxDistance,	&CScriptSound::SetMaxDistance)
			.property("volume",					&CScriptSound::GetVolume,		&CScriptSound::SetVolume)
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR,ESoundTypes>())
			.def("get_position",				&CScriptSound::GetPosition)
			.def("set_position",				&CScriptSound::SetPosition)
			.def("play",						(void (CScriptSound::*)(CLuaGameObject*))(CScriptSound::Play))
			.def("play",						(void (CScriptSound::*)(CLuaGameObject*,float))(CScriptSound::Play))
			.def("play",						(void (CScriptSound::*)(CLuaGameObject*,float,int))(CScriptSound::Play))
			.def("play_at_pos",					(void (CScriptSound::*)(CLuaGameObject*,const Fvector &))(CScriptSound::PlayAtPos))
			.def("play_at_pos",					(void (CScriptSound::*)(CLuaGameObject*,const Fvector &,float))(CScriptSound::PlayAtPos))
			.def("play_at_pos",					(void (CScriptSound::*)(CLuaGameObject*,const Fvector &,float, int))(CScriptSound::PlayAtPos))
			.def("play_clone",					(void (CScriptSound::*)(CLuaGameObject*))(CScriptSound::PlayUnlimited))
			.def("play_clone",					(void (CScriptSound::*)(CLuaGameObject*,float))(CScriptSound::PlayUnlimited))
			.def("play_clone",					(void (CScriptSound::*)(CLuaGameObject*,float,int))(CScriptSound::PlayUnlimited))
			.def("play_at_pos_clone",			(void (CScriptSound::*)(CLuaGameObject*,const Fvector &))(CScriptSound::PlayAtPosUnlimited))
			.def("play_at_pos_clone",			(void (CScriptSound::*)(CLuaGameObject*,const Fvector &,float))(CScriptSound::PlayAtPosUnlimited))
			.def("play_at_pos_clone",			(void (CScriptSound::*)(CLuaGameObject*,const Fvector &,float, int))(CScriptSound::PlayAtPosUnlimited))
			.def("stop",						&CScriptSound::Stop)
			.def("playing",						&CScriptSound::IsPlaying)
			.def("length",						&CScriptSound::Length)
	];
}
