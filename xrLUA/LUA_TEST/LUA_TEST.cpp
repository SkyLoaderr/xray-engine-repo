// LUA_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "xrCore.h"
#pragma comment(lib,"x:\\xrCore.lib")

// Lua
extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}
#pragma comment(lib,"x:\\xrLUA.lib")

// Lua-bind
#pragma warning(disable:4244)
#include <luabind\\luabind.hpp>
#pragma warning(default:4244)

using namespace luabind;

void Log(LPCSTR S)
{
	printf("%s",S);
}


//function loadModuleIntoTable(module_name)
//	-- prevent reloading
//	assert(not _LOADED[module_name],
//		'failed: already loaded module' )
//	local g = {}
//	local ns = {}
//	-- save global table
//	for k, v in _G do
//		g[k] = v
//	end
//
//	require(module_name)
//	for k, v in _G do
//		if g[k] == nil then -- something new
//			ns[k] = v -- put it into the result table
//		elseif g[k] ~= _G[k] then -- something already exists
//			ns[k] = _G[k] -- put newer into the result table
//			_G[k] = g[k] -- restore original one
//		end
//	end
//	return ns
//end

//-- module.lua
//function func1()
//print('func1!')
//end
//
//var1 = 10
//--------------
//
//lua
//
//>mymodule = loadModuleToNamespace('module.lua')
//>mymodule.func1()
//func1!
//>print(mymodule.var1)
//10
//> func1()
//stdin:1: attempt to call global `func1' (a nil value)
//	  stack traceback:
//stdin:1: in main chunk
//		  [C]: ?
//>print(var1)
//nil


namespace Game {
	enum EWeatherType {
		eWeatherTypeSun = u32(0),
		eWeatherTypeClouds,
		eWeatherTypeRain,
		eWeatherTypeDummy = u32(-1),
	};

	u32 get_time()
	{
		static int	g_iCurrentTime = 0;
		return		(g_iCurrentTime++);
	}
	
	EWeatherType get_weather()
	{
		return(eWeatherTypeSun);
	}

	u32 get_psi_plant_deactivation_time()
	{
		static int	g_iCurrentTime = 0;
		return		(g_iCurrentTime++);
	}
};

