#include "stdafx.h"

#include "script_space.h"

#include "script_lua_helper.h"
#include "script_debugger.h"
#include "script_file.h"

CDbgLuaHelper*	CDbgLuaHelper::m_pThis	= NULL;
lua_State*		CDbgLuaHelper::L		= NULL;

CDbgLuaHelper::CDbgLuaHelper()
{
	m_pThis = this;
}

CDbgLuaHelper::~CDbgLuaHelper()
{
}

extern "C" void PrintDebugString(const char* str)
{
	CScriptDebugger::GetDebugger()->Write(str);
}

#ifdef SCRIPT_FILE
bool CDbgLuaHelper::LoadDebugLines(CScriptFile* pPF)
{
	lua_State* L = lua_open();
//	BOOL bOk = luaL_loadfile(L , pPF->GetOutputPathNameExt() )==0;
	BOOL bOk = luaL_loadfile(L , pPF->GetName() )==0;

	if ( bOk )
	{
		pPF->RemoveAllDebugLines();

		int		size=lua_getlineinfo(L,NULL);
		int*	lines=new int[size];
		lua_getlineinfo(L,lines);

		for(int i=0;i<size;i++)
			pPF->AddDebugLine(lines[i]);
		delete lines;	
	}

	lua_close(L);

	return bOk;
}
#endif

void CDbgLuaHelper::UnPrepareLua(lua_State* l, int idx)
{
	lua_remove(l, idx);
}

int CDbgLuaHelper::PrepareLua(lua_State* l)
{
	// call this function immediatly before calling lua_pcall. 
	//returns index in stack for errorFunc
//	return 0;

	lua_register(l, "DEBUGGER_ERRORMESSAGE", errormessageLua );
	lua_sethook(l, hookLua, LUA_MASKLINE|LUA_MASKCALL|LUA_MASKRET, 0);

	int top = lua_gettop(l);
	lua_getglobal(l, "DEBUGGER_ERRORMESSAGE");
	lua_insert(l, top);
	return top;

}


void CDbgLuaHelper::PrepareLuaBind()
{
	luabind::set_pcall_callback	(hookLuaBind);
	luabind::set_error_callback (errormessageLuaBind);
}


int CDbgLuaHelper::OutputTop(lua_State* L)
{
	CScriptDebugger::GetDebugger()->Write(luaL_checkstring(L, -1));
	CScriptDebugger::GetDebugger()->Write("\n");
	return 0;
}

#define LEVELS1	12	/* size of the first part of the stack */
#define LEVELS2	10	/* size of the second part of the stack */

void CDbgLuaHelper::errormessageLuaBind(lua_State* l)
{
	L = l;
	errormessageLua(L);
}

int CDbgLuaHelper::errormessageLua(lua_State* l)
{
	L = l;
	int level = 1;  /* skip level 0 (it's this function) */

	int firstpart = 1;  /* still before eventual `...' */
	lua_Debug ar;
	if (!lua_isstring(L, 1))
		return lua_gettop(L);
	lua_settop(L, 1);
	lua_pushliteral(L, "\n");
	lua_pushliteral(L, "stack traceback:\n");
	while (lua_getstack(L, level++, &ar)) 
	{
		char buff[10];
		if (level > LEVELS1 && firstpart) 
		{
			/* no more than `LEVELS2' more levels? */
			if (!lua_getstack(L, level+LEVELS2, &ar))
				level--;  /* keep going */
			else 
			{
				lua_pushliteral(L, "       ...\n");  /* too many levels */
				while (lua_getstack(L, level+LEVELS2, &ar))  /* find last levels */
					level++;
			}
			firstpart = 0;
			continue;
		}

		sprintf(buff, "%4d-  ", level-1);
		lua_pushstring(L, buff);
		lua_getinfo(L, "Snl", &ar);
		lua_pushfstring(L, "%s:", ar.short_src);
		if (ar.currentline > 0)
			lua_pushfstring(L, "%d:", ar.currentline);
		switch (*ar.namewhat) 
		{
		case 'g':  /* global */ 
		case 'l':  /* local */
		case 'f':  /* field */
		case 'm':  /* method */
			lua_pushfstring(L, " in function `%s'", ar.name);
			break;
		default: 
			{
				if (*ar.what == 'm')  /* main? */
					lua_pushfstring(L, " in main chunk");
				else if (*ar.what == 'C')  /* C function? */
					lua_pushfstring(L, "%s", ar.short_src);
				else
					lua_pushfstring(L, " in function <%s:%d>", ar.short_src, ar.linedefined);
			}

		}

		lua_pushliteral(L, "\n");
		lua_concat(L, lua_gettop(L));
	}

	lua_concat(L, lua_gettop(L));

	OutputTop(L);
	const char* szSource=NULL;
	if ( ar.source[0] == '@' )
		szSource=ar.source+1;
	CScriptDebugger::GetDebugger()->ErrorBreak(szSource, ar.currentline);

	return 0;
}

