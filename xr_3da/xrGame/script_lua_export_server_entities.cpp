////////////////////////////////////////////////////////////////////////////
//	Module 		: script_lua_export_server_entities.cpp
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export server entities
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "ai_space.h"
#include "xrServer_Objects_ALife_All.h"
#include "luabind/adopt_policy.hpp"

using namespace luabind;

//ALife::_OBJECT_ID	abstract_id(const CSE_Abstract *abstract)
ALife::_OBJECT_ID	abstract_id(const CSE_ALifeObject *abstract)
{
	return			(abstract->ID);
}

//ALife::_OBJECT_ID	abstract_parent_id(const CSE_Abstract *abstract)
ALife::_OBJECT_ID	abstract_parent_id(const CSE_ALifeObject *abstract)
{
	return			(abstract->ID_Parent);
}

void CScriptEngine::export_server_entities()
{
	module(lua())
	[
//		class_<CSE_Abstract>("abstract_object")
//			.def("id",						&abstract_id)
//			.def("parent_id",				&abstract_parent_id),
//
//		class_<CSE_ALifeObject,CSE_Abstract>("alife_object")
//		class_<CSE_Abstract>("base_server_class"),
		class_<CSE_Visual>("visual_server_class"),
		
		class_<CSE_ALifeObject>("alife_object")
			.def(							constructor<LPCSTR>())
			.def("id",						&abstract_id)
			.def("parent_id",				&abstract_parent_id)
			.def("can_switch_online",		(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::can_switch_online))
			.def("can_switch_offline",		(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::can_switch_offline))
			.def("interactive",				(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::interactive)),

		class_<CSE_ALifeDynamicObject,CSE_ALifeObject>("alife_dynamic_object")
			.def(							constructor<LPCSTR>()),

		class_<CSE_ALifeDynamicObjectVisual,bases<CSE_ALifeDynamicObject,CSE_Visual> >("alife_dynamic_object_visual")
			.def(							constructor<LPCSTR>()),

		class_<CSE_ALifeInventoryItem>("alife_inventory_item")
			.def(							constructor<LPCSTR>()),

		class_<CSE_ALifeItem,bases<CSE_ALifeDynamicObjectVisual,CSE_ALifeInventoryItem> >("alife_item")
			.def(							constructor<LPCSTR>())
	];
}