// LUA_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#pragma comment(lib,"x:/xrCore.lib")
#pragma warning(pop)

// Lua
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#pragma comment(lib,"x:/xrLUA.lib")

// Lua-bind
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#pragma warning(disable:4244)
#include "luabind/luabind.hpp"
#include "luabind/adopt_policy.hpp"
#include "luabind/dependency_policy.hpp"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/discard_result_policy.hpp"
#include "luabind/iterator_policy.hpp"
#include <luabind/operator.hpp>
#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4530)

string4096 g_ca_stdout;

// I need this because we have to exclude option /EHsc (exception handling) from the project
namespace boost {
	void __stdcall throw_exception(const exception &A)
	{
		Debug.fatal("Boost exception raised %s",A.what());
	}
};

using namespace luabind;

void print_stack(lua_State *L);


////void Log(LPCSTR S)
////{
////	printf("%s",S);
////}
////
////
////function loadModuleIntoTable(module_name)
////	-- prevent reloading
////	assert(not _LOADED[module_name],
////		'failed: already loaded module' )
////	local g = {}
////	local ns = {}
////	-- save global table
////	for k, v in _G do
////		g[k] = v
////	end
////
////	require(module_name)
////	for k, v in _G do
////		if g[k] == nil then -- something new
////			ns[k] = v -- put it into the result table
////		elseif g[k] ~= _G[k] then -- something already exists
////			ns[k] = _G[k] -- put newer into the result table
////			_G[k] = g[k] -- restore original one
////		end
////	end
////	return ns
////end
//
////-- module.lua
////function func1()
////print('func1!')
////end
////
////var1 = 10
////--------------
////
////lua
////
////>mymodule = loadModuleToNamespace('module.lua')
////>mymodule.func1()
////func1!
////>print(mymodule.var1)
////10
////> func1()
////stdin:1: attempt to call global `func1' (a nil value)
////	  stack traceback:
////stdin:1: in main chunk
////		  [C]: ?
////>print(var1)
////nil
//
//
////namespace Game {
////	enum EWeatherType {
////		eWeatherTypeSun = u32(0),
////		eWeatherTypeClouds,
////		eWeatherTypeRain,
////		eWeatherTypeDummy = u32(-1),
////	};
////
////	u32 get_time()
////	{
////		static int	g_iCurrentTime = 0;
////		return		(g_iCurrentTime++);
////	}
////	
////	EWeatherType get_weather()
////	{
////		return(eWeatherTypeSun);
////	}
////
////	u32 get_psi_plant_deactivation_time()
////	{
////		static int	g_iCurrentTime = 0;
////		return		(g_iCurrentTime++);
////	}
////};
//
////class _vector {
////public:
////
////};
////
////class CItemObject {
////public:
////	Active();
////	Visible(); // only for NPC
////	Condition();
////	get_parent();
////	get_mass();
////	get_cost();
////	position();
////};
////
////class CAliveObject {
////public:
////	rank();
////	health();
////	activeweapon();
////	equipment();
////	asleep();
////	zombied();
////	checkifobjectvisible();
////
////};
////
////void vfOld()
////{
////	lua_State*	L	= lua_open();
////	if (NULL == L)	{ printf("Error Initializing lua\n"); return -1; }
////
////	luaopen_base	(L);
////	luaopen_table	(L);
////	luaopen_io		(L);
////	luaopen_string	(L);
////	luaopen_math	(L);
////	luaopen_debug	(L);
////
////	open			(L);
////	function		(L,"log",(void (*)(LPCSTR))(Log));
////
////	//	lua_newtable	(L);
////	//	lua_pushstring	(L, "core");
////	//	lua_pushvalue	(L, -2);
////	//	lua_settable	(L, LUA_GLOBALSINDEX); /* register it with given name */
////
////	//	luaL_newmetatable(L, "core");
////	//	lua_pushliteral	(L, "__index");
////	//	lua_pushvalue	(L, LUA_GLOBALSINDEX);
////	//	lua_rawset		(L, -3);
////	//	lua_pop			(L,1);
////	//	lua_setfenv		(L, lua_setmetatable(L,LUA_GLOBALSINDEX));
////
////
////
////
////
////	if (luaL_loadfile(L, "x:\\extension.lua"))
////		lua_error	(L);
////
////	//	if (lua_dofile(L, "x:\\extension.lua"))
////	//		lua_error	(L);
////
////	//	lua_newtable	(L);
////	//	lua_pushstring	(L, "core");
////	//	lua_pushvalue	(L, -2);
////	//	lua_settable	(L, LUA_GLOBALSINDEX); /* register it with given name */
////
////	//	lua_setfenv		(L, -2);
////
////	//	lua_call		(L, 0, 0);
////	/////////////////////////////////////////////////////////////////
////	lua_newtable	(L);
////	int methods = lua_gettop(L);
////
////	luaL_newmetatable(L, "core");
////	int metatable = lua_gettop(L);
////
////	// store method table in globals so that
////	// scripts can add functions written in Lua.
////	lua_pushvalue	(L, methods);
////	set				(L, LUA_GLOBALSINDEX, "core");
////
////	// hide metatable from Lua getmetatable()
////	lua_pushvalue	(L, methods);
////	set				(L, metatable, "__metatable");
////
////	lua_pushvalue	(L, methods);
////	set				(L, metatable, "__index");
////
////	lua_pushstring	(L,"_G");
////	lua_gettable	(L,LUA_GLOBALSINDEX);
////	//	lua_pushvalue	(L, LUA_GLOBALSINDEX);
////	set				(L, metatable, "__index");
////
////	lua_newtable	(L);                // mt for method table
////
////	lua_setmetatable(L, methods);
////
////	lua_pop			(L, 1);  // drop metatable and method table
////
////	lua_setfenv		(L, -2);
////
////	lua_call		(L, 0, 0);
////
////	lua_setfenv		(L, 0);
////
////	//	if (lua_dofile(L, "x:\\extension.lua"))
////	//		lua_error	(L);
////
////
////	//	lua_setfenv		(L, -2);
////	//	lua_call		(L, 0, 0);
////
////	//	lua_pushstring	(L, "core");
////	//	lua_gettable	(L, LUA_GLOBALSINDEX); /* register it with given name */
////	//	luaL_newmetatable(L, "core");
////	//	lua_pushliteral	(L, "__newindex");
////	//	lua_pushvalue	(L, LUA_GLOBALSINDEX);
////	//	lua_rawset		(L, -3);
////	//	lua_pop			(L,1);
////
////	vfPrintTable	(L,"_G");
////	vfPrintTable	(L,"os");
////	vfPrintTable	(L,"coroutine");
////	vfPrintTable	(L,"_LOADED");
////	vfPrintTable	(L,"table");
////	vfPrintTable	(L,"io");
////	vfPrintTable	(L,"string");
////	vfPrintTable	(L,"math");
////	vfPrintTable	(L,"debug");
////	vfPrintTable	(L,"core");
////
////	lua_dofile		(L, "x:\\test1.lua");
////
////	// close lua
////	lua_close		(L);
////	return 0;
////}
////
////void export2lua		(lua_State *tpLuaVirtualMachine)
////{
////	open			(tpLuaVirtualMachine);
////	
////	module(tpLuaVirtualMachine,"Game")
////	[
////		// declarations
////		def("get_time",							Game::get_time)
//////		def("get_surge_time",					Game::get_surge_time),
//////		def("get_object_by_name",				Game::get_object_by_name),
//////		
//////		namespace_("Level")
//////		[
//////			// declarations
//////			def("get_weather",					Level::get_weather)
//////			def("get_object_by_name",			Level::get_object_by_name),
//////		]
//////
////	];
////
//////	module(tpLuaVirtualMachine)
//////	[
//////		class_<CScriptObject>("CGameObject")
//////			.def(),
//////	];
//////	
//////
////	module(tpLuaVirtualMachine)
////	[
////		class_<Fvector>("Fvector")
////			.def_readwrite("x", &Fvector::x)
////			.def_readwrite("y", &Fvector::y)
////			.def_readwrite("z", &Fvector::z)
////			.def(constructor<>())
////			.def("set",							(Fvector & (Fvector::*)(float,float,float))(Fvector::set))
////			.def("set",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::set))
////			.def("add",							(Fvector & (Fvector::*)(float))(Fvector::add))
////			.def("add",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::add))
////			.def("add",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::add))
////			.def("add",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::add))
////			.def("sub",							(Fvector & (Fvector::*)(float))(Fvector::sub))
////			.def("sub",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::sub))
////			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::sub))
////			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::sub))
////			.def("mul",							(Fvector & (Fvector::*)(float))(Fvector::mul))
////			.def("mul",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::mul))
////			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mul))
////			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mul))
////			.def("div",							(Fvector & (Fvector::*)(float))(Fvector::div))
////			.def("div",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::div))
////			.def("div",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::div))
////			.def("div",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::div))
////			.def("invert",						(Fvector & (Fvector::*)())(Fvector::invert))
////			.def("invert",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::invert))
////			.def("min",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::min))
////			.def("min",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::min))
////			.def("max",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::max))
////			.def("max",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::max))
////			.def("abs",							&Fvector::abs)
////			.def("similar",						&Fvector::similar)
////			.def("set_length",					&Fvector::set_length)
////			.def("align",						&Fvector::align)
////			.def("squeeze",						&Fvector::squeeze)
////			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::clamp))
////			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &, const Fvector))(Fvector::clamp))
////			.def("inertion",					&Fvector::inertion)
////			.def("average",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::average))
////			.def("average",						(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::average))
////			.def("lerp",						&Fvector::lerp)
////			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mad))
////			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, float))(Fvector::mad))
////			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mad))
////			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &))(Fvector::mad))
////			.def("square_magnitude",			&Fvector::square_magnitude)
////			.def("magnitude",					&Fvector::magnitude)
////			.def("magnitude",					&Fvector::normalize_magn)
////			.def("normalize",					(Fvector & (Fvector::*)())(Fvector::normalize))
////			.def("normalize",					(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize))
////			.def("normalize_safe",				(Fvector & (Fvector::*)())(Fvector::normalize_safe))
////			.def("normalize_safe",				(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize_safe))
////			.def("random_dir",					(Fvector & (Fvector::*)(CRandom &))(Fvector::random_dir))
////			.def("random_dir",					(Fvector & (Fvector::*)(const Fvector &, float, CRandom &))(Fvector::random_dir))
////			.def("random_point",				(Fvector & (Fvector::*)(const Fvector &, CRandom &))(Fvector::random_point))
////			.def("random_point",				(Fvector & (Fvector::*)(float, CRandom &))(Fvector::random_point))
////			.def("dotproduct",					&Fvector::dotproduct)
////			.def("crossproduct",				&Fvector::crossproduct)
////			.def("distance_to_xz",				&Fvector::distance_to_xz)
////			.def("distance_to_sqr",				&Fvector::distance_to_sqr)
////			.def("distance_to",					&Fvector::distance_to)
//////			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, float, float, float))(Fvector::from_bary))
////			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fvector::from_bary))
//////			.def("from_bary4",					&Fvector::from_bary4)
////			.def("mknormal_non_normalized",		&Fvector::mknormal_non_normalized)
////			.def("mknormal",					&Fvector::mknormal)
////			.def("setHP",						&Fvector::setHP)
////			.def("getHP",						&Fvector::getHP)
////			.def("reflect",						&Fvector::reflect)
////			.def("slide",						&Fvector::slide)
////			.def("generate_orthonormal_basis",	&Fvector::generate_orthonormal_basis)
////	];
////
////	function		(tpLuaVirtualMachine,"log",(void (*)(LPCSTR))(Log));
////}
////
////void vfCreateTable(lua_State *L, LPCSTR S1)
////{
////	lua_pushstring	(L, S1);
////	lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
////	if (lua_isnil(L, -1)) {  /* no? */
////		lua_pop		(L, 1);
////		lua_newtable(L);  /* create it */
////		lua_pushstring(L, S1);
////		lua_pushvalue(L, -2);
////		lua_settable(L, LUA_GLOBALSINDEX);  /* register it with given name */
//////		lua_pushstring	(L, S1);
//////		lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
//////		lua_replace(L, LUA_GLOBALSINDEX);
////	}
////	lua_insert		(L, -1);  /* move library table to below upvalues */
////	lua_pop			(L, 0);  /* remove upvalues */
////}
////
////void vfTransferTable(lua_State *L, LPCSTR S)
////{
////	lua_pushstring	(L, "math");
////	lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
////	if (lua_isnil(L,-1))
////		return;
////	lua_pushstring	(L, "format");
////	lua_pushstring	(L, "string");
////	lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
////	if (lua_isnil(L,-1))
////		return;
////	lua_settable	(L,2);
//////	lua_pushnil		(L);  /* first key */
//////	for (int i=0; ; i++) {
//////		if (!lua_next(L, -i-2))
//////			break;
//////		printf("%s - %s\n",lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)));
////////		lua_pushvalue(L,-1);
////////		lua_pushvalue(L,-3);
//////		//lua_insert	();
////////		lua_settable(L,-6);
//////	}
//////	for (int j; j<i; j++)
//////		lua_settable(L,-6);
////}
////
void vfPrintTable(lua_State *L, LPCSTR S, bool bRecursive = false)
{
	int t			= -2;
//	print_stack		(L);
		
	lua_pushstring	(L, S);
	lua_gettable	(L, LUA_GLOBALSINDEX);  /* check whether lib already exists */
	
//	print_stack		(L);

	if (!lua_istable(L,-1))
		lua_error	(L);
	
//	print_stack		(L);
	
	printf			("\nContent of the table \"%s\" :\n",S);
	
	lua_pushnil		(L);  /* first key */
	while (lua_next(L, t) != 0) {
		printf		("%16s - %s\n", lua_tostring(L, -2), lua_typename(L, lua_type(L, -1)));
		
//		print_stack		(L);
		
		lua_pop		(L, 1);  /* removes `value'; keeps `key' for next iteration */
	}
	
//	print_stack		(L);
	
	if (!bRecursive) {
		lua_pop		(L,1);
		return;
	}

	lua_pushnil		(L);
	while (lua_next(L, t) != 0) {
//		print_stack	(L);
		if (lua_istable(L, -1) && xr_strcmp("_G",lua_tostring(L, -2))) {
			vfPrintTable(L,lua_tostring(L, -2),true);
		}
		lua_pop		(L, 1);
	}
	
	lua_pop			(L,1);
}

