////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_space.h"
#include "ai_script.h"
#include "ai_script_lua_extension.h"

using namespace Script;

CScript::CScript(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caFileName)
{
	m_bActive		= false;
	strcpy			(m_caScriptFileName,caFileName);
	Msg				("* Loading design script %s",caFileName);

	if (!bfLoadFile(tpLuaVirtualMachine,caFileName,true))
		return;

	string256		l_caNamespaceName, S;
	_splitpath		(caFileName,0,0,l_caNamespaceName,0);
	sprintf			(S,"\nfunction script_name()\nreturn \"%s\"\nend\n",l_caNamespaceName);
	if (!bfLoadBuffer(tpLuaVirtualMachine,S,strlen(S),caFileName))
		return;

	int				l_iErrorCode = lua_pcall(tpLuaVirtualMachine,0,0,0);
	if (l_iErrorCode) {
#ifdef DEBUG
		vfPrintOutput	(tpLuaVirtualMachine,l_caNamespaceName);
		vfPrintError	(tpLuaVirtualMachine,l_iErrorCode);
#endif
		return;
	}
	
	m_tpLuaThread	= lua_newthread(m_tpLuaVirtualMachine = tpLuaVirtualMachine);
	
	sprintf			(S,"\n%s.main()\n",l_caNamespaceName);
	if (!bfLoadBuffer(m_tpLuaThread,S,strlen(S),"@internal.script")) {
		lua_pop		(tpLuaVirtualMachine,2);
		return;
	}

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
		vfPrintOutput(m_tpLuaThread,m_caScriptFileName);
		vfPrintError(m_tpLuaThread,l_iErrorCode);
		m_bActive	= false;
		return		(false);
#endif
	}
	return			(true);
}