//class _vector {
//public:
//
//};
//
//class CItemObject {
//public:
//	Active();
//	Visible(); // only for NPC
//	Condition();
//	get_parent();
//	get_mass();
//	get_cost();
//	position();
//};
//
//class CAliveObject {
//public:
//	rank();
//	health();
//	activeweapon();
//	equipment();
//	asleep();
//	zombied();
//	checkifobjectvisible();
//
//};
//
//void vfOld()
//{
//	lua_State*	L	= lua_open();
//	if (NULL == L)	{ printf("Error Initializing lua\n"); return -1; }
//
//	luaopen_base	(L);
//	luaopen_table	(L);
//	luaopen_io		(L);
//	luaopen_string	(L);
//	luaopen_math	(L);
//	luaopen_debug	(L);
//
//	open			(L);
//	function		(L,"log",(void (*)(LPCSTR))(Log));
//
//	//	lua_newtable	(L);
//	//	lua_pushstring	(L, "core");
//	//	lua_pushvalue	(L, -2);
//	//	lua_settable	(L, LUA_GLOBALSINDEX); /* register it with given name */
//
//	//	luaL_newmetatable(L, "core");
//	//	lua_pushliteral	(L, "__index");
//	//	lua_pushvalue	(L, LUA_GLOBALSINDEX);
//	//	lua_rawset		(L, -3);
//	//	lua_pop			(L,1);
//	//	lua_setfenv		(L, lua_setmetatable(L,LUA_GLOBALSINDEX));
//
//
//
//
//
//	if (luaL_loadfile(L, "x:\\extension.lua"))
//		lua_error	(L);
//
//	//	if (lua_dofile(L, "x:\\extension.lua"))
//	//		lua_error	(L);
//
//	//	lua_newtable	(L);
//	//	lua_pushstring	(L, "core");
//	//	lua_pushvalue	(L, -2);
//	//	lua_settable	(L, LUA_GLOBALSINDEX); /* register it with given name */
//
//	//	lua_setfenv		(L, -2);
//
//	//	lua_call		(L, 0, 0);
//	/////////////////////////////////////////////////////////////////
//	lua_newtable	(L);
//	int methods = lua_gettop(L);
//
//	luaL_newmetatable(L, "core");
//	int metatable = lua_gettop(L);
//
//	// store method table in globals so that
//	// scripts can add functions written in Lua.
//	lua_pushvalue	(L, methods);
//	set				(L, LUA_GLOBALSINDEX, "core");
//
//	// hide metatable from Lua getmetatable()
//	lua_pushvalue	(L, methods);
//	set				(L, metatable, "__metatable");
//
//	lua_pushvalue	(L, methods);
//	set				(L, metatable, "__index");
//
//	lua_pushstring	(L,"_G");
//	lua_gettable	(L,LUA_GLOBALSINDEX);
//	//	lua_pushvalue	(L, LUA_GLOBALSINDEX);
//	set				(L, metatable, "__index");
//
//	lua_newtable	(L);                // mt for method table
//
//	lua_setmetatable(L, methods);
//
//	lua_pop			(L, 1);  // drop metatable and method table
//
//	lua_setfenv		(L, -2);
//
//	lua_call		(L, 0, 0);
//
//	lua_setfenv		(L, 0);
//
//	//	if (lua_dofile(L, "x:\\extension.lua"))
//	//		lua_error	(L);
//
//
//	//	lua_setfenv		(L, -2);
//	//	lua_call		(L, 0, 0);
//
//	//	lua_pushstring	(L, "core");
//	//	lua_gettable	(L, LUA_GLOBALSINDEX); /* register it with given name */
//	//	luaL_newmetatable(L, "core");
//	//	lua_pushliteral	(L, "__newindex");
//	//	lua_pushvalue	(L, LUA_GLOBALSINDEX);
//	//	lua_rawset		(L, -3);
//	//	lua_pop			(L,1);
//
//	vfPrintTable	(L,"_G");
//	vfPrintTable	(L,"os");
//	vfPrintTable	(L,"coroutine");
//	vfPrintTable	(L,"_LOADED");
//	vfPrintTable	(L,"table");
//	vfPrintTable	(L,"io");
//	vfPrintTable	(L,"string");
//	vfPrintTable	(L,"math");
//	vfPrintTable	(L,"debug");
//	vfPrintTable	(L,"core");
//
//	lua_dofile		(L, "x:\\test1.lua");
//
//	// close lua
//	lua_close		(L);
//	return 0;
//}

void export2lua		(lua_State *tpLuaVirtualMachine)
{
	open			(tpLuaVirtualMachine);
	
	module(tpLuaVirtualMachine,"Game")
	[
		// declarations
		def("get_time",							Game::get_time)
//		def("get_surge_time",					Game::get_surge_time),
//		def("get_object_by_name",				Game::get_object_by_name),
//		
//		namespace_("Level")
//		[
//			// declarations
//			def("get_weather",					Level::get_weather)
//			def("get_object_by_name",			Level::get_object_by_name),
//		]
//
	];

//	module(tpLuaVirtualMachine)
//	[
//		class_<CScriptObject>("CGameObject")
//			.def(),
//	];
//	
//
	module(tpLuaVirtualMachine)
	[
		class_<Fvector>("Fvector")
			.def_readwrite("x", &Fvector::x)
			.def_readwrite("y", &Fvector::y)
			.def_readwrite("z", &Fvector::z)
			.def(constructor<>())
			.def("set",							(Fvector & (Fvector::*)(float,float,float))(Fvector::set))
			.def("set",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::set))
			.def("add",							(Fvector & (Fvector::*)(float))(Fvector::add))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::add))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::add))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::add))
			.def("sub",							(Fvector & (Fvector::*)(float))(Fvector::sub))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::sub))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::sub))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::sub))
			.def("mul",							(Fvector & (Fvector::*)(float))(Fvector::mul))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::mul))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mul))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mul))
			.def("div",							(Fvector & (Fvector::*)(float))(Fvector::div))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::div))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::div))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::div))
			.def("invert",						(Fvector & (Fvector::*)())(Fvector::invert))
			.def("invert",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::invert))
			.def("min",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::min))
			.def("min",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::min))
			.def("max",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::max))
			.def("max",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::max))
			.def("abs",							&Fvector::abs)
			.def("similar",						&Fvector::similar)
			.def("set_length",					&Fvector::set_length)
			.def("align",						&Fvector::align)
			.def("squeeze",						&Fvector::squeeze)
			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::clamp))
			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &, const Fvector))(Fvector::clamp))
			.def("inertion",					&Fvector::inertion)
			.def("average",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::average))
			.def("average",						(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::average))
			.def("lerp",						&Fvector::lerp)
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mad))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, float))(Fvector::mad))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mad))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &))(Fvector::mad))
			.def("square_magnitude",			&Fvector::square_magnitude)
			.def("magnitude",					&Fvector::magnitude)
			.def("magnitude",					&Fvector::normalize_magn)
			.def("normalize",					(Fvector & (Fvector::*)())(Fvector::normalize))
			.def("normalize",					(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize))
			.def("normalize_safe",				(Fvector & (Fvector::*)())(Fvector::normalize_safe))
			.def("normalize_safe",				(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize_safe))
			.def("random_dir",					(Fvector & (Fvector::*)(CRandom &))(Fvector::random_dir))
			.def("random_dir",					(Fvector & (Fvector::*)(const Fvector &, float, CRandom &))(Fvector::random_dir))
			.def("random_point",				(Fvector & (Fvector::*)(const Fvector &, CRandom &))(Fvector::random_point))
			.def("random_point",				(Fvector & (Fvector::*)(float, CRandom &))(Fvector::random_point))
			.def("dotproduct",					&Fvector::dotproduct)
			.def("crossproduct",				&Fvector::crossproduct)
			.def("distance_to_xz",				&Fvector::distance_to_xz)
			.def("distance_to_sqr",				&Fvector::distance_to_sqr)
			.def("distance_to",					&Fvector::distance_to)