void print_local_table(lua_State *L, LPCSTR S, bool bRecursive = false)
{
	print_stack		(L);

	if (!lua_istable(L,-1))
		lua_error	(L);

	print_stack		(L);

	printf			("\nContent of the table \"%s\" :\n",S);

	lua_pushnil		(L);  /* first key */
	while (lua_next(L, -2) != 0) {
		printf		("%16s - %s\n", lua_tostring(L, -2), lua_typename(L, lua_type(L, -1)));

		print_stack		(L);

		lua_pop		(L, 1);  /* removes `value'; keeps `key' for next iteration */
	}

	print_stack		(L);

	if (!bRecursive) {
		lua_pop		(L,1);
		return;
	}

	lua_pushnil		(L);
	while (lua_next(L, -2) != 0) {
		print_stack	(L);
		if (lua_istable(L, -1) && xr_strcmp("_G",lua_tostring(L, -2))) {
			vfPrintTable(L,lua_tostring(L, -2),true);
		}
		lua_pop		(L, 1);
	}

	lua_pop			(L,1);
}

//
////static void set(lua_State *L, int table_index, const char *key) {
////	lua_pushstring	(L, key);
////	lua_insert		(L, -2);  // swap value and key
////	lua_settable	(L, table_index);
////}
////
void print_stack(lua_State *L)
{
	printf("\n");
	for (int i=0; lua_type(L, -i-1); i++)
		printf("%2d : %s\n",-i-1,lua_typename(L, lua_type(L, -i-1)));
//	for (int i=0; lua_type(L, i); i++)
//		printf("%2d : %s\n",i,lua_typename(L, lua_type(L, i)));
}
////
////bool create_namespace_table(lua_State *L, LPCSTR N)
////{
////	if (!xr_strlen(N))
////		return			(false);
////	LPSTR			S = (char*)xr_malloc((xr_strlen(N) + 1)*sizeof(char));
////	strcpy			(S,N);
////	LPSTR			S1 = strchr(S,'.');
////	if (S1)
////		*S1				= 0;
////	lua_pushstring	(L,S);
////	lua_gettable	(L,-2);
////	if (lua_isnil(L,-1)) {
////		lua_pop			(L,1);
////		lua_newtable	(L);
////		lua_pushstring	(L,S);
////		lua_pushvalue	(L,-2);
////		lua_settable	(L,-4);
////	}
////	else
////		if (!lua_istable(L,-1)) {
////			xr_free			(S);
////			lua_pop			(L,2);
////			printf			(" Error : the namespace name is already being used by the non-table object!\n");
////			return			(false);
////		}
////	lua_remove		(L,-2);
////	bool			l_bResult = true;
////	if (S1)
////		l_bResult	= create_namespace_table(L,++S1);
////	xr_free			(S);
////	return			(l_bResult);
////}
//
//
////typedef BOOL test_type;
////
////class CInterfaceClass {
////public:
////	virtual test_type vf(LPCSTR S) = 0;
////};
////
////class CManager {
////public:
////	xr_vector<CInterfaceClass*>	m_tpInterfaces;
////
////	CManager(){}
////
////	void Update()
////	{
////		string256 S;
////		for (int i=0; i<10; ++i) {
////			sprintf(S,"%d",i);
////			xr_vector<CInterfaceClass*>::iterator I = m_tpInterfaces.begin();
////			xr_vector<CInterfaceClass*>::iterator E = m_tpInterfaces.end();
////			for ( ; I != E; ++I)
////				(*I)->vf(S);
////		}
////	}
////
////	void Add(CInterfaceClass *tpInterfaceClass)
////	{
////		m_tpInterfaces.push_back(tpInterfaceClass);
////	}
////};
////
////CManager	tManager;
////
////CManager&	get_manager()
////{
////	return (tManager);
////}
////
////
////class CPrintClass : public CInterfaceClass {
////public:
////	CPrintClass(){}
////	virtual test_type vf(LPCSTR S) {/**printf("Base class function call : %s\n",S); /**/return 1;}
////	void add() {get_manager().Add(this);}
////	CPrintClass &test(double &a)
////	{
////		a = a + 1;
////		return(*this);
////	}
////	
////	void test1(double &a)
////	{
////		a += 1;
////	}
////};
////
////class CPrintClassWrapper : public CPrintClass {
////public:
////	luabind::object		m_tLuaBindObject;
////	CPrintClassWrapper(luabind::object tLuaBindObject) : CPrintClass(), m_tLuaBindObject(tLuaBindObject){}
////	virtual test_type vf(LPCSTR S) {return call_member<bool>(m_tLuaBindObject,"vf",S);}
////	static bool vf_static(CPrintClass *ptr, LPCSTR S) {return ptr->CPrintClass::vf(S);}
////};
////
////void test(double &a)
////{
////	a += 1.0;
////}
////
////typedef lua_State CLuaVirtualMachine;
////
////int LuaPanic(CLuaVirtualMachine *tpLuaVirtualMachine)
////{
////	printf("PANIC!\n");
////	return(0);
////}
////
////void LuaHookCall(CLuaVirtualMachine *tpLuaVirtualMachine, lua_Debug *tpLuaDebug)
////{
////	lua_getinfo(tpLuaVirtualMachine,"nSlu",tpLuaDebug);
////	LPCSTR S = "";
////	switch (tpLuaDebug->event) {
////		case LUA_HOOKCALL		: {
////			S	= "[CALL]        ";
////			break;
////		}
////		case LUA_HOOKRET		: {
////			S	= "[RETURN]      ";
////			break;
////		}
////		case LUA_HOOKLINE		: {
////			S	= "[LINE]        ";
////			break;
////		}
////		case LUA_HOOKCOUNT		: {
////			S	= "[COUNT]       ";
////			break;
////		}
////		case LUA_HOOKTAILRET	: {
////			S	= "[TAIL_RETURN] ";
////			break;
////		}
////		default					: NODEFAULT;
////	}
////	printf		(tpLuaDebug->event == LUA_HOOKLINE ? "%s%s : %s %s %s (current line %d)\n" : "%s%s : %s %s %s\n",S,tpLuaDebug->short_src,tpLuaDebug->what,tpLuaDebug->namewhat,tpLuaDebug->name ? tpLuaDebug->name : "\b",tpLuaDebug->currentline);
////}
////
////void TestLua0()
////{
////	lua_State		*L = lua_open();
////	if (!L)
////		lua_error	(L);
////
////	luaopen_base	(L);
////	luaopen_string	(L);
////	luaopen_math	(L);
////	luaopen_table	(L);
////
////	lua_pop			(L,4);
////
////	open			(L);
////
////	lua_atpanic		(L,LuaPanic);
////	lua_sethook		(L, LuaHookCall,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	0);
////
////	module(L)
////	[
////		class_<CPrintClass,CPrintClassWrapper>("pp_class")
////			.def(constructor<>())
////			.def("vf",		&CPrintClassWrapper::vf_static)
////			.def("add",		&CPrintClass::add)
////			.def("test",	&CPrintClass::test,	pure_out_value(_1, adopt(_2)))
////			.def("test1",	&CPrintClass::test1,pure_out_value(_1))
////	];
////	
//////	load_file_into_namespace(L,"x:\\extension.lua","core");
//////	load_file_into_namespace(L,"x:\\extension1.lua","core");
//////	lua_State		*T = lua_newthread(L);
//////	load_file_into_namespace(T,"x:\\test1.lua","test1",false);
//////	lua_resume		(T,0);
////	
////	lua_dofile(L,"x:\\extension.lua");
////	lua_dofile(L,"x:\\extension1.lua");
////	lua_State	*T = lua_newthread(L);
////	
////	lua_atpanic		(T,	LuaPanic);
////	lua_sethook		(T, LuaHookCall,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	10);
////
////	luaL_loadfile(T,"x:\\test1.lua");
////	lua_resume(T,0);
////	tManager.Update();
////	lua_resume(T,0);
////}
////
////void TestLua1()
////{
////	string4096		g_ca_stdout;
////	strcpy			(g_ca_stdout,"");
////	g_ca_stdout		= g_ca_stdout;
////	lua_State		*L = lua_open();
////	if (!L)
////		lua_error	(L);
////
////	luaopen_base	(L);
////	luaopen_string	(L);
////	luaopen_math	(L);
////	luaopen_table	(L);
////
////	lua_pop			(L,4);
////	
////	open			(L);
////
////	luaL_loadfile	(L,"s:\\gamedata\\scripts\\.script");
////
////	if (xr_strlen(g_ca_stdout))
////		printf		("%s\n",g_ca_stdout);
////
////	lua_close		(L);
////}
////
//// main
//bool get_namespace_table(lua_State *L, LPCSTR N)
//{
//	lua_pushstring	(L,"_G");
//	lua_gettable	(L,LUA_GLOBALSINDEX);
//	LPSTR			S2 = (char*)xr_malloc((xr_strlen(N) + 1)*sizeof(char)), S = S2;
//	strcpy			(S,N);
//	for (;;) {
//		if (!xr_strlen(S)) {
//			xr_free		(S2);
//			return		(false);
//		}
//		LPSTR			S1 = strchr(S,'.');
//		if (S1)
//			*S1				= 0;
//		lua_pushstring	(L,S);
//		lua_gettable	(L,-2);
//		if (lua_isnil(L,-1)) {
//			xr_free			(S2);
//			lua_pop			(L,2);
//			printf			(" Error : the namespace name is already being used by the non-table object!\n");
//			return			(false);
//		}
//		else
//			if (!lua_istable(L,-1)) {
//				xr_free			(S2);
//				lua_pop			(L,2);
//				printf			(" Error : the namespace name is already being used by the non-table object!\n");
//				return			(false);
//			}
//		lua_remove		(L,-2);
//		if (S1)
//			S			= ++S1;
//		else
//			break;
//	}
//	xr_free			(S2);
//	return			(true);
//}
//
//bool is_object_presented(lua_State *L, LPCSTR identifier, int type)
//{
//	lua_pushnil		(L);
//	while (lua_next(L, -2) != 0) {
//		if ((lua_type(L, -1) == type) && !xr_strcmp(identifier,lua_tostring(L, -2))) {
//			lua_pop	(L, 3);
//			return(true);
//		}
//		lua_pop		(L, 1);
//	}
//	lua_pop			(L, 1);
//	return			(false);
//}
//
//bool object_presented(lua_State *L, LPCSTR namespace_name, LPCSTR identifier, int type)
//{
//	if (!get_namespace_table(L,namespace_name)) {
//		printf		("There is no such namespace!");
//		return		(false);
//	}
//	return			(is_object_presented(L,identifier,type));
//}
//

