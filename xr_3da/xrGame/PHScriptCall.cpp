#include "stdafx.h"
#include "PHCommander.h"
#include "script_space_forward.h"
#include "script_callback_ex.h"
#include "PHScriptCall.h"
#include "script_space.h"
#include <luabind/operator.hpp>


CPHScriptCondition::CPHScriptCondition(const luabind::functor<bool> &func)
{
		m_lua_function	= xr_new<luabind::functor<bool> >(func);
}

CPHScriptCondition::~CPHScriptCondition()
{
	xr_delete(m_lua_function);
}

bool CPHScriptCondition::is_true()
{
	return (*m_lua_function)();
}

bool CPHScriptCondition::obsolete()
{
	return false;
}

CPHScriptAction::CPHScriptAction(const luabind::functor<void> &func)
{
	b_obsolete		= false;
	m_lua_function	= xr_new<luabind::functor<void> >(func);
}

CPHScriptAction::~CPHScriptAction()
{
	xr_delete(m_lua_function);
}

void CPHScriptAction::run()
{
	(*m_lua_function)();
	b_obsolete=true;
}

bool CPHScriptAction::obsolete()
{
	return b_obsolete;
}

/////////////////////////////////////////////////////////////////////////////////////////////
CPHScriptObjectAction::CPHScriptObjectAction(const luabind::object &lua_object, LPCSTR method)
{
	b_obsolete		= false;
	m_lua_object	= xr_new<luabind::object>(lua_object);
	m_method_name	= method;
}

CPHScriptObjectAction::~CPHScriptObjectAction()
{
	xr_delete(m_lua_object);
}

void CPHScriptObjectAction::run()
{
	luabind::call_member<void>(*m_lua_object,*m_method_name);
	b_obsolete=true;
}

bool CPHScriptObjectAction::obsolete()
{
	return b_obsolete;
}

CPHScriptObjectCondition::CPHScriptObjectCondition(const luabind::object &lua_object, LPCSTR method)
{
	m_lua_object	= xr_new<luabind::object>(lua_object);
	m_method_name	= method;
}

CPHScriptObjectCondition::~CPHScriptObjectCondition()
{
	xr_delete(m_lua_object);
}

bool CPHScriptObjectCondition::is_true()
{
	return luabind::call_member<bool>(*m_lua_object,*m_method_name);
}
bool CPHScriptObjectCondition::obsolete()
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPHScriptObjectActionN::CPHScriptObjectActionN( const luabind::object &object,const luabind::functor<void> &functor)
{
	m_callback.set(functor,object);
}

CPHScriptObjectActionN::~CPHScriptObjectActionN()
{
	m_callback.clear();
}

void CPHScriptObjectActionN::run()
{
	m_callback();
	b_obsolete=true;
}

bool CPHScriptObjectActionN::obsolete()
{
	return b_obsolete;
}

CPHScriptObjectConditionN::CPHScriptObjectConditionN(const luabind::object &object,const luabind::functor<bool> &functor)
{
	m_callback.set(functor,object);
}

CPHScriptObjectConditionN::~CPHScriptObjectConditionN()
{
	m_callback.clear();
}

bool CPHScriptObjectConditionN::is_true()
{
	return m_callback();
}
bool CPHScriptObjectConditionN::obsolete()
{
	return false;
}