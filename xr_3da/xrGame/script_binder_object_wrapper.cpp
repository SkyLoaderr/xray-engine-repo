////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object_wrapper.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_binder_object_wrapper.h"

CScriptBinderObjectWrapper::CScriptBinderObjectWrapper	(CLuaGameObject *object) :
	CScriptBinderObject	(object)
{
}

CScriptBinderObjectWrapper::~CScriptBinderObjectWrapper ()
{
}

void CScriptBinderObjectWrapper::reinit					()
{
	call_member<void>		("reinit");
}

void CScriptBinderObjectWrapper::reinit_static			(CScriptBinderObject *script_binder_object)
{
	script_binder_object->CScriptBinderObject::reinit	();
}

void CScriptBinderObjectWrapper::reload					(LPCSTR section)
{
	call_member<void>		("reload",section);
}

void CScriptBinderObjectWrapper::reload_static			(CScriptBinderObject *script_binder_object, LPCSTR section)
{
	script_binder_object->CScriptBinderObject::reload	(section);
}

bool CScriptBinderObjectWrapper::net_Spawn				(SpawnType DC)
{
	return			(call_member<bool>("net_spawn",DC));
}

bool CScriptBinderObjectWrapper::net_Spawn_static		(CScriptBinderObject *script_binder_object, SpawnType DC)
{
	return			(script_binder_object->CScriptBinderObject::net_Spawn(DC));
}

void CScriptBinderObjectWrapper::net_Destroy			()
{
	call_member<void>		("net_destroy");
}

void CScriptBinderObjectWrapper::net_Destroy_static		(CScriptBinderObject *script_binder_object)
{
	script_binder_object->CScriptBinderObject::net_Destroy();
}

void CScriptBinderObjectWrapper::net_Import				(NetPacket *net_packet)
{
	call_member<void>		("net_import",net_packet);
}

void CScriptBinderObjectWrapper::net_Import_static		(CScriptBinderObject *script_binder_object, NetPacket *net_packet)
{
	script_binder_object->CScriptBinderObject::net_Import	(net_packet);
}

void CScriptBinderObjectWrapper::net_Export				(NetPacket *net_packet)
{
	call_member<void>		("net_export",net_packet);
}

void CScriptBinderObjectWrapper::net_Export_static		(CScriptBinderObject *script_binder_object, NetPacket *net_packet)
{
	script_binder_object->CScriptBinderObject::net_Export	(net_packet);
}