void CDbgLuaHelper::set_lua(lua_State * l)
{
	m_pThis->L = l;
}

void CDbgLuaHelper::line_hook (lua_State *l, lua_Debug *ar)
{
	lua_getinfo(L, "lnuS", ar);
	m_pThis->m_pAr = ar;

	if ( ar->source[0] == '@' ){
		CScriptDebugger::GetDebugger()->LineHook(ar->source+1, ar->currentline);
	}
}

void CDbgLuaHelper::func_hook (lua_State *l, lua_Debug *ar)
{
	lua_getinfo(L, "lnuS", ar);
	m_pThis->m_pAr = ar;

	const char* szSource=NULL;
	if ( ar->source[0] == '@' )
	{
		szSource=ar->source+1;
	};
	CScriptDebugger::GetDebugger()->FunctionHook(szSource, ar->currentline, ar->event==LUA_HOOKCALL);
}

int CDbgLuaHelper::hookLuaBind (lua_State *l)
{
	L =l;
	lua_Debug ar;
	lua_getstack(L,0,&ar);
	lua_getinfo (L,"lnuS",&ar);
	hookLua(L,&ar);
	return 0;
}

void CDbgLuaHelper::hookLua (lua_State *l, lua_Debug *ar)
{
	L = l;
	switch(ar->event)
	{
	case LUA_HOOKTAILRET:
	case LUA_HOOKRET:
	case LUA_HOOKCALL:
		func_hook(L,ar);
		break;
	case LUA_HOOKLINE:
		line_hook(L,ar);
		break;
	}
}

const char* CDbgLuaHelper::GetSource() 
{ 
	return m_pAr->source+1; 
};


void CDbgLuaHelper::DrawStackTrace()
{
	CScriptDebugger::GetDebugger()->ClearStackTrace();

	int nLevel = 0;
	lua_Debug ar;
	char szDesc[256];
	while ( lua_getstack (L, nLevel, &ar) )
	{
		lua_getinfo(L, "lnuS", &ar);
		if ( ar.source[0]=='@' )
		{
			szDesc[0] = '\0';
			if ( ar.name )
				strcat(szDesc, ar.name);
			strcat(szDesc, ",");
			if ( ar.namewhat )
				strcat(szDesc, ar.namewhat);
			strcat(szDesc, ",");
			if ( ar.what )
				strcat(szDesc, ar.what);
			strcat(szDesc, ",");
			if ( ar.short_src )
				strcat(szDesc, ar.short_src);

			CScriptDebugger::GetDebugger()->AddStackTrace(szDesc, ar.source+1, ar.currentline);
		}

		++nLevel;
	};
}

