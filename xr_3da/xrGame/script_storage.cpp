////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_storage.h"
#include <stdarg.h>
#ifndef ENGINE_BUILD
#include "script_engine.h"
#endif

CScriptStorage::CScriptStorage		()
{
	m_virtual_machine		= 0;
	m_virtual_machine		= lua_open();
	if (!m_virtual_machine) {
		Msg					("! ERROR : Cannot initialize script virtual machine!");
		return;
	}
	// initialize lua standard library functions 
	luaopen_base			(m_virtual_machine); 
	luaopen_table			(m_virtual_machine);
	luaopen_string			(m_virtual_machine);
	luaopen_math			(m_virtual_machine);
#ifdef DEBUG
	luaopen_debug			(m_virtual_machine);
#endif
	lua_settop				(m_virtual_machine,0);
}

CScriptStorage::~CScriptStorage		()
{
	if (m_virtual_machine)
		lua_close			(m_virtual_machine);
}

int __cdecl CScriptStorage::script_log	(ScriptStorage::ELuaMessageType tLuaMessageType, LPCSTR caFormat, ...)
{
#ifndef ENGINE_BUILD
	if (!psAI_Flags.test(aiLua))
		return(0);
#endif

	LPCSTR		S = "", SS = "";
	LPSTR		S1;
	string4096	S2;
	switch (tLuaMessageType) {
		case ScriptStorage::eLuaMessageTypeInfo : {
			S	= "* [LUA] ";
			SS	= "[INFO]        ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeError : {
			S	= "! [LUA] ";
			SS	= "[ERROR]       ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeMessage : {
			S	= "[LUA] ";
			SS	= "[MESSAGE]     ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookCall : {
			S	= "[LUA][HOOK_CALL] ";
			SS	= "[CALL]        ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookReturn : {
			S	= "[LUA][HOOK_RETURN] ";
			SS	= "[RETURN]      ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookLine : {
			S	= "[LUA][HOOK_LINE] ";
			SS	= "[LINE]        ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookCount : {
			S	= "[LUA][HOOK_COUNT] ";
			SS	= "[COUNT]       ";
			break;
		}
		case ScriptStorage::eLuaMessageTypeHookTailReturn : {
			S	= "[LUA][HOOK_TAIL_RETURN] ";
			SS	= "[TAIL_RETURN] ";
			break;
		}
		default : NODEFAULT;
	}
	
	va_list	l_tMarker;
	
	va_start(l_tMarker,caFormat);

	strcpy	(S2,S);
	S1		= S2 + xr_strlen(S);
	int		l_iResult = vsprintf(S1,caFormat,l_tMarker);
	Msg		("%s",S2);
	
	strcpy	(S2,SS);
	S1		= S2 + xr_strlen(SS);
	vsprintf(S1,caFormat,l_tMarker);

#ifdef ENGINE_BUILD
#else
	ai().script_engine().m_output.w_string(S2);
#endif

	va_end	(l_tMarker);

	return	(l_iResult);
}

bool CScriptStorage::create_namespace	(LPCSTR caNamespaceName)
{
	lua_pushstring	(lua(),"_G");
	lua_gettable	(lua(),LUA_GLOBALSINDEX);
	LPSTR			S2	= xr_strdup(caNamespaceName);
	LPSTR			S	= S2;
	for (;;) {
		if (!xr_strlen(S)) {
			lua_pop		(lua(),1);
			script_log	(ScriptStorage::eLuaMessageTypeError,"the namespace name %s is incorrect!",caNamespaceName);
			xr_free		(S2);
			return		(false);
		}
		LPSTR			S1 = strchr(S,'.');
		if (S1)
			*S1				= 0;
		lua_pushstring	(lua(),S);
		lua_gettable	(lua(),-2);
		if (lua_isnil(lua(),-1)) {
			lua_pop			(lua(),1);
			lua_newtable	(lua());
			lua_pushstring	(lua(),S);
			lua_pushvalue	(lua(),-2);
			lua_settable	(lua(),-4);
		}
		else
			if (!lua_istable(lua(),-1)) {
				xr_free			(S2);
				lua_pop			(lua(),2);
				script_log		(ScriptStorage::eLuaMessageTypeError,"the namespace name %s is already being used by the non-table object!",caNamespaceName);
				return			(false);
			}
			lua_remove		(lua(),-2);
			if (S1)
				S			= ++S1;
			else
				break;
	}
	xr_free			(S2);
	return			(true);
}

