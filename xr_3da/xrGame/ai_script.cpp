////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script.h"
#include "ai_script_lua_extension.h"

using namespace Script;

CScript::CScript(LPCSTR caFileName)
{
	m_tpLuaVirtualMachine = lua_open();
	if (!m_tpLuaVirtualMachine) {
		Msg			("! ERROR : Cannot initialize script virtual machine!");
		return;
	}

	// initialize lua standard library functions 
	luaopen_base	(m_tpLuaVirtualMachine); 
	luaopen_table	(m_tpLuaVirtualMachine);
	luaopen_string	(m_tpLuaVirtualMachine);
	luaopen_math	(m_tpLuaVirtualMachine);
#ifdef DEBUG
	luaopen_debug	(m_tpLuaVirtualMachine);
#endif

	vfExportToLua	(m_tpLuaVirtualMachine);

	Msg				("* Loading script %s",caFileName);
	IReader			*l_tpFileReader = FS.r_open(caFileName);
	R_ASSERT		(l_tpFileReader);

	CLuaVirtualMachine	*l_tpThread = lua_newthread(m_tpLuaVirtualMachine);
	int				i = luaL_loadbuffer(l_tpThread,static_cast<LPCSTR>(l_tpFileReader->pointer()),(size_t)l_tpFileReader->length(),static_cast<LPCSTR>(l_tpFileReader->pointer()));
#ifdef DEBUG
	if (i)
		vfPrintError(m_tpLuaVirtualMachine,i);
#endif
	m_tpThreads.push_back	(l_tpThread);

	FS.r_close		(l_tpFileReader);
}

CScript::~CScript()
{
	lua_close		(m_tpLuaVirtualMachine);
}

void CScript::Update()
{
	for (int i=0, n = int(m_tpThreads.size()); i<n; i++) {
		int		l_iErrorCode = lua_resume	(m_tpThreads[i],0);
		if (l_iErrorCode) {
#ifdef DEBUG
			vfPrintError(m_tpThreads[i],l_iErrorCode);
#endif
			m_tpThreads.erase(m_tpThreads.begin() + i);
			i--;
			n--;
		}
	}
}