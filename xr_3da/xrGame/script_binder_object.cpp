////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_binder_object.h"
#include "ai_script_classes.h"

CScriptBinderObject::CScriptBinderObject	(CLuaGameObject *object)
{
	m_object		= object;
}

CScriptBinderObject::~CScriptBinderObject	()
{
//	if (m_object)
//		Msg			("Destroying binded object %s",m_object->Name());
}

void CScriptBinderObject::reinit			()
{
}

void CScriptBinderObject::reload			(LPCSTR section)
{
}

bool CScriptBinderObject::net_Spawn			(SpawnType DC)
{
	return			(true);
}

void CScriptBinderObject::net_Destroy		()
{
}

void CScriptBinderObject::net_Import		(NetPacket *net_packet)
{
}

void CScriptBinderObject::net_Export		(NetPacket *net_packet)
{
}