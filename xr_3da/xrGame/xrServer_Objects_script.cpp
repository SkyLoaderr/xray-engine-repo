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
#include "phnetstate.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

void CPureServerObject::script_register(lua_State *L)
{
	module(L)[
		class_<IPureLîadableObject>
			("ipure_alife_load_object"),
		class_<IPureSavableObject>
			("ipure_alife_save_object"),
		class_<IPureSerializeObject,bases<IPureLîadableObject,IPureSavableObject> >
			("ipure_alife_load_save_object"),
		class_<IPureServerObject,IPureSerializeObject>
			("ipure_server_object"),
		class_<CPureServerObject,IPureServerObject>
			("cpure_server_object")
//			.def(		constructor<>())
	];
}

LPCSTR get_section_name(const CSE_Abstract *abstract)
{
	return	(abstract->name());
}

LPCSTR get_name(const CSE_Abstract *abstract)
{
	return	(abstract->name_replace());
}

void CSE_Abstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Abstract,CPureServerObject>
			("cse_abstract")
			.def_readonly	("id",				&CSE_Abstract::ID)
			.def_readonly	("parent_id",		&CSE_Abstract::ID_Parent)
			.def			("section_name",	&get_section_name)
			.def			("name",			&get_name)
			.def			("clsid",			&CSE_Abstract::script_clsid)
			.def_readwrite	("position",		&CSE_Abstract::o_Position)
//			.def(		constructor<LPCSTR>())
	];
}

void CSE_Shape::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Shape>
			("cse_shape")
//			.def(		constructor<>())
	];
}

void CSE_Visual::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Visual>
			("cse_visual")
//			.def(		constructor<>())
//			.def(		constructor<LPCSTR>())
	];
}

void CSE_Motion::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_Motion>
			("cse_motion")
//			.def(		constructor<>())
//			.def(		constructor<LPCSTR>())
	];
}

void CSE_Spectator::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_Spectator,
			"cse_spectator",
			CSE_Abstract
		)
	];
}

void CSE_Target::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_Target,
			"cse_target",
			CSE_Abstract
		)
	];
}

void CSE_TargetAssault::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_TargetAssault,
			"cse_target_assault",
			CSE_Abstract
		)
	];
}

void CSE_Target_CS_Base::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_Target_CS_Base,
			"cse_target_cs_base",
			CSE_Abstract
		)
	];
}

void CSE_Target_CS_Cask::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_Target_CS_Cask,
			"cse_target_cs_cask",
			CSE_Abstract
		)
	];
}

void CSE_Target_CS::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_Target_CS,
			"cse_target_cs",
			CSE_Abstract
		)
	];
}

void CSE_Temporary::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_Temporary,
			"cse_temporary",
			CSE_Abstract
		)
	];
}

void CSE_Event::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract2(
			CSE_Event,
			"cse_event",
			CSE_Shape,
			CSE_Abstract
		)
	];
}

void CSE_SpawnGroup::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_SpawnGroup,
			"cse_event",
			CSE_Abstract
		)
	];
}

void CSE_PHSkeleton::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_PHSkeleton>
			("cse_ph_skeleton")
	];
}
