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
//	lua_State*	luaVM	= lua_open();
//	if (NULL == luaVM)	{ printf("Error Initializing lua\n"); return -1; }
//
//	luaopen_base	(luaVM);
//	luaopen_table	(luaVM);
//	luaopen_io		(luaVM);
//	luaopen_string	(luaVM);
//	luaopen_math	(luaVM);
//	luaopen_debug	(luaVM);
//
//	open			(luaVM);
//	function		(luaVM,"log",(void (*)(LPCSTR))(Log));
//
//	//	lua_newtable	(luaVM);
//	//	lua_pushstring	(luaVM, "core");
//	//	lua_pushvalue	(luaVM, -2);
//	//	lua_settable	(luaVM, LUA_GLOBALSINDEX); /* register it with given name */
//
//	//	luaL_newmetatable(luaVM, "core");
//	//	lua_pushliteral	(luaVM, "__index");
//	//	lua_pushvalue	(luaVM, LUA_GLOBALSINDEX);
//	//	lua_rawset		(luaVM, -3);
//	//	lua_pop			(luaVM,1);
//	//	lua_setfenv		(luaVM, lua_setmetatable(luaVM,LUA_GLOBALSINDEX));
//
//
//
//
//
//	if (luaL_loadfile(luaVM, "x:\\extension.lua"))
//		lua_error	(luaVM);
//
//	//	if (lua_dofile(luaVM, "x:\\extension.lua"))
//	//		lua_error	(luaVM);
//
//	//	lua_newtable	(luaVM);
//	//	lua_pushstring	(luaVM, "core");
//	//	lua_pushvalue	(luaVM, -2);
//	//	lua_settable	(luaVM, LUA_GLOBALSINDEX); /* register it with given name */
//
//	//	lua_setfenv		(luaVM, -2);
//
//	//	lua_call		(luaVM, 0, 0);
//	/////////////////////////////////////////////////////////////////
//	lua_newtable	(luaVM);
//	int methods = lua_gettop(luaVM);
//
//	luaL_newmetatable(luaVM, "core");
//	int metatable = lua_gettop(luaVM);
//
//	// store method table in globals so that
//	// scripts can add functions written in Lua.
//	lua_pushvalue	(luaVM, methods);
//	set				(luaVM, LUA_GLOBALSINDEX, "core");
//
//	// hide metatable from Lua getmetatable()
//	lua_pushvalue	(luaVM, methods);
//	set				(luaVM, metatable, "__metatable");
//
//	lua_pushvalue	(luaVM, methods);
//	set				(luaVM, metatable, "__index");
//
//	lua_pushstring	(luaVM,"_G");
//	lua_gettable	(luaVM,LUA_GLOBALSINDEX);
//	//	lua_pushvalue	(luaVM, LUA_GLOBALSINDEX);
//	set				(luaVM, metatable, "__index");
//
//	lua_newtable	(luaVM);                // mt for method table
//
//	lua_setmetatable(luaVM, methods);
//
//	lua_pop			(luaVM, 1);  // drop metatable and method table
//
//	lua_setfenv		(luaVM, -2);
//
//	lua_call		(luaVM, 0, 0);
//
//	lua_setfenv		(luaVM, 0);
//
//	//	if (lua_dofile(luaVM, "x:\\extension.lua"))
//	//		lua_error	(luaVM);
//
//
//	//	lua_setfenv		(luaVM, -2);
//	//	lua_call		(luaVM, 0, 0);
//
//	//	lua_pushstring	(luaVM, "core");
//	//	lua_gettable	(luaVM, LUA_GLOBALSINDEX); /* register it with given name */
//	//	luaL_newmetatable(luaVM, "core");
//	//	lua_pushliteral	(luaVM, "__newindex");
//	//	lua_pushvalue	(luaVM, LUA_GLOBALSINDEX);
//	//	lua_rawset		(luaVM, -3);
//	//	lua_pop			(luaVM,1);
//
//	vfPrintTable	(luaVM,"_G");
//	vfPrintTable	(luaVM,"os");
//	vfPrintTable	(luaVM,"coroutine");
//	vfPrintTable	(luaVM,"_LOADED");
//	vfPrintTable	(luaVM,"table");
//	vfPrintTable	(luaVM,"io");
//	vfPrintTable	(luaVM,"string");
//	vfPrintTable	(luaVM,"math");
//	vfPrintTable	(luaVM,"debug");
//	vfPrintTable	(luaVM,"core");
//
//	lua_dofile		(luaVM, "x:\\test1.lua");
//
//	// close lua
//	lua_close		(luaVM);
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

