////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script2.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

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

void CSE_ALifeHumanAbstract::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeHumanAbstract,
			"cse_alife_human_abstract",
			CSE_ALifeTraderAbstract,
			CSE_ALifeMonsterAbstract
		)
	];
}

void CSE_ALifeHumanStalker::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster1(
			CSE_ALifeHumanStalker,
			"cse_alife_human_stalker",
			CSE_ALifeHumanAbstract
		)
	];
}

void CSE_ALifeObjectIdol::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster1(
			CSE_ALifeObjectIdol,
			"cse_alife_object_idol",
			CSE_ALifeHumanAbstract
		)
	];
}

void CSE_ALifeTorridZone::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife2(
			CSE_ALifeTorridZone,
			"cse_torrid_zone",
			CSE_ALifeCustomZone,
			CSE_Motion
			)
	];
}