void CDbgLuaHelper::DrawLocalVariables()
{
	CScriptDebugger::GetDebugger()->ClearLocalVariables();

	int nLevel = CScriptDebugger::GetDebugger()->GetStackTraceLevel();
	lua_Debug ar;
	if ( lua_getstack (L, nLevel, &ar) )
	{
		int i = 1;
		const char *name;
		while ((name = lua_getlocal(L, &ar, i++)) != NULL) {
				DrawVariable(L,name,true);

			lua_pop(L, 1);  /* remove variable value */
		}
	}
}

void CDbgLuaHelper::DrawGlobalVariables()
{
	CScriptDebugger::GetDebugger()->ClearGlobalVariables();

	lua_pushvalue(L, LUA_GLOBALSINDEX);

	lua_pushnil(L);  /* first key */
	string1024 var;var[0]=0;
	while (lua_next(L, -2))
	{
//!!!!	TRACE2("%s - %s\n",	lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)));
//		sprintf(var, "%s-%s",	lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)) );
//		CScriptDebugger::GetDebugger()->AddLocalVariable(var, "global", "_g_");
		lua_pop(L, 1); // pop value, keep key for next iteration;
	}
	lua_pop(L, 1); // pop table of globals;
};

bool CDbgLuaHelper::GetCalltip(const char *szWord, char *szCalltip)
{
	int nLevel = CScriptDebugger::GetDebugger()->GetStackTraceLevel();
	lua_Debug ar;
	if ( lua_getstack (L, nLevel, &ar) )
	{
		int i = 1;
		const char *name;
		while ((name = lua_getlocal(L, &ar, i++)) != NULL) {
			if ( xr_strcmp(name, szWord)==0 )
			{
				char szRet[64];
				Describe(szRet, -1);
				sprintf(szCalltip, "local %s : %s ", name, szRet);
				lua_pop(L, 1);  /* remove variable value */
				return true;
			}

			lua_pop(L, 1);  /* remove variable value */
		}
	}

	lua_pushvalue(L, LUA_GLOBALSINDEX);

	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2))
	{
		const char* name = lua_tostring(L, -2);
		if ( xr_strcmp(name, szWord)==0 )
		{
			char szRet[64];
			Describe(szRet, -1);
			sprintf(szCalltip, "global %s : %s ", name, szRet);

			lua_pop(L, 3);  /* remove table, key, value */

			return true;
		}

		lua_pop(L, 1); // pop value, keep key for next iteration;
	}
	lua_pop(L, 1); // pop table of globals;

	return false;
}


bool CDbgLuaHelper::Eval(const char *szCode, char* szRet)
{
	CoverGlobals();

	int top = lua_gettop(L);	
	int status = luaL_loadbuffer(L, szCode, xr_strlen(szCode), szCode);
	if ( status )
		sprintf(szRet, "%s", luaL_checkstring(L, -1));
	else
	{
		status = lua_pcall(L, 0, LUA_MULTRET, 0);  /* call main */
		if ( status )
		{
			const char* szErr = luaL_checkstring(L, -1);
			const char* szErr2 = strstr(szErr, ": ");
			sprintf(szRet, "%s", szErr2?(szErr2+2):szErr);
		}
		else
			Describe(szRet, -1);
	}

	lua_settop(L, top);

	RestoreGlobals();

	return !status;
}

void CDbgLuaHelper::Describe(char *szRet, int nIndex)
{
	int ntype = lua_type(L, nIndex);
	const char* type = lua_typename(L, ntype);
	char value[64];

	switch(ntype)
	{
	case LUA_TNUMBER:
		sprintf(value, "%f", lua_tonumber(L, nIndex));
		break;
	case LUA_TSTRING:
		sprintf(value, "%.63s", lua_tostring(L, nIndex));
		break;
	case LUA_TBOOLEAN:
		sprintf(value, "%s", lua_toboolean(L, nIndex) ? "true" : "false");
		break;
	default:
		value[0] = '\0';
		break;
	}
	sprintf(szRet, "%s : %.64s", type, value);
}