template <typename T>
class CEmptyClassTemplate {
};

//template <typename DSE>
//struct CEDLSL {
//
//	template <template <typename _T> class T1>
//	struct DLSL {
//		template<typename T2>
//		struct GV : public T1<T2> {
//			T2	*_prev;
//			T2	*_next;
//
//			IC	T2	*&prev()
//			{
//				return	(_prev);
//			}
//
//			IC	T2	*&next()
//			{
//				return	(_next);
//			}
//		};
//	};
//
//	template <template <typename _T> class PGV = CEmptyClassTemplate> 
//	struct DS : public DSE::DS<DLSL<PGV>::GV> {
//		typedef typename DSE::DS<DLSL<PGV>::GV>::CGraphNode CGraphNode;
//	};
//};
//
//template <typename _index_type, typename DSE = CEDSBA>
//struct CEDSIBA {
//
//	template <template <typename _T> class T1>
//	struct DSIBA {
//		template<typename T2>
//		struct GV : public T1<T2> {
//			_index_type	_index;
//
//			IC	_index_type &index()
//			{
//				return	(_index);
//			}
//		};
//	};
//
//	template <template <typename _T> class PGV = CEmptyClassTemplate> 
//	struct DS : public DSE::DS<DSIBA<PGV>::GV> {
//		typedef typename DSE::DS<DSIBA<PGV>::GV>::CGraphNode CGraphNode;
//	};
//};
//
//template <typename DSBE, template <typename T> class DSA>
//struct CEDSBA {
//	
//	template <template <typename _T> class T1>
//	struct DSBA {
//		template<typename T2>
//		struct GV : public T1<T2> {
//		};
//	};
//
//	template <template <typename _T> class PGV> 
//	struct DS : 
//		public DSBE::DS<DSBA<PGV>::GV>,
//		public DSA<typename DSBE::DS<DSBA<PGV>::GV>::CGraphNode>
//	{
//		typedef typename DSBE::DS<DSBA<PGV>::GV>::CGraphNode CGraphNode;
//	};
//};
//
//template <typename _dist_type>
//struct CEDSB {
//
//	template <template <typename _T> class T1>
//	struct EDSB {
//		struct GV : public T1<GV> {
//			_dist_type	_f;
//			_dist_type	_g;
//			_dist_type	_h;
//			GV			*_back;
//
//			IC	_dist_type &f()
//			{
//				return	(_f);
//			}
//
//			IC	_dist_type &g()
//			{
//				return	(_g);
//			}
//
//			IC	_dist_type &h()
//			{
//				return	(_h);
//			}
//
//			IC	GV	*&back()
//			{
//				return	(_back);
//			}
//		};
//	};
//
//	template <template <typename _T> class PGV> 
//	struct DS {
//		typedef typename EDSB<PGV>::GV CGraphNode;
//	};
//};
//
//template <typename GV>
//struct CDSA {
//	typedef GV CGraphNode;
//};
//