void CScriptStorage::copy_globals	()
{
	lua_newtable	(lua());
	lua_pushstring	(lua(),"_G");
	lua_gettable	(lua(),LUA_GLOBALSINDEX);
	lua_pushnil		(lua());
	while (lua_next(lua(), -2)) {
		lua_pushvalue	(lua(),-2);
		lua_pushvalue	(lua(),-2);
		lua_settable	(lua(),-6);
		lua_pop			(lua(), 1);
	}
}

bool CScriptStorage::load_buffer	(CLuaVirtualMachine *L, LPCSTR caBuffer, size_t tSize, LPCSTR caScriptName, LPCSTR caNameSpaceName)
{
	int				l_iErrorCode;
	if (caNameSpaceName) {
		string256		insert;
		sprintf			(insert,"local this = %s\n",caNameSpaceName);
		size_t			str_len = xr_strlen(insert);
		LPSTR			script = (LPSTR)xr_malloc((str_len + tSize)*sizeof(char));
		strcpy			(script,insert);
		Memory.mem_copy	(script + str_len,caBuffer,u32(tSize));
		l_iErrorCode	= luaL_loadbuffer(L,script,tSize + str_len,caScriptName);
	}
	else
		l_iErrorCode	= luaL_loadbuffer(L,caBuffer,tSize,caScriptName);

	if (l_iErrorCode) {
#ifdef DEBUG
		if (!print_output(L,caScriptName,l_iErrorCode))
			print_error	(L,l_iErrorCode);
#endif
		return			(false);
	}
	return			(true);
}

bool CScriptStorage::do_file	(LPCSTR caScriptName, LPCSTR caNameSpaceName, bool bCall)
{
	string256		l_caLuaFileName;
	IReader			*l_tpFileReader = FS.r_open(caScriptName);
	R_ASSERT		(l_tpFileReader);
	strconcat		(l_caLuaFileName,"@",caScriptName);
	
	if (!load_buffer(lua(),static_cast<LPCSTR>(l_tpFileReader->pointer()),(size_t)l_tpFileReader->length(),l_caLuaFileName,caNameSpaceName)) {
		lua_pop		(lua(),4);
		FS.r_close	(l_tpFileReader);
		return		(false);
	}
	FS.r_close		(l_tpFileReader);

	if (bCall) {
		lua_call	(lua(),0,0);
//		int			l_iErrorCode = lua_pcall(lua(),0,0,0);
//		if (l_iErrorCode) {
//#ifdef DEBUG
//			print_output	(lua(),caScriptName,l_iErrorCode);
//			print_error	(lua(),l_iErrorCode);
//#endif
//			return	(false);
//		}
	}
	else
		lua_insert	(lua(),-4);

	return			(true);
}

void CScriptStorage::set_namespace()
{
	lua_pushnil		(lua());
	while (lua_next(lua(), -2)) {
		lua_pushvalue	(lua(),-2);
		lua_gettable	(lua(),-5);
		if (lua_isnil(lua(),-1)) {
			lua_pop			(lua(),1);
			lua_pushvalue	(lua(),-2);
			lua_pushvalue	(lua(),-2);
			lua_pushvalue	(lua(),-2);
			lua_pushnil		(lua());
			lua_settable	(lua(),-7);
			lua_settable	(lua(),-7);
		}
		else {
			lua_pop			(lua(),1);
			lua_pushvalue	(lua(),-2);
			lua_gettable	(lua(),-4);
			if (!lua_equal(lua(),-1,-2)) {
				lua_pushvalue	(lua(),-3);
				lua_pushvalue	(lua(),-2);
				lua_pushvalue	(lua(),-2);
				lua_pushvalue	(lua(),-5);
				lua_settable	(lua(),-8);
				lua_settable	(lua(),-8);
			}
			lua_pop			(lua(),1);
		}
		lua_pushvalue	(lua(),-2);
		lua_pushnil		(lua());
		lua_settable	(lua(),-6);
		lua_pop			(lua(), 1);
	}
	lua_pop			(lua(),3);
}

