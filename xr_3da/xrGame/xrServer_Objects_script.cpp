////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 23.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Server objects script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects.h"
#include "script_space.h"

using namespace luabind;

void CPureServerObject::script_register(lua_State *L)
{
	module(L)[
		class_<IPureALifeLObject>
			("ipure_alife_load_object"),
		class_<IPureALifeSObject>
			("ipure_alife_save_object"),
		class_<IPureALifeLSObject,bases<IPureALifeLObject,IPureALifeSObject> >
			("ipure_alife_load_save_object"),
		class_<IPureServerObject,IPureALifeLSObject>
			("ipure_server_object"),
		class_<CPureServerObject,IPureServerObject>
			("cpure_server_object")
//			.def(		constructor<>())
	];
}

void CSE_Abstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Abstract,CPureServerObject>
			("cse_abstract")
			.def_readonly("id",				&CSE_Abstract::ID)
			.def_readonly("parent_id",		&CSE_Abstract::ID_Parent)
//			.def(		constructor<LPCSTR>())
	];
}

void CSE_Shape::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Shape>
			("cse_shape")
			.def(		constructor<>())
	];
}

void CSE_Visual::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Visual>
			("cse_visual")
			.def(		constructor<>())
			.def(		constructor<LPCSTR>())
	];
}

void CSE_Motion::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Motion>
			("cse_motion")
			.def(		constructor<>())
			.def(		constructor<LPCSTR>())
	];
}

void CSE_Spectator::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Spectator,CSE_Abstract>
			("cse_spectator")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_Target::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Target,CSE_Abstract>
			("cse_target")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_TargetAssault::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_TargetAssault,CSE_Abstract>
			("cse_target_assault")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_Target_CS_Base::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Target_CS_Base,CSE_Abstract>
			("cse_target_cs_base")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_Target_CS_Cask::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Target_CS_Cask,CSE_Abstract>
			("cse_target_cs_cask")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_Target_CS::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Target_CS,CSE_Abstract>
			("cse_target_cs")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_Temporary::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Temporary,CSE_Abstract>
			("cse_temporary")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_Event::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Event,CSE_Shape,CSE_Abstract>
			("cse_event")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_SpawnGroup::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_SpawnGroup,CSE_Abstract>
			("cse_spawn_group")
			.def(		constructor<LPCSTR>())
	];
}

