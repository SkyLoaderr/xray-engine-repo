////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items_script2.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server items for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Items.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

void CSE_ALifeItemBolt::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemBolt,
			"cse_alife_item_bolt",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemCustomOutfit::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemCustomOutfit,
			"cse_alife_item_custom_outfit",
			CSE_ALifeItem
		)
	];
}
