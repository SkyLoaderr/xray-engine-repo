////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server items for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Items.h"
#include "script_space.h"

using namespace luabind;

void CSE_ALifeInventoryItem::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeInventoryItem>
			("cse_alife_inventory_item")
//			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItem::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItem,CSE_ALifeDynamicObjectVisual,CSE_ALifeInventoryItem>
			("cse_alife_item")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemTorch::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemTorch,CSE_ALifeItem>
			("cse_alife_item_torch")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemAmmo::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemAmmo,CSE_ALifeItem>
			("cse_alife_item_torch")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemWeapon::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemWeapon,CSE_ALifeItem>
			("cse_alife_item_weapon")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemDetector::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemDetector,CSE_ALifeItem>
			("cse_alife_item_detector")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemArtefact::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemArtefact,CSE_ALifeItem>
			("cse_alife_item_artefact")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemPDA::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemPDA,CSE_ALifeItem>
			("cse_alife_item_pda")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemDocument::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemDocument,CSE_ALifeItem>
			("cse_alife_item_document")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemGrenade::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemGrenade,CSE_ALifeItem>
			("cse_alife_item_grenade")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemExplosive::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemExplosive,CSE_ALifeItem>
			("cse_alife_item_explosive")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemBolt::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemBolt,CSE_ALifeItem>
			("cse_alife_item_bolt")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeItemCustomOutfit::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeItemCustomOutfit,CSE_ALifeItem>
			("cse_alife_item_custom_outfit")
			.def(		constructor<LPCSTR>())
	];
}
