////////////////////////////////////////////////////////////////////////////
//	Module 		: script_lua_export_alife.cpp
//	Created 	: 01.06.2004
//  Modified 	: 01.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export ALife
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "alife_story_registry.h"

using namespace luabind;

CALifeSimulator *alife			()
{
	return			(const_cast<CALifeSimulator*>(ai().get_alife()));
}

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

CSE_ALifeDynamicObject *alife_object		(const CALifeSimulator *self, ALife::_OBJECT_ID id)
{
	VERIFY			(self);
	return			(self->objects().object(id,true));
}

CSE_ALifeDynamicObject *alife_story_object	(const CALifeSimulator *self, ALife::_STORY_ID id)
{
	return			(self->story_objects().object(id,true));
}

void CScriptEngine::export_alife()
{
	module(lua())
	[
//		class_<CSE_Abstract>("abstract_object")
//			.def("id",						&abstract_id)
//			.def("parent_id",				&abstract_parent_id),
//
//		class_<CSE_ALifeObject,CSE_Abstract>("alife_object")
		class_<CSE_ALifeObject>("alife_object")
			.def(							constructor<LPCSTR>())
			.def("id",						&abstract_id)
			.def("parent_id",				&abstract_parent_id)
			.def("can_switch_online",		(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::can_switch_online))
			.def("can_switch_offline",		(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::can_switch_offline))
			.def("interactive",				(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::interactive)),

		class_<CSE_ALifeDynamicObject,CSE_ALifeObject>("alife_dynamic_object")
			.def(							constructor<LPCSTR>()),

		class_<CALifeSimulator>("alife_simulator")
			.def("object",					(CSE_ALifeDynamicObject *(*) (const CALifeSimulator *,ALife::_OBJECT_ID))(alife_object))
			.def("story_object",			(CSE_ALifeDynamicObject *(*) (const CALifeSimulator *,ALife::_STORY_ID))(alife_story_object))
			.def("set_switch_online",		(void (CALifeSimulator::*) (ALife::_OBJECT_ID,bool))(CALifeSimulator::set_switch_online))
			.def("set_switch_offline",		(void (CALifeSimulator::*) (ALife::_OBJECT_ID,bool))(CALifeSimulator::set_switch_offline))
			.def("set_interactive",			(void (CALifeSimulator::*) (ALife::_OBJECT_ID,bool))(CALifeSimulator::set_interactive)),

		def("alife",						&alife)
	];
}