template <typename _dist_type>
struct CEDSB {

	template <template <typename _T> class T1>
	struct EDSB {
		struct GV : public T1<GV> {
			_dist_type	_f;
			_dist_type	_g;
			_dist_type	_h;
			GV			*_back;

			IC	_dist_type &f()
			{
				return	(_f);
			}

			IC	_dist_type &g()
			{
				return	(_g);
			}

			IC	_dist_type &h()
			{
				return	(_h);
			}

			IC	GV	*&back()
			{
				return	(_back);
			}
		};
	};

	template <template <typename _T> class PGV> 
	struct DS {
		typedef typename EDSB<PGV>::GV CGraphNode;
	};
};

struct CEDSA {
	template <typename GV>
	struct DS {
		typedef GV CGraphNode;
	};
};

template <typename DSBE, typename DSAE>
struct CEDSBA {
	template <template <typename _T> class PGV> 
	struct DS : 
		public DSBE::DS<PGV>,
		public DSAE::DS<typename DSBE::DS<PGV>::CGraphNode>
	{
		typedef typename DSBE::DS<PGV>::CGraphNode CGraphNode;
	};
};

template <typename _index_type>
struct CEDSI {

	template <template <typename _T> class T1>
	struct DSI {
		template<typename T2>
		struct GV : public T1<T2> {
			_index_type	_index;

			IC	_index_type &index()
			{
				return	(_index);
			}
		};
	};

