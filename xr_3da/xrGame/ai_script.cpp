////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "ai_script.h"

CScript::CScript(LPCSTR caNamespaceName)
{
	string256			S;
	m_bActive			= false;
	m_script_name		= xr_strdup(caNamespaceName);
	Msg					("* Loading design script %s",caNamespaceName);

	ai().script_engine().add_file(caNamespaceName);
	ai().script_engine().process();

	m_tpLuaThread		= lua_newthread(ai().script_engine().lua());
	
//	sprintf				(S,"function start_thread()\n%s.main()\nend\n",caNamespaceName);
	sprintf				(S,"%s.main()",caNamespaceName);
	if (!ai().script_engine().load_buffer(m_tpLuaThread,S,xr_strlen(S),"@internal.script"))
		return;

//	lua_call			(m_tpLuaThread,0,0);

//	ai().script_engine().set_current_thread	(m_script_name);
//	luabind::resume_function<void>(m_tpLuaThread,"start_thread");
//	ai().script_engine().set_current_thread	("");

	m_bActive			= true;

	Update				();
}

CScript::~CScript()
{
	xr_delete			(m_script_name);
}

bool CScript::Update()
{
	if (!m_bActive)
		R_ASSERT2		(false,"Cannot resume dead Lua thread!");
	ai().script_engine().set_current_thread	(m_script_name);
	int					l_iErrorCode = lua_resume(m_tpLuaThread,0);
//	luabind::resume<void>(m_tpLuaThread);
	ai().script_engine().set_current_thread	("");
	if (l_iErrorCode) {
		if (!ai().script_engine().print_output(m_tpLuaThread,m_script_name,l_iErrorCode))
			ai().script_engine().print_error(m_tpLuaThread,l_iErrorCode);
		m_bActive		= false;
	}
	else
		if (!lua_gettop(m_tpLuaThread)) {
			m_bActive	= false;
			bool		ok = false;
			CLuaVirtualMachine *L = ai().script_engine().lua();
			for (int i=1, n=lua_gettop(L); i<=n; ++i)
				if ((lua_type(L,i) == LUA_TTHREAD) && (lua_tothread(L,i) == m_tpLuaThread)) {
					lua_remove(L,i);
					ok	= true;
					break;
				}
			VERIFY2		(ok,"Cannot find LUA thread in the LUA stack!");
			ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeInfo,"Script %s is finished!",m_script_name);
		}
		else {
			VERIFY		(lua_isnumber(m_tpLuaThread,-1));
			VERIFY		(lua_tonumber(m_tpLuaThread,-1) == 1);
			lua_pop		(m_tpLuaThread,1);
		}
	return				(m_bActive);
}