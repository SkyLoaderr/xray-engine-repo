////////////////////////////////////////////////////////////////////////////
//	Module 		: script_stack_tracker.cpp
//	Created 	: 21.04.2004
//  Modified 	: 21.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script stack tracker
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_stack_tracker.h"

CScriptStackTracker::~CScriptStackTracker	()
{
}

void CScriptStackTracker::script_hook	(CLuaVirtualMachine *L, lua_Debug *dbg)
{
	VERIFY				(L && (m_virtual_machine == L));

	switch	(dbg->event) {
		case LUA_HOOKCALL : {
			if (!lua_getstack(L,0,&m_stack[m_current_stack_level]))
				break;
			lua_getinfo	(L,"nSlu",&m_stack[m_current_stack_level]);
			if (m_current_stack_level && lua_getstack(L,1,&m_stack[m_current_stack_level - 1]))
				lua_getinfo	(L,"nSlu",&m_stack[m_current_stack_level - 1]);
			++m_current_stack_level;
			break;
							}
		case LUA_HOOKRET : {
			--m_current_stack_level;
			break;
		}
		case LUA_HOOKLINE : {
			lua_getinfo	(L,"l",dbg);
			m_stack[m_current_stack_level].currentline = dbg->currentline;
			break;
		}
		case LUA_HOOKTAILRET : {
			break;
		}
		case LUA_HOOKCOUNT : {
			lua_getinfo	(L,"l",dbg);
			m_stack[m_current_stack_level].currentline = dbg->currentline;
			break;
		}
		default : NODEFAULT;
	}
}

void CScriptStackTracker::print_error	(CLuaVirtualMachine *L)
{
	VERIFY				(L && (m_virtual_machine == L));

	for (int j=m_current_stack_level - 1, k=0; j>0; --j, ++k) {
		lua_Debug		l_tDebugInfo = m_stack[j];
		if (!l_tDebugInfo.name)
			Msg			("%2d : [C  ] C source code : %s",k,l_tDebugInfo.short_src);
		else
			if (!xr_strcmp(l_tDebugInfo.what,"C"))
				Msg		("%2d : [C  ] C source code : %s",k,l_tDebugInfo.name);
			else
				Msg		("%2d : [%s] %s(%d) : %s",k,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,l_tDebugInfo.name);
	}
	m_current_stack_level = 0;
}