	template <
		template <
			typename _T
		> 
		class PGV = CEmptyClassTemplate,
		typename EDSBA = CEDSBA
	> 
	struct DS : public EDSBA::DS<DSI<PGV>::GV> {
		typedef typename EDSBA::DS<DSI<PGV>::GV>::CGraphNode CGraphNode;
	};
};

template <
	typename EDSI, 
	typename EDSB,
	typename EDSA,
	template <
		typename _1,
		typename _2
	>
	class	 EDSBA = CEDSBA
>
struct CEDSIBA {
	template <template <typename _T> class PGV = CEmptyClassTemplate> 
	struct DS : public EDSI::DS<PGV,EDSBA<EDSB,EDSA> > {
		typedef typename EDSI::DS<PGV,EDSBA<EDSB,EDSA> >::CGraphNode CGraphNode;
	};
};

struct CEDLSL {

	template <template <typename _T> class T1>
	struct DLSL {
		template<typename T2>
		struct GV : public T1<T2> {
			T2	*_prev;
			T2	*_next;

			IC	T2	*&prev()
			{
				return	(_prev);
			}

			IC	T2	*&next()
			{
				return	(_next);
			}
		};
	};

	template <
		typename EDSIBA,
		template <typename _T> class PGV = CEmptyClassTemplate
	>
	struct DS : public EDSIBA::DS<DLSL<PGV>::GV> {
		typedef typename EDSIBA::DS<DLSL<PGV>::GV>::CGraphNode CGraphNode;
	};
};

struct CESLSL {

	template <template <typename _T> class T1>
	struct SLSL {
		template<typename T2>
		struct GV : public T1<T2> {
			T2	*_next;

			IC	T2	*&next()
			{
				return	(_next);
			}
		};
	};

	template <
		typename EDSIBA,
		template <typename _T> class PGV = CEmptyClassTemplate
	>
	struct DS : public EDSIBA::DS<SLSL<PGV>::GV> {
		typedef typename EDSIBA::DS<SLSL<PGV>::GV>::CGraphNode CGraphNode;
	};
};

template <
	typename EDSD, 
	typename EDSI, 
	typename EDSB, 
	typename EDSA,
	template <typename _T> class GV = CEmptyClassTemplate,
	template <
		typename _1,
		typename _2
	>
	class	 EDSBA = CEDSBA,
	template <
		typename _1, 
		typename _2,
		typename _3,
		template <
			typename _1,
			typename _2
		>
		class	 _4
	>
	class	 EDSIBA = CEDSIBA
>
struct CDSC : public EDSD::DS<EDSIBA<EDSI,EDSB,EDSA,EDSBA>,GV> {
	typedef typename EDSD::DS<EDSIBA<EDSI,EDSB,EDSA,EDSBA>,GV>::CGraphNode CGraphNode;
};

//template <typename _1>
//struct A {
//	struct AA {
//		_1	i;
//	};
//
//	template <typename _2>
//	struct B {
//		IC	B();
//	};
//
//	AA		aa;
//
//	IC		A();
//	IC	AA	&a();
//};
//
//template <typename _1>
//IC	A<_1>::A()
//{
//	printf("AAA");
//}
//
//template <typename _1>
//IC	typename A<_1>::AA	&A<_1>::a()
//{
//	printf	("%d",aa.i);
//	return	(aa);
//}
//
//template <typename _1>
//template <typename _2>
//IC	A<_1>::B<_2>::B()
//{
//	printf	("BBB");
//}
//
template <template <typename _T> class T1>
struct A {
	struct AA : public T1<AA> {
		AA	*pointerA;
	};
};

template<template <typename _T> class T1 = CEmptyClassTemplate>
struct B {
	template <typename T2>
	struct BB : public T1<T2> {
		T2	*pointerB;
	};
};

template <typename T>
struct CClassTemplate {
	int		x;
	T		*X;
};

template <typename T1, typename T2>
struct TT {
	typedef T1 uu;
	T1 x;
	T2 y;
};

struct PP {
	int z;
};

template<
	typename T1,
	typename T2,
	typename T3
>
struct CProblemSolver {
	typedef T1 _index_type;
	typedef T3 _edge_type;
};

struct g_ca_stdout {
	template <
		typename _Graph,
		typename _Parameters
	>
	IC		void	search					(
				const _Graph			&graph, 
//				const u32				&start_node, 
//				const u32				&dest_node, 
//				xr_vector<u32>			*node_path,
				const _Parameters		&parameters
			)
	{
		printf		("Specialization\n");
	}

//	template <
//		typename _Graph,
//		typename _Parameters
//	>
//	IC		void	search					(
//				const _Graph			&graph, 
//				const u32				&start_node, 
//				const u32				&dest_node, 
//				xr_vector<u32>			*node_path,
//				_Parameters				&parameters
//			)
//	{
//		printf		("Non-const specialization\n");
//	}
//
	template <
		typename _condition_type,
		typename _value_type,
		typename _operator_id_type,
		typename _Parameters
	>
	IC		void	search					(
				const CProblemSolver<
					_condition_type,
					_value_type,
					_operator_id_type
				>						&graph, 
				const typename CProblemSolver<
					_condition_type,
					_value_type,
					_operator_id_type
				>::_index_type			&start_node,
				const typename CProblemSolver<
					_condition_type,
					_value_type,
					_operator_id_type
				>::_index_type			&dest_node, 
				xr_vector<
					typename CProblemSolver<
						_condition_type,
						_value_type,
						_operator_id_type
					>::_edge_type
				>						*node_path,
				const _Parameters		&parameters
			)
	{
		printf		("Partial specialization\n");
	}

//	template <>
//	IC		void	search					(
//				const CProblemSolver<
//					u32,
//					u32,
//					u32
//				>						&graph, 
//				const CProblemSolver<
//					u32,
//					u32,
//					u32
//				>::_index_type			&start_node,
//				const CProblemSolver<
//					u32,
//					u32,
//					u32
//				>::_index_type			&dest_node, 
//				xr_vector<
//					CProblemSolver<
//						u32,
//						u32,
//						u32
//					>::_edge_type
//				>						*node_path,
//				PP		&parameters
//			)
//	{
//		printf		("Partial partial specialization\n");
//	}
};
//int __cdecl main(char argc, char *argv[])
//{
////	g_ca_stdout								sss;
////	PP								t0;
////	CProblemSolver<u32,bool,u32>	t1;
////	CProblemSolver<u32,u32,u32>		t2;
//
////	sss.search	(t0,0,t0);//,1,0
////	sss.search	(t1,0,0,0,t0);//,1,0
////	sss.search	(t2,0,0,0,t0);//,1,0
//
////	typedef CEDSB<float>										_data_storage_base;
////	typedef CEDSBA<_data_storage_base,CDSA>						_data_storage_base_allocator;
////	typedef CEDSIBA<u32,_data_storage_base_allocator>			_data_storage_index_base_allocator;
////	typedef CEDLSL<_data_storage_index_base_allocator>			_data_storage;
////	typedef CDSC<CESLSL,CEDSI<u32>,CEDSB<float>,CEDSA>				_data_storage;
////
////	A<int>::B<float>	AAA;
////
////	_data_storage::CGraphNode	g;
////	g.f()		= 1.f;
////	g.g()		= 2.f;
////	g.h()		= 3.f;
////	g.index()	= 4;
////	g.back()	= &g;
//////	g.prev()	= &g;
////	g.next()	= &g;
////	u64			a = 0x7fffffffffffffff;
////	printf		("%s : %I64d","string",a);
////	A<B<CClassTemplate>::BB>::AA	X;
////	X.pointerA	= &X;
////	X.pointerB	= &X;
////	X.x			= 1;
////	X.X			= &X;
//	return		(0);