bool CScriptStorage::load_file_into_namespace(LPCSTR caScriptName, LPCSTR caNamespaceName, bool bCall)
{
	if (!create_namespace(caNamespaceName))
		return		(false);
	copy_globals	();
	if (!do_file(caScriptName,caNamespaceName,bCall))
		return		(false);
	set_namespace	();
	return			(true);
}

bool CScriptStorage::namespace_loaded(LPCSTR N)
{
	lua_pushstring 			(lua(),"_G"); 
	lua_gettable 			(lua(),LUA_GLOBALSINDEX); 
	string256				S2;
	strcpy					(S2,N);
	LPCSTR					S = S2;
	for (;;) { 
		if (!xr_strlen(S))
			return			(false); 
		LPSTR				S1 = strchr(S,'.'); 
		if (S1)
			*S1				= 0; 
		lua_pushstring 		(lua(),S); 
		lua_gettable 		(lua(),-2); 
		if (lua_isnil(lua(),-1)) { 
			lua_pop			(lua(),2); 
			return			(false);	//	there is no namespace!
		}
		else 
			if (!lua_istable(lua(),-1)) { 
				lua_pop		(lua(),2); 
				Debug.fatal	(" Error : the namespace name is already being used by the non-table object!\n");
				return		(false); 
			} 
			lua_remove		(lua(),-2); 
			if (S1)
				S			= ++S1; 
			else
				break; 
	} 
	return					(true); 
}

bool CScriptStorage::object	(LPCSTR identifier, int type)
{
	lua_pushnil (lua()); 
	while (lua_next(lua(), -2)) { 
		if ((lua_type(lua(), -1) == type) && !xr_strcmp(identifier,lua_tostring(lua(), -2))) { 
			lua_pop (lua(), 3); 
			return	(true); 
		} 
		lua_pop (lua(), 1); 
	} 
	lua_pop (lua(), 1); 
	return	(false); 
}

bool CScriptStorage::object	(LPCSTR namespace_name, LPCSTR identifier, int type)
{
	if (xr_strlen(namespace_name) && !namespace_loaded(namespace_name))
		return				(false); 
	return					(object(identifier,type)); 
}

luabind::object CScriptStorage::name_space(LPCSTR namespace_name)
{
	string256			S1;
	strcpy				(S1,namespace_name);
	LPSTR				S = S1;
	luabind::object		lua_namespace = luabind::get_globals(lua());
	for (;;) {
		if (!xr_strlen(S))
			return		(lua_namespace);
		LPSTR			I = strchr(S,'.');
		if (!I)
			return		(lua_namespace[S]);
		*I				= 0;
		lua_namespace	= lua_namespace[S];
		S				= I + 1;
	}
}

bool CScriptStorage::print_output(CLuaVirtualMachine *L, LPCSTR caScriptFileName, int iErorCode)
{
	for (int i=-1; ; --i)
		if (lua_isstring(L,i)) {
			LPCSTR	S = lua_tostring(L,i);
			if (!xr_strcmp(S,"cannot resume dead coroutine")) {
				script_log	(ScriptStorage::eLuaMessageTypeInfo,"Script %s is finished",caScriptFileName);
				return	(true);
			}
			else {
				if (!i && !iErorCode)
					script_log	(ScriptStorage::eLuaMessageTypeInfo,"Output from %s",caScriptFileName);
				script_log	(iErorCode ? ScriptStorage::eLuaMessageTypeError : ScriptStorage::eLuaMessageTypeMessage,"%s",S);
			}
		}
		else {
			for ( i=0; ; ++i)
				if (lua_isstring(L,i)) {
					LPCSTR	S = lua_tostring(L,i);
					if (!xr_strcmp(S,"cannot resume dead coroutine")) {
						script_log	(ScriptStorage::eLuaMessageTypeInfo,"Script %s is finished",caScriptFileName);
						return	(true);
					}
					else {
						if (!i && !iErorCode)
							script_log	(ScriptStorage::eLuaMessageTypeInfo,"Output from %s",caScriptFileName);
						script_log	(iErorCode ? ScriptStorage::eLuaMessageTypeError : ScriptStorage::eLuaMessageTypeMessage,"%s",S);
					}
				}
				else
					return		(false);
		}
}

