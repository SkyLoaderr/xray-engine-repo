////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_lua_extension.h"

using namespace Script;

void Script::vfLoadStandardScripts(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName)
{
	string256		S,S1;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);
	LPCSTR			caScriptString = l_tpIniFile->r_string("common","script");

	u32				N = _GetItemCount(caScriptString);
	string16		I;
	for (u32 i=0; i<N; i++) {
		FS.update_path(S,"$game_scripts$",strconcat(S1,_GetItem(caScriptString,i,I),".script"));
		vfLoadFile	(tpLuaVirtualMachine,S,true);
	}
	xr_delete		(l_tpIniFile);

	sprintf			(S,"\nfunction script_name()\nreturn \"%s\"\nend\n",caScriptName);
	int				l_iErrorCode = lua_dobuffer(tpLuaVirtualMachine,S,strlen(S),"@preprocessor.script");
	Msg				("* Loading common script %s","preprocessor.script");
	vfPrintOutput	(tpLuaVirtualMachine,S);
	if (l_iErrorCode)
		vfPrintError(tpLuaVirtualMachine,l_iErrorCode);
}

void Script::vfExportToLua(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName)
{
	open			(tpLuaVirtualMachine);

	vfExportGlobals	(tpLuaVirtualMachine);
	vfExportFvector	(tpLuaVirtualMachine);
	vfExportFmatrix	(tpLuaVirtualMachine);
	vfExportGame	(tpLuaVirtualMachine);
	vfExportLevel	(tpLuaVirtualMachine);
	vfExportParticles(tpLuaVirtualMachine);
	vfExportSound	(tpLuaVirtualMachine);
	vfExportHit		(tpLuaVirtualMachine);
	vfExportActions	(tpLuaVirtualMachine);
	vfExportObject	(tpLuaVirtualMachine);

	vfLoadStandardScripts(tpLuaVirtualMachine,caScriptName);
}

void Script::vfLoadFile(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName, bool bCall)
{
	string256		l_caNamespaceName;
	_splitpath		(caScriptName,0,0,l_caNamespaceName,0);
	if (!strlen(l_caNamespaceName))
		vfLoadFileIntoNamespace(tpLuaVirtualMachine,caScriptName,"_G",bCall);
	else
		vfLoadFileIntoNamespace(tpLuaVirtualMachine,caScriptName,l_caNamespaceName,bCall);
}

void Script::vfLoadFileIntoNamespace(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName, LPCSTR caNamespaceName, bool bCall)
{
	lua_newtable	(tpLuaVirtualMachine);
	lua_pushstring	(tpLuaVirtualMachine,caNamespaceName);
	lua_pushvalue	(tpLuaVirtualMachine,-2);
	lua_settable	(tpLuaVirtualMachine,LUA_GLOBALSINDEX);
	lua_newtable	(tpLuaVirtualMachine);
	lua_pushstring	(tpLuaVirtualMachine,"_G");
	lua_gettable	(tpLuaVirtualMachine,LUA_GLOBALSINDEX);
	lua_pushnil		(tpLuaVirtualMachine);
	while (lua_next(tpLuaVirtualMachine,-2) != 0) {
		lua_pushvalue	(tpLuaVirtualMachine,-2);
		lua_pushvalue	(tpLuaVirtualMachine,-2);
		lua_settable	(tpLuaVirtualMachine,-6);
		lua_pop			(tpLuaVirtualMachine,1);
	}

	string256		l_caLuaFileName;
	IReader			*l_tpFileReader = FS.r_open(caScriptName);
	R_ASSERT		(l_tpFileReader);
	strconcat		(l_caLuaFileName,"@",caScriptName);
	int				l_iErrorCode = luaL_loadbuffer(tpLuaVirtualMachine,static_cast<LPCSTR>(l_tpFileReader->pointer()),(size_t)l_tpFileReader->length(),l_caLuaFileName);
	FS.r_close		(l_tpFileReader);

#ifdef DEBUG
	if (l_iErrorCode) {
		vfPrintOutput	(tpLuaVirtualMachine,caScriptName);
		vfPrintError	(tpLuaVirtualMachine,l_iErrorCode);
	}
#endif
	
	if (bCall)
		lua_call		(tpLuaVirtualMachine,0,0);
	else
		lua_insert		(tpLuaVirtualMachine,-4);

	lua_pushnil		(tpLuaVirtualMachine);
	while (lua_next(tpLuaVirtualMachine,-2) != 0) {
		lua_pushvalue	(tpLuaVirtualMachine,-2);
		lua_gettable	(tpLuaVirtualMachine,-5);
		if (lua_isnil(tpLuaVirtualMachine,-1)) {
			lua_pop			(tpLuaVirtualMachine,1);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_settable	(tpLuaVirtualMachine,-7);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_pushnil		(tpLuaVirtualMachine);
			lua_settable	(tpLuaVirtualMachine,-5);
		}
		else {
			lua_pop			(tpLuaVirtualMachine,1);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_gettable	(tpLuaVirtualMachine,-4);
			if (!lua_equal(tpLuaVirtualMachine,-1,-2)) {
				lua_pushvalue	(tpLuaVirtualMachine,-3);
				lua_pushvalue	(tpLuaVirtualMachine,-2);
				lua_settable	(tpLuaVirtualMachine,-7);
				lua_pop			(tpLuaVirtualMachine,1);
				lua_pushvalue	(tpLuaVirtualMachine,-2);
				lua_pushvalue	(tpLuaVirtualMachine,-2);
				lua_settable	(tpLuaVirtualMachine,-5);
			}
		}
		lua_pushvalue	(tpLuaVirtualMachine,-2);
		lua_pushnil		(tpLuaVirtualMachine);
		lua_settable	(tpLuaVirtualMachine,-6);
		lua_pop			(tpLuaVirtualMachine,1);
	}
	
	lua_pop			(tpLuaVirtualMachine,3);
}