bool create_namespace_table(lua_State *L, LPCSTR N)
{
	lua_pushstring	(L,"_G");
	lua_gettable	(L,LUA_GLOBALSINDEX);
	LPSTR			S2 = (char*)xr_malloc((xr_strlen(N) + 1)*sizeof(char)), S = S2;
	strcpy			(S,N);
	for (;;) {
		if (!xr_strlen(S)) {
			xr_free		(S2);
			return		(false);
		}
		LPSTR			S1 = strchr(S,'.');
		if (S1)
			*S1				= 0;
		lua_pushstring	(L,S);
		lua_gettable	(L,-2);
		if (lua_isnil(L,-1)) {
			lua_pop			(L,1);
			lua_newtable	(L);
			lua_pushstring	(L,S);
			lua_pushvalue	(L,-2);
			lua_settable	(L,-4);
		}
		else
			if (!lua_istable(L,-1)) {
				xr_free			(S2);
				lua_pop			(L,2);
				printf			(" Error : the namespace name is already being used by the non-table object!\n");
				return			(false);
			}
		lua_remove		(L,-2);
		if (S1)
			S			= ++S1;
		else
			break;
	}
	xr_free			(S2);
	return			(true);
}

void copy_globals(lua_State *L)
{
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
}

bool do_file(lua_State *L, LPCSTR N, LPCSTR S, bool bCall)
{
	LPSTR				SS = (LPSTR)xr_malloc(4096), SS1 = (LPSTR)xr_malloc(4096);
	string256			S1;
	sprintf				(SS,"local this = %s; function %s.script_name() return \"%s\" end ",N,N,N);
	strcpy				(S1,"@");
	strcat				(S1,S);
//	luaL_loadbuffer		(L,SS,xr_strlen(SS),S1);
//	lua_call			(L,0,0);

	FILE				*f = fopen(S,"rb");
	fseek				(f,0,SEEK_END);
	long				e = ftell(f), u = e + xr_strlen(SS);
	fseek				(f,0,SEEK_SET);
	fread				(SS + xr_strlen(SS),1,e,f);
	fclose				(f);
//	memcpy				(SS1,SS,u);
	SS[u]			= 0;

	if (luaL_loadbuffer(L,SS,u,S1)) {
		xr_free			(SS);
		xr_free			(SS1);
		printf			("\n");
		for (int i=-1; ; i++)
			if (lua_isstring(L,i))
				printf	(" %s\n",lua_tostring(L,i));
			else
				break;
		lua_pop			(L,i + 4);
		return			(false);
	}
	xr_free				(SS);
	xr_free				(SS1);
	
	if (bCall) {
		int err			= lua_pcall(L,0,0,0);
		if (err) {
			printf		("%s",lua_tostring(L,0));
			printf		("%s",lua_tostring(L,-1));
		}
	}
	else
		lua_insert		(L,-4);

	return			(true);
}

void set_namespace(lua_State *L)
{
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
}

bool load_file_into_namespace(lua_State *L, LPCSTR S, LPCSTR N, bool bCall = true)
{
//	if (!create_namespace_table(L,N))
//		return		(false);
//	copy_globals	(L);
	if (!do_file(L,N,S,bCall))
		return		(false);
//	set_namespace	(L);
	return			(true);
}

lua_State		*L = 0;