void CScriptStorage::print_error(CLuaVirtualMachine *L, int iErrorCode)
{
	switch (iErrorCode) {
		case LUA_ERRRUN : {
			Msg ("! SCRIPT RUNTIME ERROR");
			break;
		}
		case LUA_ERRMEM : {
			Msg ("! SCRIPT ERROR (memory allocation)");
			break;
		}
		case LUA_ERRERR : {
			Msg ("! SCRIPT ERROR (while running the error handler function)");
			break;
		}
		case LUA_ERRFILE : {
			Msg ("! SCRIPT ERROR (while running file)");
			break;
		}
		case LUA_ERRSYNTAX : {
			Msg ("! SCRIPT SYNTAX ERROR");
			break;
		}
		default : NODEFAULT;
	}
	
	for (int i=0; ; ++i)
		if (!print_stack_level(L,i))
			return;
}

IC	LPCSTR CScriptStorage::event2string(int iEventCode)
{
	switch (iEventCode) {
		case LUA_HOOKCALL		: return("hook call");
		case LUA_HOOKRET		: return("hook return");
		case LUA_HOOKLINE		: return("hook line");
		case LUA_HOOKCOUNT		: return("hook count");
		case LUA_HOOKTAILRET	: return("hook tail return");
		default					: NODEFAULT;
	}
#ifdef DEBUG
	return(0);
#endif
}

bool CScriptStorage::print_stack_level(CLuaVirtualMachine *L, int iStackLevel)
{
	lua_Debug	l_tDebugInfo;
	int			i;
	LPCSTR		name;

	if (lua_getstack(L, iStackLevel, &l_tDebugInfo) == 0)
		return	(false);

	Msg			("! Stack level %d",iStackLevel);

	lua_getinfo	(L,"nSluf",&l_tDebugInfo);

//	Msg			("  Event			: %d",event2string(l_tDebugInfo.event));
//	Msg			("  Name			: %s",l_tDebugInfo.name);
//	Msg			("  Name what		: %s",l_tDebugInfo.namewhat);
	Msg			("  What            : %s",l_tDebugInfo.what);
	Msg			("  Source          : %s",l_tDebugInfo.source);
//	Msg			("  Source (short)	: %s",l_tDebugInfo.short_src);
	Msg			("  Current line    : %d",l_tDebugInfo.currentline);
//	Msg			("  Nups			: %d",l_tDebugInfo.nups);
//	Msg			("  Line defined	: %d",l_tDebugInfo.linedefined);
	i			= 1;
	while (NULL != (name = lua_getlocal(L, &l_tDebugInfo, i++))) {
		Msg		("    local   %d %s", i-1, name);
		lua_pop	(L, 1);  /* remove variable value */
	}

	i = 1;
	while (NULL != (name = lua_getupvalue(L, -1, i++))) {
		Msg		("    upvalue %d %s", i-1, name);
		lua_pop	(L, 1);  /* remove upvalue value */
	}
	return		(true);
}

void CScriptStorage::flush_log()
{
	string256				log_file_name;
	strconcat               (log_file_name,Core.ApplicationName,"_",Core.UserName,"_lua.log");
	FS.update_path          (log_file_name,"$logs$",log_file_name);
	m_output.save_to		(log_file_name);
}

