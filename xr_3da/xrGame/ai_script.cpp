////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script.h"

// Lua
#pragma warning(disable:4244)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#pragma warning(disable:4267)

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#pragma comment(lib,"x:\\xrLUA.lib")

// Lua-bind
#include <luabind\\luabind.hpp>

#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4530)
#pragma warning(default:4267)

DEFINE_VECTOR(LPSTR,LPSTR_VECTOR,LPSTR_IT);

// I need this because we have to exclude option /EHsc (exception handling) from the project
namespace boost {
	void __stdcall throw_exception(const exception &A)
	{
		Debug.fatal("Boost exception raised %s",A.what());
	}
};

CScriptProcessor::CScriptProcessor(LPCSTR caFilePath)
{
	Msg				("* [SP] Initializing script processor");

	LPSTR_VECTOR	*l_tpFileList = FS.file_list_open(caFilePath);
	
	if (l_tpFileList) {
		LPSTR_IT		I = l_tpFileList->begin();
		LPSTR_IT		E = l_tpFileList->end();
		for ( ; I != E; I++) {
			string256	l_caExtension;
			_splitpath(*I,0,0,0,l_caExtension);
			if (!strcmp(l_caExtension,".script")) {
				strconcat(l_caExtension,caFilePath,*I);
				m_tpScripts.push_back(xr_new<CScript>(l_caExtension));
			}
		}
	}

	FS.file_list_close(l_tpFileList);
}

CScriptProcessor::~CScriptProcessor()
{
	SCRIPT_IT		I = m_tpScripts.begin();
	SCRIPT_IT		E = m_tpScripts.end();
	for ( ; I != E; I++)
        xr_delete	(*I);
}

void CScriptProcessor::Update()
{
	SCRIPT_IT		I = m_tpScripts.begin();
	SCRIPT_IT		E = m_tpScripts.end();
	for ( ; I != E; I++)
        (*I)->Update();
}

CScript::CScript(LPCSTR caFileName)
{
	m_tpLuaVirtualMachine = lua_open();
	if (!m_tpLuaVirtualMachine) {
		Msg			("! ERROR : Cannot initialize script processor!");
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

	Msg				("* [SP] Exproting C++ data to Lua");
	vfExportToLua	();

	Msg				("* Loading script %s",caFileName);
	IReader			*l_tpFileReader = FS.r_open(caFileName);
	R_ASSERT		(l_tpFileReader);

	Msg				("* Launching script %s",caFileName);
	m_tpThreads.push_back(lua_newthread(m_tpLuaVirtualMachine));
	lua_dostring	(m_tpThreads[0],static_cast<LPCSTR>(l_tpFileReader->pointer()));
	FS.r_close		(l_tpFileReader);
}

CScript::~CScript()
{
	// according to manual we cannot close all the threads, one must be left 
	// and then the virtual machine should be closed
//	LUA_VM_IT		I = m_tpThreads.begin(), B = I;
//	LUA_VM_IT		E = m_tpThreads.end();
//	for ( ; I != E; I++)
//		if (I != B)
//			lua_closethread(m_tpLuaVirtualMachine,*I);
	lua_close		(m_tpLuaVirtualMachine);
}

void CScript::Update()
{
	LUA_VM_IT		I = m_tpThreads.begin(), B = I;
	LUA_VM_IT		E = m_tpThreads.end();
	for ( ; I != E; I++)
		lua_resume	(*I,0);
}

void CScript::vfExportToLua()
{
	using namespace luabind;
	open			(m_tpLuaVirtualMachine);
	function		(m_tpLuaVirtualMachine,"print",(void (*) (LPCSTR,int))(Log));
//	function		(m_tpLuaVirtualMachine,"print",(void (*) (LPCSTR,...))(Msg));
}