////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_wrapper.cpp
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_wrapper.h"

CScriptMotivationWrapper::~CScriptMotivationWrapper	()
{
}

void CScriptMotivationWrapper::reinit			(_object_type *object)
{
	luabind::call_member<void>				(m_lua_instance,"reinit",object);
}

void CScriptMotivationWrapper::reinit_static	(inherited *motivation, CLuaGameObject *object)
{
	motivation->inherited::reinit	(object);
}

void CScriptMotivationWrapper::Load				(LPCSTR section)
{
	luabind::call_member<void>				(m_lua_instance,"load",section);
}

void CScriptMotivationWrapper::Load_static		(inherited *motivation, LPCSTR section)
{
	motivation->inherited::Load		(section);
}

void CScriptMotivationWrapper::reload			(LPCSTR section)
{
	luabind::call_member<void>				(m_lua_instance,"reload",section);
}

void CScriptMotivationWrapper::reload_static	(inherited *motivation, LPCSTR section)
{
	motivation->inherited::reload	(section);
}

float CScriptMotivationWrapper::evaluate		(u32 sub_motivation_id)
{
	return									(luabind::call_member<float>(m_lua_instance,"evaluate",sub_motivation_id));
}

float CScriptMotivationWrapper::evaluate_static	(inherited *motivation, u32 sub_motivation_id)
{
	return									(motivation->inherited::evaluate(sub_motivation_id));
}