void CDbgLuaHelper::CoverGlobals()
{
	lua_newtable(L);  // save there globals covered by locals

	int nLevel = CScriptDebugger::GetDebugger()->GetStackTraceLevel();
	lua_Debug ar;
	if ( lua_getstack (L, nLevel, &ar) )
	{
		int i = 1;
		const char *name;
		while ((name = lua_getlocal(L, &ar, i++)) != NULL) { /* SAVE lvalue */
			lua_pushstring(L, name);	/* SAVE lvalue name */						
			lua_pushvalue(L, -1);	/* SAVE lvalue name name */
			lua_pushvalue(L, -1);	/* SAVE lvalue name name name */
			lua_insert(L, -4);		/* SAVE name lvalue name name */
			lua_rawget(L, LUA_GLOBALSINDEX);	/* SAVE name lvalue name gvalue */

			lua_rawset(L, -5);	// save global value in local table
			/* SAVE name lvalue */

			lua_rawset(L, LUA_GLOBALSINDEX); /* SAVE */
		}
	}
}

void CDbgLuaHelper::RestoreGlobals()
{
	// there is table of covered globals on top

	lua_pushnil(L);  /* first key */
	/* SAVE nil */
	while (lua_next(L, -2))		/* SAVE key value */
	{
		lua_pushvalue(L, -2); /* SAVE key value key */
		lua_insert(L, -2); /* SAVE key key value */

		lua_rawset(L, LUA_GLOBALSINDEX);	// restore global
		/* SAVE key */
	}

	lua_pop(L, 1); // pop table of covered globals;
}

void CDbgLuaHelper::DrawVariable(lua_State * l, const char* name, bool bOpenTable)
{
	Variable var;
	strcpy(var.szName, name );

	const char * type;
	int ntype = lua_type(l, -1);
	type = lua_typename(l, ntype);
	strcpy(var.szType, type);

	char value[64];

	switch(ntype)
	{
	case LUA_TNUMBER:
		sprintf(value, "%f", lua_tonumber(l, -1));
		strcpy(var.szValue, value );
		break;

	case LUA_TBOOLEAN:
		sprintf(value, "%s", lua_toboolean(L, -1) ? "true" : "false");
		strcpy(var.szValue, value );
		break;

	case LUA_TSTRING:
		sprintf(value, "%.63s", lua_tostring(l, -1));
		strcpy(var.szValue, value );
		break;


	case LUA_TTABLE:
			var.szValue[0]=0;
			CScriptDebugger::GetDebugger()->AddLocalVariable(var);
			if(bOpenTable)
				DrawTable(l, name);
			return;
		break;


/*	case LUA_TUSERDATA:{
			luabind::detail::object_rep* obj = static_cast<luabind::detail::object_rep*>(lua_touserdata(L, -1));
			luabind::detail::lua_reference& r = obj->get_lua_table();
			lua_State * ls = NULL;
			r.get(ls);
			DrawTable(ls, name);
			return;
		}break;*/

	default:
		value[0] = '\0';
		break;
	}

	CScriptDebugger::GetDebugger()->AddLocalVariable(var);
}

void CDbgLuaHelper::DrawTable(lua_State *l, LPCSTR S, bool bRecursive)
{
//	char		str[1024];

	if (!lua_istable(l,-1))
		return;

	lua_pushnil		(l);  /* first key */
	while (lua_next(l, -2) != 0) {


		char stype[256];
		char sname[256];
		char sFullName[256];
		sprintf(stype,"%s",lua_typename(l, lua_type(l, -1)));
		sprintf(sname,"%s",lua_tostring(l, -2));
		sprintf(sFullName,"%s.%s",S,sname);
		DrawVariable(l, sFullName, true);

		lua_pop		(l, 1);  /* removes `value'; keeps `key' for next iteration */
	}

}

void CDbgLuaHelper::DrawVariableInfo(char* varName)
{

}
