////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_storage.h"
#include "script_space.h"
#include <stdarg.h>

#ifndef ENGINE_BUILD
#	include "script_engine.h"
#	include "ai_space.h"
#else
#	define NO_XRGAME_SCRIPT_ENGINE
#endif

#ifndef XRGAME_EXPORTS
#	define NO_XRGAME_SCRIPT_ENGINE
#endif

#ifndef NO_XRGAME_SCRIPT_ENGINE
#	include "ai_debug.h"
#endif

#ifdef USE_DEBUGGER
#	include "script_debugger.h"
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
	luaopen_base			(lua()); 
	luaopen_table			(lua());
	luaopen_string			(lua());
	luaopen_math			(lua());
#ifdef DEBUG
	luaopen_debug			(lua());
#endif
}

CScriptStorage::~CScriptStorage		()
{
	if (m_virtual_machine)
		lua_close			(m_virtual_machine);
}

int __cdecl CScriptStorage::script_log	(ScriptStorage::ELuaMessageType tLuaMessageType, LPCSTR caFormat, ...)
{
#ifndef NO_XRGAME_SCRIPT_ENGINE
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

#ifndef ENGINE_BUILD
	ai().script_engine().m_output.w_stringZ(S2);
#endif

	va_end	(l_tMarker);

	return	(l_iResult);
}

bool CScriptStorage::parse_namespace(LPCSTR caNamespaceName, LPSTR b, LPSTR c)
{
	strcpy			(b,"");
	strcpy			(c,"");
	LPSTR			S2	= xr_strdup(caNamespaceName);
	LPSTR			S	= S2;
	for (int i=0;;++i) {
		if (!xr_strlen(S)) {
			script_log	(ScriptStorage::eLuaMessageTypeError,"the namespace name %s is incorrect!",caNamespaceName);
			xr_free		(S2);
			return		(false);
		}
		LPSTR			S1 = strchr(S,'.');
		if (S1)
			*S1				= 0;

		if (i)
			strcat		(b,"{");
		strcat			(b,S);
		strcat			(b,"=");
		if (i)
			strcat		(c,"}");
		if (S1)
			S			= ++S1;
		else
			break;
	}
	xr_free			(S2);
	return			(true);
}

