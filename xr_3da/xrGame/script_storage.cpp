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
//	print_table				(lua(),"_G");
//	print_stack				(lua());
	luaopen_base			(lua()); 
	luaopen_table			(lua());
	luaopen_string			(lua());
	luaopen_math			(lua());
#ifdef DEBUG
	luaopen_debug			(lua());
#endif
//	print_table				(lua(),"_G");
//	print_stack				(lua());
//	lua_settop				(lua(),0);
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
	int				start = lua_gettop(lua());
	lua_pushstring	(lua(),"_G");
	lua_gettable	(lua(),LUA_GLOBALSINDEX);
	LPSTR			S2	= xr_strdup(caNamespaceName);
	LPSTR			S	= S2;
	for (;;) {
		if (!xr_strlen(S)) {
//			lua_settop	(lua(),0);
			VERIFY		(lua_gettop(ai().script_engine().lua()) >= 1);
			lua_pop		(lua(),1);
			VERIFY		(lua_gettop(lua()) == start);
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
				xr_free		(S2);

//				lua_settop	(lua(),0);
				VERIFY		(lua_gettop(ai().script_engine().lua()) >= 1);
				lua_pop		(lua(),1);
				VERIFY		(lua_gettop(lua()) == start);
				script_log	(ScriptStorage::eLuaMessageTypeError,"the namespace name %s is already being used by the non-table object!",caNamespaceName);
				return		(false);
			}
			lua_remove		(lua(),-2);
			if (S1)
				S			= ++S1;
			else
				break;
	}
	xr_free			(S2);
	VERIFY			(lua_gettop(lua()) == start + 1);
	return			(true);
}

void CScriptStorage::copy_globals	()
{
	int					start = lua_gettop(lua());
	lua_newtable		(lua());
	lua_pushstring		(lua(),"_G");
	lua_gettable		(lua(),LUA_GLOBALSINDEX);
	lua_pushnil			(lua());
	while (lua_next(lua(), -2)) {
		lua_pushvalue	(lua(),-2);
		lua_pushvalue	(lua(),-2);
		lua_settable	(lua(),-6);
		lua_pop			(lua(), 1);
	}
	VERIFY				(lua_gettop(lua()) == start + 2);
}

bool CScriptStorage::load_buffer	(CLuaVirtualMachine *L, LPCSTR caBuffer, size_t tSize, LPCSTR caScriptName, LPCSTR caNameSpaceName)
{
//	int				start = lua_gettop(L);
	int				l_iErrorCode;
	if (caNameSpaceName) {
		string256		insert;
		sprintf			(insert,"local this = %s\nfunction %s.script_name()\nreturn \"%s\"\nend\n",caNameSpaceName,caNameSpaceName,caNameSpaceName);
		size_t			str_len = xr_strlen(insert);
		LPSTR			script = (LPSTR)xr_malloc((str_len + tSize)*sizeof(char));
		strcpy			(script,insert);
		Memory.mem_copy	(script + str_len,caBuffer,u32(tSize));
		l_iErrorCode	= luaL_loadbuffer(L,script,tSize + str_len,caScriptName);
//		l_iErrorCode	= luaL_loadbuffer(L,insert,xr_strlen(insert),caScriptName);
//		VERIFY2			(!l_iErrorCode,"Unexpected error in the script file header");
//		lua_call		(L,0,0);
//		l_iErrorCode	= luaL_loadbuffer(L,caBuffer,tSize,caScriptName);
	}
	else
		l_iErrorCode	= luaL_loadbuffer(L,caBuffer,tSize,caScriptName);

	if (l_iErrorCode) {
#ifdef DEBUG
		if (!print_output(L,caScriptName,l_iErrorCode))
			print_error	(L,l_iErrorCode);
#endif
//		VERIFY			(lua_gettop(L) == start + 1);
		return			(false);
	}
//	VERIFY				(lua_gettop(L) == start + 1);
	return				(true);
}

