////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

void CSE_ALifeTraderAbstract::script_register(lua_State *L)
{
	module(L)[
		def("cast_alife_object_to_creature",&_dynamic_cast<CSE_ALifeCreatureAbstract,CSE_ALifeObject>),
		def("cast_alife_object_to_trader_abstract",&_dynamic_cast<CSE_ALifeTraderAbstract,CSE_ALifeObject>),

		class_<CSE_ALifeTraderAbstract>
			("cse_alife_trader_abstract")
//			.def(		constructor<LPCSTR>())
#ifdef XRGAME_EXPORTS
			.def("community",		CommunityName)
#endif // XRGAME_EXPORTS
	];
}

void CSE_ALifeTrader::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeTrader,
			"cse_alife_trader",
			CSE_ALifeDynamicObjectVisual,
			CSE_ALifeTraderAbstract
		)
	];
}

void CSE_ALifeCustomZone::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeCustomZone,
			"cse_custom_zone",
			CSE_ALifeDynamicObject,
			CSE_Shape
		)
	];
}

void CSE_ALifeAnomalousZone::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeAnomalousZone,
			"cse_anomalous_zone",
			CSE_ALifeCustomZone,
			CSE_ALifeSchedulable
		)
	];
}

void CSE_ALifeCreatureAbstract::script_register(lua_State *L)
{
	module(L)[
		luabind_class_creature1(
			CSE_ALifeCreatureAbstract,
			"cse_alife_creature_abstract",
			CSE_ALifeDynamicObjectVisual
		)
		.def("health",&CSE_ALifeCreatureAbstract::g_Health)
		.def("alive",&CSE_ALifeCreatureAbstract::g_Alive)
		.def_readwrite("team",&CSE_ALifeCreatureAbstract::s_team)
		.def_readwrite("squad",&CSE_ALifeCreatureAbstract::s_squad)
		.def_readwrite("group",&CSE_ALifeCreatureAbstract::s_group)
	];
}