void vfCreateTable(lua_State *luaVM, LPCSTR S1)
{
	lua_pushstring	(luaVM, S1);
	lua_gettable	(luaVM, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	if (lua_isnil(luaVM, -1)) {  /* no? */
		lua_pop		(luaVM, 1);
		lua_newtable(luaVM);  /* create it */
		lua_pushstring(luaVM, S1);
		lua_pushvalue(luaVM, -2);
		lua_settable(luaVM, LUA_GLOBALSINDEX);  /* register it with given name */
//		lua_pushstring	(luaVM, S1);
//		lua_gettable	(luaVM, LUA_GLOBALSINDEX);  /* check whether lib already exists */
//		lua_replace(luaVM, LUA_GLOBALSINDEX);
	}
	lua_insert		(luaVM, -1);  /* move library table to below upvalues */
	lua_pop			(luaVM, 0);  /* remove upvalues */
}

void vfTransferTable(lua_State *luaVM, LPCSTR S)
{
	lua_pushstring	(luaVM, "math");
	lua_gettable	(luaVM, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	if (lua_isnil(luaVM,-1))
		return;
	lua_pushstring	(luaVM, "format");
	lua_pushstring	(luaVM, "string");
	lua_gettable	(luaVM, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	if (lua_isnil(luaVM,-1))
		return;
	lua_settable	(luaVM,2);
//	lua_pushnil		(luaVM);  /* first key */
//	for (int i=0; ; i++) {
//		if (!lua_next(luaVM, -i-2))
//			break;
//		printf("%s - %s\n",lua_typename(luaVM, lua_type(luaVM, -2)), lua_typename(luaVM, lua_type(luaVM, -1)));
////		lua_pushvalue(luaVM,-1);
////		lua_pushvalue(luaVM,-3);
//		//lua_insert	();
////		lua_settable(luaVM,-6);
//	}
//	for (int j; j<i; j++)
//		lua_settable(luaVM,-6);
}

void vfPrintTable(lua_State *luaVM, LPCSTR S, bool bRecursive = false)
{
	int t			= -2;
	lua_pushstring	(luaVM, S);
	lua_gettable	(luaVM, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	
	if (!lua_istable(luaVM,-1))
		lua_error	(luaVM);
	
	printf			("\nContent of the table \"%s\" :\n",S);
	
	lua_pushnil		(luaVM);  /* first key */
	while (lua_next(luaVM, t) != 0) {
		printf		("%16s - %s\n", lua_tostring(luaVM, -2), lua_typename(luaVM, lua_type(luaVM, -1)));
		lua_pop		(luaVM, 1);  /* removes `value'; keeps `key' for next iteration */
	}
	
	if (!bRecursive)
		return;

	lua_pushnil		(luaVM);
	while (lua_next(luaVM, t) != 0) {
		if (lua_istable(luaVM, lua_type(luaVM, -1)))
			vfPrintTable(luaVM,lua_tostring(luaVM, -2),false);
		lua_pop		(luaVM, 1);
	}
}

static void set(lua_State *L, int table_index, const char *key) {
	lua_pushstring	(L, key);
	lua_insert		(L, -2);  // swap value and key
	lua_settable	(L, table_index);
}

void print_stack(lua_State *luaVM)
{
	printf("\n");
	for (int i=0; lua_type(luaVM, -i-1); i++)
		printf("%2d : %s\n",-i-1,lua_typename(luaVM, lua_type(luaVM, -i-1)));
//	for (int i=0; lua_type(luaVM, i); i++)
//		printf("%2d : %s\n",i,lua_typename(luaVM, lua_type(luaVM, i)));
}

// main
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	lua_State		*luaVM = lua_open();
	if (!luaVM)
		lua_error	(luaVM);

//	luaopen_base	(luaVM);
	print_stack		(luaVM);
	luaopen_string	(luaVM);	// S = String
	print_stack		(luaVM);
	luaopen_math	(luaVM);	// S = Math, String

//	print_stack		(luaVM);
//	lua_pop			(luaVM,2);
	print_stack		(luaVM);
	if (luaL_loadfile(luaVM, "x:\\extension.lua"))
		lua_error	(luaVM);	// S = Extension, Math, String
	print_stack		(luaVM);

//	luaopen_base	(luaVM);
//	luaopen_string	(luaVM);
//	luaopen_math	(luaVM);

	lua_newtable	(luaVM);	// S = NewTable, Extension, Math, String
	print_stack		(luaVM);
	lua_pushstring	(luaVM,"core");	// S = "core", NewTable, Extension, Math, String
	print_stack		(luaVM);
	lua_pushvalue	(luaVM,-2);	// S = NewTable, "core", NewTable, Extension, Math, String
	print_stack		(luaVM);
	lua_settable	(luaVM,LUA_GLOBALSINDEX);	// S = NewTable, Extension, Math, String
	print_stack		(luaVM);

	lua_setfenv		(luaVM,-2);	// S = NewTable, Extension, Math, String
	print_stack		(luaVM);

	lua_call		(luaVM,0,0);
	print_stack		(luaVM);

	if (luaL_loadfile(luaVM, "x:\\test1.lua"))
		lua_error	(luaVM);	// S = Extension, Math, String

	print_stack		(luaVM);
//	luaopen_base	(luaVM);
//	luaopen_string	(luaVM);
//	luaopen_math	(luaVM);

	lua_newtable	(luaVM);	// S = NewTable, Extension, Math, String
	print_stack		(luaVM);
	lua_pushstring	(luaVM,"test1");	// S = "core", NewTable, Extension, Math, String
	print_stack		(luaVM);
	lua_pushvalue	(luaVM,-2);	// S = NewTable, "core", NewTable, Extension, Math, String
	print_stack		(luaVM);
	lua_settable	(luaVM,LUA_GLOBALSINDEX);	// S = NewTable, Extension, Math, String
	print_stack		(luaVM);

	lua_setfenv		(luaVM,-2);	// S = NewTable, Extension, Math, String
	print_stack		(luaVM);

	lua_call		(luaVM,0,0);
	print_stack		(luaVM);

//	lua_dofile		(luaVM,"x:\\test1.lua");

	lua_close		(luaVM);
}