//			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, float, float, float))(Fvector::from_bary))
			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fvector::from_bary))
//			.def("from_bary4",					&Fvector::from_bary4)
			.def("mknormal_non_normalized",		&Fvector::mknormal_non_normalized)
			.def("mknormal",					&Fvector::mknormal)
			.def("setHP",						&Fvector::setHP)
			.def("getHP",						&Fvector::getHP)
			.def("reflect",						&Fvector::reflect)
			.def("slide",						&Fvector::slide)
			.def("generate_orthonormal_basis",	&Fvector::generate_orthonormal_basis)
	];

	function		(tpLuaVirtualMachine,"log",(void (*)(LPCSTR))(Log));
}

void vfCreateTable(lua_State *L, LPCSTR S1)
{
	lua_pushstring	(L, S1);
	lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	if (lua_isnil(L, -1)) {  /* no? */
		lua_pop		(L, 1);
		lua_newtable(L);  /* create it */
		lua_pushstring(L, S1);
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_GLOBALSINDEX);  /* register it with given name */
//		lua_pushstring	(L, S1);
//		lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
//		lua_replace(L, LUA_GLOBALSINDEX);
	}
	lua_insert		(L, -1);  /* move library table to below upvalues */
	lua_pop			(L, 0);  /* remove upvalues */
}

void vfTransferTable(lua_State *L, LPCSTR S)
{
	lua_pushstring	(L, "math");
	lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	if (lua_isnil(L,-1))
		return;
	lua_pushstring	(L, "format");
	lua_pushstring	(L, "string");
	lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	if (lua_isnil(L,-1))
		return;
	lua_settable	(L,2);
//	lua_pushnil		(L);  /* first key */
//	for (int i=0; ; i++) {
//		if (!lua_next(L, -i-2))
//			break;
//		printf("%s - %s\n",lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)));
////		lua_pushvalue(L,-1);
////		lua_pushvalue(L,-3);
//		//lua_insert	();
////		lua_settable(L,-6);
//	}
//	for (int j; j<i; j++)
//		lua_settable(L,-6);
}

