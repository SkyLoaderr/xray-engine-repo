////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.h
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"

DEFINE_VECTOR(CLuaVirtualMachine*,LUA_VM_VECTOR,LUA_VM_IT);

class CScript {
public:
	CLuaVirtualMachine	*m_tpLuaVirtualMachine;
	LUA_VM_VECTOR		m_tpThreads;
	string4096			m_caOutBuffer;
	string4096			m_caErrorBuffer;

						CScript						(LPCSTR				caFileName);
	virtual				~CScript					();
			void		Update						();
};