luabind::object return_this(LPCSTR namespace_name)
{
	string256			S1;
	strcpy				(S1,namespace_name);
	LPSTR				S = S1;
	luabind::object		lua_namespace = luabind::get_globals(L);
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

luabind::object lua_this()
{
	lua_Debug	l_tDebugInfo;

	lua_getstack(L,1,&l_tDebugInfo);
	lua_getinfo	(L,"nSlu",&l_tDebugInfo);
	
//	printf			("  Event			: %d",cafEventToString(l_tDebugInfo.event));
	printf			("  Name			: %s\n",l_tDebugInfo.name);
	printf			("  Name what		: %s\n",l_tDebugInfo.namewhat);
	printf			("  What			: %s\n",l_tDebugInfo.what);
	printf			("  Source			: %s\n",l_tDebugInfo.source);
	printf			("  Source (short)	: %s\n",l_tDebugInfo.short_src);
	printf			("  Current line	: %d\n",l_tDebugInfo.currentline);
	printf			("  Nups			: %d\n",l_tDebugInfo.nups);
	printf			("  Line defined	: %d\n",l_tDebugInfo.linedefined);
	
	return		(return_this(""));
}

extern void test0();
extern void test1();

luabind::object name_space(lua_State *L, LPCSTR namespace_name)
{
	string256			S1;
	strcpy				(S1,namespace_name);
	LPSTR				S = S1;
	luabind::object		lua_namespace = luabind::get_globals(L);
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

void setup_table(LPCSTR namespace_name, luabind::object table)
{
//	create_namespace_table(table.lua_state(),namespace_name);
//	luabind::object space = name_space(table.lua_state(),namespace_name);
//	luabind::object::iterator	I = table.begin();
//	luabind::object::iterator	E = table.end();
//	for ( ; I != E; ++I)
}

template <int i>
struct CLuabindClass {
	bool foo(int a, LPCSTR b/**, CLuabindClass *c, CLuabindClass &d, const CLuabindClass *e, const CLuabindClass &f/**/){return true;};
	bool foo1(int a, LPCSTR b, CLuabindClass *c, float &d){return true;};
	bool operator == (const CLuabindClass<i> &) const
	{
		return true;
	}
};

void print_help(lua_State *L);
void time_smart_ptr_test();

#ifndef BOOST_NO_STRINGSTREAM
#	include <sstream>
#else
#	include <strstream>
#endif

std::string to_string(luabind::object const& o)
{
	using namespace luabind;
	if (o.type() == LUA_TSTRING) return object_cast<std::string>(o);
	lua_State* L = o.lua_state();
	LUABIND_CHECK_STACK(L);

#ifdef BOOST_NO_STRINGSTREAM
	std::strstream s;
#else
	std::stringstream s;
#endif

	if (o.type() == LUA_TNUMBER)
	{
		s << object_cast<float>(o);
		return s.str();
	}

	s << "<" << lua_typename(L, o.type()) << ">";
#ifdef BOOST_NO_STRINGSTREAM
	s << std::ends;
#endif
	return s.str();
}

void strreplaceall(std::string &str, LPCSTR S, LPCSTR N)
{
	LPSTR	A;
	int		S_len = xr_strlen(S);
	while ((A = strstr(str.c_str(),S)) != 0)
		str.replace(A - str.c_str(),S_len,N);
}

std::string &process_signature(std::string &str)
{
	strreplaceall	(str,"custom [","");
	strreplaceall	(str,"]","");
	strreplaceall	(str,"float","number");
	strreplaceall	(str,"lua_State*, ","");
	strreplaceall	(str," ,lua_State*","");
	return			(str);
}

std::string member_to_string(luabind::object const& e, LPCSTR function_signature)
{
#if !defined(LUABIND_NO_ERROR_CHECKING)
    using namespace luabind;
	lua_State* L = e.lua_state();
	LUABIND_CHECK_STACK(L);

	if (e.type() == LUA_TFUNCTION)
	{
		e.pushvalue();
		detail::stack_pop p(L, 1);

		{
			if (lua_getupvalue(L, -1, 3) == 0) return to_string(e);
			detail::stack_pop p2(L, 1);
			if (lua_touserdata(L, -1) != reinterpret_cast<void*>(0x1337)) return to_string(e);
		}

#ifdef BOOST_NO_STRINGSTREAM
		std::strstream s;
#else
		std::stringstream s;
#endif
		{
			lua_getupvalue(L, -1, 2);
			detail::stack_pop p2(L, 1);
		}

		{
			lua_getupvalue(L, -1, 1);
			detail::stack_pop p2(L, 1);
			detail::method_rep* m = static_cast<detail::method_rep*>(lua_touserdata(L, -1));

			for (std::vector<detail::overload_rep>::const_iterator i = m->overloads().begin();
				i != m->overloads().end(); ++i)
			{
				std::string str, temp;
				i->get_signature(L, str);
				s << function_signature << process_signature(str) << ";\n";
			}
		}
#ifdef BOOST_NO_STRINGSTREAM
		s << std::ends;
#endif
		return s.str();
	}

    return to_string(e);
#else
    return "";
#endif
}

void print_class(lua_State *L, luabind::detail::class_rep *crep)
{
	// print class and bases
	{
		printf			("class %s",crep->name());
		typedef std::vector<luabind::detail::class_rep::base_info> BASES;
		const BASES &bases = crep->bases();
		BASES::const_iterator	I = bases.begin(), B = I;
		BASES::const_iterator	E = bases.end();
		if (B != E)
			printf(" : ");
		for ( ; I != E; ++I) {
			if (I != B)
				printf	(",");
			printf("%s",(*I).base->name());
		}
		printf			("\n");
	}
	// print class constants
	{
		const luabind::detail::class_rep::STATIC_CONSTANTS	&constants = crep->static_constants();
		luabind::detail::class_rep::STATIC_CONSTANTS::const_iterator	I = constants.begin();
		luabind::detail::class_rep::STATIC_CONSTANTS::const_iterator	E = constants.end();
		for ( ; I != E; ++I)
			printf		("    const %s = %d;\n",(*I).first,(*I).second);
		if (!constants.empty())
			printf		("    \n");
	}
	// print class properties
	{
		typedef std::map<const char*, luabind::detail::class_rep::callback, luabind::detail::ltstr> PROPERTIES;
		const PROPERTIES &properties = crep->properties();
		PROPERTIES::const_iterator	I = properties.begin();
		PROPERTIES::const_iterator	E = properties.end();
		for ( ; I != E; ++I)
			printf	("    property %s;\n",(*I).first);
		if (!properties.empty())
			printf		("    \n");
	}
	// print class constructors
	{
		const std::vector<luabind::detail::construct_rep::overload_t>	&constructors = crep->constructors().overloads;
		std::vector<luabind::detail::construct_rep::overload_t>::const_iterator	I = constructors.begin();
		std::vector<luabind::detail::construct_rep::overload_t>::const_iterator	E = constructors.end();
		for ( ; I != E; ++I) {
			std::string S;
			(*I).get_signature(L,S);
			printf		("    %s %s;\n",crep->name(),S.c_str());
		}
		if (!constructors.empty())
			printf		("    \n");
	}
	// print class methods
	{
		crep->get_table	(L);
		luabind::object	table(L);
		table.set		();
		for (luabind::object::iterator i = table.begin(); i != table.end(); ++i) {
			luabind::object	object = *i;
			std::string	S;
			S			= "    function ";
			S.append	(to_string(i.key()).c_str());

			strreplaceall	(S,"function __add","operator +");
			strreplaceall	(S,"function __sub","operator -");
			strreplaceall	(S,"function __mul","operator *");
			strreplaceall	(S,"function __div","operator /");
			strreplaceall	(S,"function __pow","operator ^");
			strreplaceall	(S,"function __lt","operator <");
			strreplaceall	(S,"function __le","operator <=");
			strreplaceall	(S,"function __eq","operator ==");
			printf			("%s",member_to_string(object,S.c_str()).c_str());
		}
	}
	// print 
	printf			("\n");
}

void print_free_functions	(lua_State *L, const luabind::object &object, LPCSTR header, const std::string &indent)
{
	u32							count = 0;
	luabind::object::iterator	I = object.begin();
	luabind::object::iterator	E = object.end();
	for ( ; I != E; ++I) {
		if ((*I).type() != LUA_TFUNCTION)
			continue;
		(*I).pushvalue();
		detail::free_functions::function_rep* rep = 0;
		if (lua_iscfunction(L, -1))
		{
			if (lua_getupvalue(L, -1, 2) != 0)
			{
				// check the magic number that identifies luabind's functions
				if (lua_touserdata(L, -1) == (void*)0x1337)
				{
					if (lua_getupvalue(L, -2, 1) != 0)
					{
						if (!count)
							printf("%snamespace %s {\n",indent.c_str(),header);
						++count;
						rep = static_cast<luabind::detail::free_functions::function_rep*>(lua_touserdata(L, -1));
						std::vector<luabind::detail::free_functions::overload_rep>::const_iterator	i = rep->overloads().begin();
						std::vector<luabind::detail::free_functions::overload_rep>::const_iterator	e = rep->overloads().end();
						for ( ; i != e; ++i) {
							std::string	S;
							(*i).get_signature(L,S);
							printf("    %sfunction %s%s;\n",indent.c_str(),rep->name(),process_signature(S).c_str());
						}
						lua_pop(L, 1);
					}
				}
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 1);
	}
	{
		std::string				_indent = indent;
		_indent.append			("    ");
		object.pushvalue();
		lua_pushnil		(L);
		while (lua_next(L, -2) != 0) {
			if (lua_type(L, -1) == LUA_TTABLE) {
				if (xr_strcmp("_G",lua_tostring(L, -2))) {
					LPCSTR				S = lua_tostring(L, -2);
					luabind::object		object(L);
					object.set			();
					print_free_functions(L,object,S,_indent);
				}
			}
			lua_pop	(L, 1);
		}
	}
	if (count)
		printf("%s};\n",indent.c_str());
}

void print_help(lua_State *L)
{
	printf					("\n\nList of the classes exported to LUA\n\n");
	luabind::detail::class_registry::get_registry(L)->iterate_classes(L,&print_class);
	print_free_functions	(L,luabind::get_globals(L),"","");
}

extern void lesha_test();
void bug_test();

int __cdecl main(int argc, char* argv[])
{
//	test1();
//	test0();
//	time_smart_ptr_test();
//	lesha_test();
	bug_test();
	return 0;

	printf	("xrLuaCompiler v0.1\n");
//	if (argc < 2) {
//		printf	("Syntax : xrLuaCompiler.exe <file1> <file2> ... <fileN>\nAll the files must be in the directory \"s:\\gamedata\\scripts\" \nwith \".script\" extension\n");
//		return 0;
//	}

//	setvbuf				(stderr,g_ca_stdout,_IOFBF,sizeof(g_ca_stdout));
//	setvbuf				(stdout,g_ca_stdout,_IOFBF,sizeof(g_ca_stdout));
	g_ca_stdout[0]		= 0;
	
	L = lua_open();

	if (!L)
		lua_error	(L);

	luaopen_base	(L);
	luaopen_string	(L);
	luaopen_math	(L);
	luaopen_table	(L);
	luaopen_debug	(L);

	lua_settop		(L,0);

	lua_pushstring	(L,"_0");
	lua_pushstring	(L,"_1");
	lua_pushstring	(L,"_2");
	lua_pushstring	(L,"_3");
	lua_pushstring	(L,"_4");

	open			(L);
	
	printf			("Stack level %d\n",lua_gettop(L));

	function		(L,"setup_table",&setup_table);

	module(L)
	[
//		class_<CCustomClass,CCustomClassWrapper>("CCustomClass")
//			.def(constructor<>())
//			.def("foo",&CCustomClass::foo,&CCustomClassWrapper::foo_static),
//
//		class_<CBaseClass,CBaseClassWrapper>("CBaseClass")
//			.def(constructor<>())
//			.def("factory_method",&CBaseClass::factory_method,&CBaseClassWrapper::factory_method_static,adopt(result))
//			.def("update",&CBaseClass::update),
//
		class_<CLuabindClass<0> >("CLuabindClass0")
			.def(constructor<>())
			.def("foo",&CLuabindClass<0>::foo)
			.def("foo",&CLuabindClass<0>::foo1),

		class_<CLuabindClass<1> >("CLuabindClass1")
			.def(constructor<>())
			.def(const_self == other<CLuabindClass<1> >())
			.def("foo",&CLuabindClass<1>::foo)
			.def("foo",&CLuabindClass<1>::foo1)
			.enum_("something")
			[
				value("_1",1),
				value("_2",2)
			]
	];

//	module(L,"test_module")
//	[
//		def("setup_table",&setup_table)
//	];

//	vfPrintTable	(L,"_G",true);
//	function		(L,"this",lua_this);
//	for (int i=1; i<argc; i++) 
//	{
//		string256		l_caScriptName;
//		strcpy			(l_caScriptName,"x:\\test_test.script");
//		printf			("File %s : ",l_caScriptName);
//		bool			b = load_file_into_namespace(L,l_caScriptName,"test_test",true);
//		print_stack		(L);
//		vfPrintTable	(L,"_G",true);
//		lua_dostring	(L,"test_test.main()");
//		if (xr_strlen(g_ca_stdout)) {
//			printf		("\n%s\n",g_ca_stdout);
//			strcpy		(g_ca_stdout,"");
//		}
//		else
//			if (b)
//				printf	("0 syntax errors\n");
//	}
//	luaL_loadfile		(L,"x:\\test_test.script");
//	if (xr_strlen(g_ca_stdout)) {
//		printf		("\n%s\n",g_ca_stdout);
//		strcpy		(g_ca_stdout,"");
//	}
//	lua_pcall			(L,0,0,0);
//	if (xr_strlen(g_ca_stdout)) {
//		printf		("\n%s\n",g_ca_stdout);
//		strcpy		(g_ca_stdout,"");
//	}
//	lua_dostring		(L,"a.main()");
//	if (xr_strlen(g_ca_stdout)) {
//		printf		("\n%s\n",g_ca_stdout);
//		strcpy		(g_ca_stdout,"");
//	}
//	lua_dostring		(L,"a.b.main()");
//	if (xr_strlen(g_ca_stdout)) {
//		printf		("\n%s\n",g_ca_stdout);
//		strcpy		(g_ca_stdout,"");
//	}
//	vfPrintTable	(L,"_G",true);
//
//	printf			("Stack level %d\n",lua_gettop(L));
//	
//	print_stack		(L);
//	
//	printf			("%s\n",lua_tostring(L,1));
//	printf			("%s\n",lua_tostring(L,2));
//	printf			("%s\n",lua_tostring(L,3));
//	printf			("%s\n",lua_tostring(L,4));
//	printf			("%s\n",lua_tostring(L,5));

//	print_help		(L);

//	luaL_loadfile	(L,"x:\\test_test.script");
//	if (xr_strlen(g_ca_stdout)) {
//		fputc		(0,stderr);
//		printf		("\n%s\n",g_ca_stdout);
//		fflush		(stderr);
//		strcpy		(g_ca_stdout,"");
//	}
//	lua_pcall			(L,0,0,0);

/**/
	LPCSTR					s = "main()";
	lua_dofile				(L,"x:\\bug6.script");
	for (int i=0; ;++i) {
		lua_State			*t = lua_newthread(L);
		int					thread_reference = luaL_ref(L,LUA_REGISTRYINDEX);
		int					err = luaL_loadbuffer	(t,s,strlen(s),"@_thread_main");
		if (err) {
			printf			("ERROR : %s\n",lua_tostring(L,-1));
			break;
		}
		lua_resume			(t,0);
		luaL_unref			(L,LUA_REGISTRYINDEX,thread_reference);
		lua_setgcthreshold	(L,0);
	}
/**

	lua_dofile					(L,"x:\\bug6.script");
	for (;;) {
		luabind::functor<void>	f = luabind::object_cast<luabind::functor<void> >(luabind::get_globals(L)["main"]);
		f						();
		lua_setgcthreshold		(L,0);
	}
/**/


//	lua_setgcthreshold	(L,0);
//	if (xr_strlen(g_ca_stdout)) {
//		fputc		(0,stderr);
//		printf		("\n%s\n",g_ca_stdout);
//		fflush		(stderr);
//		strcpy		(g_ca_stdout,"");
//	}
//	lua_dostring		(L,"test_test.main()");
//	if (xr_strlen(g_ca_stdout)) {
//		fputc		(0,stderr);
//		printf		("\n%s\n",g_ca_stdout);
//		fflush		(stderr);
//		strcpy		(g_ca_stdout,"");
//	}
//	lua_dostring		(L,"a.b.main()");
//	if (xr_strlen(g_ca_stdout)) {
//		fputc		(0,stderr);
//		printf		("\n%s\n",g_ca_stdout);
//		fflush		(stderr);
//		strcpy		(g_ca_stdout,"");
//	}


//	luabind::object		my_class(get_globals(L)["class_name"]);
//	CLuabindClass<0>	*p = object_cast<CLuabindClass<0>*>(my_class(),adopt(result));

//	delete			(g_custom);
	lua_setgcthreshold	(L,0);
	lua_close		(L);

//	check if we are yielded

//	L->ci->state & CI_YIELD
}

void bug_test	()
{
	lua_State				*L = lua_open();

	if (!L) {
		lua_error			(L);
		return;
	}

	luaopen_base			(L);
	luaopen_string			(L);
	luaopen_math			(L);
	luaopen_table			(L);
	luaopen_debug			(L);
	
	luabind::open			(L);

	LPCSTR					s = "main()";
	lua_dofile				(L,"x:\\bug6.script");
	for (int i=0; i<10; ++i) {
		int					top = lua_gettop(L);
		lua_State			*t = lua_newthread(L);
//		int					thread_reference = luaL_ref(L,LUA_REGISTRYINDEX);
		int					err = luaL_loadbuffer(t,s,strlen(s),"@_thread_main");
		if (err) {
			printf			("ERROR : %s\n",lua_tostring(L,-1));
			break;
		}
		lua_resume			(t,0);
//		luaL_unref			(L,LUA_REGISTRYINDEX,thread_reference);
		lua_settop			(L,top);
		lua_setgcthreshold	(L,0);
	}

	lua_close				(L);
}