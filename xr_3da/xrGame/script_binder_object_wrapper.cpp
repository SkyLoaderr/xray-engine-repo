////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object_wrapper.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_binder_object_wrapper.h"
#include "script_game_object.h"
#include "xrServer_Objects_ALife.h"

CScriptBinderObjectWrapper::CScriptBinderObjectWrapper	(CScriptGameObject *object) :
	CScriptBinderObject	(object)
{
}

CScriptBinderObjectWrapper::~CScriptBinderObjectWrapper ()
{
}

void CScriptBinderObjectWrapper::reinit					()
{
	luabind::call_member<void>		(this,"reinit");
}

void CScriptBinderObjectWrapper::reinit_static			(CScriptBinderObject *script_binder_object)
{
	script_binder_object->CScriptBinderObject::reinit	();
}

void CScriptBinderObjectWrapper::reload					(LPCSTR section)
{
	luabind::call_member<void>		(this,"reload",section);
}

void CScriptBinderObjectWrapper::reload_static			(CScriptBinderObject *script_binder_object, LPCSTR section)
{
	script_binder_object->CScriptBinderObject::reload	(section);
}

bool CScriptBinderObjectWrapper::net_Spawn				(SpawnType DC)
{
	return							(luabind::call_member<bool>(this,"net_spawn",DC));
}

bool CScriptBinderObjectWrapper::net_Spawn_static		(CScriptBinderObject *script_binder_object, SpawnType DC)
{
	return							(script_binder_object->CScriptBinderObject::net_Spawn(DC));
}

void CScriptBinderObjectWrapper::net_Destroy			()
{
	luabind::call_member<void>		(this,"net_destroy");
}

void CScriptBinderObjectWrapper::net_Destroy_static		(CScriptBinderObject *script_binder_object)
{
	script_binder_object->CScriptBinderObject::net_Destroy();
}

void CScriptBinderObjectWrapper::net_Import				(NetPacket *net_packet)
{
	luabind::call_member<void>		(this,"net_import",net_packet);
}

void CScriptBinderObjectWrapper::net_Import_static		(CScriptBinderObject *script_binder_object, NetPacket *net_packet)
{
	script_binder_object->CScriptBinderObject::net_Import	(net_packet);
}

void CScriptBinderObjectWrapper::net_Export				(NetPacket *net_packet)
{
	luabind::call_member<void>		(this,"net_export",net_packet);
}

void CScriptBinderObjectWrapper::net_Export_static		(CScriptBinderObject *script_binder_object, NetPacket *net_packet)
{
	script_binder_object->CScriptBinderObject::net_Export	(net_packet);
}

void CScriptBinderObjectWrapper::shedule_Update			(u32 time_delta)
{
	luabind::call_member<void>		(this,"update",time_delta);
}

void CScriptBinderObjectWrapper::shedule_Update_static	(CScriptBinderObject *script_binder_object, u32 time_delta)
{
	script_binder_object->CScriptBinderObject::shedule_Update	(time_delta);
}
