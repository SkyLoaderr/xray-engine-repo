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

void CSE_ALifeMonsterBase::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeMonsterBase,
			"cse_alife_monster_base",
			CSE_ALifeMonsterAbstract,
			CSE_PHSkeleton
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
		luabind_class_monster2(
			CSE_ALifeHumanStalker,
			"cse_alife_human_stalker",
			CSE_ALifeHumanAbstract,
			CSE_PHSkeleton
		)
	];
}
