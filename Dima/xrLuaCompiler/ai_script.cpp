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

CScript::CScript(LPCSTR caFileName)
{
	m_bActive			= false;
	m_caScriptFileName	= caFileName;
	Msg					("* Loading design script %s",caFileName);

	if (!ai().script_engine().load_file(caFileName,true))
		return;

	string256		l_caNamespaceName, S;
	_splitpath		(caFileName,0,0,l_caNamespaceName,0);
	sprintf			(S,"\nfunction %s.script_name()\nreturn \"%s\"\nend\n",l_caNamespaceName,l_caNamespaceName);

	if (!ai().script_engine().load_buffer(ai().script_engine().lua(),S,xr_strlen(S),caFileName))
		return;
	
	lua_call		(ai().script_engine().lua(),0,0);

	m_tpLuaThread	= lua_newthread(ai().script_engine().lua());
	
	sprintf			(S,"\n%s.main()\n",l_caNamespaceName);
	if (!ai().script_engine().load_buffer(m_tpLuaThread,S,xr_strlen(S),"@internal.script")) {
		lua_pop		(ai().script_engine().lua(),2);
		return;
	}

#ifdef DEBUG
	lua_sethook		(m_tpLuaThread, CScriptEngine::lua_hook_call,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	0);
#endif

	m_bActive		= true;
}

CScript::~CScript()
{
}

bool CScript::Update()
{
	if (!m_bActive)
		R_ASSERT2	(false,"Cannot resume dead Lua thread!");
	int				l_iErrorCode = lua_resume(m_tpLuaThread,0);
	if (l_iErrorCode) {
#ifdef DEBUG
		if (!ai().script_engine().print_output(m_tpLuaThread,*m_caScriptFileName,l_iErrorCode))
			ai().script_engine().print_error(m_tpLuaThread,l_iErrorCode);
		m_bActive	= false;
		return		(false);
#endif
	}
	return			(true);
}