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
		class_<CSE_ALifeTraderAbstract>
			("cse_alife_trader_abstract")
//			.def(		constructor<LPCSTR>())
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
			CSE_ALifeAnomalousZone,
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
	];
}

void CSE_ALifeMonsterAbstract::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeMonsterAbstract,
			"cse_alife_monster_abstract",
			CSE_ALifeCreatureAbstract,
			CSE_ALifeSchedulable
		)
	];
}

void CSE_ALifeCreatureActor::script_register(lua_State *L)
{
	module(L)[
		luabind_class_creature2(
			CSE_ALifeCreatureActor,
			"cse_alife_creature_actor",
			CSE_ALifeCreatureAbstract,
			CSE_ALifeTraderAbstract
		)
	];
}

void CSE_ALifeCreatureCrow::script_register(lua_State *L)
{
	module(L)[
		luabind_class_creature1(
			CSE_ALifeCreatureCrow,
			"cse_alife_creature_crow",
			CSE_ALifeCreatureAbstract
		)
	];
}

void CSE_ALifeMonsterRat::script_register(lua_State *L)
{
#pragma todo("Dima to Dima : export rat as an inventory item too!")
	module(L)[
		luabind_class_monster2(
			CSE_ALifeMonsterRat,
			"cse_alife_monster_rat",
			CSE_ALifeMonsterAbstract,
			CSE_ALifeInventoryItem
		)
	];
}

void CSE_ALifeMonsterZombie::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster1(
			CSE_ALifeMonsterZombie,
			"cse_alife_monster_zombie",
			CSE_ALifeMonsterAbstract
		)
	];
}

void CSE_ALifeMonsterBiting::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster1(
			CSE_ALifeMonsterBiting,
			"cse_alife_monster_biting",
			CSE_ALifeMonsterAbstract
		)
	];
}