void vfPrintTable(lua_State *L, LPCSTR S, bool bRecursive = false)
{
	int t			= -2;
	lua_pushstring	(L, S);
	lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	
	if (!lua_istable(L,-1))
		lua_error	(L);
	
	printf			("\nContent of the table \"%s\" :\n",S);
	
	lua_pushnil		(L);  /* first key */
	while (lua_next(L, t) != 0) {
		printf		("%16s - %s\n", lua_tostring(L, -2), lua_typename(L, lua_type(L, -1)));
		lua_pop		(L, 1);  /* removes `value'; keeps `key' for next iteration */
	}
	
	if (!bRecursive)
		return;

	lua_pushnil		(L);
	while (lua_next(L, t) != 0) {
		if (lua_istable(L, lua_type(L, -1)))
			vfPrintTable(L,lua_tostring(L, -2),false);
		lua_pop		(L, 1);
	}
}

static void set(lua_State *L, int table_index, const char *key) {
	lua_pushstring	(L, key);
	lua_insert		(L, -2);  // swap value and key
	lua_settable	(L, table_index);
}

void print_stack(lua_State *L)
{
	printf("\n");
	for (int i=0; lua_type(L, -i-1); i++)
		printf("%2d : %s\n",-i-1,lua_typename(L, lua_type(L, -i-1)));
//	for (int i=0; lua_type(L, i); i++)
//		printf("%2d : %s\n",i,lua_typename(L, lua_type(L, i)));
}

bool load_file_into_namespace(lua_State *L, LPCSTR S, LPCSTR N, bool bCall = true)
{
	lua_pushstring	(L,N);
	lua_gettable	(L,LUA_GLOBALSINDEX);
	if (lua_isnil(L,-1)) {
		lua_pop			(L,1);
		lua_newtable	(L);
		lua_pushstring	(L,N);
		lua_pushvalue	(L,-2);
		lua_settable	(L,LUA_GLOBALSINDEX);
	}
	else
		if (!lua_istable(L,-1)) {
			lua_pop			(L,1);
			printf			(" Error : the namespace name is already being used by the non-table object!\n");
			return			(false);
		}
	
	lua_newtable	(L);
	lua_pushstring	(L,"_G");
	lua_gettable	(L,LUA_GLOBALSINDEX);
	lua_pushnil		(L);
	while (lua_next(L, -2) != 0) {
		lua_pushvalue	(L,-2);
		lua_pushvalue	(L,-2);
		lua_settable	(L,-6);
		lua_pop			(L, 1);
	}

	if (luaL_loadfile(L,S)) {
		printf			(" Error in file %s!\n",S);
		for (int i=0; ; i++)
			if (lua_isstring(L,i))
				printf	(" %s\n",lua_tostring(L,i));
			else
				break;
		lua_pop			(L,3);
		return			(false);
	}

	if (bCall)
		lua_call		(L,0,0);
	else
		lua_insert		(L,-4);

	lua_pushnil		(L);
	while (lua_next(L, -2) != 0) {
		lua_pushvalue	(L,-2);
		lua_gettable	(L,-5);
		if (lua_isnil(L,-1)) {
			lua_pop			(L,1);
			lua_pushvalue	(L,-2);
			lua_pushvalue	(L,-2);
			lua_pushvalue	(L,-2);
			lua_pushnil		(L);
			lua_settable	(L,-7);
			lua_settable	(L,-7);
		}
		else {
			lua_pop			(L,1);
			lua_pushvalue	(L,-2);
			lua_gettable	(L,-4);
			if (!lua_equal(L,-1,-2)) {
				lua_pushvalue	(L,-3);
				lua_pushvalue	(L,-2);
				lua_pushvalue	(L,-2);
				lua_pushvalue	(L,-5);
				lua_settable	(L,-8);
				lua_settable	(L,-8);
			}
			lua_pop			(L,1);
		}
		lua_pushvalue	(L,-2);
		lua_pushnil		(L);
		lua_settable	(L,-6);
		lua_pop			(L, 1);
	}
	
	lua_pop			(L,3);
	return			(true);
}

// main
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	lua_State		*L = lua_open();
	if (!L)
		lua_error	(L);

	luaopen_base	(L);
	luaopen_string	(L);
	luaopen_math	(L);
	luaopen_table	(L);

//	open			(L);

	lua_pop			(L,4);
	load_file_into_namespace(L,"x:\\extension.lua","core");
	load_file_into_namespace(L,"x:\\extension1.lua","core");
	lua_State		*T = lua_newthread(L);
	load_file_into_namespace(T,"x:\\test1.lua","test1",false);

	lua_resume		(T,0);

	lua_close		(L);
}
