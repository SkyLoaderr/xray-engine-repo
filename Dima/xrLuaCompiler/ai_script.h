////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.h
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"
//struct CLuaVirtualMachine;

DEFINE_VECTOR(CLuaVirtualMachine*,LUA_VM_VECTOR,LUA_VM_IT);

class CScript {
	CLuaVirtualMachine	*m_tpLuaVirtualMachine;
	CLuaVirtualMachine	*m_tpLuaThread;
	string256			m_caScriptFileName;
public:
	bool				m_bActive;

						CScript						(CLuaVirtualMachine		*tpLuaVirtualMachine, LPCSTR caFileName);
	virtual				~CScript					();
			bool		Update						();
};