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
		class_<CSE_ALifeTrader,CSE_ALifeDynamicObjectVisual,CSE_ALifeTraderAbstract>
			("cse_alife_trader")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeAnomalousZone::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeAnomalousZone,CSE_ALifeDynamicObject,CSE_ALifeSchedulable,CSE_Shape>
			("cse_alife_anomalous_zone")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeCreatureAbstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeCreatureAbstract,CSE_ALifeDynamicObjectVisual>
			("cse_alife_creature_abstract")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeMonsterAbstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMonsterAbstract,CSE_ALifeCreatureAbstract,CSE_ALifeSchedulable>
			("cse_alife_monster_abstract")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeCreatureActor::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeCreatureActor,CSE_ALifeCreatureAbstract,CSE_ALifeTraderAbstract>
			("cse_alife_creature_actor")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeCreatureCrow::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeCreatureCrow,CSE_ALifeCreatureAbstract>
			("cse_alife_creature_crow")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeMonsterRat::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMonsterRat,CSE_ALifeMonsterAbstract,CSE_ALifeInventoryItem>
			("cse_alife_monster_rat")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeMonsterZombie::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMonsterZombie,CSE_ALifeMonsterAbstract>
			("cse_alife_monster_zombie")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeMonsterBiting::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMonsterBiting,CSE_ALifeMonsterAbstract>
			("cse_alife_monster_biting")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeHumanAbstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeHumanAbstract,CSE_ALifeTraderAbstract,CSE_ALifeMonsterAbstract>
			("cse_alife_human_abstract")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeHumanStalker::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeHumanStalker,CSE_ALifeHumanAbstract>
			("cse_alife_human_stalker")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeObjectIdol::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeObjectIdol,CSE_ALifeHumanAbstract>
			("cse_alife_object_idol")
			.def(		constructor<LPCSTR>())
	];
}
