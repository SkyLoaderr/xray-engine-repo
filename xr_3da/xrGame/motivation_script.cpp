////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_script.cpp
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "motivation.h"
#include "script_motivation_wrapper.h"
#include "script_space.h"
#include "script_game_object.h"

using namespace luabind;

void CMotivation<CScriptGameObject>::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptMotivation,CScriptMotivationWrapper>("motivation")
			.def_readonly("object",				&CScriptMotivation::m_object)
			.def(								constructor<>())
			.def("reinit",						&CScriptMotivationWrapper::reinit,		&CScriptMotivationWrapper::reinit_static)
			.def("load",						&CScriptMotivationWrapper::Load,		&CScriptMotivationWrapper::Load_static)
			.def("reload",						&CScriptMotivationWrapper::reload,		&CScriptMotivationWrapper::reload_static)
			.def("evaluate",					&CScriptMotivationWrapper::evaluate,	&CScriptMotivationWrapper::evaluate_static)
	];
}