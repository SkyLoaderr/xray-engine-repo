////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object_script.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_binder_object.h"
#include "script_export_space.h"
#include "script_binder_object_wrapper.h"
#include "xrServer_Objects_ALife.h"

using namespace luabind;

void CScriptBinderObject::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptBinderObject,CScriptBinderObjectWrapper>("object_binder")
			.def_readonly("object",				&CScriptBinderObject::m_object)
			.def(								constructor<CScriptGameObject*>())
			.def("reinit",						&CScriptBinderObjectWrapper::reinit,			&CScriptBinderObjectWrapper::reinit_static)
			.def("reload",						&CScriptBinderObjectWrapper::reload,			&CScriptBinderObjectWrapper::reload_static)
			.def("net_spawn",					&CScriptBinderObjectWrapper::net_Spawn,			&CScriptBinderObjectWrapper::net_Spawn_static)
			.def("net_destroy",					&CScriptBinderObjectWrapper::net_Destroy,		&CScriptBinderObjectWrapper::net_Destroy_static)
			.def("net_import",					&CScriptBinderObjectWrapper::net_Import,		&CScriptBinderObjectWrapper::net_Import_static)
			.def("net_export",					&CScriptBinderObjectWrapper::net_Export,		&CScriptBinderObjectWrapper::net_Export_static)
			.def("update",						&CScriptBinderObjectWrapper::shedule_Update,	&CScriptBinderObjectWrapper::shedule_Update_static)
	];
}