bool CScriptStorage::load_buffer	(CLuaVirtualMachine *L, LPCSTR caBuffer, size_t tSize, LPCSTR caScriptName, LPCSTR caNameSpaceName)
{
	int					l_iErrorCode;
	if (caNameSpaceName && xr_strcmp("_G",caNameSpaceName)) {
		string256		insert, a, b;

		LPCSTR			header = "\
local function script_name() \
return \"%s\" \
end \
local this = {_G=_G} \
%s this %s \
setmetatable(this, {__index = _G}) \
setfenv(1, this) \
		";

		if (!parse_namespace(caNameSpaceName,a,b))
			return		(false);
		sprintf			(insert,header,caNameSpaceName,a,b);
		size_t			str_len = xr_strlen(insert);
		LPSTR			script = (LPSTR)xr_malloc((str_len + tSize)*sizeof(char));
		strcpy			(script,insert);
		Memory.mem_copy	(script + str_len,caBuffer,u32(tSize));
		l_iErrorCode	= luaL_loadbuffer(L,script,tSize + str_len,caScriptName);
		xr_free			(script);
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
	return				(true);
}

bool CScriptStorage::do_file	(LPCSTR caScriptName, LPCSTR caNameSpaceName, bool bCall)
{
	int				start = lua_gettop(lua());
	string256		l_caLuaFileName;
	IReader			*l_tpFileReader = FS.r_open(caScriptName);
	R_ASSERT3		(l_tpFileReader,"Cannot open script file ",caScriptName);
	strconcat		(l_caLuaFileName,"@",caScriptName);
	
	if (!load_buffer(lua(),static_cast<LPCSTR>(l_tpFileReader->pointer()),(size_t)l_tpFileReader->length(),l_caLuaFileName,caNameSpaceName)) {
		VERIFY		(lua_gettop(lua()) >= 4);
		lua_pop		(lua(),4);
		VERIFY		(lua_gettop(lua()) == start - 3);
		FS.r_close	(l_tpFileReader);
		return		(false);
	}
	FS.r_close		(l_tpFileReader);

	if (bCall) {
//		int			l_iErrorCode = lua_pcall(lua(),0,0,0); //backup___Dima
		int errFuncId = -1;
#ifdef USE_DEBUGGER
		errFuncId = CScriptDebugger::GetDebugger()->PrepareLua(lua());
#endif
		int	l_iErrorCode = lua_pcall(lua(),0,0,(-1==errFuncId)?0:errFuncId); //new_Andy
#ifdef USE_DEBUGGER
		CScriptDebugger::GetDebugger()->UnPrepareLua(lua(),errFuncId);
#endif
		if (l_iErrorCode) {

#ifdef DEBUG
			print_output(lua(),caScriptName,l_iErrorCode);
			print_error	(lua(),l_iErrorCode);
#endif
			lua_settop	(lua(),start);
			return	(false);
		}
	}
	else
		lua_insert	(lua(),-4);
	return			(true);
}

bool CScriptStorage::load_file_into_namespace(LPCSTR caScriptName, LPCSTR caNamespaceName, bool bCall)
{
	int				start = lua_gettop(lua());
	if (!do_file(caScriptName,caNamespaceName,bCall)) {
		lua_settop	(lua(),start);
		return		(false);
	}
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
	LPSTR					S = S2;
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
			VERIFY			(lua_gettop(lua()) >= 2);
			lua_pop			(lua(),2); 
			VERIFY			(start == lua_gettop(lua()));
			return			(false);	//	there is no namespace!
		}
		else 
			if (!lua_istable(lua(),-1)) { 
//				lua_settop	(lua(),0);
				VERIFY		(lua_gettop(lua()) >= 1);
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
	lua_pushnil				(lua()); 
	while (lua_next(lua(), -2)) { 
		if ((lua_type(lua(), -1) == type) && !xr_strcmp(identifier,lua_tostring(lua(), -2))) { 
			VERIFY			(lua_gettop(lua()) >= 3);
			lua_pop			(lua(), 3); 
			VERIFY			(lua_gettop(lua()) == start - 1);
			return			(true); 
		} 
		lua_pop				(lua(), 1); 
	} 
	VERIFY					(lua_gettop(lua()) >= 1);
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
#ifdef USE_DEBUGGER
				CScriptDebugger::GetDebugger()->Write(S);
				CScriptDebugger::GetDebugger()->ErrorBreak();
#endif
				return	(true);
			}
			else {
				if (!i && !iErorCode)
					script_log	(ScriptStorage::eLuaMessageTypeInfo,"Output from %s",caScriptFileName);
				script_log	(iErorCode ? ScriptStorage::eLuaMessageTypeError : ScriptStorage::eLuaMessageTypeMessage,"%s",S);
#ifdef USE_DEBUGGER
				CScriptDebugger::GetDebugger()->Write(S);
				CScriptDebugger::GetDebugger()->ErrorBreak();
#endif
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
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT RUNTIME ERROR");
			break;
		}
		case LUA_ERRMEM : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT ERROR (memory allocation)");
			break;
		}
		case LUA_ERRERR : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT ERROR (while running the error handler function)");
			break;
		}
		case LUA_ERRFILE : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT ERROR (while running file)");
			break;
		}
		case LUA_ERRSYNTAX : {
			script_log (ScriptStorage::eLuaMessageTypeError,"SCRIPT SYNTAX ERROR");
			break;
		}
		default : NODEFAULT;
	}

#ifndef ENGINE_BUILD
	ai().script_engine().script_stack_tracker().print_stack(L);
#endif
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
