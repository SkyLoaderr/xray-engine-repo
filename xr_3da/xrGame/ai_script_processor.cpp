////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_processor.cpp
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script processor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_space.h"
#include "ai_script_processor.h"
#include "ai_script.h"
#include "ai_script_lua_extension.h"

DEFINE_VECTOR(LPSTR,LPSTR_VECTOR,LPSTR_IT);

// I need this because we have to exclude option /EHsc (exception handling) from the project
namespace boost {
	void __stdcall throw_exception(const exception &A)
	{
		Debug.fatal("Boost exception raised %s",A.what());
	}
};

CScriptProcessor::CScriptProcessor(LPCSTR caCaption, LPCSTR caScriptString)
{
	Msg				("* Initializing %s script processor",caCaption);

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
	lua_pop			(m_tpLuaVirtualMachine,5);
#else
	lua_pop			(m_tpLuaVirtualMachine,4);
#endif

	Script::vfExportToLua(m_tpLuaVirtualMachine);

	u32				N = _GetItemCount(caScriptString);
	string16		I;
	string256		S,S1;
	for (u32 i=0; i<N; i++) {
		FS.update_path(S,"$game_scripts$",strconcat(S1,_GetItem(caScriptString,i,I),".script"));
		R_ASSERT3	(FS.exist(S),"Script file not found!",S);
		CScript		*l_tpScript = xr_new<CScript>(m_tpLuaVirtualMachine,S);
		if (l_tpScript->m_bActive)
			m_tpScripts.push_back(l_tpScript);
		else
			xr_delete(l_tpScript);
	}
}

CScriptProcessor::~CScriptProcessor()
{
	SCRIPT_IT		I = m_tpScripts.begin();
	SCRIPT_IT		E = m_tpScripts.end();
	for ( ; I != E; I++)
		xr_delete	(*I);
	lua_close		(m_tpLuaVirtualMachine);
}

void CScriptProcessor::Update()
{
	for (int i=0, n=(int)m_tpScripts.size(); i<n; i++) {
		if (!m_tpScripts[i]->Update()) {
			xr_delete(m_tpScripts[i]);
			m_tpScripts.erase(m_tpScripts.begin() + i);
			i--;
			n--;
		}
	}
}