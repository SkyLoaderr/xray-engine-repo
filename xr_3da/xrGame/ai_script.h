////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.h
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#pragma once

struct lua_State;
typedef lua_State CLuaVirtualMachine;

DEFINE_VECTOR(CLuaVirtualMachine*,LUA_VM_VECTOR,LUA_VM_IT);

class CScript {
public:
	CLuaVirtualMachine	*m_tpLuaVirtualMachine;
	LUA_VM_VECTOR		m_tpThreads;

						CScript						(LPCSTR				caFileName);
	virtual				~CScript					();
			void		vfExportToLua				();
			void		Update						();
			void		vfPrintError				(CLuaVirtualMachine *tpLuaVirtualMachine, int iErrorCode);
};

DEFINE_VECTOR(CScript*,SCRIPT_VECTOR,SCRIPT_IT);

class CScriptProcessor {
public:
	SCRIPT_VECTOR		m_tpScripts;

						CScriptProcessor			(LPCSTR				caFilePath);
	virtual				~CScriptProcessor			();
			void		Update						();
};