bool CScriptStorage::do_file	(LPCSTR caScriptName, LPCSTR caNameSpaceName, bool bCall)
{
	int				start = lua_gettop(lua());
	string256		l_caLuaFileName;
	IReader			*l_tpFileReader = FS.r_open(caScriptName);
	R_ASSERT		(l_tpFileReader);
	strconcat		(l_caLuaFileName,"@",caScriptName);
	
	if (!load_buffer(lua(),static_cast<LPCSTR>(l_tpFileReader->pointer()),(size_t)l_tpFileReader->length(),l_caLuaFileName,caNameSpaceName)) {
//		lua_settop	(lua(),0);
		VERIFY		(lua_gettop(ai().script_engine().lua()) >= 4);
		lua_pop		(lua(),4);
		VERIFY		(lua_gettop(lua()) == start - 3);
		FS.r_close	(l_tpFileReader);
		return		(false);
	}
	FS.r_close		(l_tpFileReader);

	if (bCall) {
		lua_call	(lua(),0,0);
//		if (lua_gettop(lua()) != start)
//			lua_call(lua(),0,0);
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

	VERIFY			(lua_gettop(lua()) == start);
	return			(true);
}

void CScriptStorage::set_namespace()
{
	int				start = lua_gettop(lua());
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
//	lua_settop		(lua(),0);
	VERIFY			(lua_gettop(ai().script_engine().lua()) >= 3);
	lua_pop			(lua(),3);
	VERIFY			(lua_gettop(lua()) == start - 3);
}

bool CScriptStorage::load_file_into_namespace(LPCSTR caScriptName, LPCSTR caNamespaceName, bool bCall)
{
	int				start = lua_gettop(lua());
	if (!create_namespace(caNamespaceName)) {
		VERIFY		(lua_gettop(lua()) == start);
		return		(false);
	}
	copy_globals	();
	if (!do_file(caScriptName,caNamespaceName,bCall)) {
		VERIFY		(lua_gettop(lua()) == start);
		return		(false);
	}
	set_namespace	();
	VERIFY			(lua_gettop(lua()) == start);
	return			(true);
}

bool CScriptStorage::namespace_loaded(LPCSTR N, bool remove_from_stack)
{
	int						start = lua_gettop(lua());
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
//			lua_settop		(lua(),0);
			VERIFY			(lua_gettop(ai().script_engine().lua()) >= 2);
			lua_pop			(lua(),2); 
			VERIFY			(start == lua_gettop(lua()));
			return			(false);	//	there is no namespace!
		}
		else 
			if (!lua_istable(lua(),-1)) { 
//				lua_settop	(lua(),0);
				VERIFY		(lua_gettop(ai().script_engine().lua()) >= 1);
				lua_pop		(lua(),1); 
				VERIFY		(start == lua_gettop(lua()));
				Debug.fatal	(" Error : the namespace name is already being used by the non-table object!\n");
				return		(false); 
			} 
			lua_remove		(lua(),-2); 
			if (S1)
				S			= ++S1; 
			else
				break; 
	} 
	if (!remove_from_stack) {
		VERIFY				(lua_gettop(lua()) == start + 1);
	}
	else {
		VERIFY				(lua_gettop(lua()) >= 1);
		lua_pop				(lua(),1);
		VERIFY				(lua_gettop(lua()) == start);
	}
	return					(true); 
}

bool CScriptStorage::object	(LPCSTR identifier, int type)
{
	int						start = lua_gettop(lua());
	lua_pushnil (lua()); 
	while (lua_next(lua(), -2)) { 
		if ((lua_type(lua(), -1) == type) && !xr_strcmp(identifier,lua_tostring(lua(), -2))) { 
			VERIFY			(lua_gettop(ai().script_engine().lua()) >= 3);
			lua_pop			(lua(), 3); 
			VERIFY			(lua_gettop(lua()) == start - 1);
			return			(true); 
		} 
		lua_pop				(lua(), 1); 
	} 
//	lua_settop				(lua(),0);
	VERIFY					(lua_gettop(ai().script_engine().lua()) >= 1);
	lua_pop					(lua(), 1); 
	VERIFY					(lua_gettop(lua()) == start - 1);
	return					(false); 
}

bool CScriptStorage::object	(LPCSTR namespace_name, LPCSTR identifier, int type)
{
	int						start = lua_gettop(lua());
	if (xr_strlen(namespace_name) && !namespace_loaded(namespace_name,false)) {
		VERIFY				(lua_gettop(lua()) == start);
		return				(false); 
	}
	bool					result = object(identifier,type);
	VERIFY					(lua_gettop(lua()) == start);
	return					(result); 
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
	for (int i=0; ; --i)
		if (lua_isstring(L,i)) {
			LPCSTR	S = lua_tostring(L,i);
			if (!xr_strcmp(S,"cannot resume dead coroutine")) {
				VERIFY2	("Please do not return any values from main!!!",caScriptFileName);
				return	(true);
			}
			else {
				if (!i && !iErorCode)
					script_log	(ScriptStorage::eLuaMessageTypeInfo,"Output from %s",caScriptFileName);
				script_log	(iErorCode ? ScriptStorage::eLuaMessageTypeError : ScriptStorage::eLuaMessageTypeMessage,"%s",S);
			}
		}
		else
			if (i)
				return	(false);
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
	string256			log_file_name;
	strconcat           (log_file_name,Core.ApplicationName,"_",Core.UserName,"_lua.log");
	FS.update_path      (log_file_name,"$logs$",log_file_name);
	m_output.save_to	(log_file_name);
}

void CScriptStorage::print_stack	(CLuaVirtualMachine *L)
{
	int					start = lua_gettop(L);
	Msg					("Lua stack");
	for (int i=0, n=lua_gettop(L); i<n; ++i)
		Msg				("%2d : %s",-i-1,lua_typename(L, lua_type(L, -i-1)));
	VERIFY				(lua_gettop(L) == start);
}

void CScriptStorage::print_table	(lua_State *L, LPCSTR S, bool bRecursive)
{
	int					start = lua_gettop(L);
	int t				= -2;
	lua_pushstring		(L, S);
	lua_gettable		(L, LUA_GLOBALSINDEX);
	
	if (!lua_istable(L,-1))
		lua_error		(L);
	
	Msg					("\nContent of the table \"%s\" :",S);
	
	lua_pushnil			(L);
	while (lua_next(L, t) != 0) {
		Msg				("%16s - %s", lua_tostring(L, -2), lua_typename(L, lua_type(L, -1)));
		lua_pop			(L, 1);
	}

//	print_stack			(L);

	VERIFY				(lua_gettop(L) >= 1);
	lua_pop				(L,1); 
	VERIFY				(lua_gettop(L) == start);

	if (!bRecursive)
		return;

	lua_pushnil			(L);
	while (lua_next(L, t) != 0) {
		if (lua_istable(L, lua_type(L, -1)))
			print_table	(L,lua_tostring(L, -2),false);
		lua_pop			(L, 1);
	}

	VERIFY				(lua_gettop(L) >= 1);
	lua_pop				(L,1); 
	VERIFY				(lua_gettop(L) == start);
}
