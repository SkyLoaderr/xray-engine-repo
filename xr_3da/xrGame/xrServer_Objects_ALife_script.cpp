////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server objects for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

void CSE_ALifeSchedulable::script_register(lua_State *L)
{
	module(L)[
		class_<IPureSchedulableObject>
			("ipure_schedulable_object"),
//			.def(		constructor<>()),
		
		class_<CSE_ALifeSchedulable,IPureSchedulableObject>
			("cse_alife_schedulable")
//			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeGraphPoint::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_ALifeGraphPoint,
			"cse_alife_graph_point",
			CSE_Abstract
		)
	];
}

void CSE_ALifeObject::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife1(
			CSE_ALifeObject,
			"cse_alife_object",
			CSE_Abstract
		)
		.def_readonly("online",	&CSE_ALifeObject::m_bOnline)
	];
}

void CSE_ALifeGroupAbstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeGroupAbstract>
			("cse_alife_group_abstract")
//			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeDynamicObject::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife1(
			CSE_ALifeDynamicObject,
			"cse_alife_dynamic_object",
			CSE_ALifeObject
		)
	];
}

void CSE_ALifeDynamicObjectVisual::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeDynamicObjectVisual,
			"cse_alife_dynamic_object_visual",
			CSE_ALifeDynamicObject,
			CSE_Visual
		)
	];
}

void CSE_ALifePHSkeletonObject::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifePHSkeletonObject,
			"cse_alife_ph_skeleton_object",
			CSE_ALifeDynamicObjectVisual,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeScriptZone::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeScriptZone,
			"cse_alife_script_zone",
			CSE_ALifeDynamicObject,
			CSE_Shape
		)
	];
}

void CSE_ALifeLevelChanger::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeLevelChanger,
			"cse_alife_level_changer",
			CSE_ALifeScriptZone,
			CSE_Shape
		)
	];
}

void CSE_ALifeObjectPhysic::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeObjectPhysic,
			"cse_alife_object_physic",
			CSE_ALifeDynamicObjectVisual,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeObjectHangingLamp::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeObjectHangingLamp,
			"cse_alife_object_hanging_lamp",
			CSE_ALifeDynamicObjectVisual,
			CSE_PHSkeleton
		)
	];
}
