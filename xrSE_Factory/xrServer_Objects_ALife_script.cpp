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
		class_<CSE_ALifeGraphPoint,CSE_Abstract>
			("cse_alife_graph_point")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeObject::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeObject,CSE_Abstract>
			("cse_alife_object")
			.def(							constructor<LPCSTR>())
			.def("can_switch_online",		(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::can_switch_online))
			.def("can_switch_offline",		(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::can_switch_offline))
			.def("interactive",				(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::interactive))
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
		class_<CSE_ALifeDynamicObject,CSE_ALifeObject>
			("cse_alife_dynamic_object")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeDynamicObjectVisual::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeDynamicObjectVisual,CSE_ALifeDynamicObject,CSE_Visual>
			("cse_alife_dynamic_object_visual")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifePHSkeletonObject::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifePHSkeletonObject,CSE_ALifeDynamicObjectVisual>
			("cse_alife_ph_skeleton_object")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeScriptZone::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeScriptZone,CSE_ALifeDynamicObject,CSE_Shape>
			("cse_alife_script_zone")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeLevelChanger::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeLevelChanger,CSE_ALifeScriptZone>
			("cse_alife_level_changer")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeObjectPhysic::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeObjectPhysic,CSE_ALifePHSkeletonObject>
			("cse_alife_object_physic")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeObjectHangingLamp::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeObjectHangingLamp,CSE_ALifeDynamicObjectVisual>
			("cse_alife_object_hanging_lamp")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeObjectProjector::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeObjectProjector,CSE_ALifeDynamicObjectVisual>
			("cse_alife_object_projector")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeHelicopter::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeHelicopter,CSE_ALifeDynamicObjectVisual,CSE_Motion>
			("cse_alife_helicopter")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeCar::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeCar,CSE_ALifeDynamicObjectVisual>
			("cse_alife_car")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeObjectBreakable::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeObjectBreakable,CSE_ALifeDynamicObjectVisual>
			("cse_alife_object_breakable")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeMountedWeapon::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMountedWeapon,CSE_ALifeDynamicObjectVisual>
			("cse_alife_mounted_weapon")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeTeamBaseZone::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeTeamBaseZone,CSE_ALifeScriptZone>
			("cse_alife_team_base_zone")
			.def(		constructor<LPCSTR>())
	];
}
