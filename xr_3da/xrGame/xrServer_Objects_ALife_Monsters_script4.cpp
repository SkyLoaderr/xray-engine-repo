////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script4.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"
#include "alife_human_brain.h"

using namespace luabind;

CALifeHumanBrain *get_brain(CSE_ALifeHumanAbstract *human)
{
	THROW	(human);
	return	(&human->brain());
}

void clear_smart_terrain(CSE_ALifeHumanAbstract *human)
{
	THROW						(human);
	human->m_smart_terrain_id	= 0xffff;
}

ALife::_OBJECT_ID smart_terrain_id	(CSE_ALifeHumanAbstract *human)
{
	THROW						(human);
	return						(human->m_smart_terrain_id);
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

void CSE_ALifeHumanAbstract::script_register(lua_State *L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeHumanAbstract,
			"cse_alife_human_abstract",
			CSE_ALifeTraderAbstract,
			CSE_ALifeMonsterAbstract
		)
		.def("smart_terrain_id",	&smart_terrain_id)
		.def("clear_smart_terrain",	&clear_smart_terrain)
		.def("brain",				&get_brain)
	];
}
